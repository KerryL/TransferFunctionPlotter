/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainFrame.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains the class functionality (event handlers, etc.) for the
//				 MainFrame class.  Uses wxWidgets for the GUI components.
// History:

// Standard C++ headers
#include <algorithm>

// wxWidgets headers
#include <wx/grid.h>
#include <wx/colordlg.h>
#include <wx/splitter.h>

// Local headers
#include "application/mainFrame.h"
#include "application/plotterApp.h"
#include "renderer/plotRenderer.h"
#include "renderer/color.h"
#include "utilities/dataset2D.h"
#include "utilities/math/plotMath.h"
#include "utilities/math/expressionTree.h"

// *nix Icons
#ifdef __WXGTK__
#include "../../res/icons/tfplot16.xpm"
#include "../../res/icons/tfplot24.xpm"
#include "../../res/icons/tfplot32.xpm"
#include "../../res/icons/tfplot48.xpm"
#include "../../res/icons/tfplot64.xpm"
#include "../../res/icons/tfplot128.xpm"
#endif

//==========================================================================
// Class:			MainFrame
// Function:		MainFrame
//
// Description:		Constructor for MainFrame class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition,
								 wxDefaultSize, wxDEFAULT_FRAME_STYLE)
{
	CreateControls();
	SetProperties();
}

//==========================================================================
// Class:			MainFrame
// Function:		~MainFrame
//
// Description:		Denstructor for MainFrame class.  Frees memory and
//					releases GUI object managers.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MainFrame::~MainFrame()
{
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateControls
//
// Description:		Creates sizers and controls and lays them out in the window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::CreateControls(void)
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxSplitterWindow *splitter = new wxSplitterWindow(this);
	topSizer->Add(splitter, 1, wxGROW);

	wxPanel *lowerPanel = new wxPanel(splitter);
	wxBoxSizer *lowerSizer = new wxBoxSizer(wxHORIZONTAL);
	lowerSizer->Add(CreateButtons(lowerPanel), 0, wxALL, 5);
	lowerSizer->Add(CreateOptionsGrid(lowerPanel), 1, wxGROW | wxALL, 5);
	lowerPanel->SetSizer(lowerSizer);

	CreatePlotArea(splitter);
	splitter->SplitHorizontally(plotArea, lowerPanel, plotArea->GetSize().GetHeight());
	splitter->SetSashGravity(1.0);
	splitter->SetMinimumPaneSize(150);

	SetSizerAndFit(topSizer);
}

//==========================================================================
// Class:			MainFrame
// Function:		CreatePlotArea
//
// Description:		Creates the main plot control.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotRenderer* pointing to plotArea
//
//==========================================================================
PlotRenderer* MainFrame::CreatePlotArea(wxWindow *parent)
{
#ifdef __WXGTK__
	// Under GTK, we get a segmentation fault or X error on call to SwapBuffers in RenderWindow.
	// Adding the double-buffer arugment fixes this.  Under windows, the double-buffer argument
	// causes the colors to go funky.  So we have this #if.
	int args[] = {WX_GL_DOUBLEBUFFER, 0};
	plotArea = new PlotRenderer(parent, wxID_ANY, args, *this);
#else
	plotArea = new PlotRenderer(parent, wxID_ANY, NULL, *this);
#endif

	plotArea->SetMinSize(wxSize(650, 320));
	plotArea->SetGridOn();
	plotArea->SetXLogarithmic(true);

	return plotArea;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateButtons
//
// Description:		Creates and formats the options grid.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxGrid* pointing to optionsGrid
//
//==========================================================================
wxSizer* MainFrame::CreateButtons(wxWindow *parent)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	wxButton *add = new wxButton(parent, idAddButton, _T("Add"));
	wxButton *remove = new wxButton(parent, idRemoveButton, _T("Remove"));
	wxButton *removeAll = new wxButton(parent, idRemoveAllButton, _T("Remove All"));

	sizer->Add(add, 0, wxALL, 2);
	sizer->Add(remove, 0, wxALL, 2);
	sizer->Add(removeAll, 0, wxALL, 2);

	wxStaticText *version = new wxStaticText(parent, wxID_ANY, TransferFunctionPlotterApp::versionString);
	sizer->Add(version, 0, wxALL, 2);

	return sizer;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateOptionsGrid
//
// Description:		Creates and formats the options grid.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxGrid* pointing to optionsGrid
//
//==========================================================================
wxGrid* MainFrame::CreateOptionsGrid(wxWindow *parent)
{
	optionsGrid = new wxGrid(parent, wxID_ANY);

	optionsGrid->BeginBatch();

	optionsGrid->CreateGrid(0, colCount, wxGrid::wxGridSelectRows);
	optionsGrid->SetRowLabelSize(0);
	optionsGrid->SetColFormatNumber(colLineSize);
	optionsGrid->SetColFormatNumber(colMarkerSize);
	optionsGrid->SetColFormatFloat(colLeftCursor);
	optionsGrid->SetColFormatFloat(colRightCursor);
	optionsGrid->SetColFormatFloat(colDifference);
	optionsGrid->SetColFormatBool(colVisible);
	optionsGrid->SetColFormatBool(colRightAxis);

	optionsGrid->SetColLabelValue(colName, _T("Curve"));
	optionsGrid->SetColLabelValue(colColor, _T("Color"));
	optionsGrid->SetColLabelValue(colLineSize, _T("Line"));
	optionsGrid->SetColLabelValue(colMarkerSize, _T("Marker"));
	optionsGrid->SetColLabelValue(colLeftCursor, _T("Left Cursor"));
	optionsGrid->SetColLabelValue(colRightCursor, _T("Right Cursor"));
	optionsGrid->SetColLabelValue(colDifference, _T("Difference"));
	optionsGrid->SetColLabelValue(colVisible, _T("Visible"));
	optionsGrid->SetColLabelValue(colRightAxis, _T("Right Axis"));

	optionsGrid->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	optionsGrid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

	unsigned int i;
	for (i = 1; i < colCount; i++)// Skip the name column
		optionsGrid->AutoSizeColLabelSize(i);

	optionsGrid->EndBatch();

	return optionsGrid;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetProperties
//
// Description:		Sets the window properties for this window.  Includes
//					title, frame size, and default fonts.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetProperties(void)
{
	SetTitle(TransferFunctionPlotterApp::transferFunctionPlotterTitle);
	SetName(TransferFunctionPlotterApp::transferFunctionPlotterName);
	Center();

#ifdef __WXMSW__
	SetIcon(wxIcon(_T("ICON_ID_MAIN"), wxBITMAP_TYPE_ICO_RESOURCE));
#elif __WXGTK__
	SetIcon(wxIcon(tfplot16_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfplot24_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfplot32_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfplot48_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfplot64_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcontfplot128_xpm, wxBITMAP_TYPE_XPM));
#endif
}

//==========================================================================
// Class:			MainFrame
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	// Buttons
	EVT_BUTTON(idAddButton,							MainFrame::AddButtonClicked)

	// Grid control
	EVT_GRID_CELL_RIGHT_CLICK(MainFrame::GridRightClickEvent)
	EVT_GRID_CELL_LEFT_DCLICK(MainFrame::GridDoubleClickEvent)
	EVT_GRID_CELL_LEFT_CLICK(MainFrame::GridLeftClickEvent)
	EVT_GRID_CELL_CHANGE(MainFrame::GridCellChangeEvent)

	// Context menu
	EVT_MENU(idPlotContextToggleGridlines,			MainFrame::ContextToggleGridlines)
	EVT_MENU(idPlotContextAutoScale,				MainFrame::ContextAutoScale)
	EVT_MENU(idPlotContextWriteImageFile,			MainFrame::ContextWriteImageFile)

	EVT_MENU(idPlotContextBGColor,					MainFrame::ContextPlotBGColor)
	EVT_MENU(idPlotContextGridColor,				MainFrame::ContextGridColor)

	EVT_MENU(idPlotContextToggleBottomGridlines,	MainFrame::ContextToggleGridlinesBottom)
	EVT_MENU(idPlotContextSetBottomLogarithmic,		MainFrame::ContextSetLogarithmicBottom)
	EVT_MENU(idPlotContextAutoScaleBottom,			MainFrame::ContextAutoScaleBottom)

	EVT_MENU(idPlotContextToggleLeftGridlines,		MainFrame::ContextToggleGridlinesLeft)
	EVT_MENU(idPlotContextSetLeftLogarithmic,		MainFrame::ContextSetLogarithmicLeft)
	EVT_MENU(idPlotContextAutoScaleLeft,			MainFrame::ContextAutoScaleLeft)

	EVT_MENU(idPlotContextToggleRightGridlines,		MainFrame::ContextToggleGridlinesRight)
	EVT_MENU(idPlotContextSetRightLogarithmic,		MainFrame::ContextSetLogarithmicRight)
	EVT_MENU(idPlotContextAutoScaleRight,			MainFrame::ContextAutoScaleRight)
END_EVENT_TABLE();

//==========================================================================
// Class:			MainFrame
// Function:		ContextWriteImageFile
//
// Description:		Calls the object of interest's write image file method.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextWriteImageFile(wxCommandEvent& WXUNUSED(event))
{
	wxArrayString pathAndFileName = GetFileNameFromUser(_T("Save Image File"), wxEmptyString, wxEmptyString,
		_T("Bitmap Image (*.bmp)|*.bmp|JPEG Image (*.jpg, *.jpeg)|*.jpg;*.jpeg|PNG Image (*.png)|*.png|TIFF Image (*.tif)|*.tif"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (pathAndFileName.IsEmpty())
		return;

	plotArea->WriteImageToFile(pathAndFileName[0]);
}

//==========================================================================
// Class:			MainFrame
// Function:		GetFileNameFromUser
//
// Description:		Displays a dialog asking the user to specify a file name.
//					Arguments allow this to be for opening or saving files,
//					with different options for the wildcards.
//
// Input Arguments:
//		dialogTitle			= wxString containing the title for the dialog
//		defaultDirectory	= wxString specifying the initial directory
//		defaultFileName		= wxString specifying the default file name
//		wildcard			= wxString specifying the list of file types to
//							  allow the user to select
//		style				= long integer specifying the type of dialog
//							  (this is usually wxFD_OPEN or wxFD_SAVE)
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing the paths and file names of the specified files,
//		or and empty array if the user cancels
//
//==========================================================================
wxArrayString MainFrame::GetFileNameFromUser(wxString dialogTitle, wxString defaultDirectory,
										 wxString defaultFileName, wxString wildcard, long style)
{
	wxArrayString pathsAndFileNames;

	wxFileDialog dialog(this, dialogTitle, defaultDirectory, defaultFileName,
		wildcard, style);

	dialog.CenterOnParent();
	if (dialog.ShowModal() == wxID_OK)
	{
		// If this was an open dialog, we want to get all of the selected paths,
		// otherwise, just get the one selected path
		if (style & wxFD_OPEN)
			dialog.GetPaths(pathsAndFileNames);
		else
			pathsAndFileNames.Add(dialog.GetPath());
	}

	return pathsAndFileNames;
}

void MainFrame::AddButtonClicked(wxCommandEvent& WXUNUSED(event))
{
	wxString s = wxGetTextFromUser(_T("Specify Transfer Function"));
	ExpressionTree e;
	Dataset2D *magnitude = new Dataset2D;
	Dataset2D *phase = new Dataset2D;
	wxString r = e.Solve(s, 0.1, 1000.0, 1000, *magnitude, *phase);
	if (!r.IsEmpty())
	{
		wxMessageBox(r);
		delete magnitude;
		delete phase;
		return;
	}
	AddCurve(magnitude, s + _T("; Magnitude [dB]"));
	AddCurve(phase, s + _T("; Phase [deg]"), true);
}

void MainFrame::RemoveAllButtonClicked(wxCommandEvent& WXUNUSED(event))
{
	ClearAllCurves();
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateGridContextMenu
//
// Description:		Displays a context menu for the grid control.
//
// Input Arguments:
//		position	= const wxPoint& specifying the position to display the menu
//		row			= const unsigned int& specifying the row that was clicked
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::CreateGridContextMenu(const wxPoint &position, const unsigned int &row)
{
	// FIXME:  Remove?
}

//==========================================================================
// Class:			MainFrame
// Function:		CreatePlotContextMenu
//
// Description:		Displays a context menu for the plot.
//
// Input Arguments:
//		position	= const wxPoint& specifying the position to display the menu
//		context		= const PlotContext& describing the area of the plot
//					  on which the click occured
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::CreatePlotContextMenu(const wxPoint &position, const PlotContext &context)
{
	wxMenu *contextMenu;

	switch (context)
	{
	case plotContextXAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextToggleBottomGridlines);
		contextMenu->Check(idPlotContextSetBottomLogarithmic, plotArea->GetXLogarithmic());
		break;

	case plotContextLeftYAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextToggleLeftGridlines);
		contextMenu->Check(idPlotContextSetLeftLogarithmic, plotArea->GetLeftLogarithmic());
		break;

	case plotContextRightYAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextToggleRightGridlines);
		contextMenu->Check(idPlotContextSetRightLogarithmic, plotArea->GetRightLogarithmic());
		break;

	default:
	case plotContextPlotArea:
		contextMenu = CreatePlotAreaContextMenu();
		break;
	}

	PopupMenu(contextMenu, position);

	delete contextMenu;
	contextMenu = NULL;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreatePlotAreaContextMenu
//
// Description:		Displays a context menu for the specified plot axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxMenu*
//
//==========================================================================
wxMenu* MainFrame::CreatePlotAreaContextMenu(void) const
{
	wxMenu *contextMenu = new wxMenu();
	contextMenu->Append(idPlotContextToggleGridlines, _T("Toggle Gridlines"));
	contextMenu->Append(idPlotContextAutoScale, _T("Auto Scale"));
	contextMenu->Append(idPlotContextWriteImageFile, _T("Write Image File"));
	contextMenu->AppendSeparator();
	contextMenu->Append(idPlotContextBGColor, _T("Set Background Color"));
	contextMenu->Append(idPlotContextGridColor, _T("Set Gridline Color"));

	return contextMenu;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateAxisContextMenu
//
// Description:		Displays a context menu for the specified plot axis.
//
// Input Arguments:
//		baseEventId	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxMenu*
//
//==========================================================================
wxMenu* MainFrame::CreateAxisContextMenu(const unsigned int &baseEventId) const
{
	wxMenu* contextMenu = new wxMenu();

	contextMenu->Append(baseEventId, _T("Toggle Axis Gridlines"));
	contextMenu->Append(baseEventId + 1, _T("Auto Scale Axis"));
	contextMenu->AppendCheckItem(baseEventId + 2, _T("Logarithmic Scale"));

	return contextMenu;
}

//==========================================================================
// Class:			MainFrame
// Function:		ClearAllCurves
//
// Description:		Removes all curves from the plot.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ClearAllCurves(void)
{
	while (plotList.GetCount() > 0)
		RemoveCurve(0);
}

//==========================================================================
// Class:			MainFrame
// Function:		AddCurve
//
// Description:		Adds an existing dataset to the plot.
//
// Input Arguments:
//		data	= Dataset2D* to add
//		name	= wxString specifying the label for the curve
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddCurve(Dataset2D *data, wxString name, const bool &rightAxis)
{
	plotList.Add(data);

	optionsGrid->BeginBatch();
	if (optionsGrid->GetNumberRows() == 0)
		AddTimeRowToGrid();
	unsigned int index = AddDataRowToGrid(name);
	optionsGrid->EndBatch();

	plotArea->AddCurve(*data);
	UpdateCurveProperties(index - 1, GetNextColor(index), true, rightAxis);
	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			MainFrame
// Function:		AddTimeRowToGrid
//
// Description:		Adds the entry for the time data to the options grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddTimeRowToGrid(void)
{
	optionsGrid->AppendRows();

	unsigned int i;
	for (i = 0; i < colCount; i++)
		optionsGrid->SetReadOnly(0, i, true);
}

//==========================================================================
// Class:			MainFrame
// Function:		AddDataRowToGrid
//
// Description:		Adds the entry for the data to the options grid.
//
// Input Arguments:
//		name	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the index of the new data
//
//==========================================================================
unsigned int MainFrame::AddDataRowToGrid(const wxString &name)
{
	unsigned int index = optionsGrid->GetNumberRows();
	optionsGrid->AppendRows();

	unsigned int maxLineSize(5);
	unsigned int maxMarkerSize(5);

	optionsGrid->SetCellEditor(index, colVisible, new wxGridCellBoolEditor);
	optionsGrid->SetCellEditor(index, colRightAxis, new wxGridCellBoolEditor);
	optionsGrid->SetCellEditor(index, colLineSize, new wxGridCellNumberEditor(1, maxLineSize));
	optionsGrid->SetCellEditor(index, colMarkerSize, new wxGridCellNumberEditor(-1, maxMarkerSize));

	unsigned int i;
	for (i = 0; i < colDifference; i++)
			optionsGrid->SetReadOnly(index, i, true);
	optionsGrid->SetReadOnly(index, colLineSize, false);
	optionsGrid->SetReadOnly(index, colMarkerSize, false);
	optionsGrid->SetCellValue(index, colName, name);

	Color color = GetNextColor(index);

	optionsGrid->SetCellBackgroundColour(index, colColor, color.ToWxColor());
	optionsGrid->SetCellValue(index, colLineSize, _T("1"));
	optionsGrid->SetCellValue(index, colMarkerSize, _T("-1"));
	optionsGrid->SetCellValue(index, colVisible, _T("1"));

	int width = optionsGrid->GetColumnWidth(colName);
	optionsGrid->AutoSizeColumn(colName);
	if (optionsGrid->GetColumnWidth(colName) < width)
		optionsGrid->SetColumnWidth(colName, width);

	return index;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetNextColor
//
// Description:		Determines the next color to use (cycles through all the
//					pre-defined colors).
//
// Input Arguments:
//		index	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		Color to sue
//
//==========================================================================
Color MainFrame::GetNextColor(const unsigned int &index) const
{
	unsigned int colorIndex = (index - 1) % 10;
	if (colorIndex == 0)
		return Color::ColorBlue;
	else if (colorIndex == 1)
		return Color::ColorRed;
	else if (colorIndex == 2)
		return Color::ColorGreen;
	else if (colorIndex == 3)
		return Color::ColorMagenta;
	else if (colorIndex == 4)
		return Color::ColorCyan;
	else if (colorIndex == 5)
		return Color::ColorOrange;
	else if (colorIndex == 6)
		return Color::ColorGray;
	else if (colorIndex == 7)
		return Color::ColorPurple;
	else if (colorIndex == 8)
		return Color::ColorLightBlue;
	else if (colorIndex == 9)
		return Color::ColorBlack;
	else
		assert(false);

	// The following colors we opt'ed not to use - either too hard to see or too similar to other colors
	// Color::ColorYellow
	// Color::ColorDrabGreen
	// Color::ColorPaleGreen
	// Color::ColorPink

	return Color::ColorBlack;
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveCurve
//
// Description:		Removes a curve from the plot.
//
// Input Arguments:
//		i	= const unsigned int& specifying curve to remove
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RemoveCurve(const unsigned int &i)
{
	optionsGrid->DeleteRows(i + 1);

	if (optionsGrid->GetNumberRows() == 1)
		optionsGrid->DeleteRows();

	optionsGrid->AutoSizeColumns();

	plotArea->RemoveCurve(i);
	plotList.Remove(i);
}

//==========================================================================
// Class:			MainFrame
// Function:		GridRightClickEvent
//
// Description:		Handles right-click events on the grid control.  Displays
//					context menu.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridRightClickEvent(wxGridEvent &event)
{
	optionsGrid->SelectRow(event.GetRow());
	CreateGridContextMenu(event.GetPosition() + optionsGrid->GetPosition()
		+ optionsGrid->GetParent()->GetPosition(), event.GetRow());
}

//==========================================================================
// Class:			MainFrame
// Function:		GridDoubleClickEvent
//
// Description:		Handles double click event for the grid control.  If the
//					click occurs on the color box, display the color dialog.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridDoubleClickEvent(wxGridEvent &event)
{
	// No color for Time
	int row = event.GetRow();
	if (row == 0)
		return;

	if (event.GetCol() != colColor)
	{
		event.Skip();
		return;
	}

	wxColourData colorData;
	colorData.SetColour(optionsGrid->GetCellBackgroundColour(row, colColor));

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
	dialog.SetTitle(_T("Choose Line Color"));
	if (dialog.ShowModal() == wxID_OK)
    {
        colorData = dialog.GetColourData();
		optionsGrid->SetCellBackgroundColour(row, colColor, colorData.GetColour());
		UpdateCurveProperties(row - 1);
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		GridLeftClickEvent
//
// Description:		Handles grid cell change events (for boolean controls).
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridLeftClickEvent(wxGridEvent &event)
{
	unsigned int row = event.GetRow();

	// This stuff may be necessary after bug is fixed with wxGrid::GetSelectedRows()?
	// See ButtonRemoveCurveClickedEvent() for details
	//optionsGrid->SetSelectionMode(wxGrid::wxGridSelectRows);
	//optionsGrid->SelectRow(row, event.ControlDown());

	// Skip to handle row selection (with SHIFT and CTRL) and also boolean column click handlers
	event.Skip();

	// Was this click in one of the boolean columns and not in the time row?
	if (row == 0 || (event.GetCol() != colVisible && event.GetCol() != colRightAxis))
		return;

	if (optionsGrid->GetCellValue(row, event.GetCol()).Cmp(_T("1")) == 0)
		optionsGrid->SetCellValue(row, event.GetCol(), wxEmptyString);
	else
		optionsGrid->SetCellValue(row, event.GetCol(), _T("1"));

	UpdateCurveProperties(row - 1);
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCurveProperties
//
// Description:		Updates the specified curve properties.
//
// Input Arguments:
//		index	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateCurveProperties(const unsigned int &index)
{
	Color color;
	color.Set(optionsGrid->GetCellBackgroundColour(index + 1, colColor));
	UpdateCurveProperties(index, color,
		!optionsGrid->GetCellValue(index + 1, colVisible).IsEmpty(),
		!optionsGrid->GetCellValue(index + 1, colRightAxis).IsEmpty());
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCurveProperties
//
// Description:		Updates the specified curve properties to match the arguments.
//
// Input Arguments:
//		index		= const unsigned int&
//		color		= const Color&
//		visible		= const bool&
//		rightAxis	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateCurveProperties(const unsigned int &index, const Color &color,
	const bool &visible, const bool &rightAxis)
{
	unsigned long lineSize;
	long markerSize;
	optionsGrid->GetCellValue(index + 1, colLineSize).ToULong(&lineSize);
	optionsGrid->GetCellValue(index + 1, colMarkerSize).ToLong(&markerSize);
	plotArea->SetCurveProperties(index, color, visible, rightAxis, lineSize, markerSize);
}

//==========================================================================
// Class:			MainFrame
// Function:		GridLeftClickEvent
//
// Description:		Handles grid cell change events (for text controls).
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridCellChangeEvent(wxGridEvent &event)
{
	unsigned int row(event.GetRow());
	if (row == 0 || (event.GetCol() != colLineSize && event.GetCol() != colMarkerSize))
	{
		event.Skip();
		return;
	}

	UpdateCurveProperties(row - 1);
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlines
//
// Description:		Toggles gridlines for the entire plot on and off.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextToggleGridlines(wxCommandEvent& WXUNUSED(event))
{
	if (plotArea->GetGridOn())
		plotArea->SetGridOff();
	else
		plotArea->SetGridOn();

	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScale
//
// Description:		Autoscales the plot.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextAutoScale(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScale();
	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCursorValues
//
// Description:		Updates the values for the cursors and their differences
//					in the options grid.
//
// Input Arguments:
//		leftVisible		= const bool& indicating whether or not the left
//						  cursor is visible
//		rightVisible	= const bool& indicating whether or not the right
//						  cursor is visible
//		leftValue		= const double& giving the value of the left cursor
//		rightValue		= const double& giving the value of the right cursor
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue)
{
	if (optionsGrid == NULL)
		return;

	// TODO:  This would be nicer with smart precision so we show enough digits but not too many

	// For each curve, update the cursor values
	int i;
	for (i = 1; i < optionsGrid->GetRows(); i++)
	{
		UpdateSingleCursorValue(i, leftValue, colLeftCursor, leftVisible);
		UpdateSingleCursorValue(i, rightValue, colRightCursor, rightVisible);

		if (leftVisible && rightVisible)
		{
			double left(leftValue), right(rightValue);
			plotList[i - 1]->GetYAt(left);
			plotList[i - 1]->GetYAt(right);
			optionsGrid->SetCellValue(i, colDifference, wxString::Format("%f", right - left));
			optionsGrid->SetCellValue(0, colDifference, wxString::Format("%f", rightValue - leftValue));
		}
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateSingleCursorValue
//
// Description:		Updates a single cursor value.
//
// Input Arguments:
//		row		= const unsigned int& specifying the grid row
//		value	= const double& specifying the value to populate
//		column	= const unsigned int& specifying which grid column to populate
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateSingleCursorValue(const unsigned int &row,
	double value, const unsigned int &column, const bool &isVisible)
{
	if (isVisible)
	{
		optionsGrid->SetCellValue(0, column, wxString::Format("%f", value));

		if (plotList[row - 1]->GetYAt(value))
			optionsGrid->SetCellValue(row, column, _T("*") + wxString::Format("%f", value));
		else
			optionsGrid->SetCellValue(row, column, wxString::Format("%f", value));
	}
	else
	{
		optionsGrid->SetCellValue(0, column, wxEmptyString);
		optionsGrid->SetCellValue(row, column, wxEmptyString);

		// The difference column only exists if both cursors are visible
		optionsGrid->SetCellValue(0, colDifference, wxEmptyString);
		optionsGrid->SetCellValue(row, colDifference, wxEmptyString);
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		GetCurrentAxisRange
//
// Description:		Returns the range for the specified axis.
//
// Input Arguments:
//		axis	= const PlotContext&
//
// Output Arguments:
//		min		= double&
//		max		= double&
//
// Return Value:
//		bool, true on success, false otherwise
//
//==========================================================================
bool MainFrame::GetCurrentAxisRange(const PlotContext &axis, double &min, double &max) const
{
	switch (axis)
	{
	case plotContextXAxis:
		min = plotArea->GetXMin();
		max = plotArea->GetXMax();
		break;

	case plotContextLeftYAxis:
		min = plotArea->GetLeftYMin();
		max = plotArea->GetLeftYMax();
		break;

	case plotContextRightYAxis:
		min = plotArea->GetRightYMin();
		max = plotArea->GetRightYMax();
		break;

	default:
	case plotContextPlotArea:
		// Plot area is not a valid context in which we can set axis limits
		return false;
	}

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetNewAxisRange
//
// Description:		Returns the range for the specified axis.
//
// Input Arguments:
//		axis	= const PlotContext&
//		min		= const double&
//		max		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetNewAxisRange(const PlotContext &axis, const double &min, const double &max)
{
	switch (axis)
	{
	case plotContextLeftYAxis:
		plotArea->SetLeftYLimits(min, max);
		break;

	case plotContextRightYAxis:
		plotArea->SetRightYLimits(min, max);
		break;

	default:
	case plotContextXAxis:
		plotArea->SetXLimits(min, max);
	}

	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlinesBottom
//
// Description:		Toggles gridlines for the bottom axis.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextToggleGridlinesBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetBottomGrid(!plotArea->GetBottomGrid());
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScaleBottom
//
// Description:		Auto-scales the bottom axis.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextAutoScaleBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScaleBottom();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlinesLeft
//
// Description:		Toggles gridlines for the bottom axis.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextToggleGridlinesLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetLeftGrid(!plotArea->GetLeftGrid());
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScaleLeft
//
// Description:		Toggles gridlines for the bottom axis.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextAutoScaleLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScaleLeft();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlinesRight
//
// Description:		Toggles gridlines for the bottom axis.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextToggleGridlinesRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetRightGrid(!plotArea->GetRightGrid());
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScaleRight
//
// Description:		Toggles gridlines for the bottom axis.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextAutoScaleRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScaleRight();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextPlotBGColor
//
// Description:		Displays a dialog allowing the user to specify the plot's
//					background color.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextPlotBGColor(wxCommandEvent& WXUNUSED(event))
{
	wxColourData colorData;
	colorData.SetColour(plotArea->GetBackgroundColor().ToWxColor());

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
	dialog.SetTitle(_T("Choose Background Color"));
	if (dialog.ShowModal() == wxID_OK)
    {
		Color color;
		color.Set(dialog.GetColourData().GetColour());
		plotArea->SetBackgroundColor(color);
		plotArea->UpdateDisplay();
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextGridColor
//
// Description:		Dispalys a dialog box allowing the user to specify the
//					gridline color.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextGridColor(wxCommandEvent& WXUNUSED(event))
{
	wxColourData colorData;
	colorData.SetColour(plotArea->GetGridColor().ToWxColor());

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
	dialog.SetTitle(_T("Choose Background Color"));
	if (dialog.ShowModal() == wxID_OK)
    {
		Color color;
		color.Set(dialog.GetColourData().GetColour());
		plotArea->SetGridColor(color);
		plotArea->UpdateDisplay();
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetLogarithmicBottom
//
// Description:		Event handler for right Y-axis context menu Set Logarithmic event.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextSetLogarithmicBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetXLogarithmic(!plotArea->GetXLogarithmic());
	plotArea->ClearZoomStack();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetLogarithmicLeft
//
// Description:		Event handler for right Y-axis context menu Set Logarithmic event.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextSetLogarithmicLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetLeftLogarithmic(!plotArea->GetLeftLogarithmic());
	plotArea->ClearZoomStack();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetLogarithmicRight
//
// Description:		Event handler for right Y-axis context menu Set Logarithmic event.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextSetLogarithmicRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetRightLogarithmic(!plotArea->GetRightLogarithmic());
	plotArea->ClearZoomStack();
}

//==========================================================================
// Class:			MainFrame
// Function:		SetMarkerSize
//
// Description:		Sets the marker size for the specified curve.
//
// Input Arguments:
//		curve	= const unsigned int&
//		size	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetMarkerSize(const unsigned int &curve, const int &size)
{
	optionsGrid->SetCellValue(curve + 1, colMarkerSize, wxString::Format("%i", size));
	UpdateCurveProperties(curve);
}
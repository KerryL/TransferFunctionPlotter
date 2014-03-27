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
#include "application/tfDialog.h"
#include "renderer/plotRenderer.h"
#include "renderer/color.h"
#include "utilities/dataset2D.h"
#include "utilities/math/plotMath.h"

// *nix Icons
#ifdef __WXGTK__
#include "res/tfPlotter16.xpm"
#include "res/tfPlotter24.xpm"
#include "res/tfPlotter32.xpm"
#include "res/tfPlotter48.xpm"
#include "res/tfPlotter64.xpm"
#include "res/tfPlotter128.xpm"
#include "res/tfPlotter256.xpm"
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
	wxSplitterWindow *mainSplitter = new wxSplitterWindow(this);
	topSizer->Add(mainSplitter, 1, wxGROW);

	wxPanel *lowerPanel = new wxPanel(mainSplitter);
	wxBoxSizer *lowerSizer = new wxBoxSizer(wxHORIZONTAL);
	lowerSizer->Add(CreateButtons(lowerPanel), 0, wxALL, 5);
	lowerSizer->Add(CreateOptionsGrid(lowerPanel), 1, wxGROW | wxALL, 5);
	lowerPanel->SetSizer(lowerSizer);

	wxWindow *upperPanel = new wxPanel(mainSplitter);
	selectedAmplitudePlot = CreatePlotArea(upperPanel, _T("Amplitude"), _T("Amplitude [dB]"));
	selectedPhasePlot = CreatePlotArea(upperPanel, _T("Phase"), _T("Phase [deg]"));
	totalAmplitudePlot = CreatePlotArea(upperPanel, _T("Amplitude (Total)"), _T("Amplitude [dB]"));
	totalPhasePlot = CreatePlotArea(upperPanel, _T("Phase (Total)"), _T("Phase [deg]"));

	wxGridSizer *upperSizer = new wxGridSizer(2,0,0);
	upperSizer->Add(selectedAmplitudePlot, 1, wxGROW);
	upperSizer->Add(totalAmplitudePlot, 1, wxGROW);
	upperSizer->Add(selectedPhasePlot, 1, wxGROW);
	upperSizer->Add(totalPhasePlot, 1, wxGROW);
	upperPanel->SetSizer(upperSizer);

	SetXLabels();

	mainSplitter->SplitHorizontally(upperPanel, lowerPanel, 500);
	mainSplitter->SetSize(GetClientSize());
	mainSplitter->SetSashGravity(1.0);
	mainSplitter->SetMinimumPaneSize(255);

	SetSizerAndFit(topSizer);
	mainSplitter->SetSashPosition(mainSplitter->GetSashPosition(), false);
}

//==========================================================================
// Class:			MainFrame
// Function:		CreatePlotArea
//
// Description:		Creates the main plot control.
//
// Input Arguments:
//		parent	= wxWindow*
//		title	= const wxString&
//		yLabel	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotRenderer* pointing to plotArea
//
//==========================================================================
PlotRenderer* MainFrame::CreatePlotArea(wxWindow *parent, const wxString &title, const wxString &yLabel)
{
#ifdef __WXGTK__
	// Under GTK, we get a segmentation fault or X error on call to SwapBuffers in RenderWindow.
	// Adding the double-buffer arugment fixes this.  Under windows, the double-buffer argument
	// causes the colors to go funky.  So we have this #if.
	int args[] = {WX_GL_DOUBLEBUFFER, 0};
	PlotRenderer *plotArea = new PlotRenderer(parent, wxID_ANY, args, *this);
#else
	PlotRenderer *plotArea = new PlotRenderer(parent, wxID_ANY, NULL, *this);
#endif

	plotArea->SetMinSize(wxSize(650, 320));
	plotArea->SetGridOn();
	plotArea->SetXLogarithmic(true);
	plotArea->SetTitle(title);
	plotArea->SetLeftYLabel(yLabel);

	return plotArea;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateButtons
//
// Description:		Creates buttons to add/remove transfer functions.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer*
//
//==========================================================================
wxSizer* MainFrame::CreateButtons(wxWindow *parent)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	wxButton *add = new wxButton(parent, idAddButton, _T("Add"));
	wxButton *remove = new wxButton(parent, idRemoveButton, _T("Remove"));
	wxButton *removeAll = new wxButton(parent, idRemoveAllButton, _T("Remove All"));

	sizer->Add(add, 1, wxGROW | wxALL, 2);
	sizer->Add(remove, 1, wxGROW | wxALL, 2);
	sizer->Add(removeAll, 1, wxGROW | wxALL, 2);

	sizer->AddSpacer(10);
	sizer->Add(CreateInputControls(parent));

	wxStaticText *version = new wxStaticText(parent, wxID_ANY, TransferFunctionPlotterApp::versionString);
	sizer->Add(version, 0, wxALL, 2);

	return sizer;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateInputControls
//
// Description:		Creates additional input controls.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer*
//
//==========================================================================
wxSizer* MainFrame::CreateInputControls(wxWindow *parent)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText *freqUnitsLabel = new wxStaticText(parent, wxID_ANY, _T("Frequency Units:"));
	frequencyUnitsHertzRadioButton = new wxRadioButton(parent, wxID_ANY, _T("Hz"));
	wxRadioButton *frequencyUnitsRadRadioButton = new wxRadioButton(parent, wxID_ANY, _T("rad/sec"));
	frequencyUnitsHertzRadioButton->SetValue(true);

	sizer->Add(freqUnitsLabel, 0, wxALL, 2);
	sizer->Add(frequencyUnitsHertzRadioButton, 0, wxALL, 2);
	sizer->Add(frequencyUnitsRadRadioButton, 0, wxALL, 2);
	sizer->AddSpacer(10);

	double minFreq(0.01), maxFreq(100.0);
	wxStaticText *minFreqLabel = new wxStaticText(parent, wxID_ANY, _T("Min. Frequency"));
	minFrequencyTextBox = new wxTextCtrl(parent, wxID_ANY, wxString::Format("%0.2f", minFreq));
	wxStaticText *maxFreqLabel = new wxStaticText(parent, wxID_ANY, _T("Max. Frequency"));
	maxFrequencyTextBox = new wxTextCtrl(parent, wxID_ANY, wxString::Format("%0.1f", maxFreq));
	dataManager.SetFrequencyRange(minFreq, maxFreq);

	minFrequencyTextBox->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(MainFrame::TextBoxChangeEvent), NULL, this);
	maxFrequencyTextBox->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(MainFrame::TextBoxChangeEvent), NULL, this);

	wxFlexGridSizer *gridSizer = new wxFlexGridSizer(2, 5, 5);
	gridSizer->Add(minFreqLabel);
	gridSizer->Add(minFrequencyTextBox);
	gridSizer->Add(maxFreqLabel);
	gridSizer->Add(maxFrequencyTextBox);

	sizer->Add(gridSizer, 0, wxALL, 2);

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
	optionsGrid->SetColFormatFloat(colLeftCursor);
	optionsGrid->SetColFormatFloat(colRightCursor);
	optionsGrid->SetColFormatFloat(colDifference);
	optionsGrid->SetColFormatBool(colVisible);
	optionsGrid->SetColFormatBool(colRightAxis);

	optionsGrid->SetColLabelValue(colName, _T("Curve"));
	optionsGrid->SetColLabelValue(colColor, _T("Color"));
	optionsGrid->SetColLabelValue(colLineSize, _T("Line"));
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
	SetIcon(wxIcon(tfPlotter16_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter24_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter32_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter48_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter64_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter128_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(tfPlotter256_xpm, wxBITMAP_TYPE_XPM));
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
	EVT_BUTTON(idRemoveButton,						MainFrame::RemoveButtonClicked)
	EVT_BUTTON(idRemoveAllButton,					MainFrame::RemoveAllButtonClicked)

	EVT_RADIOBUTTON(wxID_ANY,						MainFrame::RadioButtonChangeEvent)

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
	EVT_MENU(idPlotContextAutoScaleBottom,			MainFrame::ContextAutoScaleBottom)
	EVT_MENU(idPlotContextToggleBottomMinorGridlines,	MainFrame::ContextToggleMinorGridlinesBottom)

	EVT_MENU(idPlotContextToggleLeftGridlines,		MainFrame::ContextToggleGridlinesLeft)
	EVT_MENU(idPlotContextAutoScaleLeft,			MainFrame::ContextAutoScaleLeft)

	EVT_MENU(idPlotContextToggleRightGridlines,		MainFrame::ContextToggleGridlinesRight)
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

	//plotArea->WriteImageToFile(pathAndFileName[0]);// FIXME:  Re-implement
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

//==========================================================================
// Class:			MainFrame
// Function:		AddButtonClicked
//
// Description:		Displays the add transfer function dialog.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddButtonClicked(wxCommandEvent& WXUNUSED(event))
{
	TFDialog dialog(this);
	if (dialog.ShowModal() != wxID_OK)
		return;

	AddCurve(dialog.GetNumerator(), dialog.GetDenominator());
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveButtonClicked
//
// Description:		Removes selected plot.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RemoveButtonClicked(wxCommandEvent& WXUNUSED(event))
{
	wxArrayInt selection = optionsGrid->GetSelectedRows();
	unsigned int i;
	for (i = 0; i < selection.Count(); i++)
	{
		if (selection[i] == 0)
			continue;
		RemoveCurve(selection[i] - 1);
	}

	UpdatePlots();
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveAllButtonClicked
//
// Description:		Removes all plots.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RemoveAllButtonClicked(wxCommandEvent& WXUNUSED(event))
{
	ClearAllCurves();
	UpdatePlots();
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
		contextMenu->Insert(1, idPlotContextToggleBottomMinorGridlines, _T("Toggle Minor Gridlines"));
		break;

	case plotContextLeftYAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextToggleLeftGridlines);
		break;

	case plotContextRightYAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextToggleRightGridlines);
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
	unsigned int i;
	for (i = dataManager.GetCount(); i > 0; i--)
		RemoveCurve(i - 1);

	UpdatePlots();
}

//==========================================================================
// Class:			MainFrame
// Function:		AddCurve
//
// Description:		Adds an new dataset to the plot.
//
// Input Arguments:
//		numerator	= wxString
//		denominator	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddCurve(wxString numerator, wxString denominator)
{
	if (!dataManager.AddTransferFunction(numerator, denominator))
		return;

	optionsGrid->BeginBatch();
	if (optionsGrid->GetNumberRows() == 0)
		AddXRowToGrid();
	unsigned int index = AddDataRowToGrid("(" + numerator + ")/(" + denominator + ")");
	optionsGrid->EndBatch();

	if (totalAmplitudePlot->GetCurveCount() == 0)
	{
		totalAmplitudePlot->AddCurve(*dataManager.GetTotalAmplitudeData());
		totalPhasePlot->AddCurve(*dataManager.GetTotalPhaseData());
	}

	UpdateSelectedTransferFunction(index - 1);
	UpdateCurveProperties(index - 1, GetNextColor(index), true, false);
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCurve
//
// Description:		Updates an existing dataset.
//
// Input Arguments:
//		i			= unsigned int
//		numerator	= wxString
//		denominator	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateCurve(unsigned int i, wxString numerator, wxString denominator)
{
	if (!dataManager.UpdateTransferFunction(i, numerator, denominator))
		return;

	optionsGrid->SetCellValue("(" + numerator + ")/(" + denominator + ")", i + 1, 0);
	UpdatePlots();
}

void MainFrame::UpdateSelectedTransferFunction(const unsigned int &i)
{
	//selectedAmplitudePlot->RemoveAllCurves();
	//selectedPhasePlot->RemoveAllCurves();
	selectedAmplitudePlot->AddCurve(*dataManager.GetAmplitudeData(i));
	selectedPhasePlot->AddCurve(*dataManager.GetPhaseData(i));
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
void MainFrame::AddXRowToGrid(void)
{
	optionsGrid->AppendRows();

	unsigned int i;
	for (i = 0; i < colCount; i++)
		optionsGrid->SetReadOnly(0, i, true);

	optionsGrid->SetCellValue(0, 0, xLabel);
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

	optionsGrid->SetCellRenderer(index, colVisible, new wxGridCellBoolRenderer);
	optionsGrid->SetCellRenderer(index, colRightAxis, new wxGridCellBoolRenderer);
	optionsGrid->SetCellEditor(index, colLineSize, new wxGridCellNumberEditor(1, maxLineSize));

	unsigned int i;
	for (i = 0; i < colCount; i++)
			optionsGrid->SetReadOnly(index, i, true);
	optionsGrid->SetReadOnly(index, colLineSize, false);
	optionsGrid->SetCellValue(index, colName, name);

	Color color = GetNextColor(index);

	optionsGrid->SetCellBackgroundColour(index, colColor, color.ToWxColor());
	optionsGrid->SetCellValue(index, colLineSize, _T("1"));
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

	dataManager.RemoveTransferFunctions(i);
	if (dataManager.GetCount() == 0)
	{
		selectedAmplitudePlot->RemoveAllCurves();
		selectedPhasePlot->RemoveAllCurves();
		totalAmplitudePlot->RemoveAllCurves();
		totalPhasePlot->RemoveAllCurves();
	}
	else
	{
		selectedAmplitudePlot->RemoveCurve(i);
		selectedPhasePlot->RemoveCurve(i);
	}
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

	if (event.GetCol() == colName)
		UpdateCurve(row - 1);
	
	if (event.GetCol() != colColor)
		return;

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

void MainFrame::UpdateCurve(unsigned int i)
{
	TFDialog dialog(this, dataManager.GetNumerator(i), dataManager.GetDenominator(i));
	if (dialog.ShowModal() != wxID_OK)
		return;

	UpdateCurve(i, dialog.GetNumerator(), dialog.GetDenominator());
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
	optionsGrid->GetCellValue(index + 1, colLineSize).ToULong(&lineSize);
	selectedAmplitudePlot->SetCurveProperties(index, color, visible, rightAxis, lineSize, -1);
	selectedPhasePlot->SetCurveProperties(index, color, visible, rightAxis, lineSize, -1);

	totalAmplitudePlot->SetCurveProperties(0, Color::ColorBlue, true, false, 1, 0);
	totalPhasePlot->SetCurveProperties(0, Color::ColorBlue, true, false, 1, 0);
}

//==========================================================================
// Class:			MainFrame
// Function:		GridCellChangeEvent
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
	if (row == 0 || event.GetCol() != colLineSize)
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
	if (selectedAmplitudePlot->GetGridOn())
	{
		selectedAmplitudePlot->SetGridOff();
		selectedPhasePlot->SetGridOff();
		totalAmplitudePlot->SetGridOff();
		totalPhasePlot->SetGridOff();
	}
	else
	{
		selectedAmplitudePlot->SetGridOn();
		selectedPhasePlot->SetGridOn();
		totalAmplitudePlot->SetGridOn();
		totalPhasePlot->SetGridOn();
	}

	UpdatePlots();
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
	selectedAmplitudePlot->AutoScale();
	selectedPhasePlot->AutoScale();
	totalAmplitudePlot->AutoScale();
	totalPhasePlot->AutoScale();

	UpdatePlots();
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
	/*int i;
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
	}*/
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
	/*if (isVisible)
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
	}*/
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
		min = selectedAmplitudePlot->GetXMin();
		max = selectedAmplitudePlot->GetXMax();
		break;

	case plotContextLeftYAxis:
		min = selectedAmplitudePlot->GetLeftYMin();
		max = selectedAmplitudePlot->GetLeftYMax();
		break;

	case plotContextRightYAxis:
		min = selectedAmplitudePlot->GetRightYMin();
		max = selectedAmplitudePlot->GetRightYMax();
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
		selectedAmplitudePlot->SetLeftYLimits(min, max);
		selectedPhasePlot->SetLeftYLimits(min, max);
		totalAmplitudePlot->SetLeftYLimits(min, max);
		totalPhasePlot->SetLeftYLimits(min, max);
		break;

	case plotContextRightYAxis:
		selectedAmplitudePlot->SetRightYLimits(min, max);
		selectedPhasePlot->SetRightYLimits(min, max);
		totalAmplitudePlot->SetRightYLimits(min, max);
		totalPhasePlot->SetRightYLimits(min, max);
		break;

	default:
	case plotContextXAxis:
		selectedAmplitudePlot->SetXLimits(min, max);
		selectedPhasePlot->SetXLimits(min, max);
		totalAmplitudePlot->SetXLimits(min, max);
		totalPhasePlot->SetXLimits(min, max);
	}

	UpdatePlots();
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
	selectedAmplitudePlot->SetBottomGrid(!selectedAmplitudePlot->GetBottomGrid());
	selectedPhasePlot->SetBottomGrid(!selectedPhasePlot->GetBottomGrid());
	totalAmplitudePlot->SetBottomGrid(!totalAmplitudePlot->GetBottomGrid());
	totalPhasePlot->SetBottomGrid(!totalPhasePlot->GetBottomGrid());
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleMinorGridlinesBottom
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
void MainFrame::ContextToggleMinorGridlinesBottom(wxCommandEvent& WXUNUSED(event))
{
	selectedAmplitudePlot->SetBottomMinorGrid(!selectedAmplitudePlot->GetBottomMinorGrid());
	selectedPhasePlot->SetBottomMinorGrid(!selectedPhasePlot->GetBottomMinorGrid());
	totalAmplitudePlot->SetBottomMinorGrid(!totalAmplitudePlot->GetBottomMinorGrid());
	totalPhasePlot->SetBottomMinorGrid(!totalPhasePlot->GetBottomMinorGrid());
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
	selectedAmplitudePlot->AutoScaleBottom();
	selectedPhasePlot->AutoScaleBottom();
	totalAmplitudePlot->AutoScaleBottom();
	totalPhasePlot->AutoScaleBottom();
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
	selectedAmplitudePlot->SetLeftGrid(!selectedAmplitudePlot->GetLeftGrid());
	selectedPhasePlot->SetLeftGrid(!selectedPhasePlot->GetLeftGrid());
	totalAmplitudePlot->SetLeftGrid(!totalAmplitudePlot->GetLeftGrid());
	totalPhasePlot->SetLeftGrid(!totalPhasePlot->GetLeftGrid());
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
	selectedAmplitudePlot->AutoScaleLeft();
	selectedPhasePlot->AutoScaleLeft();
	totalAmplitudePlot->AutoScaleLeft();
	totalPhasePlot->AutoScaleLeft();
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
	selectedAmplitudePlot->SetRightGrid(!selectedAmplitudePlot->GetRightGrid());
	selectedPhasePlot->SetRightGrid(!selectedPhasePlot->GetRightGrid());
	totalAmplitudePlot->SetRightGrid(!totalAmplitudePlot->GetRightGrid());
	totalPhasePlot->SetRightGrid(!totalPhasePlot->GetRightGrid());
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
	selectedAmplitudePlot->AutoScaleRight();
	selectedPhasePlot->AutoScaleRight();
	totalAmplitudePlot->AutoScaleRight();
	totalPhasePlot->AutoScaleRight();
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
	colorData.SetColour(selectedAmplitudePlot->GetBackgroundColor().ToWxColor());

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
	dialog.SetTitle(_T("Choose Background Color"));
	if (dialog.ShowModal() == wxID_OK)
    {
		Color color;
		color.Set(dialog.GetColourData().GetColour());

		selectedAmplitudePlot->SetBackgroundColor(color);
		selectedPhasePlot->SetBackgroundColor(color);
		totalAmplitudePlot->SetBackgroundColor(color);
		totalPhasePlot->SetBackgroundColor(color);

		UpdatePlots();
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
	colorData.SetColour(selectedAmplitudePlot->GetGridColor().ToWxColor());

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
	dialog.SetTitle(_T("Choose Background Color"));
	if (dialog.ShowModal() == wxID_OK)
    {
		Color color;
		color.Set(dialog.GetColourData().GetColour());

		selectedAmplitudePlot->SetGridColor(color);
		selectedPhasePlot->SetGridColor(color);
		totalAmplitudePlot->SetGridColor(color);
		totalPhasePlot->SetGridColor(color);

		UpdatePlots();
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		SetXLabels
//
// Description:		Sets the x-label for all four plots depending on selected
//					frequency units.
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
void MainFrame::SetXLabels(void)
{
	if (frequencyUnitsHertzRadioButton->GetValue())
		xLabel = _T("Frequency [Hz]");
	else
		xLabel = _T("Frequency [rad/sec]");

	selectedAmplitudePlot->SetXLabel(xLabel);
	selectedPhasePlot->SetXLabel(xLabel);
	totalAmplitudePlot->SetXLabel(xLabel);
	totalPhasePlot->SetXLabel(xLabel);
}

//==========================================================================
// Class:			MainFrame
// Function:		TextBoxChangeEvent
//
// Description:		Event handler for text box change (really loss of focus)
//					events.
//
// Input Arguments:
//		event	= wxFocusEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::TextBoxChangeEvent(wxFocusEvent& event)
{
	double min, max;
	if (minFrequencyTextBox->GetValue().ToDouble(&min) &&
		maxFrequencyTextBox->GetValue().ToDouble(&max))
		dataManager.SetFrequencyRange(min, max);
	UpdatePlots();
	event.Skip();// Without skipping the event, the cursot gets stuck in the box and we never loose focus
}

//==========================================================================
// Class:			MainFrame
// Function:		RadioButtonChangeEvent
//
// Description:		Event handler for radio button change events.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RadioButtonChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	SetXLabels();
	if (frequencyUnitsHertzRadioButton->GetValue())
		dataManager.SetFrequencyUnitsHertz();
	else
		dataManager.SetFrequencyUnitsRadPerSec();
	UpdatePlots();
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdatePlots
//
// Description:		Updates all rendered plots.
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
void MainFrame::UpdatePlots(void)
{
	selectedAmplitudePlot->UpdateDisplay();
	selectedPhasePlot->UpdateDisplay();
	totalAmplitudePlot->UpdateDisplay();
	totalPhasePlot->UpdateDisplay();
}

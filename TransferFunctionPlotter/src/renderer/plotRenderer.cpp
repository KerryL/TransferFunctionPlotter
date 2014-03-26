/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotRenderer.cpp
// Created:  5/4/2011
// Author:  K. Loux
// Description:  Derived from RenderWindow, this class is used to display plots on
//				 the screen.
// History:

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "renderer/plotRenderer.h"
#include "application/plotObject.h"
#include "application/mainFrame.h"
#include "renderer/primitives/zoomBox.h"
#include "renderer/primitives/plotCursor.h"
#include "renderer/primitives/axis.h"

//==========================================================================
// Class:			PlotRenderer
// Function:		PlotRenderer
//
// Description:		Constructor for PlotRenderer class.
//
// Input Arguments:
//		parent		= wxWindow* pointing to this object's parent window
//		id			= wxWindowID
//		args		= int[] NOTE: Under GTK, must contain WX_GL_DOUBLEBUFFER at minimum
//		_mainFrame	= MainFrame& reference to this applications's main window
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotRenderer::PlotRenderer(wxWindow *parent, wxWindowID id, int args[], MainFrame &_mainFrame)
							 : RenderWindow(*parent, id, args, wxDefaultPosition,
							 wxDefaultSize), mainFrame(_mainFrame)
{
	CreateActors();

	SetView3D(false);

	draggingLeftCursor = false;
	draggingRightCursor = false;

	ignoreNextMouseMove = false;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		~PlotRenderer
//
// Description:		Destructor for PlotRenderer class.
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
PlotRenderer::~PlotRenderer()
{
	delete plot;
	plot = NULL;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		Event Tables
//
// Description:		Event table for the PlotRenderer class.
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
BEGIN_EVENT_TABLE(PlotRenderer, RenderWindow)
	EVT_SIZE(PlotRenderer::OnSize)

	// Interaction events
	EVT_MOUSEWHEEL(		PlotRenderer::OnMouseWheelEvent)
	EVT_MOTION(			PlotRenderer::OnMouseMoveEvent)

	EVT_LEAVE_WINDOW(	PlotRenderer::OnMouseLeaveWindowEvent)

	// Click events
	EVT_LEFT_DCLICK(	PlotRenderer::OnDoubleClickEvent)
	EVT_RIGHT_UP(		PlotRenderer::OnRightButtonUpEvent)
	EVT_LEFT_UP(		PlotRenderer::OnLeftButtonUpEvent)
	EVT_LEFT_DOWN(		PlotRenderer::OnLeftButtonDownEvent)
	EVT_MIDDLE_UP(		PlotRenderer::OnMiddleButtonUpEvent)
END_EVENT_TABLE()

//==========================================================================
// Class:			PlotRenderer
// Function:		UpdateDisplay
//
// Description:		Updates the displayed plots to match the current data.
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
void PlotRenderer::UpdateDisplay(void)
{
	plot->Update();
	Refresh();
}

//==========================================================================
// Class:			PLOT_RENDERER
// Function:		CreateActors
//
// Description:		Creates the actors for this plot.
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
void PlotRenderer::CreateActors(void)
{
	// Create plot area
	plot = new PlotObject(*this);

	// Also create the zoom box and cursors, even though they aren't drawn yet
	zoomBox = new ZoomBox(*this);
	leftCursor = new PlotCursor(*this, *plot->GetBottomAxis());
	rightCursor = new PlotCursor(*this, *plot->GetBottomAxis());
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnSize
//
// Description:		Handles EVT_SIZE events for this class.  Required to make
//					the plot size update with the window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		event	= wxSizeEvent&
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnSize(wxSizeEvent &event)
{
	ignoreNextMouseMove = true;

	if (leftCursor->GetIsVisible())
		leftCursor->SetVisibility(true);
	if (rightCursor->GetIsVisible())
		rightCursor->SetVisibility(true);

	UpdateDisplay();

	// Skip this event so the base class OnSize event fires, too
	event.Skip();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnMouseWheelEvent
//
// Description:		Event handler for the mouse wheel event.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnMouseWheelEvent(wxMouseEvent &event)
{
	if (view3D)
	{
		event.Skip();
		return;
	}

	// ZOOM in or out
	double zoomScaleX = 0.05;// [% of current scale]
	double zoomScaleY = 0.05;// [% of current scale]

	// If the CTRL key is down (and not SHIFT), only scale the X-axis
	if (event.ControlDown() && !event.ShiftDown())
		zoomScaleY = 0.0;

	// If the SHIFT key is down (and not CTRL), only scale the Y-axis
	else if (event.ShiftDown() && !event.ControlDown())
		zoomScaleX = 0.0;

	// TODO:  Focus the zooming around the cursor
	// Adjust the axis limits to achieve zooming
	double xDelta = (plot->GetXMax() - plot->GetXMin()) * zoomScaleX * event.GetWheelRotation() / 120.0;
	double yLeftDelta = (plot->GetLeftYMax() - plot->GetLeftYMin()) * zoomScaleY * event.GetWheelRotation() / 120.0;
	double yRightDelta = (plot->GetLeftYMax() - plot->GetLeftYMin()) * zoomScaleY * event.GetWheelRotation() / 120.0;

	plot->SetXMin(plot->GetXMin() + xDelta);
	plot->SetXMax(plot->GetXMax() - xDelta);
	plot->SetLeftYMin(plot->GetLeftYMin() + yLeftDelta);
	plot->SetLeftYMax(plot->GetLeftYMax() - yLeftDelta);
	plot->SetRightYMin(plot->GetRightYMin() + yRightDelta);
	plot->SetRightYMax(plot->GetRightYMax() - yRightDelta);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnMouseMoveEvent
//
// Description:		Event handler for the mouse move event.  Only used to
//					capture drag events for rotating, panning, or dollying
//					the scene.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnMouseMoveEvent(wxMouseEvent &event)
{
	if (view3D)
	{
		event.Skip();
		return;
	}

	if (!event.Dragging() || ignoreNextMouseMove)// ignoreNextMouseMove prevents panning on maximize by double clicking title bar
	{
		ignoreNextMouseMove = false;
		StoreMousePosition(event);
		return;
	}

	if (draggingLeftCursor)
		leftCursor->SetLocation(event.GetX());
	else if (draggingRightCursor)
		rightCursor->SetLocation(event.GetX());
	// ZOOM:  Left or Right mouse button + CTRL or SHIFT
	else if ((event.ControlDown() || event.ShiftDown()) && (event.RightIsDown() || event.LeftIsDown()))
		ProcessZoom(event);
	// ZOOM WITH BOX: Right mouse button
	else if (event.RightIsDown())
		ProcessZoomWithBox(event);
	// PAN:  Left mouse button (includes with any buttons not caught above)
	else if (event.LeftIsDown())
		ProcessPan(event);
	else// Not recognized
	{
		StoreMousePosition(event);
		return;
	}

	StoreMousePosition(event);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnRightButtonUpEvent
//
// Description:		Handles end of zoom-by-box events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnRightButtonUpEvent(wxMouseEvent &event)
{
	if (!zoomBox->GetIsVisible())
	{
		ProcessRightClick(event);
		return;
	}

	ProcessZoomBoxEnd();
	SaveCurrentZoom();
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnMiddleButtonUpEvent
//
// Description:		Undoes the zoom level (if available).
//
// Input Arguments:
//		event	= wxMouseEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnMiddleButtonUpEvent(wxMouseEvent& WXUNUSED(event))
{
	UndoZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ZoomChanged
//
// Description:		Determines if the zoom level is different from the previous
//					zoom level.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool PlotRenderer::ZoomChanged(void) const
{
	if (zoom.size() == 0)
		return true;

	Zoom lastZoom = zoom.top();

	if (lastZoom.xMin == plot->GetXMin() &&
		lastZoom.xMax == plot->GetXMax() &&
		lastZoom.leftYMin == plot->GetLeftYMin() &&
		lastZoom.leftYMax == plot->GetLeftYMax() &&
		lastZoom.rightYMin == plot->GetRightYMin() &&
		lastZoom.rightYMax == plot->GetRightYMax())
		return false;

	return true;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SaveCurrentZoom
//
// Description:		Saves the current zoom level.
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
void PlotRenderer::SaveCurrentZoom(void)
{
	if (!ZoomChanged())
		return;

	Zoom currentZoom;

	currentZoom.xMin = plot->GetXMin();
	currentZoom.xMax = plot->GetXMax();
	currentZoom.leftYMin = plot->GetLeftYMin();
	currentZoom.leftYMax = plot->GetLeftYMax();
	currentZoom.rightYMin = plot->GetRightYMin();
	currentZoom.rightYMax = plot->GetRightYMax();

	zoom.push(currentZoom);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		UndoZoom
//
// Description:		Reverts to the previous zoom level.
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
void PlotRenderer::UndoZoom(void)
{
	if (zoom.size() < 2)
		return;

	zoom.pop();// Pop the current zoom in order to read the previous zoom
	Zoom lastZoom = zoom.top();

	plot->SetXMin(lastZoom.xMin);
	plot->SetXMax(lastZoom.xMax);
	plot->SetLeftYMin(lastZoom.leftYMin);
	plot->SetLeftYMax(lastZoom.leftYMax);
	plot->SetRightYMin(lastZoom.rightYMin);
	plot->SetRightYMax(lastZoom.rightYMax);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ClearZoomStack
//
// Description:		Empties the zoom stack.
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
void PlotRenderer::ClearZoomStack(void)
{
	while (zoom.size() > 0)
		zoom.pop();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetGridOn
//
// Description:		Returns status of the grid lines.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for visible, false for hidden
//
//==========================================================================
bool PlotRenderer::GetGridOn(void)
{
	return plot->GetGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetGridOn
//
// Description:		Turns on plot grid.
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
void PlotRenderer::SetGridOn()
{
	plot->SetGrid(true);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetGridOn
//
// Description:		Turns off plot grid.
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
void PlotRenderer::SetGridOff()
{
	plot->SetGrid(false);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetBottomGrid
//
// Description:		Returns the status of the bottom grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of bottom grid
//
//==========================================================================
bool PlotRenderer::GetBottomGrid(void) const
{
	return plot->GetBottomAxis()->GetGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetBottomMinorGrid
//
// Description:		Returns the status of the bottom minor grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of bottom grid
//
//==========================================================================
bool PlotRenderer::GetBottomMinorGrid(void) const
{
	return plot->GetBottomAxis()->GetMinorGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetLeftGrid
//
// Description:		Returns the status of the left grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of left grid
//
//==========================================================================
bool PlotRenderer::GetLeftGrid(void) const
{
	return plot->GetLeftYAxis()->GetGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetRightGrid
//
// Description:		Returns the status of the right grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of right grid
//
//==========================================================================
bool PlotRenderer::GetRightGrid(void) const
{
	return plot->GetRightYAxis()->GetGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetBottomGrid
//
// Description:		Sets the status of the bottom axis' grid.
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
void PlotRenderer::SetBottomGrid(const bool &grid)
{
	plot->SetXGrid(grid);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetBottomMinorGrid
//
// Description:		Sets the status of the bottom axis' minor grid.
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
void PlotRenderer::SetBottomMinorGrid(const bool &grid)
{
	plot->SetXMinorGrid(grid);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLeftGrid
//
// Description:		Sets the status of the left axis' grid.
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
void PlotRenderer::SetLeftGrid(const bool &grid)
{
	plot->SetLeftYGrid(grid);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetRightGrid
//
// Description:		Sets the status of the right axis' grid.
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
void PlotRenderer::SetRightGrid(const bool &grid)
{
	plot->SetRightYGrid(grid);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetCurveProperties
//
// Description:		Sets properties for the specified curve object.
//
// Input Arguments:
//		index		= const unsigned int& specifying the curve
//		color		= const Color& of the curve
//		visible		= const bool& indiciating whether or not the curve is to
//					  be drawn
//		rightAxis	= const bool& indicating whether the curve should be tied
//					  to the left or right axis
//		lineSize	= const unsigned int&
//		markerSize	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetCurveProperties(const unsigned int &index, const Color &color,
									  const bool &visible, const bool &rightAxis,
									  const unsigned int &lineSize, const int &markerSize)
{
	plot->SetCurveProperties(index, color, visible, rightAxis, lineSize, markerSize);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetXLimits
//
// Description:		Sets the axis limits for the X axis.
//
// Input Arguments:
//		min	= const double&
//		max = const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetXLimits(const double &min, const double &max)
{
	if (max > min)
	{
		plot->SetXMax(max);
		plot->SetXMin(min);
	}
	else
	{
		plot->SetXMax(min);
		plot->SetXMin(max);
	}

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLeftYLimits
//
// Description:		Sets the axis limits for the left Y axis.
//
// Input Arguments:
//		min	= const double&
//		max = const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetLeftYLimits(const double &min, const double &max)
{
	if (max > min)
	{
		plot->SetLeftYMax(max);
		plot->SetLeftYMin(min);
	}
	else
	{
		plot->SetLeftYMax(min);
		plot->SetLeftYMin(max);
	}

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetRightYLimits
//
// Description:		Sets the axis limits for the right Y axis.
//
// Input Arguments:
//		min	= const double&
//		max = const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetRightYLimits(const double &min, const double &max)
{
	if (max > min)
	{
		plot->SetRightYMax(max);
		plot->SetRightYMin(min);
	}
	else
	{
		plot->SetRightYMax(min);
		plot->SetRightYMin(max);
	}

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		AddCurve
//
// Description:		Sets properties for the specified curve object.
//
// Input Arguments:
//		data	= const Dataset2D& to be plotted
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::AddCurve(const Dataset2D &data)
{
	plot->AddCurve(data);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		RemoveAllCurves
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
void PlotRenderer::RemoveAllCurves()
{
	plot->RemoveExistingPlots();

	if (plot->GetCurveCount() == 0)
		ClearZoomStack();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		RemoveCurve
//
// Description:		Removes all curves from the plot.
//
// Input Arguments:
//		index	= const unsigned int& specifying the curve to be removed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::RemoveCurve(const unsigned int& index)
{
	plot->RemovePlot(index);

	if (plot->GetCurveCount() == 0)
		ClearZoomStack();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		AutoScale
//
// Description:		Enables auto-scaling of the axes.
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
void PlotRenderer::AutoScale()
{
	plot->ResetAutoScaling();
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		AutoScaleBottom
//
// Description:		Enables auto-scaling of the bottom axis.
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
void PlotRenderer::AutoScaleBottom()
{
	plot->SetAutoScaleBottom();
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		AutoScaleLeft
//
// Description:		Enables auto-scaling of the left axis.
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
void PlotRenderer::AutoScaleLeft()
{
	plot->SetAutoScaleLeft();
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		AutoScaleRight
//
// Description:		Enables auto-scaling of the right axis.
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
void PlotRenderer::AutoScaleRight()
{
	plot->SetAutoScaleRight();
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetXLabel
//
// Description:		Sets the text for the x-axis label.
//
// Input Arguments:
//		text	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetXLabel(wxString text)
{
	plot->SetXLabel(text);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLeftYLabel
//
// Description:		Sets the text for the y-axis label.
//
// Input Arguments:
//		text	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetLeftYLabel(wxString text)
{
	plot->SetLeftYLabel(text);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetRightYLabel
//
// Description:		Sets the text for the y-axis label.
//
// Input Arguments:
//		text	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetRightYLabel(wxString text)
{
	plot->SetRightYLabel(text);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetTitle
//
// Description:		Sets the plot title text.
//
// Input Arguments:
//		text	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetTitle(wxString text)
{
	plot->SetTitle(text);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnMouseLeaveWindowEvent
//
// Description:		Cleans up some zoom box and cursor items.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnMouseLeaveWindowEvent(wxMouseEvent& WXUNUSED(event))
{
	// Hide the zoom box (but only if it's not already hidden!)
	if (zoomBox->GetIsVisible())
		zoomBox->SetVisibility(false);

	draggingLeftCursor = false;
	draggingRightCursor = false;

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnDoubleClickEvent
//
// Description:		Handles double click events.  Allows user to change axis
//					limits or create a cursor.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnDoubleClickEvent(wxMouseEvent &event)
{
	unsigned int x = event.GetX();
	unsigned int y = event.GetY();

	// If the click is within the plot area, move a cursor there and make it visible
	if (x > plot->GetLeftYAxis()->GetOffsetFromWindowEdge() &&
		x < GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge() &&
		y > plot->GetTopAxis()->GetOffsetFromWindowEdge() &&
		y < GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		ProcessPlotAreaDoubleClick(x);
	else
		ProcessOffPlotDoubleClick(x, y);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnMouseLeftDownEvent
//
// Description:		Checks to see if the user is dragging a cursor.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnLeftButtonDownEvent(wxMouseEvent &event)
{
	// Check to see if we're on a cursor
	if (leftCursor->IsUnder(event.GetX()))
		draggingLeftCursor = true;
	else if (rightCursor->IsUnder(event.GetX()))
		draggingRightCursor = true;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnLeftButtonUpEvent
//
// Description:		Makes sure we stop dragging when we stop clicking.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnLeftButtonUpEvent(wxMouseEvent& WXUNUSED(event))
{
	draggingLeftCursor = false;
	draggingRightCursor = false;

	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetLeftCursorVisible
//
// Description:		Returns status of left cursor visibility flag.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of left cursor visibility flag
//
//==========================================================================
bool PlotRenderer::GetLeftCursorVisible(void) const
{
	return leftCursor->GetIsVisible();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetRightCursorVisible
//
// Description:		Returns status of right cursor visibility flag.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of right cursor visibility flag
//
//==========================================================================
bool PlotRenderer::GetRightCursorVisible(void) const
{
	return rightCursor->GetIsVisible();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetLeftCursorValue
//
// Description:		Returns x-value of left cursor.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotRenderer::GetLeftCursorValue(void) const
{
	return leftCursor->GetValue();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetRightCursorValue
//
// Description:		Makes sure we stop dragging when we stop clicking.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotRenderer::GetRightCursorValue(void) const
{
	return rightCursor->GetValue();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		UpdateCursors
//
// Description:		Updates the cursor calculations.
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
void PlotRenderer::UpdateCursors(void)
{
	// Tell the cursors they need to recalculate
	leftCursor->SetModified();
	rightCursor->SetModified();

	// Calculations are performed on Draw
	leftCursor->Draw();
	rightCursor->Draw();

	Refresh();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetXMin(void) const
{
	return plot->GetBottomAxis()->GetMinimum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetXMax(void) const
{
	return plot->GetBottomAxis()->GetMaximum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetLeftYMin(void) const
{
	return plot->GetLeftYAxis()->GetMinimum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetLeftYMax(void) const
{
	return plot->GetLeftYAxis()->GetMaximum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetRightYMin(void) const
{
	return plot->GetRightYAxis()->GetMinimum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetRightYMax(void) const
{
	return plot->GetRightYAxis()->GetMaximum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetGridColor
//
// Description:		Returns the color of the gridlines for this plot.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Color
//
//==========================================================================
Color PlotRenderer::GetGridColor(void) const
{
	return plot->GetGridColor();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetGridColor
//
// Description:		Sets the color of the gridlines for this plot.
//
// Input Arguments:
//		color	= const Color&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetGridColor(const Color &color)
{
	plot->SetGridColor(color);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXLogarithmic
//
// Description:		Returns a boolean indicating whether or not the X axis
//					is scaled logarithmicly.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, indicating whether or not the X axis has a logarithmic scale
//
//==========================================================================
bool PlotRenderer::GetXLogarithmic(void) const
{
	if (plot->GetBottomAxis())
		return plot->GetBottomAxis()->IsLogarithmic();

	return false;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetLeftLogarithmic
//
// Description:		Returns a boolean indicating whether or not the left Y axis
//					is scaled logarithmicly.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, indicating whether or not the left Y axis has a logarithmic scale
//
//==========================================================================
bool PlotRenderer::GetLeftLogarithmic(void) const
{
	if (plot->GetLeftYAxis())
		return plot->GetLeftYAxis()->IsLogarithmic();

	return false;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetRightLogarithmic
//
// Description:		Returns a boolean indicating whether or not the right Y axis
//					is scaled logarithmicly.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, indicating whether or not the right Y axis has a logarithmic scale
//
//==========================================================================
bool PlotRenderer::GetRightLogarithmic(void) const
{
	if (plot->GetRightYAxis())
		return plot->GetRightYAxis()->IsLogarithmic();

	return false;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetXLogarithmic
//
// Description:		Sets the X axis to be logarithmic or standard scaling.
//
// Input Arguments:
//		log	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetXLogarithmic(const bool &log)
{
	plot->SetXLogarithmic(log);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLeftLogarithmic
//
// Description:		Sets the left Y axis to be logarithmic or standard scaling.
//
// Input Arguments:
//		log	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetLeftLogarithmic(const bool &log)
{
	plot->SetLeftLogarithmic(log);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetRightLogarithmic
//
// Description:		Sets the right Y axis to be logarithmic or standard scaling.
//
// Input Arguments:
//		log	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetRightLogarithmic(const bool &log)
{
	plot->SetRightLogarithmic(log);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXAxisZoomed
//
// Description:		Determines if the x-axis is zoomed (not autoscaled) and
//					returns the result.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the user zoomed in (or panned) along the x-axis
//
//==========================================================================
bool PlotRenderer::GetXAxisZoomed(void) const
{
	if (!plot->GetBottomAxis())
		return false;

	return !plot->GetXAxisAutoScaled();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessZoom
//
// Description:		Handles mouse-drag zoom events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessZoom(wxMouseEvent &event)
{
	// CTRL for Left Y-zoom
	// SHIFT for Right Y-zoom

	// ZOOM in or out
	double zoomXScale = 0.005 * (event.GetX() - lastMousePosition[0]);// [% of current scale]
	double zoomYScale = 0.005 * (event.GetY() - lastMousePosition[1]);// [% of current scale]

	// TODO:  Focus the zooming around the cursor
	// Adjust the axis limits
	double xDelta = (plot->GetXMax() - plot->GetXMin()) * zoomXScale;
	double yLeftDelta = (plot->GetLeftYMax() - plot->GetLeftYMin()) * zoomYScale * (int)event.ControlDown();
	double yRightDelta = (plot->GetRightYMax() - plot->GetRightYMin()) * zoomYScale * (int)event.ShiftDown();

	// Left mouse button fixes left and bottom corner, right button fixes right and top corner
	if (event.LeftIsDown())
	{
		plot->SetXMax(plot->GetXMax() - xDelta);
		plot->SetLeftYMax(plot->GetLeftYMax() + yLeftDelta);
		plot->SetRightYMax(plot->GetRightYMax() + yRightDelta);
	}
	else
	{
		plot->SetXMin(plot->GetXMin() - xDelta);
		plot->SetLeftYMin(plot->GetLeftYMin() + yLeftDelta);
		plot->SetRightYMin(plot->GetRightYMin() + yRightDelta);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessZoomWithBox
//
// Description:		Handles mouse-drag zoom box events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessZoomWithBox(wxMouseEvent &event)
{
	if (!zoomBox->GetIsVisible())
	{
		zoomBox->SetVisibility(true);
		zoomBox->SetAnchorCorner(lastMousePosition[0], GetSize().GetHeight() - lastMousePosition[1]);
	}

	unsigned int x = event.GetX();
	unsigned int y = event.GetY();

	// Make sure we're still over the plot area - if we're not, draw the box as if we were
	if (x < plot->GetLeftYAxis()->GetOffsetFromWindowEdge())
		x = plot->GetLeftYAxis()->GetOffsetFromWindowEdge();
	else if (x > GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge())
		x = GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge();

	if (y < plot->GetTopAxis()->GetOffsetFromWindowEdge())
		y = plot->GetTopAxis()->GetOffsetFromWindowEdge();
	else if (y > GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		y = GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge();

	// Tell the zoom box where to draw the floaing corner
	zoomBox->SetFloatingCorner(x, GetSize().GetHeight() - y);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessPan
//
// Description:		Handles mouse-drag pan events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessPan(wxMouseEvent &event)
{
	PanBottomXAxis(event);
	PanLeftYAxis(event);
	PanRightYAxis(event);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		PanBottomXAxis
//
// Description:		Pans the bottom x-axis in resposne to mouse moves.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::PanBottomXAxis(wxMouseEvent &event)
{
	int width = GetSize().GetWidth() - plot->GetLeftYAxis()->GetOffsetFromWindowEdge() - plot->GetRightYAxis()->GetOffsetFromWindowEdge();

	if (plot->GetBottomAxis()->IsLogarithmic())
	{
		int pixelDelta = event.GetX() - lastMousePosition[0];
		plot->SetXMin(plot->GetBottomAxis()->PixelToValue(
			(int)plot->GetLeftYAxis()->GetOffsetFromWindowEdge() - pixelDelta));
		plot->SetXMax(plot->GetBottomAxis()->PixelToValue(
			GetSize().GetWidth() - (int)plot->GetRightYAxis()->GetOffsetFromWindowEdge() - pixelDelta));
	}
	else
	{
		double xDelta = (plot->GetXMax() - plot->GetXMin()) * (event.GetX() - lastMousePosition[0]) / width;
		plot->SetXMin(plot->GetXMin() - xDelta);
		plot->SetXMax(plot->GetXMax() - xDelta);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		PanLeftYAxis
//
// Description:		Pans the left y-axis in resposne to mouse moves.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::PanLeftYAxis(wxMouseEvent &event)
{
	int height = GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge() - plot->GetTopAxis()->GetOffsetFromWindowEdge();

	if (plot->GetLeftYAxis()->IsLogarithmic())
	{
		int pixelDelta = event.GetY() - lastMousePosition[1];
		plot->SetLeftYMin(plot->GetLeftYAxis()->PixelToValue(
			(int)plot->GetBottomAxis()->GetOffsetFromWindowEdge() + pixelDelta));
		plot->SetLeftYMax(plot->GetLeftYAxis()->PixelToValue(
			GetSize().GetHeight() - (int)plot->GetTopAxis()->GetOffsetFromWindowEdge() + pixelDelta));
	}
	else
	{
		double yLeftDelta = (plot->GetLeftYMax() - plot->GetLeftYMin()) * (event.GetY() - lastMousePosition[1]) / height;
		plot->SetLeftYMin(plot->GetLeftYMin() + yLeftDelta);
		plot->SetLeftYMax(plot->GetLeftYMax() + yLeftDelta);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		PanRightYAxis
//
// Description:		Pans the right y-axis in resposne to mouse moves.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::PanRightYAxis(wxMouseEvent &event)
{
	int height = GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge() - plot->GetTopAxis()->GetOffsetFromWindowEdge();

	if (plot->GetRightYAxis()->IsLogarithmic())
	{
		int pixelDelta = event.GetY() - lastMousePosition[1];
		plot->SetRightYMin(plot->GetRightYAxis()->PixelToValue(
			(int)plot->GetBottomAxis()->GetOffsetFromWindowEdge() + pixelDelta));
		plot->SetRightYMax(plot->GetRightYAxis()->PixelToValue(
			GetSize().GetHeight() - (int)plot->GetTopAxis()->GetOffsetFromWindowEdge() + pixelDelta));
	}
	else
	{
		double yRightDelta = (plot->GetRightYMax() - plot->GetRightYMin()) * (event.GetY() - lastMousePosition[1]) / height;
		plot->SetRightYMin(plot->GetRightYMin() + yRightDelta);
		plot->SetRightYMax(plot->GetRightYMax() + yRightDelta);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessPlotAreaDoubleClick
//
// Description:		Processes double click events occurring within the plot area.
//
// Input Arguments:
//		x	= const unsigned int&, location of click along x-axis
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessPlotAreaDoubleClick(const unsigned int &x)
{
	double value(plot->GetBottomAxis()->PixelToValue(x));

	if (!leftCursor->GetIsVisible())
	{
		leftCursor->SetVisibility(true);
		leftCursor->SetLocation(x);
	}
	else if (!rightCursor->GetIsVisible())
	{
		rightCursor->SetVisibility(true);
		rightCursor->SetLocation(x);
	}
	else
	{
		// Both cursors are visible - move the closer one to the click spot
		// NOTE:  Another option is to always alternate which one was moved?
		if (fabs(leftCursor->GetValue() - value) < fabs(rightCursor->GetValue() - value))
			leftCursor->SetLocation(x);
		else
			rightCursor->SetLocation(x);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessOffPlotDoubleClick
//
// Description:		Process double click events that occured outside the plot area.
//
// Input Arguments:
//		x	= const unsigned int&, location of click along x-axis
//		y	= const unsigned int&, location of click along y-axis
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessOffPlotDoubleClick(const unsigned int &x, const unsigned int &y)
{
/*	// Determine the context
	MainFrame::PlotContext context;
	if (x < plot->GetLeftYAxis()->GetOffsetFromWindowEdge() &&
		y > plot->GetTopAxis()->GetOffsetFromWindowEdge() &&
		y < GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		context = MainFrame::plotContextLeftYAxis;
	else if (x > GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge() &&
		y > plot->GetTopAxis()->GetOffsetFromWindowEdge() &&
		y < GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		context = MainFrame::plotContextRightYAxis;
	else if (y > GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge() &&
		x > plot->GetLeftYAxis()->GetOffsetFromWindowEdge() &&
		x < GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge())
		context = MainFrame::plotContextXAxis;
	else
		context = MainFrame::plotContextPlotArea;

	// Display the dialog
	mainFrame.DisplayAxisRangeDialog(context);*/
	// FIXME:  Remove?
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessRightClick
//
// Description:		Processes right click events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessRightClick(wxMouseEvent &event)
{
	// Determine the context
	MainFrame::PlotContext context;
	unsigned int x = event.GetX();
	unsigned int y = event.GetY();
	if (x < plot->GetLeftYAxis()->GetOffsetFromWindowEdge() &&
		y > plot->GetTopAxis()->GetOffsetFromWindowEdge() &&
		y < GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		context = MainFrame::plotContextLeftYAxis;
	else if (x > GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge() &&
		y > plot->GetTopAxis()->GetOffsetFromWindowEdge() &&
		y < GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		context = MainFrame::plotContextRightYAxis;
	else if (y > GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge() &&
		x > plot->GetLeftYAxis()->GetOffsetFromWindowEdge() &&
		x < GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge())
		context = MainFrame::plotContextXAxis;
	else
		context = MainFrame::plotContextPlotArea;

	// Display the context menu (further events handled by MainFrame)
	mainFrame.CreatePlotContextMenu(GetPosition() + event.GetPosition(), context);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessZoomBoxEnd
//
// Description:		Completes zoom and cleans up box after user releases zoom box.
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
void PlotRenderer::ProcessZoomBoxEnd(void)
{
	zoomBox->SetVisibility(false);

	// Make sure the box isn't too small
	int limit = 5;// [pixels]
	if (abs(int(zoomBox->GetXAnchor() - zoomBox->GetXFloat())) > limit &&
		abs(int(zoomBox->GetYAnchor() - zoomBox->GetYFloat())) > limit)
	{
		// Determine the new zoom range
		// Remember: OpenGL uses Bottom Left as origin, normal windows use Top Left as origin
		double xMin = plot->GetBottomAxis()->PixelToValue(
			std::min<unsigned int>(zoomBox->GetXFloat(), zoomBox->GetXAnchor()));
		double xMax = plot->GetBottomAxis()->PixelToValue(
			std::max<unsigned int>(zoomBox->GetXFloat(), zoomBox->GetXAnchor()));
		double yLeftMin = plot->GetLeftYAxis()->PixelToValue(
			std::min<unsigned int>(zoomBox->GetYFloat(), zoomBox->GetYAnchor()));
		double yLeftMax = plot->GetLeftYAxis()->PixelToValue(
			std::max<unsigned int>(zoomBox->GetYFloat(), zoomBox->GetYAnchor()));
		double yRightMin = plot->GetRightYAxis()->PixelToValue(
			std::min<unsigned int>(zoomBox->GetYFloat(), zoomBox->GetYAnchor()));
		double yRightMax = plot->GetRightYAxis()->PixelToValue(
			std::max<unsigned int>(zoomBox->GetYFloat(), zoomBox->GetYAnchor()));

		SetXLimits(xMin, xMax);
		SetLeftYLimits(yLeftMin, yLeftMax);
		SetRightYLimits(yRightMin, yRightMax);
	}
}

unsigned int PlotRenderer::GetCurveCount(void)
{
	return plot->GetCurveCount();
}
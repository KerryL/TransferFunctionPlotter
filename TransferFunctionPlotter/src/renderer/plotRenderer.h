/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotRenderer.h
// Created:  5/4/2011
// Author:  K. Loux
// Description:  Derived from RenderWindow, this class is used to display plots on
//				 the screen.
// History:

#ifndef _PLOT_RENDERER_H_
#define _PLOT_RENDERER_H_

// Standard C++ headers
#include <stack>

// Local headers
#include "renderer/renderWindow.h"

// wxWidgets forward declarations
class wxString;

// Local forward declarations
class PlotObject;
class MainFrame;
class Dataset2D;
class ZoomBox;
class PlotCursor;

class PlotRenderer : public RenderWindow
{
public:
	// Constructor
	PlotRenderer(wxWindow *parent, wxWindowID id, int args[], MainFrame &_mainFrame);

	// Destructor
	~PlotRenderer();

	// Gets properties for actors
	bool GetBottomGrid(void) const;
	bool GetBottomMinorGrid(void) const;
	bool GetLeftGrid(void) const;
	bool GetRightGrid(void) const;

	Color GetGridColor(void) const;

	double GetXMin(void) const;
	double GetXMax(void) const;
	double GetLeftYMin(void) const;
	double GetLeftYMax(void) const;
	double GetRightYMin(void) const;
	double GetRightYMax(void) const;

	bool GetXLogarithmic(void) const;
	bool GetLeftLogarithmic(void) const;
	bool GetRightLogarithmic(void) const;

	bool GetXAxisZoomed(void) const;

	// Sets properties for actors
	void SetGridOn(void);
	void SetGridOff(void);
	void SetBottomGrid(const bool &grid);
	void SetBottomMinorGrid(const bool &grid);
	void SetLeftGrid(const bool &grid);
	void SetRightGrid(const bool &grid);

	void SetGridColor(const Color &color);

	void SetCurveProperties(const unsigned int &index, const Color &color,
		const bool &visible, const bool &rightAxis, const unsigned int &lineSize,
		const int &markerSize);
	void SetXLimits(const double &min, const double &max);
	void SetLeftYLimits(const double &min, const double &max);
	void SetRightYLimits(const double &min, const double &max);

	void SetXLabel(wxString text);
	void SetLeftYLabel(wxString text);
	void SetRightYLabel(wxString text);
	void SetTitle(wxString text);

	void AddCurve(const Dataset2D &data);
	void RemoveAllCurves(void);
	void RemoveCurve(const unsigned int &index);

	void AutoScale(void);
	void AutoScaleBottom(void);
	void AutoScaleLeft(void);
	void AutoScaleRight(void);

	void SetXLogarithmic(const bool &log);
	void SetLeftLogarithmic(const bool &log);
	void SetRightLogarithmic(const bool &log);

	bool GetGridOn(void);

	// Called to update the screen
	void UpdateDisplay(void);

	bool GetLeftCursorVisible(void) const;
	bool GetRightCursorVisible(void) const;
	double GetLeftCursorValue(void) const;
	double GetRightCursorValue(void) const;

	void UpdateCursors(void);

	MainFrame *GetMainFrame(void) { return &mainFrame; };

	void SaveCurrentZoom(void);
	void ClearZoomStack(void);

	unsigned int GetCurveCount(void);

private:
	// Called from the PlotRenderer constructor only in order to initialize the display
	void CreateActors(void);

	// The actors necessary to create the plot
	PlotObject *plot;

	// Parent window
	MainFrame &mainFrame;

	// Overload of size event
	void OnSize(wxSizeEvent &event);

	// Overload of interaction events
	void OnMouseWheelEvent(wxMouseEvent &event);
	void OnMouseMoveEvent(wxMouseEvent &event);
	void OnRightButtonUpEvent(wxMouseEvent &event);
	void OnLeftButtonUpEvent(wxMouseEvent &event);
	void OnLeftButtonDownEvent(wxMouseEvent &event);
	void OnMiddleButtonUpEvent(wxMouseEvent &event);

	void OnMouseLeaveWindowEvent(wxMouseEvent &event);
	void OnDoubleClickEvent(wxMouseEvent &event);

	ZoomBox *zoomBox;
	PlotCursor *leftCursor;
	PlotCursor *rightCursor;

	bool draggingLeftCursor;
	bool draggingRightCursor;

protected:
	void ProcessZoom(wxMouseEvent &event);
	void ProcessZoomWithBox(wxMouseEvent &event);
	void ProcessPan(wxMouseEvent &event);

	void PanBottomXAxis(wxMouseEvent &event);
	void PanLeftYAxis(wxMouseEvent &event);
	void PanRightYAxis(wxMouseEvent &event);

	void ProcessPlotAreaDoubleClick(const unsigned int &x);
	void ProcessOffPlotDoubleClick(const unsigned int &x, const unsigned int &y);

	void ProcessRightClick(wxMouseEvent &event);
	void ProcessZoomBoxEnd(void);

	struct Zoom
	{
		double xMin;
		double xMax;
		double leftYMin;
		double leftYMax;
		double rightYMin;
		double rightYMax;
	};

	std::stack<Zoom> zoom;
	void UndoZoom(void);
	bool ZoomChanged(void) const;

	bool ignoreNextMouseMove;

	// For the event table
	DECLARE_EVENT_TABLE()
};

#endif// _PLOT_RENDERER_H_

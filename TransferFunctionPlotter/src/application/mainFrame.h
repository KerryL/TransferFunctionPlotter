/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainFrame.h
// Created:  9/18/2012
// Author:  K. Loux
// Description:  Main frame for the application.
// History:

#ifndef _MAIN_FRAME_H_
#define _MAIN_FRAME_H_

// Standard C++ headers
#include <fstream>
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/dataset2D.h"
#include "application/dataManager.h"

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;
class wxSplitterWindow;
class wxSplitterEvent;

// Local forward declarations
class PlotRenderer;
class Color;

// The main frame class
class MainFrame : public wxFrame
{
public:
	// Constructor
	MainFrame();

	// Destructor
	~MainFrame();

	enum PlotContext
	{
		plotContextXAxis,
		plotContextLeftYAxis,
		plotContextRightYAxis,
		plotContextPlotArea
	};

	void CreatePlotContextMenu(const wxPoint &position, const PlotContext &context);

	void UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue);

private:
	// Functions that do some of the frame initialization and control positioning
	void CreateControls(void);
	void SetProperties(void);

	PlotRenderer* CreatePlotArea(wxWindow *parent, const wxString &title, const wxString &yLabel);
	wxSizer* CreateButtons(wxWindow *parent);
	wxGrid* CreateOptionsGrid(wxWindow *parent);
	wxSizer* CreateInputControls(wxWindow *parent);

	// Controls
	wxGrid *optionsGrid;

	wxRadioButton *frequencyUnitsHertzRadioButton;

	wxTextCtrl *minFrequencyTextBox;
	wxTextCtrl *maxFrequencyTextBox;

	PlotRenderer *selectedAmplitudePlot;
	PlotRenderer *selectedPhasePlot;
	PlotRenderer *totalAmplitudePlot;
	PlotRenderer *totalPhasePlot;

	enum Columns
	{
		colName = 0,
		colColor,
		colLineSize,
		colLeftCursor,
		colRightCursor,
		colDifference,
		colVisible,
		colRightAxis,

		colCount
	};

	// For displaying a menu that was crated by this form
	// NOTE:  When calculating the Position to display this context menu,
	// consider that the coordinates for the calling object might be different
	// from the coordinates for this object!
	void CreateGridContextMenu(const wxPoint &position, const unsigned int &row);

	void ClearAllCurves(void);
	void AddCurve(wxString numerator, wxString denominator);
	void UpdateCurve(unsigned int i);
	void UpdateCurve(unsigned int i, wxString numerator, wxString denominator);
	void RemoveCurve(const unsigned int &i);

	Color GetNextColor(const unsigned int &index) const;
	void AddXRowToGrid(void);
	unsigned int AddDataRowToGrid(const wxString &name);

	void UpdateCurveProperties(const unsigned int &index, const Color &color,
		const bool &visible, const bool &rightAxis);
	void UpdateCurveProperties(const unsigned int &index);

	void UpdateSelectedTransferFunction(const unsigned int &i);

	void SetXLabels(void);
	wxString xLabel;

	DataManager dataManager;

	// The event IDs
	enum MainFrameEventID
	{
		idAddButton = wxID_HIGHEST + 100,
		idRemoveButton,
		idRemoveAllButton,

		idPlotContextToggleGridlines,
		idPlotContextAutoScale,
		idPlotContextWriteImageFile,

		idPlotContextBGColor,
		idPlotContextGridColor,

		idPlotContextToggleBottomGridlines,// Maintain this order for each axis' context IDs
		idPlotContextAutoScaleBottom,
		idPlotContextToggleBottomMinorGridlines,

		idPlotContextToggleLeftGridlines,
		idPlotContextAutoScaleLeft,

		idPlotContextToggleRightGridlines,
		idPlotContextAutoScaleRight
	};

	wxMenu *CreateAxisContextMenu(const unsigned int &baseEventId) const;
	wxMenu *CreatePlotAreaContextMenu(void) const;

	// Event handlers-----------------------------------------------------
	// Buttons
	void AddButtonClicked(wxCommandEvent &event);
	void RemoveButtonClicked(wxCommandEvent &event);
	void RemoveAllButtonClicked(wxCommandEvent &event);

	// Other input controls
	void TextBoxChangeEvent(wxFocusEvent &event);
	void RadioButtonChangeEvent(wxCommandEvent &event);

	// Grid events
	void GridRightClickEvent(wxGridEvent &event);
	void GridDoubleClickEvent(wxGridEvent &event);
	void GridLeftClickEvent(wxGridEvent &event);
	void GridCellChangeEvent(wxGridEvent &event);

	// Context menu events
	void ContextToggleGridlines(wxCommandEvent &event);
	void ContextAutoScale(wxCommandEvent &event);
	void ContextWriteImageFile(wxCommandEvent &event);

	void ContextPlotBGColor(wxCommandEvent &event);
	void ContextGridColor(wxCommandEvent &event);

	void ContextToggleGridlinesBottom(wxCommandEvent &event);
	void ContextAutoScaleBottom(wxCommandEvent &event);
	void ContextSetRangeBottom(wxCommandEvent &event);
	void ContextToggleMinorGridlinesBottom(wxCommandEvent &event);

	void ContextToggleGridlinesLeft(wxCommandEvent &event);
	void ContextAutoScaleLeft(wxCommandEvent &event);
	void ContextSetRangeLeft(wxCommandEvent &event);

	void ContextToggleGridlinesRight(wxCommandEvent &event);
	void ContextAutoScaleRight(wxCommandEvent &event);
	void ContextSetRangeRight(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	bool XScalingFactorIsKnown(double &factor, wxString *label) const;
	bool UnitStringToFactor(const wxString &unit, double &factor) const;
	wxString ExtractUnitFromDescription(const wxString &description) const;
	bool FindWrappedString(const wxString &s, wxString &contents,
		const wxChar &open, const wxChar &close) const;

	void UpdateSingleCursorValue(const unsigned int &row, double value,
		const unsigned int &column, const bool &isVisible);

	bool GetCurrentAxisRange(const PlotContext &axis, double &min, double &max) const;
	void SetNewAxisRange(const PlotContext &axis, const double &min, const double &max);

	wxArrayString GetFileNameFromUser(wxString dialogTitle, wxString defaultDirectory,
		wxString defaultFileName, wxString wildcard, long style);

	void UpdatePlots(void);

	DECLARE_EVENT_TABLE();
};

#endif// _MAIN_FRAME_H_
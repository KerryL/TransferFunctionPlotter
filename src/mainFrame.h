/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainFrame.h
// Created:  9/18/2012
// Author:  K. Loux
// Description:  Main frame for the application.

#ifndef _MAIN_FRAME_H_
#define _MAIN_FRAME_H_

// Local headers
#include "dataManager.h"

// LibPlot2D headers
#include <lp2d/gui/guiInterface.h>

// wxWidgets headers
#include <wx/wx.h>

// Standard C++ headers
#include <fstream>
#include <vector>

// LibPlot2D forward delcarations
namespace LibPlot2D
{
class PlotRenderer;
class Color;
}

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;
class wxSplitterWindow;
class wxSplitterEvent;

class MainFrame : public wxFrame
{
public:
	MainFrame();
	~MainFrame();

private:
	// Functions that do some of the frame initialization and control positioning
	void CreateControls();
	void SetProperties();

	LibPlot2D::PlotRenderer* CreatePlotArea(wxWindow *parent, LibPlot2D::GuiInterface& guiInterface, const wxString& title, const wxString& yLabel);
	wxSizer* CreateButtons(wxWindow *parent);
	wxGrid* CreateOptionsGrid(wxWindow *parent);
	wxSizer* CreateInputControls(wxWindow *parent);

	// Controls
	wxGrid *optionsGrid;

	wxRadioButton *frequencyUnitsHertzRadioButton;

	wxTextCtrl *minFrequencyTextBox;
	wxTextCtrl *maxFrequencyTextBox;

	LibPlot2D::PlotRenderer *selectedAmplitudePlot;
	LibPlot2D::PlotRenderer *selectedPhasePlot;
	LibPlot2D::PlotRenderer *totalAmplitudePlot;
	LibPlot2D::PlotRenderer *totalPhasePlot;

	LibPlot2D::GuiInterface selectedAmplitudeInterface;
	LibPlot2D::GuiInterface selectedPhaseInterface;
	LibPlot2D::GuiInterface totalAmplitudeInterface;
	LibPlot2D::GuiInterface totalPhaseInterface;

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

	void ClearAllCurves();
	void AddCurve(wxString numerator, wxString denominator);
	void UpdateCurve(unsigned int i);
	void UpdateCurve(unsigned int i, wxString numerator, wxString denominator);
	void RemoveCurve(const unsigned int &i);

	LibPlot2D::Color GetNextColor(const unsigned int &index) const;
	void AddXRowToGrid();
	unsigned int AddDataRowToGrid(const wxString &name);

	void UpdateCurveProperties(const unsigned int &index, const LibPlot2D::Color &color,
		const bool &visible, const bool &rightAxis);
	void UpdateCurveProperties(const unsigned int &index);

	void UpdateSelectedTransferFunction(const unsigned int &i);

	void SetXLabels();
	wxString xLabel;

	DataManager dataManager;

	// The event IDs
	enum MainFrameEventID
	{
		idAddButton = wxID_HIGHEST + 100,
		idRemoveButton,
		idRemoveAllButton,
	};

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
	// End event handlers-------------------------------------------------

	bool XScalingFactorIsKnown(double &factor, wxString *label) const;
	bool UnitStringToFactor(const wxString &unit, double &factor) const;
	wxString ExtractUnitFromDescription(const wxString &description) const;
	bool FindWrappedString(const wxString &s, wxString &contents,
		const wxChar &open, const wxChar &close) const;

	void UpdateSingleCursorValue(const unsigned int &row, double value,
		const unsigned int &column, const bool &isVisible);

	wxArrayString GetFileNameFromUser(wxString dialogTitle, wxString defaultDirectory,
		wxString defaultFileName, wxString wildcard, long style);

	void UpdatePlots();

	DECLARE_EVENT_TABLE();
};

#endif// _MAIN_FRAME_H_

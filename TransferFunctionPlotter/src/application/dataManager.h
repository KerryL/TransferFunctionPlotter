/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataManager.h
// Created:  9/21/2012
// Author:  K. Loux
// Description:  Data container/manager.  Allows single TFs to appear in options grid,
//				 while maintaining separate data for amplitude, phase (and total for
//				 all TFs).
// History:

// Standard C++ headers
#include <vector>
#include <utility>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/managedList.h"
#include "utilities/dataset2D.h"

class DataManager
{
public:
	DataManager();

	bool AddTransferFunction(const wxString &numerator, const wxString &denominator);
	bool UpdateTransferFunction(const unsigned int &i, const wxString &numerator, const wxString &denominator);

	void SetFrequencyUnitsHertz(void);
	void SetFrequencyUnitsRadPerSec(void);
	void SetFrequencyRange(const double &min, const double &max);

	Dataset2D* GetAmplitudeData(const unsigned int &i) { return amplitudePlots[i]; };
	Dataset2D* GetPhaseData(const unsigned int &i) { return phasePlots[i]; };
	Dataset2D* GetTotalAmplitudeData(void);
	Dataset2D* GetTotalPhaseData(void);

	wxString GetNumerator(const unsigned int &i) const { return transferFunctions[i].first; };
	wxString GetDenominator(const unsigned int &i) const { return transferFunctions[i].second; };

	unsigned int GetCount(void) { return amplitudePlots.GetCount(); };

	void RemoveAllTransferFunctions(void);
	void RemoveTransferFunctions(const unsigned int &i);

	void UpdateTotalTransferFunctionData(void);

private:
	double minFreq, maxFreq;// [Hz]
	bool frequencyHertz;

	static const unsigned int numberOfPoints;

	ManagedList<Dataset2D> amplitudePlots;
	ManagedList<Dataset2D> phasePlots;

	bool totalNeedsUpdate;
	Dataset2D totalAmplitude;
	Dataset2D totalPhase;

	std::vector<std::pair<wxString, wxString> > transferFunctions;
	wxString ConstructTotalTransferFunction(void) const;
	void UpdateAllTransferFunctionData(void);

	wxString AssembleTransferFunctionString(const wxString &numerator, const wxString &denominator) const;
	wxString AssembleTransferFunctionString(const std::pair<wxString, wxString> &tf) const;
};
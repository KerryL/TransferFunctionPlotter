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

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/managedList.h"
#include "utilities/dataset2D.h"

class DataManager
{
public:
	DataManager();

	bool AddTransferFunction(const wxString &tf);

	void SetFrequencyUnitsHertz(void);
	void SetFrequencyUnitsRadPerSec(void);
	void SetFrequencyRange(const double &min, const double &max);

	Dataset2D* GetAmplitudeData(const unsigned int &i) { return amplitudePlots[i]; };
	Dataset2D* GetPhaseData(const unsigned int &i) { return phasePlots[i]; };
	Dataset2D* GetTotalAmplitudeData(void) { return &totalAmplitude; };
	Dataset2D* GetTotalPhaseData(void) { return &totalPhase; };

	unsigned int GetCount(void) { return amplitudePlots.GetCount(); };

	void RemoveAllTransferFunctions(void);
	void RemoveTransferFunctions(const unsigned int &i);

private:
	double minFreq, maxFreq;// [Hz]
	bool frequencyHertz;

	ManagedList<Dataset2D> amplitudePlots;
	ManagedList<Dataset2D> phasePlots;

	Dataset2D totalAmplitude;
	Dataset2D totalPhase;

	wxArrayString transferFunctions;
	wxString ConstructTotalTransferFunction(void) const;
	void UpdateTotalTransferFunctionData(void);
	void UpdateAllTransferFunctionData(void);
};
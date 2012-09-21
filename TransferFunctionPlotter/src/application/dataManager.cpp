/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataManager.cpp
// Created:  9/21/2012
// Author:  K. Loux
// Description:  Data container/manager.  Allows single TFs to appear in options grid,
//				 while maintaining separate data for amplitude, phase (and total for
//				 all TFs).
// History:

// Local headers
#include "application/dataManager.h"
#include "utilities/math/plotMath.h"
#include "utilities/math/expressionTree.h"

DataManager::DataManager()
{
	frequencyHertz = true;
	minFreq = 0.1;
	maxFreq = 100.0;
}

bool DataManager::AddTransferFunction(const wxString &tf)
{
	Dataset2D *magnitude = new Dataset2D;
	Dataset2D *phase = new Dataset2D;

	ExpressionTree expressionTree;
	wxString errorString = expressionTree.Solve(tf, minFreq, maxFreq, 1000, *magnitude, *phase);
	if (!errorString.IsEmpty())
	{
		wxMessageBox(errorString);
		delete magnitude;
		delete phase;
		return false;
	}

	amplitudePlots.Add(magnitude);
	phasePlots.Add(phase);

	transferFunctions.Add(tf);
	UpdateTotalTransferFunctionData();

	return true;
}

void DataManager::SetFrequencyUnitsHertz(void)
{
	if (frequencyHertz)
		return;
	frequencyHertz = true;
	UpdateAllTransferFunctionData();
}

void DataManager::SetFrequencyUnitsRadPerSec(void)
{
	if (!frequencyHertz)
		return;
	frequencyHertz = false;
	UpdateAllTransferFunctionData();
}

void DataManager::SetFrequencyRange(const double &min, const double &max)
{
	if (min >= max)
		return;

	minFreq = min;
	maxFreq = max;

	UpdateAllTransferFunctionData();
	UpdateTotalTransferFunctionData();
}

wxString DataManager::ConstructTotalTransferFunction(void) const
{
	wxString totalTF;
	unsigned int i;
	for (i = 0; i < transferFunctions.Count(); i++)
	{
		totalTF.Append(_T("(") + transferFunctions[i] + _T(")"));
		if (i < transferFunctions.Count() - 1)
			totalTF.Append(_T("*"));
	}

	return totalTF;
}

void DataManager::UpdateTotalTransferFunctionData(void)
{
	wxString totalTF = ConstructTotalTransferFunction();

	ExpressionTree expressionTree;
	if (frequencyHertz)
		expressionTree.Solve(totalTF, minFreq, maxFreq, 1000, totalAmplitude, totalPhase);
	else
	{
		expressionTree.Solve(totalTF, minFreq * 2.0 * PlotMath::pi,
			maxFreq * 2.0 * PlotMath::pi, 1000, totalAmplitude, totalPhase);

		totalAmplitude.MultiplyXData(0.5 / PlotMath::pi);
		totalPhase.MultiplyXData(0.5 / PlotMath::pi);
	}
}

void DataManager::RemoveAllTransferFunctions(void)
{
	amplitudePlots.Clear();
	phasePlots.Clear();
	transferFunctions.Clear();

	// FIXME:  What about total?
}

void DataManager::RemoveTransferFunctions(const unsigned int &i)
{
	amplitudePlots.Remove(i);
	phasePlots.Remove(i);
	transferFunctions.RemoveAt(i);

	UpdateTotalTransferFunctionData();
}

void DataManager::UpdateAllTransferFunctionData(void)
{
	ExpressionTree expressionTree;
	unsigned int i;
	for (i = 0; i < amplitudePlots.GetCount(); i++)
	{
		if (frequencyHertz)
			expressionTree.Solve(transferFunctions[i], minFreq, maxFreq, 1000,
			*amplitudePlots[i], *phasePlots[i]);
		else
		{
			expressionTree.Solve(transferFunctions[i], minFreq * 2.0 * PlotMath::pi,
			maxFreq * 2.0 * PlotMath::pi, 1000, *amplitudePlots[i], *phasePlots[i]);

			amplitudePlots[i]->MultiplyXData(0.5 / PlotMath::pi);
			phasePlots[i]->MultiplyXData(0.5 / PlotMath::pi);
		}
	}
}
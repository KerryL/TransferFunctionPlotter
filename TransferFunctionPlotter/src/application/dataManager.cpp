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

bool DataManager::AddTransferFunction(const wxString &numerator, const wxString &denominator)
{
	Dataset2D *magnitude = new Dataset2D;
	Dataset2D *phase = new Dataset2D;

	ExpressionTree expressionTree;
	wxString errorString = expressionTree.Solve(AssembleTransferFunctionString(numerator, denominator),
		minFreq, maxFreq, 1000, *magnitude, *phase);
	if (!errorString.IsEmpty())
	{
		wxMessageBox(errorString);
		delete magnitude;
		delete phase;
		return false;
	}

	amplitudePlots.Add(magnitude);
	phasePlots.Add(phase);

	transferFunctions.push_back(std::make_pair(numerator, denominator));
	UpdateTotalTransferFunctionData();

	return true;
}

bool DataManager::UpdateTransferFunction(const unsigned int &i, const wxString &numerator, const wxString &denominator)
{
	Dataset2D *magnitude = new Dataset2D;
	Dataset2D *phase = new Dataset2D;

	ExpressionTree expressionTree;
	wxString errorString = expressionTree.Solve(AssembleTransferFunctionString(numerator, denominator),
		minFreq, maxFreq, 1000, *magnitude, *phase);
	if (!errorString.IsEmpty())
	{
		wxMessageBox(errorString);
		delete magnitude;
		delete phase;
		return false;
	}

	assert(magnitude->GetNumberOfPoints() == amplitudePlots[i]->GetNumberOfPoints());
	assert(phase->GetNumberOfPoints() == phasePlots[i]->GetNumberOfPoints());

	unsigned int j;
	for (j = 0; j < magnitude->GetNumberOfPoints(); j++)
	{
		amplitudePlots[i]->GetXPointer()[j] = magnitude->GetXData(j);
		amplitudePlots[i]->GetYPointer()[j] = magnitude->GetYData(j);
		phasePlots[i]->GetXPointer()[j] = phase->GetXData(j);
		phasePlots[i]->GetYPointer()[j] = phase->GetYData(j);
	}

	delete magnitude;
	delete phase;

	transferFunctions[i].first = numerator;
	transferFunctions[i].second = denominator;
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
}

wxString DataManager::ConstructTotalTransferFunction(void) const
{
	wxString totalNum, totalDen;
	unsigned int i;
	for (i = 0; i < transferFunctions.size(); i++)
	{
		totalNum.Append(_T("(") + transferFunctions[i].first + _T(")"));
		totalDen.Append(_T("(") + transferFunctions[i].second + _T(")"));
		if (i < transferFunctions.size() - 1)
		{
			totalNum.Append(_T("*"));
			totalDen.Append(_T("*"));
		}
	}

	return AssembleTransferFunctionString(totalNum, totalDen);
}

void DataManager::UpdateTotalTransferFunctionData(void)
{
	wxString totalTF = ConstructTotalTransferFunction();

	if (totalTF.IsEmpty())
		return;

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
	transferFunctions.clear();

	// FIXME:  What about total?
}

void DataManager::RemoveTransferFunctions(const unsigned int &i)
{
	amplitudePlots.Remove(i);
	phasePlots.Remove(i);
	transferFunctions.erase(transferFunctions.begin() + i);

	UpdateTotalTransferFunctionData();
}

void DataManager::UpdateAllTransferFunctionData(void)
{
	ExpressionTree expressionTree;
	unsigned int i;
	for (i = 0; i < amplitudePlots.GetCount(); i++)
	{
		if (frequencyHertz)
			expressionTree.Solve(AssembleTransferFunctionString(transferFunctions[i]), minFreq, maxFreq, 1000,
			*amplitudePlots[i], *phasePlots[i]);
		else
		{
			expressionTree.Solve(AssembleTransferFunctionString(transferFunctions[i]), minFreq * 2.0 * PlotMath::pi,
			maxFreq * 2.0 * PlotMath::pi, 1000, *amplitudePlots[i], *phasePlots[i]);

			amplitudePlots[i]->MultiplyXData(0.5 / PlotMath::pi);
			phasePlots[i]->MultiplyXData(0.5 / PlotMath::pi);
		}
	}

	UpdateTotalTransferFunctionData();
}

wxString DataManager::AssembleTransferFunctionString(const wxString &numerator, const wxString &denominator) const
{
	return "(" + numerator + ")/(" + denominator + ")";
}

wxString DataManager::AssembleTransferFunctionString(const std::pair<wxString, wxString> &tf) const
{
	return AssembleTransferFunctionString(tf.first, tf.second);
}
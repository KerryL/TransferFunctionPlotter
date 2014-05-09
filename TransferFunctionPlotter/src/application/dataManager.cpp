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

const unsigned int DataManager::numberOfPoints(5000);

DataManager::DataManager()
{
	frequencyHertz = true;
	minFreq = 0.01;
	maxFreq = 100.0;
}

bool DataManager::AddTransferFunction(const wxString &numerator, const wxString &denominator)
{
	amplitudePlots.Add(new Dataset2D(numberOfPoints));
	phasePlots.Add(new Dataset2D(numberOfPoints));

	transferFunctions.push_back(std::make_pair(numerator, denominator));
	return UpdateTransferFunction(amplitudePlots.GetCount() - 1, numerator, denominator);
}

bool DataManager::UpdateTransferFunction(const unsigned int &i, const wxString &numerator, const wxString &denominator)
{
	Dataset2D *magnitude = new Dataset2D;
	Dataset2D *phase = new Dataset2D;

	transferFunctions[i].first = numerator;
	transferFunctions[i].second = denominator;

	ExpressionTree expressionTree;
	wxString errorString;
	if (frequencyHertz)
		errorString = expressionTree.Solve(AssembleTransferFunctionString(transferFunctions[i]),
			minFreq, maxFreq, numberOfPoints, *magnitude, *phase);
	else
	{
		errorString = expressionTree.Solve(AssembleTransferFunctionString(transferFunctions[i]),
			minFreq * 0.5 / PlotMath::pi, maxFreq * 0.5 / PlotMath::pi, numberOfPoints, *magnitude, *phase);

		magnitude->MultiplyXData(2.0 * PlotMath::pi);
		phase->MultiplyXData(2.0 * PlotMath::pi);
	}

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

	totalNeedsUpdate = true;

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
		expressionTree.Solve(totalTF, minFreq, maxFreq, numberOfPoints, totalAmplitude, totalPhase);
	else
	{
		expressionTree.Solve(totalTF, minFreq * 0.5 / PlotMath::pi,
			maxFreq * 0.5 / PlotMath::pi, numberOfPoints, totalAmplitude, totalPhase);

		totalAmplitude.MultiplyXData(2.0 * PlotMath::pi);
		totalPhase.MultiplyXData(2.0 * PlotMath::pi);
	}

	totalNeedsUpdate = false;
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
	unsigned int i;
	for (i = 0; i < amplitudePlots.GetCount(); i++)
		UpdateTransferFunction(i, transferFunctions[i].first, transferFunctions[i].second);

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

Dataset2D* DataManager::GetTotalAmplitudeData(void)
{
	if (totalNeedsUpdate)
		UpdateTotalTransferFunctionData();
	return &totalAmplitude;
}

Dataset2D* DataManager::GetTotalPhaseData(void)
{
	if (totalNeedsUpdate)
		UpdateTotalTransferFunctionData();
	return &totalPhase;
}
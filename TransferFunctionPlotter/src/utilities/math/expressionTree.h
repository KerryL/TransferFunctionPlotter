/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  expressionTree.h
// Created:  5/6/2011
// Author:  K. Loux
// Description:  Handles user-specified mathematical operations on datasets.
// History:

#ifndef _EXPRESSION_TREE_H_
#define _EXPRESSION_TREE_H_

// Standard C++ headers
#include <queue>
#include <stack>

// Local headers
#include "utilities/managedList.h"
#include "utilities/dataset2D.h"
#include "utilities/math/complex.h"

// wxWidgets forward declarations
class wxString;

class ExpressionTree
{
public:
	// Main solver method
	wxString Solve(wxString expression, const double &minFreq, const double &maxFreq,
		const unsigned int &resolution, Dataset2D &magnitude, Dataset2D &phase);

private:
	std::queue<wxString> outputQueue;
	std::vector<Complex> dataVector;

	void PrepareDataset(const double &minFreq, const double &maxFreq,
		const unsigned int &resolution, Dataset2D &magnitude, Dataset2D &phase);

	wxString ParseExpression(const wxString &expression);
	wxString EvaluateExpression(void);

	void ProcessOperator(std::stack<wxString> &operatorStack, const wxString &s);
	void ProcessCloseParenthese(std::stack<wxString> &operatorStack);

	bool NextIsNumber(const wxString &s, unsigned int *stop = NULL) const;
	bool NextIsOperator(const wxString &s, unsigned int *stop = NULL) const;
	bool NextIsS(const wxString &s, unsigned int *stop = NULL) const;

	bool IsLeftAssociative(const wxChar &c) const;
	bool OperatorShift(const wxString &stackString, const wxString &newString) const;

	void PopStackToQueue(std::stack<wxString> &stack);
	bool EmptyStackToQueue(std::stack<wxString> &stack);
	unsigned int GetPrecedence(const wxString &s) const;

	void PushToStack(const double &value, std::stack<double> &doubleStack,
		std::stack<bool> &useDoubleStack) const;
	void PushToStack(const std::vector<Complex> &vector, std::stack<std::vector<Complex> > &vectorStack,
		std::stack<bool> &useDoubleStack) const;
	bool PopFromStack(std::stack<double> &doubleStack, std::stack<std::vector<Complex> > &vectorStack,
		std::stack<bool> &useDoubleStack, double &value, std::vector<Complex> &vector) const;

	double ApplyOperation(const wxString &operation, const double &first, const double &second) const;
	std::vector<Complex> ApplyOperation(const wxString &operation, const std::vector<Complex> &first, const double &second) const;
	std::vector<Complex> ApplyOperation(const wxString &operation, const double &first, const std::vector<Complex> &second) const;
	std::vector<Complex> ApplyOperation(const wxString &operation, const std::vector<Complex> &first,
		const std::vector<Complex> &second) const;

	bool EvaluateNext(const wxString &next, std::stack<double> &doubleStack,
		std::stack<std::vector<Complex> > &vectorStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateOperator(const wxString &operation, std::stack<double> &doubleStack,
		std::stack<std::vector<Complex> > &vectorStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateNumber(const wxString &number, std::stack<double> &doubleStack,
		std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool ExpressionTree::EvaluateS(std::stack<std::vector<Complex> > &vectorStack,
		std::stack<bool> &useDoubleStack) const;

	bool ParenthesesBalanced(const wxString &expression) const;
};

#endif// _EXPRESSION_TREE_H_
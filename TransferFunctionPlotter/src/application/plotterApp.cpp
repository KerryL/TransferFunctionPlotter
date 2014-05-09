/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotterApp.cpp
// Created:  9/18/2012
// Author:  K. Loux
// Description:  The application class.
// History:

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "application/plotterApp.h"
#include "application/mainFrame.h"

// Implement the application (have wxWidgets set up the appropriate entry points, etc.)
IMPLEMENT_APP(TransferFunctionPlotterApp);

//==========================================================================
// Class:			TransferFunctionPlotterApp
// Function:		Constant Declarations
//
// Description:		Constant declarations for the TransferFunctionPlotterApp class.
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
const wxString TransferFunctionPlotterApp::transferFunctionPlotterTitle = _T("Transfer Function Plotter");
const wxString TransferFunctionPlotterApp::transferFunctionPlotterName = _T("TransferFunctionPlotterApplication");
const wxString TransferFunctionPlotterApp::creator = _T("Kerry Loux");
const wxString TransferFunctionPlotterApp::versionString = _T("v0.2");

//==========================================================================
// Class:			TransferFunctionPlotterApp
// Function:		OnInit
//
// Description:		Initializes the application window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool = true for successful window initialization, false for error
//
//==========================================================================
bool TransferFunctionPlotterApp::OnInit()
{
	SetAppName(transferFunctionPlotterName);
	SetVendorName(creator);

	mainFrame = new MainFrame();

	if (mainFrame == NULL)
		return false;

	mainFrame->Show(true);

	return true;
}

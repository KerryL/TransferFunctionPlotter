/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  complex_krl.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class functionality for complex number class.
// History:

// Local headers
#include "utilities/math/complex.h"

// Standard C++ headers
#include <iostream>

// wxWidgets headers
#include <wx/wx.h>

using namespace std;

//==========================================================================
// Class:			ComplexKRL
// Function:		ComplexKRL
//
// Description:		Constructor for the ComplexKRL class.
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
ComplexKRL::ComplexKRL()
{
}

//==========================================================================
// Class:			ComplexKRL
// Function:		ComplexKRL
//
// Description:		Constructor for the ComplexKRL class.
//
// Input Arguments:
//		_real		= const double& real component of the complex number
//		_imaginary	= const double& imaginary component of the complex number
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ComplexKRL::ComplexKRL(const double &_real, const double &_imaginary)
{
	// Assign the arguments to the class members
	real = _real;
	imaginary = _imaginary;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		~ComplexKRL
//
// Description:		Destructor for the ComplexKRL class.
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
ComplexKRL::~ComplexKRL()
{
}

//==========================================================================
// Class:			ComplexKRL
// Function:		Constant Definitions
//
// Description:		Defines class level constants for the ComplexKRL class.
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
const ComplexKRL ComplexKRL::i(0.0, 1.0);

//==========================================================================
// Class:			friend of ComplexKRL
// Function:		operator <<
//
// Description:		Writes the value of Target to a stream.
//
// Input Arguments:
//		writeOut	= ostream& to write out to
//		complex		= const ComplexKRL& value to be written to the stream
//
// Output Arguments:
//		None
//
// Return Value:
//		&ostream containing the formatted value
//
//==========================================================================
ostream &operator << (ostream &writeOut, const ComplexKRL &complex)
{
	writeOut << complex.Print();

	return writeOut;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		Print
//
// Description:		Prints this object to a string.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the formatted value of this object
//
//==========================================================================
wxString ComplexKRL::Print(void) const
{
	wxString temp;

	if (imaginary >= 0)
		temp.Printf("%0.3f + %0.3f i", real, imaginary);
	else
		temp.Printf("%0.3f - %0.3f i", real, -imaginary);

	return temp;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator +
//
// Description:		Addition operator for the ComplexKRL class.
//
// Input Arguments:
//		complex	= const ComplexKRL& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Complex result of the addition
//
//==========================================================================
const ComplexKRL ComplexKRL::operator + (const ComplexKRL &complex) const
{
	// Make a copy of this object
	ComplexKRL temp = *this;

	// Do the addition
	temp += complex;

	return temp;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator -
//
// Description:		Subraction operator for the ComplexKRL class.
//
// Input Arguments:
//		complex	= const ComplexKRL& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		const ComplexKRL result of the subtraction
//
//==========================================================================
const ComplexKRL ComplexKRL::operator - (const ComplexKRL &complex) const
{
	// Make a copy of this object
	ComplexKRL temp = *this;

	// Do the subtraction
	temp -= complex;

	return temp;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator *
//
// Description:		Multiplication operator for the ComplexKRL class.
//
// Input Arguments:
//		complex	= const ComplexKRL& to multiply with this
//
// Output Arguments:
//		None
//
// Return Value:
//		const ComplexKRL result of the multiplication
//
//==========================================================================
const ComplexKRL ComplexKRL::operator * (const ComplexKRL &complex) const
{
	// Make a copy of this object
	ComplexKRL temp = *this;

	// Do the multiplication
	temp *= complex;

	return temp;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator /
//
// Description:		Division operator for the ComplexKRL class.
//
// Input Arguments:
//		complex	= const ComplexKRL& to divide by this
//
// Output Arguments:
//		None
//
// Return Value:
//		const ComplexKRL result of the division
//
//==========================================================================
const ComplexKRL ComplexKRL::operator / (const ComplexKRL &complex) const
{
	// Make a copy of this object
	ComplexKRL temp = *this;

	// Do the division
	temp /= complex;

	return temp;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		GetPolarLength
//
// Description:		Converts to polar coordinates, returns the length (r).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double ComplexKRL::GetPolarLength(void) const
{
	return sqrt(real * real + imaginary * imaginary);
}

//==========================================================================
// Class:			ComplexKRL
// Function:		GetPolarAngle
//
// Description:		Converts to polar coordinates, returns the angle (theta).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double [rad]
//
//==========================================================================
double ComplexKRL::GetPolarAngle(void) const
{
	return atan2(imaginary, real);
}

//==========================================================================
// Class:			ComplexKRL
// Function:		ToPower
//
// Description:		Raises this object to the specified power.
//
// Input Arguments:
//		power	= const double& specifiying the power to which this will be raised
//
// Output Arguments:
//		None
//
// Return Value:
//		ComplexKRL& result of the power
//
//==========================================================================
ComplexKRL& ComplexKRL::ToPower(const double &power)
{
	// Convert this from Cartesian to polar form
	double r = GetPolarLength();
	double theta = GetPolarAngle();

	// Use De Moivre's theorem to raise this to a power
	r = pow(r, power);
	theta *= power;

	// Convert back to Cartesian form
	real = r * cos(theta);
	imaginary = r * sin(theta);

	return *this;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		ToPower
//
// Description:		Raises this object to the specified power (const version).
//
// Input Arguments:
//		power	= const double& specifiying the power to which this will be raised
//
// Output Arguments:
//		None
//
// Return Value:
//		const ComplexKRL result of the power
//
//==========================================================================
const ComplexKRL ComplexKRL::ToPower(const double &power) const
{
	ComplexKRL result(*this);
	result.ToPower(power);

	return result;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		ToPower
//
// Description:		Raises this object to the specified complex power.
//
// Input Arguments:
//		power	= const ComplexKRL& specifiying the power to which this will be raised
//
// Output Arguments:
//		None
//
// Return Value:
//		ComplexKRL& result of the power
//
//==========================================================================
ComplexKRL& ComplexKRL::ToPower(const ComplexKRL &power)
{
	ComplexKRL original(*this);

	double r = GetPolarLength();
	double theta = GetPolarAngle();

	double factor = pow(r, power.real) * exp(-power.imaginary * theta);
	double angle = power.imaginary * log(r) + power.real * theta;

	real = factor * cos(angle);
	imaginary = factor * sin(angle);

	return *this;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		ToPower
//
// Description:		Raises this object to the specified complex power (const version).
//
// Input Arguments:
//		power	= const ComplexKRL& specifiying the power to which this will be raised
//
// Output Arguments:
//		None
//
// Return Value:
//		const ComplexKRL result of the power
//
//==========================================================================
const ComplexKRL ComplexKRL::ToPower(const ComplexKRL &power) const
{
	ComplexKRL result(*this);

	return result.ToPower(power);
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator +=
//
// Description:		Addition assignment operator for the ComplexKRL class.
//
// Input Arguments:
//		complex	= const ComplexKRL& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		ComplexKRL& result of the addition
//
//==========================================================================
ComplexKRL& ComplexKRL::operator += (const ComplexKRL &complex)
{
	// Add the real components
	real += complex.real;

	// Add the imaginary components
	imaginary += complex.imaginary;

	return *this;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator -=
//
// Description:		Subraction assignment operator for the ComplexKRL class.
//
// Input Arguments:
//		complex	= const ComplexKRL& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		ComplexKRL& result of the subtraction
//
//==========================================================================
ComplexKRL& ComplexKRL::operator -= (const ComplexKRL &complex)
{
	// Subtract the real components
	real -= complex.real;

	// Subtract the imaginary components
	imaginary -= complex.imaginary;

	return *this;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator *=
//
// Description:		Multiplication assignment operator for the ComplexKRL class.
//
// Input Arguments:
//		complex	= const Complex& to multiply by this
//
// Output Arguments:
//		None
//
// Return Value:
//		ComplexKRL& result of the multiplication
//
//==========================================================================
ComplexKRL& ComplexKRL::operator *= (const ComplexKRL &complex)
{
	double temp = real;

	// Similar to a dot product, the real component of the result
	// is the sum of the products of the like components
	real = real * complex.real - imaginary * complex.imaginary;

	// Similar to a cross product, the imaginary component of the
	// result is the sum of the products of the opposite components
	imaginary = temp * complex.imaginary + imaginary * complex.real;

	return *this;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator /=
//
// Description:		Division assignment operator for the ComplexKRL class.
//
// Input Arguments:
//		complex	= const ComplexKRL& to divide by this
//
// Output Arguments:
//		None
//
// Return Value:
//		ComplexKRL& result of the division
//
//==========================================================================
ComplexKRL& ComplexKRL::operator /= (const ComplexKRL &complex)
{
	double temp = real;

	// Compute the real portion of the result
	real = (real * complex.real + imaginary * complex.imaginary) /
		(complex.real * complex.real + complex.imaginary * complex.imaginary);

	// Compute the imaginary portion of the result
	imaginary = (imaginary * complex.real - temp * complex.imaginary) /
		(complex.real * complex.real + complex.imaginary * complex.imaginary);

	return *this;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator ==
//
// Description:		Equal to comparison operator for the ComplexKRL class.
//
// Input Arguments:
//		complex	= const ComplexKRL& to compare to this
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for equal to, false for not equal to
//
//==========================================================================
bool ComplexKRL::operator == (const ComplexKRL &complex) const
{
	// Check to see if both the real and imaginary components are equal
	if (real == complex.real && imaginary == complex.imaginary)
		return true;
	else
		return false;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator !=
//
// Description:		Not equal to comparison operator for the ComplexKRL class.
//
// Input Arguments:
//		complex	= const ComplexKRL& to compare to this
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, false for equal to, true for not equal to
//
//==========================================================================
bool ComplexKRL::operator != (const ComplexKRL &complex) const
{
	return !(*this == complex);
}

//==========================================================================
// Class:			ComplexKRL
// Function:		GetConjugate
//
// Description:		Returns the complex conjugate of this object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const ComplexKRL with the same real part as this and an imaginary component
//		with opposite magnitude as this
//
//==========================================================================
const ComplexKRL ComplexKRL::GetConjugate(void) const
{
	ComplexKRL temp;

	// Direct assignment of the real component
	temp.real = real;

	// The imaginary part is the opposite of this
	temp.imaginary = -imaginary;

	return temp;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator +
//
// Description:		Addition operator for the ComplexKRL class.
//
// Input Arguments:
//		value	= const double& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		const ComplexKRL result of the addition
//
//==========================================================================
const ComplexKRL ComplexKRL::operator + (const double &value) const
{
	ComplexKRL temp;

	// Add the real component
	temp.real = real + value;

	// Direct assignment of the imaginary componet
	temp.imaginary = imaginary;

	return temp;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator -
//
// Description:		Subtraction operator for the ComplexKRL class.
//
// Input Arguments:
//		value	= const double& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		const ComplexKRL result of the subtraction
//
//==========================================================================
const ComplexKRL ComplexKRL::operator - (const double &value) const
{
	ComplexKRL temp;

	// Subtract the real component
	temp.real = real - value;

	// Direct assignment of the imaginary component
	temp.imaginary = imaginary;

	return temp;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator *
//
// Description:		Multiplication operator for the ComplexKRL class.
//
// Input Arguments:
//		value	= const double& to multiply by this
//
// Output Arguments:
//		None
//
// Return Value:
//		const ComplexKRL result of the multiplication
//
//==========================================================================
const ComplexKRL ComplexKRL::operator * (const double &value) const
{
	ComplexKRL temp;

	// Perform the multiplication on both components
	temp.real = real * value;
	temp.imaginary = imaginary * value;

	return temp;
}

//==========================================================================
// Class:			ComplexKRL
// Function:		operator /
//
// Description:		Division operator for the ComplexKRL class.
//
// Input Arguments:
//		value	= const double& to divide by this
//
// Output Arguments:
//		None
//
// Return Value:
//		const ComplexKRL result of the division
//
//==========================================================================
const ComplexKRL ComplexKRL::operator / (const double &value) const
{
	ComplexKRL temp;

	// Perform the division on both components
	temp.real = real / value;
	temp.imaginary = imaginary / value;

	return temp;
}

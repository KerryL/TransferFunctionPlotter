/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  complex.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class declaration for complex number class.
// History:

#ifndef _Complex_H_
#define _Complex_H_

using namespace std;

// wxWidgets forward declarations
class wxString;

class Complex
{
public:
	// Constructor
	Complex();
	Complex(const double &_real, const double &_imaginary);

	// Destructor
	~Complex();

	// Prints the value to a string
	wxString Print(void) const;

	// Gets the complex conjugate of this object
	const Complex GetConjugate(void) const;

	double GetPolarLength(void) const;
	double GetPolarAngle(void) const;

	// Operators
	const Complex operator + (const Complex &complex) const;
	const Complex operator - (const Complex &complex) const;
	const Complex operator * (const Complex &complex) const;
	const Complex operator / (const Complex &complex) const;
	Complex& operator += (const Complex &complex);
	Complex& operator -= (const Complex &complex);
	Complex& operator *= (const Complex &complex);
	Complex& operator /= (const Complex &complex);
	bool operator == (const Complex &complex) const;
	bool operator != (const Complex &complex) const;
	const Complex operator + (const double &value) const;
	const Complex operator - (const double &value) const;
	const Complex operator * (const double &value) const;
	const Complex operator / (const double &value) const;

	// Raises this object to the specified power
	Complex& ToPower(const double &power);
	const Complex ToPower(const double &power) const;
	Complex& ToPower(const Complex &power);
	const Complex ToPower(const Complex &power) const;

	// For streaming the value
	friend ostream &operator << (ostream &writeOut, const Complex &complex);

	// The actual data contents of this class
	double real;
	double imaginary;

	// Defining the square root of negative 1
	static const Complex i;
};

#endif// _Complex_H_
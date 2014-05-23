/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  complex_krl.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class declaration for complex number class.
// History:

#ifndef COMPLEX_KRL_H_
#define COMPLEX_KRL_H_

// Standard C++ headers
#include <ostream>

// wxWidgets forward declarations
class wxString;

class ComplexKRL
{
public:
	// Constructor
	ComplexKRL();
	ComplexKRL(const double &_real, const double &_imaginary);

	// Destructor
	~ComplexKRL();

	// Prints the value to a string
	wxString Print(void) const;

	// Gets the complex conjugate of this object
	const ComplexKRL GetConjugate(void) const;

	double GetPolarLength(void) const;
	double GetPolarAngle(void) const;

	// Operators
	const ComplexKRL operator + (const ComplexKRL &complex) const;
	const ComplexKRL operator - (const ComplexKRL &complex) const;
	const ComplexKRL operator * (const ComplexKRL &complex) const;
	const ComplexKRL operator / (const ComplexKRL &complex) const;
	ComplexKRL& operator += (const ComplexKRL &complex);
	ComplexKRL& operator -= (const ComplexKRL &complex);
	ComplexKRL& operator *= (const ComplexKRL &complex);
	ComplexKRL& operator /= (const ComplexKRL &complex);
	bool operator == (const ComplexKRL &complex) const;
	bool operator != (const ComplexKRL &complex) const;
	const ComplexKRL operator + (const double &value) const;
	const ComplexKRL operator - (const double &value) const;
	const ComplexKRL operator * (const double &value) const;
	const ComplexKRL operator / (const double &value) const;

	// Raises this object to the specified power
	ComplexKRL& ToPower(const double &power);
	const ComplexKRL ToPower(const double &power) const;
	ComplexKRL& ToPower(const ComplexKRL &power);
	const ComplexKRL ToPower(const ComplexKRL &power) const;

	// For streaming the value
	friend std::ostream &operator << (std::ostream &writeOut, const ComplexKRL &complex);

	// The actual data contents of this class
	double real;
	double imaginary;

	// Defining the square root of negative 1
	static const ComplexKRL i;
};

#endif// COMPLEX_KRL_H_

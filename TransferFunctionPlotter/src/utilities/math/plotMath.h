/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotMath.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains useful functions that don't fit better in another class.  Hopefully this
//				 file will one day be absolved into a real class instead of just being a kludgy
//				 collection of functions.
// History:

#ifndef _PLOT_MATH_H_
#define _PLOT_MATH_H_

// Standard C++ headers
#include <limits>// For QNaN
#include <vector>

// Local headers
#include "utilities/math/complex.h"

// Local forward declarations
class Vector;
class Dataset2D;

namespace PlotMath
{

	// Constant declarations
	const double NEARLY_ZERO = 1.0e-12;
	const double QNAN = std::numeric_limits<double>::quiet_NaN();
	const double pi = 3.141592653589793238462643;

	// Prototypes =====================================================
	// For determining if a number is close enough to zero to regard as zero
	bool IsZero(const double &toCheck);
	bool IsZero(const Vector &toCheck);

	template<typename T>
	bool IsNaN(const T &value);

	template<typename T>
	bool IsInf(const T &value);

	template<typename T>
	bool IsValid(const T &value);

	// Ensure the value is between two definined limits
	double Clamp(const double &value, const double &lowerLimit, const double &upperLimit);

	double RangeToPlusMinusPi(const double &angle);
	double RangeToPlusMinus180(const double &angle);

	void Unwrap(Dataset2D &data);

	// Returns the sign of the argument
	double Sign(const double &value);

	// Modulo operator defined for doubles (so not really a modulo, I guess)
	double Modulo(const double &value, const double &div);

	Dataset2D ApplyBitMask(const Dataset2D &data, const unsigned int &bit);
	unsigned int ApplyBitMask(const unsigned &value, const unsigned int &bit);

	Dataset2D GetMagnitudeData(const std::vector<Complex> &data);
	Dataset2D GetPhaseData(const std::vector<Complex> &data);

	Dataset2D ConvertToDecibels(const Dataset2D &data);

	template<typename T>
	std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b);
	template<typename T>
	std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b);
	template<typename T>
	std::vector<T> operator*(const std::vector<T>& a, const std::vector<T>& b);
	template<typename T>
	std::vector<T> operator/(const std::vector<T>& a, const std::vector<T>& b);
}

// Template methods must be defined here:
//==========================================================================
// Namespace:		PlotMath
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//
// Input Arguments:
//		value	= const T& to check
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//==========================================================================
template<typename T>
bool PlotMath::IsNaN(const T &value)
{
	return value != value;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		IsInf
//
// Description:		Determines if the specified number is infinite.
//
// Input Arguments:
//		value	= const T&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is ininite
//
//==========================================================================
template<typename T>
bool PlotMath::IsInf(const T &value)
{
	return std::numeric_limits<T>::has_infinity &&
		value == std::numeric_limits<T>::infinity();
}

//==========================================================================
// Namespace:		PlotMath
// Function:		IsValid
//
// Description:		Determines if the specified value is a valid number.
//
// Input Arguments:
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is valid
//
//==========================================================================
template<typename T>
bool PlotMath::IsValid(const T &value)
{
	return !IsNaN<T>(value) && !IsInf<T>(value);
}

template<typename T>
std::vector<T> PlotMath::operator+(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> result(a.size());

	unsigned int i;
	for (i = 0; i < result.size(); i++)
		result[i] = a[i] + b[i];

	return result;
}

template<typename T>
std::vector<T> PlotMath::operator-(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> result(a.size());

	unsigned int i;
	for (i = 0; i < result.size(); i++)
		result[i] = a[i] - b[i];

	return result;
}

template<typename T>
std::vector<T> PlotMath::operator*(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> result(a.size());

	unsigned int i;
	for (i = 0; i < result.size(); i++)
		result[i] = a[i] * b[i];

	return result;
}

template<typename T>
std::vector<T> PlotMath::operator/(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> result(a.size());

	unsigned int i;
	for (i = 0; i < result.size(); i++)
		result[i] = a[i] / b[i];

	return result;
}

#endif// _PLOT_MATH_H_
/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotMath.cpp
// Created:  3/24/2008
// Author:  K. Loux
// Description:  Contains useful functions that don't fit better in another class.  Hopefully this
//				 file will one day be absolved into a real class instead of just being a kludgy
//				 collection of functions.
// History:

// Standard C++ headers
#include <cstdlib>
#include <assert.h>
#include <limits>

// Local headers
#include "utilities/math/plotMath.h"
#include "utilities/math/vector.h"
#include "utilities/math/matrix.h"
#include "utilities/dataset2D.h"

//==========================================================================
// Namespace:		PlotMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//
// Input Arguments:
//		toCheck	= const double& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the number is less than NEARLY_ZERO
//
//==========================================================================
bool PlotMath::IsZero(const double &toCheck)
{
	if (fabs(toCheck) < NEARLY_ZERO)
		return true;
	else
		return false;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//					This function checks the magnitude of the VECTOR.
//
// Input Arguments:
//		toCheck	= const Vector& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the magnitude is less than NEARLY_ZERO
//
//==========================================================================
bool PlotMath::IsZero(const Vector &toCheck)
{
	// Check each component of the vector
	if (toCheck.Length() < NEARLY_ZERO)
		return true;

	return false;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		Clamp
//
// Description:		Ensures the specified value is between the limits.  In the
//					event that the value is out of the specified bounds, the
//					value that is returned is equal to the limit that the value
//					has exceeded.
//
// Input Arguments:
//		value		= const double& reference to the value which we want to clamp
//		lowerLimit	= const double& lower bound of allowable values
//		upperLimit	= const double& upper bound of allowable values
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the clamped value
//
//==========================================================================
double PlotMath::Clamp(const double &value, const double &lowerLimit, const double &upperLimit)
{
	// Make sure the arguments are valid
	assert(lowerLimit < upperLimit);

	if (value < lowerLimit)
		return lowerLimit;
	else if (value > upperLimit)
		return upperLimit;

	return value;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		RangeToPlusMinusPi
//
// Description:		Adds or subtracts 2 * pi to the specified angle until the
//					angle is between -pi and pi.
//
// Input Arguments:
//		angle		= const double& reference to the angle we want to bound
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the re-ranged angle
//
//==========================================================================
double PlotMath::RangeToPlusMinusPi(const double &angle)
{
	return Modulo(angle + pi, 2.0 * pi) - pi;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		RangeToPlusMinus180
//
// Description:		Adds or subtracts 180 to the specified angle until the
//					angle is between -180 and 180.
//
// Input Arguments:
//		angle		= const double& reference to the angle we want to bound
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the re-ranged angle
//
//==========================================================================
double PlotMath::RangeToPlusMinus180(const double &angle)
{
	return Modulo(angle + 180.0, 360.0) - 180.0;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		Unwrap
//
// Description:		Minimizes the jump between adjacent points by adding/subtracting
//					multiples of 2 * pi.
//
// Input Arguments:
//		data	= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotMath::Unwrap(Dataset2D &data)
{
	double threshold(pi);
	unsigned int i;
	for (i = 1; i < data.GetNumberOfPoints(); i++)
	{
		if (data.GetYData(i) - data.GetYData(i - 1) > threshold)
			data.GetYPointer()[i] -= 2 * pi;
		if (data.GetYData(i) - data.GetYData(i - 1) < -threshold)
			data.GetYPointer()[i] += 2 * pi;
	}
}

//==========================================================================
// Namespace:		PlotMath
// Function:		Sign
//
// Description:		Returns 1.0 for positive, -1.0 for negative and 0.0 for zero.
//
// Input Arguments:
//		value		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotMath::Sign(const double &value)
{
	if (value > 0.0)
		return 1.0;
	else if (value < 0.0)
		return -1.0;
	else
		return 0.0;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		Modulo
//
// Description:		Modulo for doubles.  Returns a value between zero and divisor.
//
// Input Arguments:
//		value		= const double&
//		div			= cosnt double&, divisor
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotMath::Modulo(const double &value, const double &div)
{
	double modulo(value);
	while (modulo >= fabs(div))
		modulo -= div;
	while (modulo < 0.0)
		modulo += div;

	return modulo;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		ApplyBitMask
//
// Description:		Extracts a single bit from values of the specified dataset.
//
// Input Arguments:
//		data	= const Dataset2D&
//		bit		= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
Dataset2D PlotMath::ApplyBitMask(const Dataset2D &data, const unsigned int &bit)
{
	Dataset2D set(data);
	unsigned int i;
	for (i = 0; i < set.GetNumberOfPoints(); i++)
		set.GetYPointer()[i] = ApplyBitMask((unsigned int)set.GetYPointer()[i], bit);
	return set;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		ApplyBitMask
//
// Description:		Extracts a single bit from the value.
//
// Input Arguments:
//		value	= const unsigned int&
//		bit		= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
unsigned int PlotMath::ApplyBitMask(const unsigned &value, const unsigned int &bit)
{
	return (value >> bit) & 1;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		GetMagnitudeData
//
// Description:		Creates a dataset containing the magnitude of the specified
//					complex-valued vector.  Note that the x-component of the
//					dataset will be uninitialized.
//
// Input Arguments:
//		data	= const std::vector<ComplexKRL>&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D PlotMath::GetMagnitudeData(const std::vector<ComplexKRL> &data)
{
	Dataset2D result(data.size());

	unsigned int i;
	for (i = 0; i < result.GetNumberOfPoints(); i++)
		result.GetYPointer()[i] = sqrt(data[i].real * data[i].real + data[i].imaginary * data[i].imaginary);

	return result;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		GetPhaseData
//
// Description:		Creates a dataset containing the phase of the specified
//					complex-valued vector.  Note that the x-component of the
//					dataset will be uninitialized.
//
// Input Arguments:
//		data	= const std::vector<ComplexKRL>&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D PlotMath::GetPhaseData(const std::vector<ComplexKRL> &data)
{
	Dataset2D result(data.size());

	unsigned int i;
	for (i = 0; i < result.GetNumberOfPoints(); i++)
		result.GetYPointer()[i] = atan2(data[i].imaginary, data[i].real);

	Unwrap(result);
	result *= 180.0 / pi;

	return result;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		ConvertToDecibels
//
// Description:		Converts the specified magnitude data to decibels.
//
// Input Arguments:
//		data	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D PlotMath::ConvertToDecibels(const Dataset2D &data)
{
	Dataset2D results(data);

	unsigned int i;
	for (i = 0; i < results.GetNumberOfPoints(); i++)
		results.GetYPointer()[i] = 20.0 * log10(data.GetYData(i));

	return results;
}

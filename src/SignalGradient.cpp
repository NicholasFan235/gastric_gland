/*

Copyright (c) 2005-2021, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "SignalGradient.hpp"

/** Pointer to the single instance */
template<unsigned DIM>
SignalGradient<DIM>* SignalGradient<DIM>::mpInstance = nullptr;
template<unsigned DIM>
SignalGradient<DIM>* SignalGradient<DIM>::mpBmpInstance = nullptr;
template<unsigned DIM>
SignalGradient<DIM>* SignalGradient<DIM>::mpEgfInstance = nullptr;

template<unsigned DIM>
SignalGradient<DIM>* SignalGradient<DIM>::Instance()
{
    if (mpInstance == nullptr)
    {
        mpInstance = new SignalGradient;
    }
    return mpInstance;
}
template<unsigned DIM>
SignalGradient<DIM>* SignalGradient<DIM>::Instance()
{
    if (mpBmpInstance == nullptr)
    {
        mpBmpInstance = new SignalGradient;
    }
    return mpBmpInstance;
}
template<unsigned DIM>
SignalGradient<DIM>* SignalGradient<DIM>::Instance()
{
    if (mpEgfInstance == nullptr)
    {
        mpEgfInstance = new SignalGradient;
    }
    return mpEgfInstance;
}

template<unsigned DIM>
SignalGradient<DIM>::SignalGradient()
    : mCryptLength(DOUBLE_UNSET),
      mLengthSet(false),
      mType(NONE),
      mpCellPopulation(nullptr),
      mTypeSet(false),
      mConstantValueForTesting(0),
      mUseConstantValueForTesting(false),
      mConcentrationParameter(1.0),
      mCryptProjectionParameterA(0.5),
      mCryptProjectionParameterB(2.0)
{
    // Make sure there's only one instance - enforces correct serialization
    assert(mpInstance == nullptr);
}

template<unsigned DIM>
SignalGradient<DIM>::~SignalGradient()
{
}

template<unsigned DIM>
void SignalGradient<DIM>::Destroy()
{
    if (mpInstance)
    {
        delete mpInstance;
        mpInstance = nullptr;
    }
    if (mpBmpInstance)
    {
        delete mpBmpInstance;
        mpBmpInstance = nullptr;
    }
    if (mpEgfInstance)
    {
        delete mpEgfInstance;
        mpEgfInstance = nullptr;
    }
}

template<unsigned DIM>
double SignalGradient<DIM>::GetLevel(CellPtr pCell)
{
    if (mUseConstantValueForTesting)  // to test a cell and cell-cycle models without a cell population
    {
        return mConstantValueForTesting;
    }

    assert(mpCellPopulation!=nullptr);
    assert(mTypeSet);
    assert(mLengthSet);

    double height;

    if (mWntType == RADIAL)
    {
        double a = GetCryptProjectionParameterA();
        double b = GetCryptProjectionParameterB();
        height = a*pow(norm_2(mpCellPopulation->GetLocationOfCellCentre(pCell)), b);
    }
    else
    {
        height = mpCellPopulation->GetLocationOfCellCentre(pCell)[DIM-1];
    }

    return GetLevel(height);
}

template<unsigned DIM>
c_vector<double, DIM> SignalGradient<DIM>::GetGradient(CellPtr pCell)
{
    if (mUseConstantValueForTesting)  // to test a cell and cell-cycle models without a cell population
    {
        return zero_vector<double>(DIM);
    }
    assert(mpCellPopulation!=nullptr);
    assert(mTypeSet);
    assert(mLengthSet);

    c_vector<double, DIM> location_of_cell = mpCellPopulation->GetLocationOfCellCentre(pCell);

    return GetGradient(location_of_cell);
}

template<unsigned DIM>
void SignalGradient<DIM>::SetCellPopulation(AbstractCellPopulation<DIM>& rCellPopulation)
{
    mpCellPopulation = &rCellPopulation;
}

template<unsigned DIM>
AbstractCellPopulation<DIM>& SignalGradient<DIM>::rGetCellPopulation()
{
    return *mpCellPopulation;
}

template<unsigned DIM>
double SignalGradient<DIM>::GetCryptLength()
{
    return mCryptLength;
}

template<unsigned DIM>
void SignalGradient<DIM>::SetCryptLength(double cryptLength)
{
    assert(cryptLength > 0.0);
    if (mLengthSet==true)
    {
        EXCEPTION("Destroy has not been called");
    }

    mCryptLength = cryptLength;
    mLengthSet = true;
}

template<unsigned DIM>
SignalGradientType SignalGradient<DIM>::GetType()
{
    return mType;
}

template<unsigned DIM>
void SignalGradient<DIM>::SetType(SignalGradientType type)
{
    if (mTypeSet==true)
    {
        EXCEPTION("Destroy has not been called");
    }
    mWntType = type;
    mTypeSet = true;
}

template<unsigned DIM>
double SignalGradient<DIM>::GetLevel(double height)
{
    if (mWntType == NONE)
    {
        return 0.0;
    }

    // Need to call SetCryptLength first
    assert(mLengthSet);

    double wnt_level = -1.0; // Test this is changed before leaving method.

    // The first type of Wnt concentration to try
    if (mType==LINEAR || mType==RADIAL)
    {
        if ((height >= -1e-9) && (height < mConcentrationParameter*GetCryptLength()))
        {
            wnt_level = 1.0 - height/(mConcentrationParameter*GetCryptLength());
        }
        else
        {
            wnt_level = 0.0;
        }
    }

    if (mType==EXPONENTIAL)
    {
        if ((height >= -1e-9) && (height < GetCryptLength()))
        {
            wnt_level = exp(-height/(GetCryptLength()*mConcentrationParameter));
        }
        else
        {
            wnt_level = 0.0;
        }
    }

    if (mType==NEGLINEAR)
    {
        if ((height >= -1e-9) && (height < mConcentrationParameter*GetCryptLength()))
        {
            wnt_level = height/(mConcentrationParameter*GetCryptLength());
        }
        else
        {
            wnt_level = 0.0;
        }
    }

    if (mType==NEGEXPONENTIAL)
    {
        if ((height >= -1e-9) && (height < GetCryptLength()))
        {
            wnt_level = exp(-(GetCryptLength()-height)/(GetCryptLength()*mConcentrationParameter));
        }
        else
        {
            wnt_level = 0.0;
        }
    }

    assert(wnt_level >= 0.0);

    return wnt_level;
}

template<unsigned DIM>
c_vector<double, DIM> SignalGradient<DIM>::GetGradient(c_vector<double, DIM>& rLocation)
{
    c_vector<double, DIM> wnt_gradient = zero_vector<double>(DIM);

    if (mType!=NONE)
    {
        if (mType==LINEAR)
        {
            if ((rLocation[DIM-1] >= -1e-9) && (rLocation[DIM-1] < mConcentrationParameter*GetCryptLength()))
            {
                wnt_gradient[DIM-1] = -1.0/(mConcentrationParameter*GetCryptLength());
            }
        }
        else if (mType==RADIAL) // RADIAL Wnt concentration
        {
            double a = GetCryptProjectionParameterA();
            double b = GetCryptProjectionParameterB();
            double r = norm_2(rLocation);
            double r_critical = pow(mConcentrationParameter*GetCryptLength()/a, 1.0/b);

            double dwdr = 0.0;

            if (r>=-1e-9 && r<r_critical)
            {
                dwdr = -mConcentrationParameter*GetCryptLength()*pow(r, b-1.0)/a;
            }

            for (unsigned i=0; i<DIM; i++)
            {
                wnt_gradient[i] = rLocation[i]*dwdr/r;
            }
        }
        else if (mType == NEGLINEAR)
        {
            if ((rLocation[DIM-1] >= -1e-9) && (rLocation[DIM-1] < mConcentrationParameter*GetCryptLength()))
            {
                wnt_gradient[DIM-1] = 1.0/(mConcentrationParameter*GetCryptLength());
            }
        }
        else
        {
            EXCEPTION("No method to calculate gradient of this signal gradient type");
        }
    }
    return wnt_gradient;
}

template<unsigned DIM>
bool SignalGradient<DIM>::IsSetUp()
{
    bool result = false;
    if (mTypeSet && mLengthSet && mpCellPopulation!=nullptr && mType!=NONE)
    {
        result = true;
    }
    return result;
}

template<unsigned DIM>
void SignalGradient<DIM>::SetConstantValueForTesting(double value)
{
    if (value < 0)
    {
        EXCEPTION("SignalGradient<DIM>::SetConstantWntValueForTesting - Wnt value for testing should be non-negative.\n");
    }
    mConstantValueForTesting = value;
    mUseConstantValueForTesting = true;
    if (!mTypeSet)
    {
        mType = NONE;
    }
}

template<unsigned DIM>
double SignalGradient<DIM>::GetConcentrationParameter()
{
    return mConcentrationParameter;
}

template<unsigned DIM>
void SignalGradient<DIM>::SetConcentrationParameter(double concentrationParameter)
{
    assert(concentrationParameter > 0.0);
    mConcentrationParameter = concentrationParameter;
}

template<unsigned DIM>
double SignalGradient<DIM>::GetCryptProjectionParameterA()
{
    return mCryptProjectionParameterA;
}

template<unsigned DIM>
double SignalGradient<DIM>::GetCryptProjectionParameterB()
{
    return mCryptProjectionParameterB;
}

template<unsigned DIM>
void SignalGradient<DIM>::SetCryptProjectionParameterA(double cryptProjectionParameterA)
{
    assert(cryptProjectionParameterA >= 0.0);
    mCryptProjectionParameterA = cryptProjectionParameterA;
}

template<unsigned DIM>
void SignalGradient<DIM>::SetCryptProjectionParameterB(double cryptProjectionParameterB)
{
    assert(cryptProjectionParameterB >= 0.0);
    mCryptProjectionParameterB = cryptProjectionParameterB;
}

// Explicit instantiation
template class SignalGradient<1>;
template class SignalGradient<2>;
template class SignalGradient<3>;

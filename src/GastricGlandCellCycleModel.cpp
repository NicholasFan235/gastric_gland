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

#include "GastricGlandCellCycleModel.hpp"
#include "Exception.hpp"
#include "StemCellProliferativeType.hpp"
#include "TransitCellProliferativeType.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "FoveolarCellProliferativeType.hpp"
#include "NeckCellProliferativeType.hpp"
#include "CellLabel.hpp"
#include "WildTypeCellMutationState.hpp"
#include "ApcOneHitCellMutationState.hpp"
#include "ApcTwoHitCellMutationState.hpp"
#include "BetaCateninOneHitCellMutationState.hpp"

GastricGlandCellCycleModel::GastricGlandCellCycleModel() :
  mIsthmusBeginHeight(0.7),
  mIsthmusEndHeight(0.8),
  mBaseHeight(0.02)
{
    SetTransitCellG1Duration(10.0);
}

GastricGlandCellCycleModel::GastricGlandCellCycleModel(const GastricGlandCellCycleModel& rModel)
   : AbstractSimplePhaseBasedCellCycleModel(rModel),
   mIsthmusBeginHeight(rModel.mIsthmusBeginHeight),
   mIsthmusEndHeight(rModel.mIsthmusEndHeight),
   mBaseHeight(rModel.mBaseHeight)
{
    /*
     * Initialize only those member variables defined in this class.
     *
     * The member variables mCurrentCellCyclePhase, mG1Duration,
     * mMinimumGapDuration, mStemCellG1Duration, mTransitCellG1Duration,
     * mSDuration, mG2Duration and mMDuration are initialized in the
     * AbstractPhaseBasedCellCycleModel constructor.
     *
     * The member variables mBirthTime, mReadyToDivide and mDimension
     * are initialized in the AbstractCellCycleModel constructor.
     *
     * Note that mG1Duration and the cell's proliferative type are
     * (re)set as soon as InitialiseDaughterCell() is called on the
     * new cell-cycle model.
     */
    SetTransitCellG1Duration(10.0);
}

AbstractCellCycleModel* GastricGlandCellCycleModel::CreateCellCycleModel()
{
    return new GastricGlandCellCycleModel(*this);
}

void GastricGlandCellCycleModel::SetG1Duration()
{
    assert(mpCell != nullptr);

    RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();

    if (mpCell->GetCellProliferativeType()->IsSubType<StemCellProliferativeType>() ||
        mpCell->GetCellProliferativeType()->IsSubType<TransitCellProliferativeType>())
    {
        mG1Duration = p_gen->NormalRandomDeviate(GetTransitCellG1Duration(), 1.0);
    }
    else if (mpCell->GetCellProliferativeType()->IsSubType<DifferentiatedCellProliferativeType>())
    {
        mG1Duration = DBL_MAX; //p_gen->NormalRandomDeviate(GetTransitCellG1Duration(), 1.0);
    }
    else
    {
        NEVER_REACHED;
    }

    // Check that the normal random deviate has not returned a small or negative G1 duration
    if (mG1Duration < mMinimumGapDuration)
    {
        mG1Duration = mMinimumGapDuration;
    }
}

void GastricGlandCellCycleModel::UpdateCellCyclePhase()
{
    assert(mpCell != nullptr);
    
    if (GetWntType() != LINEAR)
    {
        EXCEPTION("Gastric gland cell cycle model only allowed for LINEAR Wnt concentration.");
    }
    double height = 1-GetWntLevel();

    // Allow the cell to divide if in either Base or Isthmus region
    // Use Wnt signal strength to determine position in gland
    if (height < mBaseHeight)
    {
        // If in Base
        // Make transit cell
        if (!mpCell->GetCellProliferativeType()->IsType<TransitCellProliferativeType>())
        {
            boost::shared_ptr<AbstractCellProperty> p_transit_type =
                mpCell->rGetCellPropertyCollection().GetCellPropertyRegistry()->Get<TransitCellProliferativeType>();
            // Change the cell's type if it just transitioned
            mpCell->SetCellProliferativeType(p_transit_type);
            SetG1Duration(); // Reset the G1 duration to a transit cell's G1 duration
            mG1Duration += GetAge(); // Put cell at start of G1 phase, otherwise aged cell will immediately divide
        }
    }
    else if (height < mIsthmusBeginHeight)
    {
        // If in Neck
        if (!mpCell->GetCellProliferativeType()->IsType<NeckCellProliferativeType>()
            && (mCurrentCellCyclePhase == G_ONE_PHASE || mCurrentCellCyclePhase == G_ZERO_PHASE))
        {
            boost::shared_ptr<AbstractCellProperty> p_neck_type =
                mpCell->rGetCellPropertyCollection().GetCellPropertyRegistry()->Get<NeckCellProliferativeType>();
            mpCell->SetCellProliferativeType(p_neck_type);
        }
    }
    else if (height < mIsthmusEndHeight)
    {
        // If in Isthmus
        // Make transit cell
        if (!mpCell->GetCellProliferativeType()->IsType<TransitCellProliferativeType>())
        {
            boost::shared_ptr<AbstractCellProperty> p_transit_type =
                mpCell->rGetCellPropertyCollection().GetCellPropertyRegistry()->Get<TransitCellProliferativeType>();
            // Change the cell's type if it just transitioned
            mpCell->SetCellProliferativeType(p_transit_type);
            SetG1Duration(); // Reset the G1 duration to a transit cell's G1 duration
            mG1Duration += GetAge(); // Put cell at start of G1 phase, otherwise aged cell will immediately divide
        }
    }
    else
    {
        // If in Foveolar
        if (!mpCell->GetCellProliferativeType()->IsType<FoveolarCellProliferativeType>()
            && (mCurrentCellCyclePhase == G_ONE_PHASE || mCurrentCellCyclePhase == G_ZERO_PHASE))
        {
            boost::shared_ptr<AbstractCellProperty> p_neck_type =
                mpCell->rGetCellPropertyCollection().GetCellPropertyRegistry()->Get<FoveolarCellProliferativeType>();
            mpCell->SetCellProliferativeType(p_neck_type);
        }
    }
    

    double time_since_birth = GetAge();
    assert(time_since_birth >= 0);

    if (mpCell->GetCellProliferativeType()->IsSubType<DifferentiatedCellProliferativeType>())
    {
        mCurrentCellCyclePhase = G_ZERO_PHASE;
    }
    else if (time_since_birth < GetMDuration())
    {
        mCurrentCellCyclePhase = M_PHASE;
    }
    else if (time_since_birth < GetMDuration() + mG1Duration)
    {
        mCurrentCellCyclePhase = G_ONE_PHASE;
    }
    else if (time_since_birth < GetMDuration() + mG1Duration + GetSDuration())
    {
        mCurrentCellCyclePhase = S_PHASE;
    }
    else if (time_since_birth < GetMDuration() + mG1Duration + GetSDuration() + GetG2Duration())
    {
        mCurrentCellCyclePhase = G_TWO_PHASE;
    }
}

void GastricGlandCellCycleModel::InitialiseDaughterCell()
{
    AbstractSimplePhaseBasedCellCycleModel::InitialiseDaughterCell();
}

bool GastricGlandCellCycleModel::CanCellTerminallyDifferentiate()
{
    return false;
}

double GastricGlandCellCycleModel::GetIsthmusBeginHeight() const { return mIsthmusBeginHeight; }
void GastricGlandCellCycleModel::SetIsthmusBeginHeight(double height) { mIsthmusBeginHeight = height; }

double GastricGlandCellCycleModel::GetIsthmusEndHeight() const { return mIsthmusEndHeight; }
void GastricGlandCellCycleModel::SetIsthmusEndHeight(double height) { mIsthmusEndHeight = height; }

double GastricGlandCellCycleModel::GetBaseHeight() const { return mBaseHeight; }
void GastricGlandCellCycleModel::SetBaseHeight(double height) { mBaseHeight = height; }

double GastricGlandCellCycleModel::GetWntLevel() const
{
    assert(mpCell != nullptr);
    double level = 0;

    switch (mDimension)
    {
        case 1:
        {
            const unsigned DIM = 1;
            level = WntConcentration<DIM>::Instance()->GetWntLevel(mpCell);
            break;
        }
        case 2:
        {
            const unsigned DIM = 2;
            level = WntConcentration<DIM>::Instance()->GetWntLevel(mpCell);
            break;
        }
        case 3:
        {
            const unsigned DIM = 3;
            level = WntConcentration<DIM>::Instance()->GetWntLevel(mpCell);
            break;
        }
        default:
            NEVER_REACHED;
    }
    return level;
}

WntConcentrationType GastricGlandCellCycleModel::GetWntType()
{
    WntConcentrationType wnt_type;
    switch (mDimension)
    {
        case 1:
        {
            const unsigned DIM = 1;
            wnt_type = WntConcentration<DIM>::Instance()->GetType();
            break;
        }
        case 2:
        {
            const unsigned DIM = 2;
            wnt_type = WntConcentration<DIM>::Instance()->GetType();
            break;
        }
        case 3:
        {
            const unsigned DIM = 3;
            wnt_type = WntConcentration<DIM>::Instance()->GetType();
            break;
        }
        case UNSIGNED_UNSET:
        {
            // If you trip this you have tried to use a simulation without setting the dimension.
            NEVER_REACHED;
        }
        default:
            NEVER_REACHED;
    }
    return wnt_type;
}

void GastricGlandCellCycleModel::OutputCellCycleModelParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<IsthmusBeginHeight>" << mIsthmusBeginHeight << "</IsthmusBeginHeight>\n";
    *rParamsFile << "\t\t\t<IsthmusEndHeight>" << mIsthmusEndHeight << "</IsthmusEndHeight>\n";
    *rParamsFile << "\t\t\t<BaseHeight>" << mBaseHeight << "</BaseHeight>\n";

    // Call method on direct parent class
    AbstractSimplePhaseBasedCellCycleModel::OutputCellCycleModelParameters(rParamsFile);
}

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
CHASTE_CLASS_EXPORT(GastricGlandCellCycleModel)

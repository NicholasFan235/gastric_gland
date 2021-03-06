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

#ifndef GASTRICGLANDCELLCYCLEMODEL_HPP
#define GASTRICGLANDCELLCYCLEMODEL_HPP

#include "AbstractSimplePhaseBasedCellCycleModel.hpp"
#include "RandomNumberGenerator.hpp"
#include "WntConcentration.hpp"

/**
 * Simple Wnt-dependent cell-cycle model.
 */
class GastricGlandCellCycleModel : public AbstractSimplePhaseBasedCellCycleModel
{
private:

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive the cell-cycle model, never used directly - boost uses this.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractSimplePhaseBasedCellCycleModel>(*this);

        RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
        archive & *p_gen;
    }

protected:

    /**
     * Whether to use different mean G1 durations for different cell types.
     * For use in SetG1Duration().
     */

    double mIsthmusBeginHeight;
    double mIsthmusEndHeight;
    double mBaseHeight;

    /**
     * @return the Wnt level experienced by the cell.
     */
    double GetWntLevel() const;

    /**
     * @return the type of Wnt concentration (LINEAR, RADIAL, EXPONENTIAL or NONE).
     * This affects how the cell cycle phase is updated.
     */
    WntConcentrationType GetWntType();

    /**
     * Stochastically set the G1 duration. The G1 duration is taken
     * from a normal distribution whose mean is the G1 duration given
     * in AbstractPhaseBasedCellCycleModel, for the cell type, and whose standard deviation
     * is 1.
     *
     * Called on cell creation at the start of a simulation, and for both
     * parent and daughter cells at cell division.
     */
    void SetG1Duration();

    /**
     * Protected copy-constructor for use by CreateCellCycleModel.
     * The only way for external code to create a copy of a cell cycle model
     * is by calling that method, to ensure that a model of the correct subclass is created.
     * This copy-constructor helps subclasses to ensure that all member variables are correctly copied when this happens.
     *
     * This method is called by child classes to set member variables for a daughter cell upon cell division.
     * Note that the parent cell cycle model will have had ResetForDivision() called just before CreateCellCycleModel() is called,
     * so performing an exact copy of the parent is suitable behaviour. Any daughter-cell-specific initialisation
     * can be done in InitialiseDaughterCell().
     *
     * @param rModel the cell cycle model to copy.
     */
    GastricGlandCellCycleModel(const GastricGlandCellCycleModel& rModel);

public:

    /**
     * Constructor - just a default, mBirthTime is now set in the AbstractPhaseBasedCellCycleModel class.
     * mG1Duration is set very high, it is set for the individual cells when InitialiseDaughterCell is called.
     */
    GastricGlandCellCycleModel();

    /**
     * Overridden UpdateCellCyclePhase() method.
     */
    virtual void UpdateCellCyclePhase();

    /**
     * Overridden InitialiseDaughterCell() method.
     */
    virtual void InitialiseDaughterCell();

    /**
     * Overridden builder method to create new copies of
     * this cell-cycle model.
     *
     * @return the new cell-cycle model
     */
    virtual AbstractCellCycleModel* CreateCellCycleModel();

    /**
     * Overridden CanCellTerminallyDifferentiate() method.
     * @return whether cell can terminally differentiate
     */
    virtual bool CanCellTerminallyDifferentiate();

    double GetIsthmusBeginHeight() const;
    void SetIsthmusBeginHeight(double height);

    double GetIsthmusEndHeight() const;
    void SetIsthmusEndHeight(double height);

    double GetBaseHeight() const;
    void SetBaseHeight(double height);

    /**
     * Overridden OutputCellCycleModelParameters() method.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    virtual void OutputCellCycleModelParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
// Declare identifier for the serializer
CHASTE_CLASS_EXPORT(GastricGlandCellCycleModel)

#endif /*GASTRICGLANDCELLCYCLEMODEL_HPP_*/

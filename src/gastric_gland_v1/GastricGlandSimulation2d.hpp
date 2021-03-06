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

#ifndef GASTRICGLANDSIMULATION2D_HPP_
#define GASTRICGLANDSIMULATION2D_HPP_

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>

#include "WntConcentration.hpp"
#include "OffLatticeSimulation.hpp"
#include "VertexBasedCellPopulation.hpp"
#include "GastricGlandSimulationBoundaryCondition.hpp"
#include "CryptCentreBasedDivisionRule.hpp"
#include "CryptVertexBasedDivisionRule.hpp"

/**
 * A 2D crypt simulation object. For more details on the crypt geometry, see the
 * papers by van Leeuwen et al (2009) [doi:10.1111/j.1365-2184.2009.00627.x] and
 * Osborne et al (2010) [doi:10.1098/rsta.2010.0173].
 */
class GastricGlandSimulation2d : public OffLatticeSimulation<2>
{

protected:

    unsigned m_cellAncestorIndex;
    unsigned m_maxCells;

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive the simulation and member variable.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<OffLatticeSimulation<2> >(*this);

        SerializableSingleton<WntConcentration<2> >* p_wnt_wrapper = WntConcentration<2>::Instance()->GetSerializationWrapper();
        archive & p_wnt_wrapper;
    }

    /**
     * Overridden SetupSolve() method.
     *
     * Write initial beta catenin results to file if required.
     */
    void SetupSolve();

    bool StoppingEventHasOccurred() override;

public:

    /**
     * Constructor.
     *
     * @param rCellPopulation A cell population object
     * @param deleteCellPopulationInDestructor Whether to delete the cell population on destruction to
     *     free up memory (defaults to false)
     * @param initialiseCells whether to initialise cells (defaults to true, set to false when loading from an archive)
     */
    GastricGlandSimulation2d(AbstractCellPopulation<2>& rCellPopulation,
                      bool deleteCellPopulationInDestructor=false,
                      bool initialiseCells=true,
                      unsigned index=0);

    /**
     * Destructor.
     *
     * This frees the CryptSimulationBoundaryCondition.
     */
    virtual ~GastricGlandSimulation2d();

    /**
     * Set method for mFixedBottomCells.
     */
    void FixBottomCells();

    void LabelIsthmusCellAncestors();

    void LabelBaseCellAncestors();

    void LabelNeckCellAncestors();

    unsigned GetCellAncestorIndex() const;

    unsigned GetMaxCells() const;
    void SetMaxCells(unsigned n);

    /**
     * Outputs simulation parameters to file
     *
     * As this method is pure virtual, it must be overridden
     * in subclasses.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputSimulationParameters(out_stream& rParamsFile);
};

// Declare identifier for the serializer
#include "SerializationExportWrapper.hpp"
CHASTE_CLASS_EXPORT(GastricGlandSimulation2d)

namespace boost
{
namespace serialization
{
/**
 * Serialize information required to construct a GastricGlandSimulation2d.
 */
template<class Archive>
inline void save_construct_data(
    Archive & ar, const GastricGlandSimulation2d * t, const unsigned int file_version)
{
    // Save data required to construct instance
    const AbstractCellPopulation<2>* p_cell_population = &(t->rGetCellPopulation());
    ar & p_cell_population;
    unsigned ancestorIndex = t->GetCellAncestorIndex();
    ar & ancestorIndex;
}

/**
 * De-serialize constructor parameters and initialise a GastricGlandSimulation2d.
 */
template<class Archive>
inline void load_construct_data(
    Archive & ar, GastricGlandSimulation2d * t, const unsigned int file_version)
{
    // Retrieve data from archive required to construct new instance
    AbstractCellPopulation<2>* p_cell_population;
    ar & p_cell_population;
    unsigned ancestorIndex;
    ar & ancestorIndex;

    // Invoke inplace constructor to initialise instance
    ::new(t)GastricGlandSimulation2d(*p_cell_population, true, false, ancestorIndex);
}
}
} // namespace

#endif /*GASTRICGLANDSIMULATION2D_HPP_*/

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

#ifndef EXPERIMENTALPARIETALCELLKILLER_HPP_
#define EXPERIMENTALPARIETALCELLKILLER_HPP_

#include "AbstractCellKiller.hpp"

#include "RandomNumberGenerator.hpp"
#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>

/**
 *  A cell killer that kills cells if they are outside the domain.
 *  The domain is assumed to start at x=0 and y=0. By default only cells
 *  are sloughed if y>mSloughLength. To slough the sides call the constructor
 *  with the appropriate parameter.
 */
template<unsigned DIM>
class ExperimentalParietalCellKiller : public AbstractCellKiller<DIM>
{
private:

    double m_deathChance;
    double m_activationTime;
    bool m_hasActivated;


    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive the object.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractCellKiller<DIM> >(*this);

        RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();
        archive & *p_gen;
    }

public:

    ExperimentalParietalCellKiller(
        AbstractCellPopulation<DIM>* pCrypt,
        double death_chance,
        double activation_time,
        bool has_activated=false);

    /**
     * Destructor
     */
    virtual ~ExperimentalParietalCellKiller(){};

    double GetDeathChance() const;
    void SetDeathChance(double chance);

    double GetActivationTime() const;

    void SetActivationTime(double time);

    bool HasActivated() const;

    /**
     * Loops over cells and kills cells outside boundary.
     */
    virtual void CheckAndLabelCellsForApoptosisOrDeath();

    /**
     * Outputs cell killer parameters to file
     *
     * As this method is pure virtual, it must be overridden
     * in subclasses.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputCellKillerParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(ExperimentalParietalCellKiller)

namespace boost
{
namespace serialization
{
/**
 * Serialize information required to construct a ExperimentalParietalCellKiller.
 */
template<class Archive, unsigned DIM>
inline void save_construct_data(
    Archive & ar, const ExperimentalParietalCellKiller<DIM> * t, const unsigned int file_version)
{
    // Save data required to construct instance
    const AbstractCellPopulation<DIM>* const p_gland = t->GetCellPopulation();
    ar << p_gland;
    double death_chance = t->GetDeathChance();
    ar << death_chance;
    double activation_time = t->GetActivationTime();
    ar << activation_time;
    bool has_activated = t->HasActivated();
    ar << has_activated;
}

/**
 * De-serialize constructor parameters and initialise a ExperimentalParietalCellKiller.
 */
template<class Archive, unsigned DIM>
inline void load_construct_data(
    Archive & ar, ExperimentalParietalCellKiller<DIM> * t, const unsigned int file_version)
{
    // Retrieve data from archive required to construct new instance
    AbstractCellPopulation<DIM>* p_gland;
    ar >> p_gland;
    double death_chance;
    ar >> death_chance;
    double activation_time;
    ar >> activation_time;
    bool has_activated;
    ar >> has_activated;

    // Invoke inplace constructor to initialise instance
    ::new(t)ExperimentalParietalCellKiller<DIM>(p_gland, death_chance, activation_time, has_activated);
}
} // namespace serialization
} // namespace boost

#endif /*EXPERIMENTALPARIETALCELLKILLER_HPP_*/

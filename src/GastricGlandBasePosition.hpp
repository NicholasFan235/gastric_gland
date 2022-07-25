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

#ifndef GASTRICGLANDBASEPOSITION_HPP_
#define GASTRICGLANDBASEPOSITION_HPP_

#include "ChasteSerialization.hpp"
#include "SerializableSingleton.hpp"
#include <boost/serialization/base_object.hpp>

#include <iostream>

#include "AbstractCellPopulation.hpp"


/**
 *  Singleton Wnt concentration object.
 */
template<unsigned DIM>
class GastricGlandBasePosition : public SerializableSingleton<GastricGlandBasePosition<DIM> >
{
private:

    /** Pointer to the singleton instance of WntConcentration */
    static GastricGlandBasePosition* mpInstance;

    c_vector<double, DIM> m_basePosition;

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archive the object and its member variables.
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
    }

protected:

    /**
     * Protected constuctor. Not to be called, use Instance() instead.
     */
    GastricGlandBasePosition();

public:

    /**
     * Return a pointer to the WntConcentration object.
     * The first time this is called, the object is created.
     *
     * @return  A pointer to the singleton WntConcentration object.
     */
    static GastricGlandBasePosition* Instance();

    /**
     * Destructor - frees up the singleton instance.
     */
    virtual ~GastricGlandBasePosition();

    /**
     * Destroy the current WntConcentration instance.
     *  Should be called at the end of a simulation.
     */
    static void Destroy();

    /**
     * Get the y-position of the lowest cell
     *
     * @return the y-location of the lowest cell
     */
    c_vector<double, DIM> GetBasePosition() const;

    /**
     * Set the base position recorded in the singleton
     * 
     * Used by GlandBaseTrackingModifier to store the location of the lowest cell at each timepoint
     * 
     * @param position y-location of the lowest cell
     */
    void SetBasePosition(c_vector<double, DIM> position);

};

#endif /*GASTRICGLANDBASEPOSITION_HPP_*/

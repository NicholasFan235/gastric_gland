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
/*
 *
 *  Chaste tutorial - this page gets automatically changed to a wiki page
 *  DO NOT remove the comments below, and if the code has to be changed in
 *  order to run, please check the comments are still accurate
 *
 *
 */

#ifndef _MESH_BASED_CRYPT_SIMULATIONS_HPP_
#define _MESH_BASED_CRYPT_SIMULATIONS_HPP_

/*
 * = Examples showing how to run crypt simulations on periodic meshes with different cell-cycle models =
 *
 * EMPTYLINE
 *
 * == Introduction ==
 *
 * EMPTYLINE
 *
 * In this tutorial we show how Chaste can be used to simulate a cylindrical model of an
 * intestinal crypt. Full details of the computational model can be found in the paper by
 * van Leeuwen ''et al.'' (2009) [doi:10.1111/j.1365-2184.2009.00627.x].
 *
 * As in previous cell-based Chaste tutorials, we begin by including the necessary header files.
 */
#include "CheckpointArchiveTypes.hpp"
#include "SmartPointers.hpp"

#include "SimulationTime.hpp"
#include "RandomNumberGenerator.hpp"
#include "CellPropertyRegistry.hpp"
#include "CellId.hpp"

class GastricGlandSimulation
{
public:
    GastricGlandSimulation() = default;
    ~GastricGlandSimulation() = default;

    void simplifiedModel(
        std::string testName,
        int nWidth, int nHeight,
        double glandHeight
    );


    void setUp()
    {
        SimulationTime::Instance()->SetStartTime(0.0);
        RandomNumberGenerator::Instance()->Reseed(0);
        // //Unnecessary since previous test's tearDown will have cleared:
        // CellPropertyRegistry::Instance()->Clear();
        CellId::ResetMaxCellId();
    }

    /**
     * Overridden teardown() method. Clears up singleton classes.
     */
    void tearDown()
    {
        SimulationTime::Destroy();
        RandomNumberGenerator::Destroy();
        CellPropertyRegistry::Instance()->Clear(); // Destroys properties which are still held by a shared pointer
        //WntConcentration<2>::Destroy();
    }
};

#endif // _MESH_BASED_CRYPT_SIMULATIONS_HPP_

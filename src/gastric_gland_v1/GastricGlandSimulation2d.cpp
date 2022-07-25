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

#include "GastricGlandSimulation2d.hpp"
#include "CellAncestor.hpp"
#include "CellBetaCateninWriter.hpp"
#include "MeshBasedCellPopulation.hpp"
#include "SmartPointers.hpp"
#include "StemCellProliferativeType.hpp"
#include "TransitCellProliferativeType.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "NeckCellProliferativeType.hpp"
#include "FoveolarCellProliferativeType.hpp"
#include "VanLeeuwen2009WntSwatCellCycleModelHypothesisOne.hpp"
#include "VanLeeuwen2009WntSwatCellCycleModelHypothesisTwo.hpp"
#include "WntConcentration.hpp"

GastricGlandSimulation2d::GastricGlandSimulation2d(AbstractCellPopulation<2>& rCellPopulation,
                                     bool deleteCellPopulationInDestructor,
                                     bool initialiseCells,
                                     unsigned ancestorIndex)
    : OffLatticeSimulation<2>(rCellPopulation,
                             deleteCellPopulationInDestructor,
                             initialiseCells),
      m_cellAncestorIndex(ancestorIndex)
{
    /* Throw an exception message if not using a  MeshBasedCellPopulation or a VertexBasedCellPopulation.
     * This is to catch NodeBasedCellPopulations as AbstactOnLatticeBasedCellPopulations are caught in
     * the OffLatticeSimulation constructor.
     */
    if ((dynamic_cast<MeshBasedCellPopulation<2>*>(&rCellPopulation) == nullptr))
    {
        EXCEPTION("GastricGlandSimulation2d is to be used with MeshBasedCellPopulation (or subclasses) only");
    }

    if (dynamic_cast<MeshBasedCellPopulation<2>*>(&mrCellPopulation))
    {
        MAKE_PTR(CryptCentreBasedDivisionRule<2>, p_centre_div_rule);
        static_cast<MeshBasedCellPopulation<2>*>(&mrCellPopulation)->SetCentreBasedDivisionRule(p_centre_div_rule);
    }
    else // VertexBasedCellPopulation
    {
        MAKE_PTR(CryptVertexBasedDivisionRule<2>, p_vertex_div_rule);
        static_cast<VertexBasedCellPopulation<2>*>(&mrCellPopulation)->SetVertexBasedDivisionRule(p_vertex_div_rule);
    }

    if (!mDeleteCellPopulationInDestructor)
    {
        // Pass a GastricGlandSimulationBoundaryCondition object into mBoundaryConditions
        MAKE_PTR_ARGS(GastricGlandSimulationBoundaryCondition<2>, p_bc, (&rCellPopulation));
        AddCellPopulationBoundaryCondition(p_bc);
    }
}

GastricGlandSimulation2d::~GastricGlandSimulation2d()
{
}

void GastricGlandSimulation2d::SetupSolve()
{
    // First call method on base class
    OffLatticeSimulation<2>::SetupSolve();

    /*
     * To check if beta-catenin results will be written to file, we test if the first
     * cell has a cell-cycle model that is a subclass of AbstractVanLeeuwen2009WntSwatCellCycleModel.
     * In doing so, we assume that all cells in the simulation have the same cell-cycle
     * model.
     */
    if (dynamic_cast<AbstractVanLeeuwen2009WntSwatCellCycleModel*>(this->mrCellPopulation.Begin()->GetCellCycleModel()))
    {
        mrCellPopulation.AddCellWriter<CellBetaCateninWriter>();
    }

    if (dynamic_cast<AbstractVanLeeuwen2009WntSwatCellCycleModel*>(mrCellPopulation.Begin()->GetCellCycleModel()))
    {
        *mpVizSetupFile << "BetaCatenin\n";
    }
}

void GastricGlandSimulation2d::FixBottomCells()
{
    // The CryptSimulationBoundaryCondition object is the first element of mBoundaryConditions
    boost::static_pointer_cast<GastricGlandSimulationBoundaryCondition<2> >(mBoundaryConditions[0])->SetUseFixedBottomCells(true);
}

void GastricGlandSimulation2d::LabelIsthmusCellAncestors()
{
    double threshold_height = 10.0;
    for (AbstractCellPopulation<2>::Iterator cell_iter = mrCellPopulation.Begin();
         cell_iter != mrCellPopulation.End();
         ++cell_iter)
    {
        CellPtr pCell = *cell_iter;
        if (mrCellPopulation.GetLocationOfCellCentre(*cell_iter)[1] > threshold_height &&
            (pCell->GetCellProliferativeType()->IsSubType<StemCellProliferativeType>() ||
            pCell->GetCellProliferativeType()->IsSubType<TransitCellProliferativeType>()))
        {
            MAKE_PTR_ARGS(CellAncestor, p_cell_ancestor, (m_cellAncestorIndex++));
            cell_iter->SetAncestor(p_cell_ancestor);
        }
    }
}

void GastricGlandSimulation2d::LabelBaseCellAncestors()
{
    double threshold_height = 1.0;
    for (AbstractCellPopulation<2>::Iterator cell_iter = mrCellPopulation.Begin();
         cell_iter != mrCellPopulation.End();
         ++cell_iter)
    {
        CellPtr pCell = *cell_iter;
        if (mrCellPopulation.GetLocationOfCellCentre(*cell_iter)[1] < threshold_height &&
            (pCell->GetCellProliferativeType()->IsSubType<StemCellProliferativeType>() ||
            pCell->GetCellProliferativeType()->IsSubType<TransitCellProliferativeType>()))
        {
            MAKE_PTR_ARGS(CellAncestor, p_cell_ancestor, (m_cellAncestorIndex++));
            cell_iter->SetAncestor(p_cell_ancestor);
        }
    }
}

void GastricGlandSimulation2d::LabelNeckCellAncestors()
{
    for (AbstractCellPopulation<2>::Iterator cell_iter = mrCellPopulation.Begin();
         cell_iter != mrCellPopulation.End();
         ++cell_iter)
    {
        CellPtr pCell = *cell_iter;
        if (pCell->GetCellProliferativeType()->IsType<NeckCellProliferativeType>())
        {
            MAKE_PTR_ARGS(CellAncestor, p_cell_ancestor, (m_cellAncestorIndex++));
            cell_iter->SetAncestor(p_cell_ancestor);
        }
    }
}

unsigned GastricGlandSimulation2d::GetCellAncestorIndex() const
{
    return m_cellAncestorIndex;
}

void GastricGlandSimulation2d::OutputSimulationParameters(out_stream& rParamsFile)
{
    double width = mrCellPopulation.GetWidth(0);
    bool use_fixed_bottom_cells = boost::static_pointer_cast<GastricGlandSimulationBoundaryCondition<2> >(mBoundaryConditions[0])->GetUseFixedBottomCells();

    *rParamsFile << "\t\t<CryptCircumference>" << width << "</CryptCircumference>\n";
    *rParamsFile << "\t\t<UseFixedBottomCells>" << use_fixed_bottom_cells << "</UseFixedBottomCells>\n";

    // Call method on direct parent class
    OffLatticeSimulation<2>::OutputSimulationParameters(rParamsFile);
}

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
CHASTE_CLASS_EXPORT(GastricGlandSimulation2d)

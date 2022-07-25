#include "GastricGlandBaseCellKiller.hpp"

#include "AbstractCentreBasedCellPopulation.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "Exception.hpp"

template <unsigned DIM>
GastricGlandBaseCellKiller<DIM>::GastricGlandBaseCellKiller(AbstractCellPopulation<DIM>* pCellPopulation, double cutoffHeight) :
    AbstractCellKiller<DIM>(pCellPopulation),
    m_cutoffHeight(cutoffHeight)
{}

template <unsigned DIM>
void GastricGlandBaseCellKiller<DIM>::CheckAndLabelCellsForApoptosisOrDeath()
{
    switch (DIM)
    {
        case 1:
        {
            EXCEPTION("GastricGlandBaseCellKiller not implemented in 1D");
            break;
        }
        case 2:
        {
            for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = this->mpCellPopulation->Begin(); cell_iter != this->mpCellPopulation->End(); ++cell_iter)
            {
                c_vector<double, 2> location;
                location = this->mpCellPopulation->GetLocationOfCellCentre(*cell_iter);

                CellPtr pCell = *cell_iter;

                if ((location[1] < m_cutoffHeight) && (pCell->GetAge() > 20))
                {
                    cell_iter->Kill();
                }
            }
            break;
        }
        case 3:
        {
            EXCEPTION("GastricGlandBaseCellKiller Not Implemented in 3D");
            break;
        }
        default :
        {
            NEVER_REACHED;
        }

    }
}

template <unsigned DIM>
void GastricGlandBaseCellKiller<DIM>::OutputCellKillerParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<CutoffHeight>" << m_cutoffHeight << "</CutoffHeight>\n";

    AbstractCellKiller<DIM>::OutputCellKillerParameters(rParamsFile);
}

template class GastricGlandBaseCellKiller<2u>;

#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(GastricGlandBaseCellKiller)

#include "FoveolarCellKiller.hpp"

#include "AbstractCentreBasedCellPopulation.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "FoveolarCellProliferativeType.hpp"
#include "Exception.hpp"

template <unsigned DIM>
FoveolarCellKiller<DIM>::FoveolarCellKiller(AbstractCellPopulation<DIM>* pCellPopulation, double cutoffAge) :
    AbstractCellKiller<DIM>(pCellPopulation),
    m_cutoffAge(cutoffAge)
{}

template <unsigned DIM>
void FoveolarCellKiller<DIM>::CheckAndLabelCellsForApoptosisOrDeath()
{
    switch (DIM)
    {
        case 1:
        {
            EXCEPTION("FoveolarCellKiller not implemented in 1D");
            break;
        }
        case 2:
        {
            for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = this->mpCellPopulation->Begin(); cell_iter != this->mpCellPopulation->End(); ++cell_iter)
            {
                c_vector<double, 2> location;
                CellPtr pCell = *cell_iter;

                if (pCell->GetCellProliferativeType()->IsSubType<FoveolarCellProliferativeType>() &&
                    pCell->GetAge() > m_cutoffAge)
                {
                    cell_iter->Kill();
                }
            }
            break;
        }
        case 3:
        {
            EXCEPTION("FoveolarCellKiller Not Implemented in 3D");
            break;
        }
        default :
        {
            NEVER_REACHED;
        }

    }
}

template <unsigned DIM>
double FoveolarCellKiller<DIM>::getCutoffAge() const
{
    return m_cutoffAge;
}

template <unsigned DIM>
void FoveolarCellKiller<DIM>::OutputCellKillerParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<CutoffAge>" << m_cutoffAge << "</CutoffAge>\n";

    AbstractCellKiller<DIM>::OutputCellKillerParameters(rParamsFile);
}

template class FoveolarCellKiller<2u>;

#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(FoveolarCellKiller)

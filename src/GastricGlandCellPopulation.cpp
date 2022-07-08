

#include "GastricGlandCellPopulation.hpp"
#include "CellLocationIndexWriter.hpp"
#include "FoveolarCellProliferativeType.hpp"
#include "NeckCellProliferativeType.hpp"


template<unsigned DIM>
GastricGlandCellPopulation<DIM>::GastricGlandCellPopulation(
    MutableMesh<DIM, DIM>& rMesh,
    std::vector<CellPtr>& rCells,
    const std::vector<unsigned> locationIndices,
    double mitosisRequiredSize,
    bool deleteMesh,
    double ghostSpringStiffness)
    :   MeshBasedCellPopulationWithGhostNodes<DIM>(
            rMesh, rCells, locationIndices, deleteMesh, ghostSpringStiffness),
        mMitosisRequiredSize(mitosisRequiredSize)
{}

template<unsigned DIM>
GastricGlandCellPopulation<DIM>::GastricGlandCellPopulation(
    MutableMesh<DIM, DIM>& rMesh,
    double mitosisRequiredSize,
    double ghostSpringStiffness)
    :   MeshBasedCellPopulationWithGhostNodes<DIM>(rMesh, ghostSpringStiffness),
        mMitosisRequiredSize(mitosisRequiredSize)
{
}

template<unsigned DIM>
GastricGlandCellPopulation<DIM>::~GastricGlandCellPopulation()
{
}

inline double GetCellRestLength(CellPtr pCell)
{
    if (pCell->GetCellProliferativeType()->IsType<FoveolarCellProliferativeType>())
    {
        return 0.3;
    }
    else
    {
        return 0.5;
    }
}

template <unsigned DIM>
bool GastricGlandCellPopulation<DIM>::IsRoomToDivide(CellPtr pCell)
{
    double area_cell = this->GetVolumeOfCell(pCell);
    double rest_length = GetCellRestLength(pCell);
    double side_length = 0.577 * rest_length * 2; // Side length assuming hexagonal packing: 2 * tan(30) * rest_length
    double hexagon_area = 2.598 * side_length * side_length; // Hexagon area: 3sqrt(3)/2 * side_length^2

    return area_cell >= mMitosisRequiredSize * hexagon_area;
}

template <unsigned DIM>
double GastricGlandCellPopulation<DIM>::GetRestLength(unsigned indexA, unsigned indexB)
{
    CellPtr pCellA = this->GetCellUsingLocationIndex(indexA);
    CellPtr pCellB = this->GetCellUsingLocationIndex(indexB);

    return GetCellRestLength(pCellA) + GetCellRestLength(pCellB);
}

template <unsigned DIM>
double GastricGlandCellPopulation<DIM>::GetMitosisRequiredSize() const
{
    return mMitosisRequiredSize;
}

template <unsigned DIM>
void GastricGlandCellPopulation<DIM>::SetMitosisRequiredSize(double size)
{
    mMitosisRequiredSize = size;
}

template<unsigned DIM>
void GastricGlandCellPopulation<DIM>::OutputCellPopulationParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<MitosisRequiredSize>" << mMitosisRequiredSize << "</MitosisRequiredSize>\n";

    // Call method on direct parent class
    MeshBasedCellPopulationWithGhostNodes<DIM>::OutputCellPopulationParameters(rParamsFile);
}

// Explicit instantiation
template class GastricGlandCellPopulation<1>;
template class GastricGlandCellPopulation<2>;
template class GastricGlandCellPopulation<3>;

// Serialization for Boost >= 1.36
#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(GastricGlandCellPopulation)

#include "GastricGlandSimulation.hpp"

#include "CylindricalHoneycombMeshGenerator.hpp"
#include "GastricGlandCellsGenerator.hpp"
#include "UniformG1GenerationalCellCycleModel.hpp"
#include "MeshBasedCellPopulationWithGhostNodes.hpp"
#include "CryptSimulation2d.hpp"

#include "VoronoiDataWriter.hpp"
#include "CellPopulationAreaWriter.hpp"
#include "CellVolumesWriter.hpp"
#include "CellAncestorWriter.hpp"
#include "CellAgesWriter.hpp"

// Cell population writers
#include "CellMutationStatesCountWriter.hpp"
#include "CellPopulationAreaWriter.hpp"
#include "CellProliferativePhasesCountWriter.hpp"
#include "CellProliferativeTypesCountWriter.hpp"
#include "VoronoiDataWriter.hpp"

#include "LinearSpringWithVariableSpringConstantsForce.hpp"
#include "SloughingCellKiller.hpp"

#include <iostream>
#include <vector>

void GastricGlandSimulation::simplifiedModel(
    std::string testName,
    int nWidth, int nHeight,
    double glandHeight)
{
    setUp();

    CylindricalHoneycombMeshGenerator generator(nWidth, nHeight, 2);
    Cylindrical2dMesh* p_mesh = generator.GetCylindricalMesh();
    std::vector<unsigned int> location_indices = generator.GetCellLocationIndices();

    std::vector<CellPtr> cells;
    GastricGlandCellsGenerator<UniformG1GenerationalCellCycleModel> cells_generator;
    cells_generator.Generate(cells, p_mesh, location_indices, true);

    MeshBasedCellPopulationWithGhostNodes<2> cell_population(*p_mesh, cells, location_indices);

    CryptSimulation2d simulator(cell_population);

    cell_population.SetWriteVtkAsPoints(false);
    cell_population.AddPopulationWriter<VoronoiDataWriter>();
    cell_population.AddPopulationWriter<CellPopulationAreaWriter>();
    cell_population.AddCellWriter<CellVolumesWriter>();
    cell_population.AddCellWriter<CellAncestorWriter>();
    cell_population.AddCellWriter<CellAgesWriter>();

    simulator.SetOutputDirectory("ToyGastricGland/");
    std::cout << "Writing to output directory: " << simulator.GetOutputDirectory() << std::endl;
    simulator.SetEndTime(100);
    
    simulator.SetSamplingTimestepMultiple(12);

    MAKE_PTR(LinearSpringWithVariableSpringConstantsForce<2>, p_linear_force);
    p_linear_force->SetEdgeBasedSpringConstant(false);
    simulator.AddForce(p_linear_force);

    MAKE_PTR_ARGS(SloughingCellKiller<2>, p_killer, (&cell_population, glandHeight));
    simulator.AddCellKiller(p_killer);

    simulator.UseJiggledBottomCells();

    simulator.Solve();
    tearDown();
    std::cout << "Completed Toy Gastric Gland Model" << std::endl;
}

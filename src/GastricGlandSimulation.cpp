#include "GastricGlandSimulation.hpp"

#include "CylindricalHoneycombMeshGenerator.hpp"
#include "GastricGlandCellsGenerator.hpp"
#include "UniformG1GenerationalCellCycleModel.hpp"
#include "GastricGlandCellCycleModel.hpp"
#include "WntConcentration.hpp"
#include "GastricGlandCellPopulation.hpp"
#include "CryptSimulation2d.hpp"
#include "GastricGlandSimulation2d.hpp"

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
#include "GastricGlandBaseCellKiller.hpp"
#include "ExperimentalParietalCellKiller.hpp"

#include <iostream>
#include <vector>

void GastricGlandSimulation::simplifiedModel(
    std::string testName,
    int nWidth, int nHeight,
    double glandHeight,
    bool useAreaBasedDampingConstant,
    bool useEdgeBasedSpringConstant,
    double areaForDivision,
    double foveolarSizeMultiplier)
{
    setUp();

    CylindricalHoneycombMeshGenerator generator(nWidth, nHeight, 2);
    Cylindrical2dMesh* p_mesh = generator.GetCylindricalMesh();
    std::vector<unsigned int> location_indices = generator.GetCellLocationIndices();

    std::cout << "Initialized Mesh" << std::endl;

    std::vector<CellPtr> cells;
    GastricGlandCellsGenerator<GastricGlandCellCycleModel> cells_generator;
    cells_generator.Generate(cells, p_mesh, location_indices, true);

    std::cout << "Created Cells" << std::endl;

    GastricGlandCellPopulation<2> cell_population(*p_mesh, cells, location_indices, areaForDivision, foveolarSizeMultiplier);

    WntConcentration<2>::Instance()->SetType(LINEAR);
    WntConcentration<2>::Instance()->SetCellPopulation(cell_population);
    WntConcentration<2>::Instance()->SetCryptLength(glandHeight);
    std::cout << "Setup Wnt Concentration" << std::endl;

    GastricGlandSimulation2d simulator(cell_population);

    cell_population.SetWriteVtkAsPoints(false);
    cell_population.AddPopulationWriter<VoronoiDataWriter>();
    cell_population.AddPopulationWriter<CellPopulationAreaWriter>();
    cell_population.AddCellWriter<CellVolumesWriter>();
    cell_population.AddCellWriter<CellAncestorWriter>();
    cell_population.AddCellWriter<CellAgesWriter>();

    simulator.SetOutputDirectory(testName + "/");
    std::cout << "Writing to output directory: " << simulator.GetOutputDirectory() << std::endl;
    simulator.SetEndTime(200);
    
    simulator.SetSamplingTimestepMultiple(12);

    cell_population.SetAreaBasedDampingConstant(useAreaBasedDampingConstant);

    MAKE_PTR(LinearSpringWithVariableSpringConstantsForce<2>, p_linear_force);
    p_linear_force->SetEdgeBasedSpringConstant(useEdgeBasedSpringConstant);
    simulator.AddForce(p_linear_force);

    MAKE_PTR_ARGS(SloughingCellKiller<2>, p_killer, (&cell_population, glandHeight));
    simulator.AddCellKiller(p_killer);

    //MAKE_PTR_ARGS(ExperimentalParietalCellKiller<2>, p_experiment, (&cell_population, 0.4, 100));
    //simulator.AddCellKiller(p_experiment);

    simulator.UseJiggledBottomCells();
    simulator.LabelBaseCellAncestors();
    simulator.LabelIsthmusCellAncestors();

    simulator.Solve();
    tearDown();
    std::cout << "Completed Toy Gastric Gland Model" << std::endl;
}

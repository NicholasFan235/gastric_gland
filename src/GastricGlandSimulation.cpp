#include "GastricGlandSimulation.hpp"

#include "CellBasedSimulationArchiver.hpp"
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

// V2 Features
#include "GastricGlandBasePosition.hpp"
#include "GlandBaseTrackingModifier.hpp"
#include "GastricGlandCellCycleModelV2.hpp"
#include "FoveolarCellKiller.hpp"
#include "Parameters.hpp"
#include "ExecutableSupport.hpp"


#include <iostream>
#include <vector>

int GastricGlandSimulation::run(int argc, char *argv[])
{
    try
    {
        // Process arguments
        if (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help" ))
        {
            std::cerr << "Usage: " << argv[0] << " [KEY=VALUE]...\n";
            std::cerr << std::endl;
            std::cerr << GastricGlandParameters::help() << std::endl;
            return ExecutableSupport::EXIT_OK;
        }

        std::string parameter_string;
        for (int i = 1; i < argc; i++)
        {
            parameter_string += argv[i];
            parameter_string += '\n';
        }

        std::map<std::string, std::string> map = makeMap(parameter_string);
        //validate(map);
        GastricGlandParameters params;
        params.update(map);

        // Print output
        std::cerr << params << std::endl;

        // Run simulation
        simplifiedModel(params);

        return ExecutableSupport::EXIT_OK;
    }
    catch (const Exception& e)
    {
        ExecutableSupport::PrintError(e.GetMessage());

        return ExecutableSupport::EXIT_ERROR;
    }
}

void GastricGlandSimulation::simplifiedModel(
    const GastricGlandParameters& params)
{
    setUp(params.seed);

    CylindricalHoneycombMeshGenerator generator(
        params.num_cells_across, params.num_cells_high,
        params.num_ghost_layers);
    Cylindrical2dMesh* p_mesh = generator.GetCylindricalMesh();
    std::vector<unsigned int> location_indices = generator.GetCellLocationIndices();

    std::vector<CellPtr> cells;
    GastricGlandCellsGenerator<GastricGlandCellCycleModelV2> cells_generator;
    cells_generator.Generate(cells, p_mesh, location_indices, true);
    for (auto& cell : cells)
    {
        GastricGlandCellCycleModelV2* pCycle = dynamic_cast<GastricGlandCellCycleModelV2*>(
            cell->GetCellCycleModel());
        pCycle->SetBaseHeight(params.base_height);
        pCycle->SetIsthmusBeginHeight(params.isthmus_begin_height);
        pCycle->SetIsthmusEndHeight(params.isthmus_end_height);
        pCycle->SetBaseG1Duration(params.base_g1_duration);
        pCycle->SetIsthmusG1Duration(params.isthmus_g1_duration);
    }

    GastricGlandCellPopulation<2> cell_population(*p_mesh, cells, location_indices,
        0.0, params.foveolar_cell_size_multiplier);

    WntConcentration<2>::Instance()->SetType(LINEAR);
    WntConcentration<2>::Instance()->SetCellPopulation(cell_population);
    WntConcentration<2>::Instance()->SetCryptLength(params.gland_height);
    GastricGlandBasePosition<2>::Instance()->SetBasePosition(c_vector<double, 2>());

    GastricGlandSimulation2d simulator(cell_population);

    cell_population.SetWriteVtkAsPoints(false);
    cell_population.AddPopulationWriter<VoronoiDataWriter>();
    cell_population.AddPopulationWriter<CellPopulationAreaWriter>();
    cell_population.AddCellWriter<CellVolumesWriter>();
    cell_population.AddCellWriter<CellAncestorWriter>();
    cell_population.AddCellWriter<CellAgesWriter>();

    simulator.SetOutputDirectory(params.output_directory + "/sim_" + params.simulation_id);
    std::cout << "Writing to output directory: " << simulator.GetOutputDirectory() << std::endl;
    simulator.SetEndTime(params.simulation_time);
    simulator.SetDt(params.dt);
    simulator.SetSamplingTimestepMultiple(params.sampling_timestep_multiple);

    cell_population.SetDampingConstantNormal(params.damping_constant);
    cell_population.SetAreaBasedDampingConstant(params.use_area_based_damping_constant);

    MAKE_PTR(LinearSpringWithVariableSpringConstantsForce<2>, p_linear_force);
    p_linear_force->SetEdgeBasedSpringConstant(params.use_edge_based_spring_constant);
    simulator.AddForce(p_linear_force);

    if (params.use_sloughing)
    {
        MAKE_PTR_ARGS(SloughingCellKiller<2>, p_killer, (&cell_population, params.gland_height));
        simulator.AddCellKiller(p_killer);
    }

    if (params.use_foveolar_max_age)
    {
        MAKE_PTR_ARGS(FoveolarCellKiller<2>, p_foveolarKiller, (&cell_population,
            params.foveolar_cell_max_age));
        simulator.AddCellKiller(p_foveolarKiller);
    }

    if (params.do_parietal_killing_experiment)
    {
        MAKE_PTR_ARGS(ExperimentalParietalCellKiller<2>, p_experiment, (&cell_population,
            params.parietal_killing_ratio, params.parietal_killing_experiment_time));
        simulator.AddCellKiller(p_experiment);
    }

    MAKE_PTR(GlandBaseTrackingModifier<2>, p_baseTrackingModifier);
    simulator.AddSimulationModifier(p_baseTrackingModifier);

    simulator.SetMaxCells(params.max_cells);

    simulator.FixBottomCells();
    if (params.label_ancestors)
    {
        simulator.LabelBaseCellAncestors();
        simulator.LabelIsthmusCellAncestors();
        simulator.LabelNeckCellAncestors();
    }


    std::cout << "Beginning Solve()..." << std::endl;
    simulator.Solve();

    CellBasedSimulationArchiver<2, GastricGlandSimulation2d>::Save(&simulator);

    for (int i = 1; i <= 4; i++)
    {
        // Load where left off
        GastricGlandSimulation2d* p_simulator =
            CellBasedSimulationArchiver<2, GastricGlandSimulation2d>::Load(
                "SteadyStateCrypt", params.simulation_time*i);
        
        p_simulator->LabelBaseCellAncestors();
        p_simulator->LabelIsthmusCellAncestors();
        p_simulator->LabelNeckCellAncestors();
        p_simulator->SetEndTime(params.simulation_time*(i+1));
        p_simulator->Solve();
        CellBasedSimulationArchiver<2, GastricGlandSimulation2d>::Save(p_simulator);
        delete p_simulator;
    }

    tearDown();
    std::cout << "Completed Toy Gastric Gland Model" << std::endl;
}

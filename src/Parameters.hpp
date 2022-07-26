#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <string>
#include <map>
#include <vector>

struct GastricGlandParameters
{
    std::string output_directory = "gastric-gland";
    std::string simulation_id = "0";

    unsigned seed = 0;

    double simulation_time = 100;
    double dt = 1.0/120.0;
    unsigned sampling_timestep_multiple = 12;

    unsigned num_cells_across = 10;
    unsigned num_cells_high = 40;
    unsigned num_ghost_layers = 2;
    double gland_height = 40.0;
    unsigned max_cells = 1000;

    double base_height = 3.0;
    double isthmus_begin_height = 28.0;
    double isthmus_end_height = 32.0;

    bool label_ancestors = true;
    double damping_constant = 1.0;
    bool use_area_based_damping_constant = true;
    bool use_edge_based_spring_constant = false;

    double foveolar_cell_size_multiplier = 0.8;
    bool use_foveolar_max_age = false;
    double foveolar_cell_max_age = 200;
    bool use_sloughing = true;
    double base_g1_duration = 200;
    double isthmus_g1_duration = 10;


    // Parietal Killing Experiment
    bool do_parietal_killing_experiment = false;
    double parietal_killing_experiment_time = 100;
    double parietal_killing_ratio = 0.4;

    void update(const std::map<std::string, std::string>& map);

    static std::string help();

    static const std::vector<std::string> valid_keys;
};

std::ostream& operator<<(std::ostream& os, const GastricGlandParameters& p);

std::map<std::string, std::string> makeMap(const std::string& input);

bool is_whitespace(const char letter);
void remove_trailing_whitespace(std::string& str);
void remove_leading_whitespace(std::string& str);
void remove_leading_trailing_whitespace(std::string& str);

void validate(const std::map<std::string, std::string>& map);

#endif // PARAMETERS_HPP_
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <set>
#include <iostream>

#include "Exception.hpp"

#include "Parameters.hpp"

template <typename T>
inline void retrieve(const std::map<std::string, std::string>& map, const std::string& key, T& out)
{
    if (map.find(key) != map.end())
    {
        std::stringstream ss(map.at(key));
        ss >> out;
    }
}

void GastricGlandParameters::update(const std::map<std::string, std::string>& map)
{
    retrieve<std::string>(map, "output-directory", output_directory);
    retrieve<std::string>(map, "simulation-id", simulation_id);
    retrieve<unsigned>(map, "seed", seed);
    retrieve<double>(map, "simulation-time", simulation_time);
    retrieve<double>(map, "dt", dt);
    retrieve<unsigned>(map, "sampling-timestep-multiple", sampling_timestep_multiple);
    
    retrieve<unsigned>(map, "num-cells-across", num_cells_across);
    retrieve<unsigned>(map, "num-cells-high", num_cells_high);
    retrieve<unsigned>(map, "num-ghost-layers", num_ghost_layers);
    retrieve<double>(map, "gland-height", gland_height);
    retrieve<unsigned>(map, "max-cells", max_cells);

    retrieve<double>(map, "base-height", base_height);
    retrieve<double>(map, "isthmus-begin-height", isthmus_begin_height);
    retrieve<double>(map, "isthmus-end-height", isthmus_end_height);

    retrieve<bool>(map, "label-ancestors", label_ancestors);
    retrieve<double>(map, "damping-constant", damping_constant);
    retrieve<bool>(map, "use-area-based-damping-constant", use_area_based_damping_constant);
    retrieve<bool>(map, "use-edge-based-spring-constant", use_edge_based_spring_constant);

    retrieve<double>(map, "foveolar-cell-size-multiplier", foveolar_cell_size_multiplier);
    retrieve<bool>(map, "use-foveolar-max-age", use_foveolar_max_age);
    retrieve<double>(map, "foveolar-cell-max-age", foveolar_cell_max_age);
    retrieve<bool>(map, "use-sloughing", use_sloughing);
    retrieve<double>(map, "base-g1-duration", base_g1_duration);
    retrieve<double>(map, "isthmus-g1-duration", isthmus_g1_duration);

    retrieve<bool>(map, "do-parietal-killing-experiment", do_parietal_killing_experiment);
    retrieve<double>(map, "parietal-killing-experiment-time", parietal_killing_experiment_time);
    retrieve<double>(map, "parietal-killing-ratio", parietal_killing_ratio);
}

std::ostream& operator<<(std::ostream& os, const GastricGlandParameters& p)
{
    os << "Gastric Gland Parameters" << std::endl;
    os << "\nSimulation Config:" << std::endl;
    os << "    output-directory: " << p.output_directory << std::endl;
    os << "    simulation-id: " << p.simulation_id << std::endl;
    os << "    seed: " << p.seed << std::endl;
    os << "    simulation-time: " << p.simulation_time << std::endl;
    os << "    dt: " << p.dt << std::endl;
    os << "    sampling-timestep-multiple: " << p.sampling_timestep_multiple << std::endl;

    os << "\nGland Config:" << std::endl;
    os << "    num-cells-across: " << p.num_cells_across << std::endl;
    os << "    num-cells-high: " << p.num_cells_high << std::endl;
    os << "    num-ghost-layers: " << p.num_ghost_layers << std::endl;
    os << "    gland-height: " << p.gland_height << std::endl;
    os << "    max-cells: " << p.max_cells << std::endl;
    os << "    base-height: " << p.base_height << std::endl;
    os << "    isthmus-begin-height: " << p.isthmus_begin_height << std::endl;
    os << "    isthmus-end-height: " << p.isthmus_end_height << std::endl;
    os << std::endl;
    os << "    foveolar-cell-size-multiplier: " << p.foveolar_cell_size_multiplier << std::endl;
    os << "    use-foveolar-max-age: " << p.use_foveolar_max_age << std::endl;
    os << "    foveolar-cell-max-age: " << p.foveolar_cell_max_age << std::endl;
    os << "    use-sloughing: " << p.use_sloughing << std::endl;
    os << "    base-g1-duration: " << p.base_g1_duration << std::endl;
    os << "    isthmus-g1-duration: " << p.isthmus_g1_duration << std::endl;
    os << "    label-ancestors: " << p.label_ancestors << std::endl;
    os << "    damping-constant: " << p.damping_constant << std::endl;
    os << "    use-area-based-damping-constant: " << p.use_area_based_damping_constant << std::endl;
    os << "    use-edge-based-spring-constant: " << p.use_edge_based_spring_constant << std::endl;

    os << "\nParietal Cell Killing Experiment:" << std::endl;
    os << "    do-parietal-killing-experiment: " << p.do_parietal_killing_experiment << std::endl;
    os << "    parietal-killing-experiment-time: " << p.parietal_killing_experiment_time << std::endl;
    os << "    parietal-killing-ratio: " << p.parietal_killing_ratio << std::endl;

    return os;
}

const std::vector<std::string> GastricGlandParameters::valid_keys = {
    "output-directory", "simulation-id", "seed", "simulation-time",
    "dt", "sampling-timestep-multiple",

    "num-cells-across", "num-cells-high", "num-ghost-layers", "gland-height", "max-cells",
    "base-height", "isthmus-begin-height", "isthmus-end-height",

    "foveolar-cell-size-multiplier", "use-foveolar-max-age", "foveolar-cell-max-age",
    "use-sloughing", "base-g1-duration", "isthmus-g1-duration",
    "label-ancestors", "damping_constant", "use-area-based-damping-constant",
    "use-edge-based-spring-constant",

    "do-parietal-killing-experiment", "parietal-killing-experiment-time",
    "parietal-killing-ratio"
};

std::string GastricGlandParameters::help()
{
    std::stringstream ss;
    ss << "Vaild keys for GastricGlandParameters:" << std::endl;
    for (const std::string& s : GastricGlandParameters::valid_keys)
    {
        ss << "    \"" << s << '"' << std::endl;
    }
    return ss.str();
}

bool is_whitespace(const char letter)
{
    return (letter == ' ')
        || (letter == '\t')
        || (letter == '\n');
}

void remove_trailing_whitespace(std::string& str)
{
    auto pos = str.cend();

    // Iterate backwards until non-whitespace character is found
    while (is_whitespace(*--pos));

    // pos is off by one
    pos++;

    // Erase trailing whitespace and return string
    str.erase(pos, str.cend());
}

void remove_leading_whitespace(std::string& str)
{
    auto pos = str.cbegin();

    // Iterate backwards until non-whitespace character is found
    while (is_whitespace(*pos))
    {
        pos++;
    }

    // Erase trailing whitespace and return string
    str.erase(str.cbegin(), pos);
}

void remove_leading_trailing_whitespace(std::string& str)
{
    remove_leading_whitespace(str);
    remove_trailing_whitespace(str);
}

std::map<std::string, std::string> makeMap(const std::string& input)
{
    // Initialise dictionary
    std::map<std::string, std::string> dict;

    std::stringstream ss(input);
    std::string line;
    while (std::getline(ss, line))
    {
        // Split line by '='
        std::stringstream line_ss(line);

        std::string key;
        std::getline(line_ss, key, '=');

        // Skip line if no '=' is found
        if (line_ss.eof())
        {
            continue;
        }

        // Because I skip when no '=' is found, and I am using a stringstream,
        // it should not fail
        if (!line_ss)
        {
            NEVER_REACHED;
        }

        std::string value;
        std::getline(line_ss, value, '=');

        // Remove leading and trailing whitespace
        remove_leading_trailing_whitespace(key);
        remove_leading_trailing_whitespace(value);

        if ((key.size() == 0) || (value.size() == 0))
        {
            std::string error_msg;
            error_msg += "\"";
            error_msg += line;
            error_msg += "\" is missing key or value";
            EXCEPTION(error_msg);
        }

        dict[key] = value;
    }

    for (const auto& p : dict)
    {
        std::cout << '"' << p.first << "\": \"" << p.second << '"' << std::endl;
    }
    std::cout << std::endl;

    return dict;
}

void validate(const std::map<std::string, std::string>& map)
{
    std::vector<std::string> invalid_keys = std::vector<std::string>();
    std::vector<std::string> input_keys = std::vector<std::string>();
    for (const auto& p : map) input_keys.push_back(p.first);
    
    std::set_difference(input_keys.cbegin(), input_keys.cend(),
        GastricGlandParameters::valid_keys.cbegin(), GastricGlandParameters::valid_keys.cend(),
        std::back_inserter(invalid_keys));

    if (!invalid_keys.empty())
    {
        std::stringstream ss;
        ss << "Invalid arguments passed for GastricGlandParameters: " << std::endl;
        for (const std::string& s : invalid_keys) ss << "    \"" << s << '"' << std::endl;
        ss << "Try runnnig with -h or --help for a list of valid keys." << std::endl;
        EXCEPTION(ss.str());
    }
}
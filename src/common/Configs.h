#ifndef MICROMACHINES_CONFIGS_H
#define MICROMACHINES_CONFIGS_H

#define CONFIG_PATH "configs.json"

#include <vector>
#include <string>
#include "json/json.hpp"
#include <model/Vehicle/CarSpecs.h>

using json = nlohmann::json;

class Configs {
    json conf_json;
    Configs();

public:
    int max_players;
    int max_len_name;
    int time_to_start;
    int server_frames;
    int socket_pending_connections;

    float respawn_time;
    float modifier_spawn_prob;
    float mod_oil_prob;
    float mod_mud_prob;
    float mod_fix_prob;
    float mod_rock_prob;
    float mod_boost_prob;

    bool use_dynamic_objects;

    CarSpecs specs{};

    std::string map_path;
    std::string server_port;
    std::string plugins_path;

    int total_maps;
    std::vector<std::string> maps;

    static Configs& get_configs();
};

#endif //MICROMACHINES_CONFIGS_H

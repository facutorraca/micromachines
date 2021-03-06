#ifndef MICROMACHINES_MAPLOADER_H
#define MICROMACHINES_MAPLOADER_H

#include <list>
#include <string>
#include <unordered_map>
#include "json/json.hpp"
#include <common/Coordinate.h>
#include <model/Vehicle/Car.h>
#include <model/Terrains/Terrain.h>

class RacingTrack;

class MapLoader {

    std::string map_name;
    std::string map_paths;

    Orientation finish_line_orientation;
    std::vector<Coordinate> finish_line;
    std::vector<Coordinate> unranked_places;
    std::vector<std::unique_ptr<Terrain>> track;
    std::unordered_map<int32_t, Coordinate> podium;

    private:
        void set_begin_distance_to_tiles();

        void load_racing_track(RacingTrack& racingTrack);

        void open_files(nlohmann::json& map_file, nlohmann::json& tiles_file);

        void add_tile_behaviour(int32_t type_ID, int32_t i, int32_t j, int32_t rot, RacingTrack& racingTrack);

    public:
        explicit MapLoader(std::string map_path, std::string map_name);

        void load_map(RacingTrack& racing_track, ClientUpdater& updater);

        void change_map(std::string new_map);
};

#endif //MICROMACHINES_MAPLOADER_H

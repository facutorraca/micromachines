#ifndef MICROMACHINES_RACINGTRACK_H
#define MICROMACHINES_RACINGTRACK_H

#include <list>
#include <memory>
#include "Podium.h"
#include "Box2D/Box2D.h"
#include <unordered_map>
#include "ContactListener.h"
#include <model/FinishLine.h>
#include "StaticTrackObject.h"
#include <model/Vehicle/Car.h>
#include <server/ClientUpdater.h>
#include <common/ProtectedQueue.h>
#include <common/ProtocolSocket.h>
#include <model/Modifiers/Modifier.h>
#include <server/MapLoader.h>
#include "model/Terrains/Terrain.h"
#include "DynamicTrackObject.h"

class RacingTrack {
    MapLoader map_loader;

    b2World racing_track;

    float32 time_step;
    int32 velocity_iterations;
    int32 position_iterations;

    Podium* podium;
    FinishLine* finish_line;
    std::vector<Coordinate> pit_stops;
    std::vector<Coordinate> spawn_points;

    std::vector<std::unique_ptr<Terrain>> track;
    std::list<std::unique_ptr<Terrain>> terrains;
    std::list<StaticTrackObject> static_track_objects;
    std::list<DynamicTrackObject> dynamic_track_objects;

    ContactListener contact_listener;

    public:
        explicit RacingTrack(std::string& map_path, std::string& map_name);

        void update();

        void restart();

        void add_car(Car& car);

        void remove_car(Car& car);

        Coordinate get_random_track_position();

        void change_map(std::string new_map);

        std::vector<Coordinate>& get_pit_stop_position();

        void prepare_track(ClientUpdater& updater);

        void add_pit_stop(Coordinate pit_stop);

        void add_car_to_podium(Car& car, int32_t ID);

        void add_spawn_point(Coordinate spawn_point);

        void add_track(std::unique_ptr<Terrain>&& track);

        void add_unranked_place(Coordinate unranked_place);

        void add_terrain(std::unique_ptr<Terrain>&& terrain);

        void send_dynamic_object_update(ClientUpdater& updater);

        void add_static_track_object(StaticTrackObject&& object);

        void add_dynamic_track_object(DynamicTrackObject&& object);

        void add_modifier(const std::shared_ptr<Modifier>& modifier);

        void set_spawn_points_to_cars(std::unordered_map<int32_t, Car>& cars);

        void add_podium(Coordinate f_place, Coordinate s_place, Coordinate t_place);

        void add_finish_line(Coordinate begin, Coordinate end, Orientation orientation);

        ~RacingTrack();
};


#endif //MICROMACHINES_RACINGTRACK_H

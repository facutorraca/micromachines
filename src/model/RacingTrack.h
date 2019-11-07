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
#include "model/Terrains/Terrain.h"

class RacingTrack {
    b2World racing_track;

    float32 time_step;
    int32 velocity_iterations;
    int32 position_iterations;

    int32_t height;
    int32_t width;
    int32_t track_terrain;

    Podium* podium;
    FinishLine* finish_line;
    std::vector<Coordinate> spawn_points;

    std::vector<std::unique_ptr<Terrain>> track;
    std::list<std::unique_ptr<Terrain>> terrains;
    std::list<StaticTrackObject> static_track_objects;

    ContactListener contact_listener;

    public:
        explicit RacingTrack(std::string& map_path, std::string& map_name);

        void update();

        void add_car(Car& car);

        Coordinate get_random_track_position();

        void set_track_terrain(int32_t terrain);

        void add_car_to_podium(Car& car, int32_t ID);

        void add_spawn_point(Coordinate spawn_point);

        void add_track(std::unique_ptr<Terrain>&& track);

        void add_terrain(std::unique_ptr<Terrain>&& terrain);

        void send(ClientUpdater& client_updater, int32_t ID);

        void set_finish_line(Coordinate begin, Coordinate end);

        void add_static_track_object(StaticTrackObject&& object);

        void set_track_size(int32_t height, int32_t track_width);

        void set_spawn_points_to_cars(std::unordered_map<int32_t, Car>& cars);

        void set_podium(Coordinate f_place, Coordinate s_place, Coordinate t_place);

        ~RacingTrack();
};


#endif //MICROMACHINES_RACINGTRACK_H

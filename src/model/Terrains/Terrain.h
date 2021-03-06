#ifndef MICROMACHINES_TERRAIN_H
#define MICROMACHINES_TERRAIN_H

#include <cstdint>
#include "Box2D/Box2D.h"
#include <model/Sensor.h>
#include <server/UpdateClient.h>
#include <model/Vehicle/Wheel.h>
#include <model/Orientation.h>

class Terrain : public Sensor {

    protected:
        Coordinate coordinate;

        int32_t ID;
        bool is_limit;
        int32_t begin_distance;

        b2Body* terrain_body;
        b2Fixture* terrain_fixture;

    public:
        Terrain(int32_t ID, int32_t x, int32_t y);

        Terrain(Terrain&& other) noexcept;

        void set_orientation(Orientation new_orientation);

        virtual void apply_terrain_effect(Body* body) = 0;

        void set_begin_distance(int32_t distance);

        virtual void set_terrain_user_data() = 0;

        void apply_effect(Body* body) override;

        void add_to_world(b2World& world);

        Coordinate get_map_coordinate();

        void set_as_limit();

        int32_t get_ID();
};

#endif //MICROMACHINES_TERRAIN_H

#include "Car.h"
#include "Dead.h"
#include "Wheel.h"
#include "Alive.h"
#include <iostream>
#include "CarSpecs.h"
#include "common/Key.h"
#include "EngineOff.h"
#include "EngineOn.h"
#include <common/Sizes.h>
#include <common/MsgTypes.h>
#include <common/EntityType.h>
#include <model/Modifiers/OilEffect.h>
#include <model/Modifiers/RockEffect.h>
#include <model/Modifiers/BoostEffect.h>
#include <model/Modifiers/MudEffect.h>

#define NOT_PRESSED 0

#define MAX_ROTATION_ANGLE 45.0f
#define ROTATION_PER_SECOND 90.0

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

#define L_BACK_WHEEL_POS 0
#define R_BACK_WHEEL_POS 1
#define L_FRONT_WHEEL_POS 2
#define R_FRONT_WHEEL_POS 3

Car::Car(CarSpecs specs):
    specs(specs),
    lap_altered(false),
    life(specs.max_life),
    throttle(NOT_PRESSED),
    engine_state(new EngineOff()),
    steering_wheel(NOT_PRESSED),
    lap_state(new LapRunning()),
    car_state( new Alive()),
    respawn(0, 0, 0)
{
    this->car_body = nullptr;
    this->front_left_joint = nullptr;
    this->front_right_joint = nullptr;
    this->begin_distance = 0;
}

void Car::add_to_world(b2World &world) {
    /*create car body*/
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    this->car_body = world.CreateBody(&bodyDef);

    /*reduce the world velocity of bodies*/
    this->car_body->SetAngularDamping(0.1);

    b2PolygonShape polygon_shape;
    polygon_shape.SetAsBox(CAR_WIDTH/2 + 0.05, CAR_HEIGHT/2 + 0.05);
    this->car_body->CreateFixture(/*shape*/&polygon_shape, 0.2f);

    this->car_body->SetUserData(this);
    this->create_wheels(world);
}

void Car::create_wheels(b2World& world) {
    //common joint parameters
    b2RevoluteJointDef joint_params;
    joint_params.bodyA = this->car_body; /*Main body*/
    joint_params.enableLimit = true;
    joint_params.localAnchorB.SetZero(); /*center of tire*/

    /*back left tire*/
    auto* back_left_wheel = new Wheel(world, this->specs.max_forward_speed,
                                      this->specs.max_backward_speed,
                                      this->specs.back_wheel_max_force,
                                      this->specs.back_max_lateral_impulse);
    joint_params.bodyB = back_left_wheel->get_body();
    joint_params.localAnchorA.Set(CAR_WIDTH/2.15, -(CAR_HEIGHT/2)*0.62);
    world.CreateJoint(&joint_params);
    this->wheels.push_back(back_left_wheel);

    //back right tire
    auto* back_right_wheel = new Wheel(world, this->specs.max_forward_speed,
                                       this->specs.max_backward_speed,
                                       this->specs.back_wheel_max_force,
                                       this->specs.back_max_lateral_impulse);
    joint_params.bodyB = back_right_wheel->get_body();
    joint_params.localAnchorA.Set(-CAR_WIDTH/2.15, -(CAR_HEIGHT/2)*0.62);
    world.CreateJoint(&joint_params);
    this->wheels.push_back(back_right_wheel);

    //front left tire
    auto* front_left_wheel = new Wheel(world, this->specs.max_forward_speed,
                                       this->specs.max_backward_speed,
                                       this->specs.front_wheel_max_force,
                                       this->specs.front_max_lateral_impulse);
    joint_params.bodyB = front_left_wheel->get_body();
    joint_params.localAnchorA.Set( CAR_WIDTH/2.15, (CAR_HEIGHT/2)*0.62);
    front_left_joint = (b2RevoluteJoint*)world.CreateJoint(&joint_params);
    this->wheels.push_back(front_left_wheel);

    //front right tire
    auto* front_right_wheel = new Wheel(world, this->specs.max_forward_speed,
                                        this->specs.max_backward_speed,
                                        this->specs.front_wheel_max_force,
                                        this->specs.front_max_lateral_impulse);
    joint_params.bodyB = front_right_wheel->get_body();
    joint_params.localAnchorA.Set( -CAR_WIDTH/2.15, (CAR_HEIGHT/2)*0.62);
    front_right_joint = (b2RevoluteJoint*)world.CreateJoint(&joint_params);
    this->wheels.push_back(front_right_wheel);
}

void Car::update() {
    if (this->car_state->try_respawn(this->respawn, this->car_body, this->wheels)) {
        this->car_state.reset(new Alive());
        this->life.restart_life();
        this->turn_on();
    }

    for (auto & wheel : wheels)
        wheel->update(throttle);

    /*axis direction*/
    float desire_angle = get_desire_angle(steering_wheel);
    /*0.5 seconds a complete rotation*/
    float rotation_per_step = (ROTATION_PER_SECOND * DEGTORAD) / 60.0f;

    /*front_left_joint & front_right_joint has the same angle*/
    float current_angle = front_left_joint->GetJointAngle();
    auto r = this->car_body->GetLinearVelocity().Length();
    float angle_to_turn = desire_angle*(exp(-r/32)) - current_angle;

    angle_to_turn = b2Clamp(angle_to_turn, -rotation_per_step, rotation_per_step);

    /*update axis*/
    front_left_joint->SetLimits(current_angle + angle_to_turn, current_angle + angle_to_turn);
    front_right_joint->SetLimits(current_angle + angle_to_turn, current_angle + angle_to_turn);
}

float Car::get_desire_angle(int32_t steering_wheel_movement) {
    float desire_angle = 0;
    if (steering_wheel_movement == TURN_RIGHT) {
        desire_angle = MAX_ROTATION_ANGLE * DEGTORAD;
    }
    if (steering_wheel_movement == TURN_LEFT) {
        desire_angle = -MAX_ROTATION_ANGLE * DEGTORAD;
    }
    return desire_angle;
}

void Car::move(int32_t movement) {
    this->engine_state->move(movement, this->throttle, this->steering_wheel);
}

void Car::stop(int32_t movement) {
    this->engine_state->stop(movement, this->throttle, this->steering_wheel);
}

void Car::collide(Body* body) {
    if (body->get_ID() == TYPE_CAR)
        ((Car*)body)->make_damage(15);
}

int32_t Car::get_ID() {
    return TYPE_CAR;
}

void Car::change_lap_state(std::unique_ptr<LapState> new_lap_state) {
    this->lap_state = std::move(new_lap_state);
    this->lap_altered = true;
}

void Car::modify_laps(LapCounter& lap_counter, int32_t car_ID) {
    if (lap_altered) {//Just for performance
        this->lap_state = this->lap_state->modify_car_laps(lap_counter, car_ID);
        if (lap_counter.car_complete_laps(car_ID)) {
            this->throttle = NOT_PRESSED;
            this->life.restart_life();
            this->engine_state.reset(new EngineOff());
        }
        this->lap_altered = false;
    }
}

bool Car::move_to(Coordinate coordinate, bool soft) {
    if (soft && this->car_body->GetLinearVelocity().Length() > 20)
        //avoid teleporting very roughly
        return false;

    this->car_body->SetLinearVelocity(b2Vec2(0,0));
    this->car_body->SetAngularVelocity(0);

    this->throttle = NOT_PRESSED;
    this->steering_wheel = NOT_PRESSED;

    float x_pos = coordinate.get_x() * TILE_TERRAIN_SIZE;
    float y_pos = coordinate.get_y() * TILE_TERRAIN_SIZE;
    float angle = coordinate.get_angle() * DEGTORAD;

    this->car_body->SetTransform(b2Vec2(x_pos, y_pos), angle);
    for (auto& wheel : this->wheels)
        wheel->move_to(coordinate);

    return true;
}

void Car::turn_on() {
    this->engine_state.reset(new EngineOn());
}

void Car::send_updates(int32_t ID, ClientUpdater &client_updater) {
    std::vector<int32_t> params  {MSG_UPDATE_ENTITY, TYPE_CAR, ID,
                                 (int32_t)(METER_TO_PIXEL * (car_body->GetPosition().x - (CAR_WIDTH*0.5))),
                                 (int32_t)(METER_TO_PIXEL * (car_body->GetPosition().y - (CAR_HEIGHT*0.5))),
                                 (int32_t)(RADTODEG * car_body->GetAngle()),
                                 (int32_t)(METER_TO_PIXEL * car_body->GetLinearVelocity().Length())};

    for (auto& wheel : wheels) {
        params.emplace_back(int32_t(METER_TO_PIXEL * (wheel->get_position().x - (WIDTH_WHEEL*0.5))));
        params.emplace_back(int32_t(METER_TO_PIXEL * (wheel->get_position().y - (HEIGHT_WHEEL*0.5))));
        params.emplace_back(int32_t(wheel->get_angle()));
    }

    if (this->throttle == BRAKE)
        client_updater.send_to_all(UpdateClient(std::vector<int32_t>{MSG_CAR_SLOWDOWN, ID}));

    client_updater.send_to_all(UpdateClient(std::move(params)));
    this->life.send_updates(ID, client_updater);
    this->wheels[L_BACK_WHEEL_POS]->send_effect_update(ID, client_updater);
}

void Car::repair() {
    this->life.restart_life();
}

void Car::apply_oil_effect() {
    this->wheels[L_BACK_WHEEL_POS]->apply_effect(std::unique_ptr<Effect>(new OilEffect()));
    this->wheels[R_BACK_WHEEL_POS]->apply_effect(std::unique_ptr<Effect>(new OilEffect()));
}

void Car::apply_rock_effect() {
    for (auto& wheel : this->wheels) {
        wheel->apply_effect(std::unique_ptr<Effect>(new RockEffect()));
    }
    this->make_damage(10);
}

void Car::apply_boost_effect() {
    for (auto& wheel : this->wheels) {
        wheel->apply_effect(std::unique_ptr<Effect>(new BoostEffect()));
    }
}

void Car::apply_mud_effect() {
    for (auto& wheel : this->wheels) {
        wheel->apply_effect(std::unique_ptr<Effect>(new MudEffect()));
    }
}

void Car::get_dto_info(int32_t ID, DTO_Car &car_info) {
    car_info.ID = ID;
    car_info.specs = this->specs;
    car_info.modifier = NO_MODIFIER;
    this->life.get_dto_info(car_info.life);
}

void apply_modifier_plugin(Car* car, modifier_t modifier) {
    switch (modifier) {
        case OIL:
            car->apply_oil_effect();
            break;
        case FIX:
            car->repair();
            break;
        case ROCK:
            car->apply_rock_effect();
            break;
        case MUD:
            car->apply_mud_effect();
            break;
        case BOOST:
            car->apply_boost_effect();
            break;
        case NO_MODIFIER:
            break;
    }
}

void Car::apply_plugin(DTO_Car &car_info) {
    this->wheels[L_BACK_WHEEL_POS]->apply_plugin(car_info.specs.max_forward_speed,
                                                 car_info.specs.max_backward_speed,
                                                 car_info.specs.back_wheel_max_force,
                                                 car_info.specs.back_max_lateral_impulse);

    this->wheels[R_BACK_WHEEL_POS]->apply_plugin(car_info.specs.max_forward_speed,
                                                 car_info.specs.max_backward_speed,
                                                 car_info.specs.back_wheel_max_force,
                                                 car_info.specs.back_max_lateral_impulse);

    this->wheels[L_FRONT_WHEEL_POS]->apply_plugin(car_info.specs.max_forward_speed,
                                                  car_info.specs.max_backward_speed,
                                                  car_info.specs.front_wheel_max_force,
                                                  car_info.specs.front_max_lateral_impulse);

    this->wheels[R_FRONT_WHEEL_POS]->apply_plugin(car_info.specs.max_forward_speed,
                                                  car_info.specs.max_backward_speed,
                                                  car_info.specs.front_wheel_max_force,
                                                  car_info.specs.front_max_lateral_impulse);

    this->life.apply_plugin(car_info.life);
    this->specs = car_info.specs;
    apply_modifier_plugin(this, car_info.modifier);
}

void Car::make_damage(int32_t damage) {
    this->life.make_damage(damage);
    if (this->life.is_dead()) {
        this->engine_state.reset( new EngineOff());
        this->car_state.reset( new Dead());
    }
}

void Car::set_begin_distance(int32_t new_begin_distance) {
    this->begin_distance = new_begin_distance;
}

int32_t Car::get_begin_distance() {
    return this->begin_distance;
}

void Car::set_respawn(Coordinate new_respawn) {
    this->car_state->set_respawn_position(this->respawn, new_respawn);
}

void Car::explode() {
    this->life.kill();
    this->engine_state.reset( new EngineOff());
    this->car_state.reset(new Dead());
}

void Car::remove_from_race(b2World& world) {
    for (auto& wheel : this->wheels) {
        world.DestroyBody(wheel->get_body());
    }
    world.DestroyBody(this->car_body);
}

Car::~Car() {
    for (auto & wheel : this->wheels) {
        delete wheel;
    }
}

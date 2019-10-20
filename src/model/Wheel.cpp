#include <iostream>
#include "Wheel.h"
#include "common/Key.h"
#include "Box2D/Box2D.h"
#include "RacingTrack.h"

#define WIDTH_WHEEL 0.3f
#define HEIGHT_WHEEL 1.0f
#define RADTODEG 57.295779513082320876f

Wheel::Wheel(RacingTrack& racing_track, float max_forward_speed, float max_backward_speed, float max_driver_force, float max_lateral_impulse) {
    this->max_lateral_impulse = max_lateral_impulse; /*Capacity of changing direction without skidding*/
    this->max_backward_speed = max_backward_speed;
    this->max_forward_speed = max_forward_speed;
    this->max_driver_force= max_driver_force; /*Capacity of changing speed*/

    b2BodyDef wheel_body_def;
    wheel_body_def.type = b2_dynamicBody;
    /*Allocates memory for the wheels's body*/
    this->wheel_body = racing_track.get_world().CreateBody(&wheel_body_def);

    b2PolygonShape polygon;
    polygon.SetAsBox(WIDTH_WHEEL, HEIGHT_WHEEL);
    this->wheel_body->CreateFixture(/*Shape*/&polygon, 1);
}

b2Vec2 Wheel::get_lateral_velocity() {
    /*GetWorldVector return the current orientation of these versor*/
    b2Vec2 current_lateral_orientation = wheel_body->GetWorldVector(b2Vec2(1, 0));
    return b2Dot(current_lateral_orientation,wheel_body->GetLinearVelocity()) * current_lateral_orientation;
}

b2Vec2 Wheel::get_forward_velocity() {
    /*GetWorldVector return the current orientation of these versor*/
    b2Vec2 current_forward_orientation = wheel_body->GetWorldVector(b2Vec2(0, 1));
    return b2Dot(current_forward_orientation, wheel_body->GetLinearVelocity()) * current_forward_orientation;
}

void Wheel::update_speed(uint8_t key) {
    /*Find if whe want to go UP or DOWN*/
    float desire_speed = 0;
    if (key == KEY_UP) {
        desire_speed = this->max_forward_speed;
    } else if (key == KEY_DOWN) {
        desire_speed = this->max_backward_speed;
    } else {
        return;
    }

    /*Find current speed in forward direction*/
    b2Vec2 current_forward_velocity = this->wheel_body->GetWorldVector(b2Vec2(0, 1) );
    float current_speed = b2Dot(this->get_forward_velocity(), current_forward_velocity);

    /*Apply necessary force*/
    float force = 0;
    if (desire_speed > current_speed) {
        force = max_driver_force;
    } else if (desire_speed < current_speed) {
        force = (-max_driver_force);
    } else {
        return;
    }
    /*Apply a force on the wheels center*/
    this->wheel_body->ApplyForce(force * current_forward_velocity,this->wheel_body->GetWorldCenter(),true);
}

void Wheel::update_friction() {
    /*lateral linear velocity*/
    b2Vec2 impulse = this->wheel_body->GetMass() * -this->get_lateral_velocity();

    /*---------------------------------------------*/
    /* restrict that impulse to some maximum value */
    /* and the tire will slip when the             */
    /* circumstances require a greater correction  */
    /* than allowable                              */
    /*---------------------------------------------*/
    if (impulse.Length()/*Absolute value*/ > this->max_lateral_impulse) {
        impulse *= this->max_lateral_impulse / impulse.Length();
    }
    this->wheel_body->ApplyLinearImpulse(impulse, this->wheel_body->GetWorldCenter(), true);

    //angular velocity
    this->wheel_body->ApplyAngularImpulse(0.1f * this->wheel_body->GetInertia() * -this->wheel_body->GetAngularVelocity(), true);

    //forward linear velocity
    b2Vec2 current_forward_velocity = this->get_forward_velocity();
    float current_forward_speed = current_forward_velocity.Normalize(); /*Normalize returns absolute value*/
    float drag_force_magnitude = -2 * current_forward_speed;
    this->wheel_body->ApplyForce(drag_force_magnitude * current_forward_velocity, this->wheel_body->GetWorldCenter(), true);
}

void Wheel::update(uint8_t key) {
    this->update_friction();
    this->update_speed(key);
}

const b2Vec2& Wheel::get_position() {
    return this->wheel_body->GetPosition();
}

float Wheel::get_angle() {
    return RADTODEG * this->wheel_body->GetAngle();
}

b2Body* Wheel::get_body() {
    return this->wheel_body;
}

Wheel::~Wheel() {
    /* Capay os deberia destruir el sever*/
    this->wheel_body->GetWorld()->DestroyBody(this->wheel_body);
}

Wheel::Wheel(Wheel &&other_wheel) noexcept {
    this->wheel_body = other_wheel.wheel_body;
    this->max_lateral_impulse = other_wheel.max_lateral_impulse;
    this->max_backward_speed = other_wheel.max_backward_speed;
    this->max_forward_speed = other_wheel.max_forward_speed;
    this->max_driver_force = other_wheel.max_driver_force;

    other_wheel.wheel_body = nullptr;
    other_wheel.max_lateral_impulse = 0;
    other_wheel.max_backward_speed = 0;
    other_wheel.max_forward_speed = 0;
    other_wheel.max_driver_force = 0;
}

//
// Created by javif on 30/10/2019.
//

#ifndef MICROMACHINES_SETCARHEALTH_H
#define MICROMACHINES_SETCARHEALTH_H

#include "Command.h"

class SetCarHealth : public Command{
    int32_t car_id;
    int32_t health;
public:
    explicit SetCarHealth(Scenario& scenario, int32_t id, int32_t health):
        Command(scenario),
        car_id(id),
        health(health){}

    std::unique_ptr<Menu> apply() override {
        scenario.setCarHealth(car_id, health);
        return std::unique_ptr<Menu>();
    }
};

#endif //MICROMACHINES_SETCARHEALTH_H

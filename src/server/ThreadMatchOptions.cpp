#include <utility>
#include "server/Player.h"
#include "ThreadMatchOptions.h"
#include <common/SocketError.h>
#include <common/MsgTypes.h>
#include <common/Configs.h>
#include "common/ProtectedQueue.h"
#include "MatchTable.h"

#define START_MATCH 0
#define CANCEL_MATCH 1

ThreadMatchOptions::ThreadMatchOptions(Player&& player, std::shared_ptr<Match>&& match):
    player(std::move(player)),
    match(std::move(match)),
    dead(false)
{}

void ThreadMatchOptions::start_match_options(ProtectedQueue<std::shared_ptr<Match>>* not_ready_matches,  MatchTable& matches) {
    this->thread = std::thread(&ThreadMatchOptions::run_match_options, this, not_ready_matches, std::ref(matches));
}

void ThreadMatchOptions::start_player_options() {
    this->thread = std::thread(&ThreadMatchOptions::run_player_options, this);
}

bool ThreadMatchOptions::options_set() {
    return this->dead;
}

void ThreadMatchOptions::stop() {
    this->player.kill();
    this->dead = true;
}

void ThreadMatchOptions::run_match_options(ProtectedQueue<std::shared_ptr<Match>>* not_ready_matches, MatchTable& matches) {
    try {
        uint8_t option = 5;

        while (option != START_MATCH && option != CANCEL_MATCH) {
            option = this->player.receive_option();
            /*MATCH APPLY OPTION*/

            if (option == CANCEL_MATCH) {
                this->match->send_cancel_match_flag();
                matches.remove_match(this->match->get_match_name());
            } else if (option == MSG_SET_MAP) {
                uint8_t new_map_index = this->player.receive_option();
                if (new_map_index < Configs::get_configs().maps.size())
                    this->match->change_map(Configs::get_configs().maps.at(new_map_index));
            } else {
                /*Put the creator on the match*/
                this->match->add_player(std::move(player));
                not_ready_matches->push(this->match);
            }
        }
        this->dead = true;
    } catch (const SocketError& exception) {
        matches.remove_match(this->match->get_match_name());
        this->match->send_cancel_match_flag();
        this->dead = true;
    }
}

void ThreadMatchOptions::run_player_options() {
    try {
        uint8_t option = 0;
        option = this->player.receive_option();

        this->match->add_player(std::move(player));

        /* for futures options*/
        this->dead = true;
    } catch (const SocketError& exception) {
        this->dead = true;
    }
}

void ThreadMatchOptions::join() {
    this->thread.join();
}


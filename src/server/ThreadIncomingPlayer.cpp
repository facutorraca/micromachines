#include <iostream>
#include "server/Player.h"
#include <common/Configs.h>
#include <common/MsgTypes.h>
#include "common/SocketError.h"
#include "ThreadIncomingPlayer.h"
#include "common/ProtectedQueue.h"
#include <common/ProtocolSocket.h>

#define JOIN_MODE 1
#define CREATE_MODE 2

#define BIT_ERROR 1
#define BIT_SUCCESS 0

ThreadIncomingPlayer::ThreadIncomingPlayer(ProtocolSocket&& p_socket, ProtectedQueue<Player>& incoming_players, MatchTable& matches):
    incoming_players(incoming_players),
    p_socket(std::move(p_socket)),
    matches(matches),
    dead(false)
{
    this->match_mode_set = false;
    this->username_set = false;
    this->match_name_set = false;
}

bool ThreadIncomingPlayer::answered() {
    return this->dead;
}

void ThreadIncomingPlayer::stop() {
    this->p_socket.close();
    this->dead = true;
}


void ThreadIncomingPlayer::receive_match_name(std::string& match_name, uint8_t mode) {
    size_t max_len_name = Configs::get_configs().max_len_name;

    this->p_socket.receive_n(match_name, max_len_name);
    if (mode == CREATE_MODE && !this->matches.match_name_available(match_name)){
        match_name.clear(); match_name.resize(max_len_name);
        this->p_socket.send((uint8_t)BIT_ERROR);
        //this->p_socket.receive_n(match_name, max_len_name);
        return;
    }
    this->p_socket.send((uint8_t)BIT_SUCCESS);
    this->match_name_set = true;
}

void ThreadIncomingPlayer::receive_username(std::string& username, std::string& match_name, uint8_t mode) {
    size_t max_len_name = Configs::get_configs().max_len_name;

    this->p_socket.receive_n(username, max_len_name);
    if (mode == JOIN_MODE && !this->matches.username_available(username, match_name)){
        username.clear(); username.resize(max_len_name);
        this->p_socket.send((uint8_t)BIT_ERROR);
        //this->p_socket.receive_n(username, max_len_name);
        return;
    }
    this->p_socket.send((uint8_t)BIT_SUCCESS);
    this->username_set = true;

}

void send_maps(ProtocolSocket& p_socket) {
    std::string maps_to_send;

    for(auto& map : Configs::get_configs().maps) {
        int index = map.find(".json");
        std::string map_name_copy = map;

        if (index !=  std::string::npos)
            map_name_copy.replace(index, 5,"");

        maps_to_send.append(map_name_copy);
        maps_to_send.append("\n");
    }

    p_socket.send(maps_to_send);
}

void ThreadIncomingPlayer::run() {
    try {
        this->matches.send_matches(this->p_socket);

        size_t max_len_name = Configs::get_configs().max_len_name;

        uint8_t mode;
        this->p_socket.receive(mode);

        uint8_t option;
        std::string username;
        std::string match_name;

        while (!match_name_set || !username_set) {
            this->p_socket.receive(option);

            switch (option) {
                case MSG_SET_USERNAME:
                    username.resize(max_len_name, '\0');
                    if (!match_name_set) {
                        this->p_socket.receive_n(username, max_len_name);
                        this->p_socket.send((uint8_t)BIT_ERROR);
                    } else {
                        this->receive_username(username, match_name, mode);
                    }
                    break;
                case MSG_SET_MATCH_NAME:
                    match_name.resize(max_len_name, '\0');
                    this->receive_match_name(match_name, mode);
                    break;
                case MSG_CHANGE_MODE:
                    this->p_socket.receive(mode);
                    this->username_set = false;
                    this->match_name_set = false;
                    break;
                case MSG_GET_MATCHES:
                    this->matches.send_matches(this->p_socket);
                    break;
                case MSG_GET_MAP:
                    send_maps(this->p_socket);
                    break;
                default:
                    break;
            }
        }

        Player new_player(std::move(this->p_socket), mode, username, match_name);
        this->incoming_players.push(std::move(new_player));
        this->dead = true;
    } catch (const SocketError& exception) {
        this->stop();
    }
}


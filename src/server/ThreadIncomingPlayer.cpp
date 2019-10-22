#include <iostream>
#include "server/Player.h"
#include "common/SocketError.h"
#include "ThreadIncomingPlayer.h"
#include "common/ProtectedQueue.h"
#include <common/ProtocolSocket.h>

#define JOIN_MODE 1
#define CREATE_MODE 2

#define BIT_ERROR 1
#define BIT_SUCCESS 0

#define MAX_LEN_NAME 20

ThreadIncomingPlayer::ThreadIncomingPlayer(ProtocolSocket&& p_socket, ProtectedQueue<Player>& incoming_players, MatchTable& matches):
    incoming_players(incoming_players),
    p_socket(std::move(p_socket)),
    matches(matches),
    dead(false)
{}

bool ThreadIncomingPlayer::answered() {
    return this->dead;
}

void ThreadIncomingPlayer::stop() {
    this->p_socket.close();
    this->dead = true;
}


void ThreadIncomingPlayer::receive_match_name(std::string& match_name, uint8_t mode) {
    uint8_t bit_error = BIT_ERROR;
    uint8_t bit_success = BIT_SUCCESS;

    this->p_socket.receive(match_name);
    while (mode == CREATE_MODE && !this->matches.match_name_available(match_name)){
        match_name.clear(); match_name.resize(MAX_LEN_NAME);
        this->p_socket.send(bit_error);
        this->p_socket.receive(match_name);
    }
    this->p_socket.send(bit_success);
}

void ThreadIncomingPlayer::receive_username(std::string& username, std::string& match_name, uint8_t mode) {
    uint8_t bit_error = BIT_ERROR;
    uint8_t bit_success = BIT_SUCCESS;

    this->p_socket.receive(username);
    while (mode == JOIN_MODE && !this->matches.username_available(username, match_name)){
        username.clear(); username.resize(MAX_LEN_NAME);
        this->p_socket.send(bit_error);
        this->p_socket.receive(username);
    }
    this->p_socket.send(bit_success);
}

void ThreadIncomingPlayer::run() {
    try {
        this->matches.send_matches(this->p_socket);

        uint8_t mode;
        this->p_socket.receive(mode);

        std::string match_name(MAX_LEN_NAME, '\0');
        this->receive_match_name(match_name, mode);

        std::string username(MAX_LEN_NAME, '\0');
        this->receive_username(username, match_name, mode);

        Player new_player(std::move(this->p_socket), mode, username, match_name);
        this->incoming_players.push(std::move(new_player));

        this->dead = true;
    } catch (const SocketError& exception) {
        this->dead = true;
    }

}

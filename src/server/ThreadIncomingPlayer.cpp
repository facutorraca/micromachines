#include <iostream>
#include "server/Player.h"
#include "common/SocketError.h"
#include "ThreadIncomingPlayer.h"
#include "common/ProtectedQueue.h"
#include <common/ProtocolSocket.h>

#define JOIN_MODE 1
#define CREATE_MODE 2
#define ERROR_MATCH_NAME "ERROR: Match name is already used \n"
#define ACCEPTED_MATCH_NAME "OK: Match name is available \n"
#define ERROR_USERNAME "ERROR: Username is already used in match: "
#define ACCEPTED_USERNAME "OK: Username is available \n"

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
    std::vector<uint8_t> buffer(4096);

    std::string match_name_error(ERROR_MATCH_NAME);
    std::string match_name_accepted(ACCEPTED_MATCH_NAME);

    this->p_socket.receive(buffer);
    match_name.assign(reinterpret_cast<const char *>(buffer.data()), buffer.size());
    while (mode == CREATE_MODE && !this->matches.match_name_available(match_name)){
        buffer.clear(); buffer.resize(4096);
        this->p_socket.send(match_name_error);
        this->p_socket.receive(buffer);
        match_name.assign(reinterpret_cast<const char *>(buffer.data()), buffer.size());
    }
    this->p_socket.send(match_name_accepted);
}

void ThreadIncomingPlayer::receive_username(std::string& username, std::string& match_name, uint8_t mode) {
    std::vector<uint8_t> buffer(4096);

    std::string username_error(ERROR_USERNAME + match_name + "\n");
    std::string username_accepted(ACCEPTED_USERNAME);

    this->p_socket.receive(buffer);
    username.assign(reinterpret_cast<const char *>(buffer.data()), buffer.size());
    while (mode == JOIN_MODE && !this->matches.username_available(username, match_name)){
        buffer.clear(); buffer.resize(4096);
        this->p_socket.send(username_error);
        this->p_socket.receive(buffer);
        username.assign(reinterpret_cast<const char *>(buffer.data()), buffer.size());
        std::cout<< "sigo" << "\n";
    }
    this->p_socket.send(username_accepted);
}

void ThreadIncomingPlayer::run() {
    try {
        this->matches.send_matches(this->p_socket);

        uint8_t mode;
        this->p_socket.receive(mode);

        std::vector<uint8_t> buffer(4096);

        std::string match_name;
        this->receive_match_name(match_name, mode);

        std::string username;
        this->receive_username(username, match_name, mode);

        Player new_player(std::move(this->p_socket), mode, username, match_name);
        this->incoming_players.push(std::move(new_player));

        this->dead = true;
    } catch (const SocketError& exception) {
        this->dead = true;
    }

}

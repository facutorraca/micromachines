#include <map>
#include <list>
#include <mutex>
#include <iostream>
#include "MatchTable.h"
#include <common/ProtocolSocket.h>

void MatchTable::add(const std::string& match_name, Match* match) {
    std::lock_guard<std::mutex> lock(this->mtx);
    if (!this->map.insert(std::pair<std::string, Match*>(match_name, match)).second) {
        std::cout << "error" << "\n";
    }
    std::cout << this->map.size() <<"\n";
}

Match* MatchTable::get(const std::string& match_name) {
    std::lock_guard<std::mutex> lock(this->mtx);
    return this->map[match_name];
}

void MatchTable::remove_end_matches() {
    std::lock_guard<std::mutex> lock(this->mtx);
    for (auto it = this->map.begin(); it != this->map.end();) {
        if ((*it).second->ended()) {
            it = this->map.erase(it);
        } else {
            it++;
        }
    }
}

void MatchTable::send_matches(ProtocolSocket &p_socket) {
    int match_index = 1;
    for (auto & it : this->map) {
        std::string match_name_to_send = it.second->get_match_name_to_send(match_index);
        p_socket.send(match_name_to_send);
        match_index++;
    }
    p_socket.send_end_byte();
}







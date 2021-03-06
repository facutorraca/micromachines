#ifndef MICROMACHINES_PLAYER_H
#define MICROMACHINES_PLAYER_H

#include <atomic>
#include <string>
#include "UpdateRace.h"
#include "UpdateClient.h"
#include "common/Socket.h"
#include <common/ProtocolSocket.h>

class Player {
    ProtocolSocket p_socket;

    /*-----Options-----*/
    uint8_t mode;

    int32_t ID;
    std::string username;
    std::string match_name;

    bool playing;

    std::atomic<bool> view_changed;
    unsigned current_view_ID;

    public:
        Player(ProtocolSocket&& p_socket, uint8_t mode, std::string username, std::string match_name);

        Player(Player&& other) noexcept;

        void update_view(int32_t total_players, ClientUpdater& updater);

        void set_finished(ClientUpdater& updater);

        void restart_playing(ClientUpdater& updater);

        bool is_called(std::string& username);

        void send(std::vector<int32_t>& msg);

        void send(UpdateClient update);

        void send(std::string& msg);

        void send(uint8_t flag);

        std::string get_match_name();

        UpdateRace receive_update();

        std::string get_username();

        uint8_t receive_option();

        void set_ID(int32_t id);

        bool is_on_join_mode();

        bool is_playing();

        void set_view(int32_t ID);

        int32_t get_ID();

        void kill();

        ~Player();
};

#endif //MICROMACHINES_PLAYER_H

#ifndef MICROMACHINES_THREADINCOMINGPLAYER_H
#define MICROMACHINES_THREADINCOMINGPLAYER_H

#include <atomic>
#include "Thread.h"
#include "Player.h"
#include "MatchTable.h"
#include "common/Socket.h"
#include "ProtectedQueue.h"
#include <common/ProtocolSocket.h>

class ThreadIncomingPlayer : public Thread {
    ProtocolSocket p_socket;

    MatchTable& matches;
    ProtectedQueue<Player>& incoming_players;

    std::atomic<bool> dead;

    private:
        void run() override;

    public:
        ThreadIncomingPlayer(ProtocolSocket&& p_socket, ProtectedQueue<Player>& incoming_players, MatchTable& matches);

        bool answered();
};

#endif //MICROMACHINES_THREADINCOMINGPLAYER_H

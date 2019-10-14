#ifndef MICROMACHINES_THREADACCEPTOR_H
#define MICROMACHINES_THREADACCEPTOR_H

#include <list>
#include <string>
#include <atomic>
#include "Thread.h"
#include "Player.h"
#include "MatchTable.h"
#include "SocketAcceptor.h"
#include "ProtectedQueue.h"
#include "ThreadIncomingPlayer.h"

class ThreadAcceptor : public Thread {
    SocketAcceptor acceptor;

    MatchTable& matches;
    ProtectedQueue<Player>& incoming_players;

    std::list<ThreadIncomingPlayer*> new_players;
    std::atomic<bool> server_running{};

    private:
        void run() override;

        void remove_confirmed_players();

        void remove_all_players();

    public:
        ThreadAcceptor(const std::string& port, ProtectedQueue<Player>& incoming_players, MatchTable& matches);

        void stop();
};


#endif //MICROMACHINES_THREADACCEPTOR_H

#ifndef MICROMACHINES_SERVER_H
#define MICROMACHINES_SERVER_H

#include <list>
#include <memory>
#include "MatchTable.h"
#include "common/ProtectedQueue.h"
#include "ThreadAcceptor.h"
#include "ThreadMatchStarter.h"
#include "ThreadPlayerLocator.h"

class Server {
    MatchTable matches;
    ProtectedQueue<Player> incoming_players;
    ProtectedQueue<std::shared_ptr<Match>> not_ready_matches;

    ThreadAcceptor* acceptor;
    ThreadMatchStarter* match_starter;
    ThreadPlayerLocator* player_locator;

    std::list<std::shared_ptr<Match>> running_matches;

    private:
        static void wait_quit();

        void stop_matches();

    public:
        explicit Server(const std::string& port);

        void start();

        ~Server();
};

#endif //MICROMACHINES_SERVER_H

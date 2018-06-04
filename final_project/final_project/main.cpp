//
//  main.cpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#include <iostream>


#include "BankServer.hpp"
#include "Server.hpp"
#include "PaxosDecorator.hpp"
#include "BankServerInterface.hpp"
#include "Utility.hpp"

int main(int argc, const char * argv[]) {

    srand(time(NULL));

    BankServer *server = new BankServer(0, utility::NETWORK_SIZE, utility::NET_CONFIG_FILE, utility::INITIAL_BALANCE);
    PaxosDecorator *paxos = new PaxosDecorator(server, utility::REQUEST_QUEUE_SIZE);
    BankServerInferface interface(server, paxos);

    interface.launch_inferface();

    delete server;
    delete paxos;

    return 0;
}

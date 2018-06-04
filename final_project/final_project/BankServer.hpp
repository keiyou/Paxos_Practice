//
//  BankServer.hpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#ifndef BankServer_hpp
#define BankServer_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#include "Server.hpp"


class BankServer: public Server {
private:
    int balance;
    std::mutex balance_lock;
public:
    BankServer(int siteNumber, int networkSize, std::string configFile, int initial_balance);
    ~BankServer();

    int get_balance();
    void transfer_money(int from, int to, int credit);
    void print_balance();
};

#endif /* BankServer_hpp */

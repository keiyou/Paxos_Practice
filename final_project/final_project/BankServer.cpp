//
//  BankServer.cpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#include "BankServer.hpp"
#include "Utility.hpp"


BankServer::BankServer(int siteNumber, int networkSize, std::string configFile, int initial_balance): Server(siteNumber, networkSize, configFile){
    this->balance = initial_balance;

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Construct Bank Server\n");
};

BankServer::~BankServer(){
    if(settings::DEBUG_FLAG)
        printf("DEBUG: Deconstruct Bank Server\n");
}


int BankServer::get_balance(){
    return this->balance;
}

void BankServer::transfer_money(int from, int to, int credit){
    this->balance_lock.lock();
    if(this->siteNumber == from){
        this->balance -= credit;
    }
    if(this->siteNumber == to){
        this->balance += credit;
    }
    this->balance_lock.unlock();

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Transfer Request Processed\n");
}

void BankServer::print_balance(){
    std::lock_guard<std::mutex> lk(this->balance_lock);
    printf("Site %s Balance: %d\n", utility::SERVER_NAME[this->siteNumber].c_str(), balance);
}

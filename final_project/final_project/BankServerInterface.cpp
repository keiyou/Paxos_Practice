//
//  BankServerInferface.cpp
//  final_project
//
//  Created by feiyang on 2018/5/27.
//

#include "BankServerInterface.hpp"

#include "Operation.hpp"
#include "Utility.hpp"

#include <algorithm>
#include <sstream>


bool notdigit(char s){
    return !isdigit(s);
}

void BankServerInferface::process_helper(std::string str){
    if(str == "printblockchain"){
        this->paxos->print_blockchain();
        return;
    }
    if(str == "printbalance"){
        this->server->print_balance();
        return;
    }
    if(str == "printqueue"){
        this->paxos->print_queue();
        return;
    }
    if(str == "startserver"){
        this->paxos->start_server(this->paxos);
        std::cout << "--- Server Started\n" << std::endl;
        return;
    }
    if(str == "closeserver"){
        this->paxos->shut_down_server();
    }
    if(str == "exit"){
        this->paxos->shut_down_server();
        std::cout << "--- Server Shut Down\n" << std::endl;
        exitFlag = true;
        return;
    }
    if(str == "debug"){
        settings::DEBUG_FLAG = !settings::DEBUG_FLAG;
    }
    if(str.find("moneytransfer") != std::string::npos){
        std::replace_if(str.begin(), str.end(), notdigit, ' ');
        std::stringstream ss(str);
        int amount = -1, creditNode = -1;
        ss >> amount >> creditNode;
        if(amount<0 || creditNode<0 || creditNode >= utility::NETWORK_SIZE){
            std::cout << "--- Invalid Node or Balance\n" << std::endl;
            return;
        }
        if(amount > this->server->get_prebalance()){
            std::cout << "--- No Enough Credit\n" << std::endl;
            return;
        }
        this->server->use_prebalance(amount);

        TransferOperation* op = new TransferOperation(this->server, this->server->get_site_number(), creditNode, amount);
        int err = this->paxos->new_request(op);
        if(err == 1){
            std::cout << "--- Warning: Network Unavaibale\n" << std::endl;
        }
        return;
    }
    if(str.find("neton") != std::string::npos){
        std::replace_if(str.begin(), str.end(), notdigit, ' ');
        std::stringstream ss(str);
        int node = -1;
        ss >> node;
        if(node < 0 || node >= utility::NETWORK_SIZE){
            std::cout << "--- Invalid Node\n" << std::endl;
            return;
        }
        this->server->net_up(node);
        return;
    }
    if(str.find("netoff") != std::string::npos){
        std::replace_if(str.begin(), str.end(), notdigit, ' ');
        std::stringstream ss(str);
        int node = -1;
        ss >> node;
        if(node < 0 || node >= utility::NETWORK_SIZE){
            std::cout << "--- Invalid Node\n" << std::endl;
            return;
        }
        this->server->net_down(node);
        return;
    }
    if(str == ""){
        return;
    }
    std::cout << "--- Unknown Input\n" << std::endl;
}

BankServerInferface::BankServerInferface(BankServer *s, PaxosDecorator *p){
    this->server = s;
    this->paxos = p;
    this->exitFlag = false;
}

void BankServerInferface::launch_inferface(){
    while(!exitFlag){
        std::string usrInput;
        std::getline(std::cin,usrInput);
        std::transform(usrInput.begin(), usrInput.end(), usrInput.begin(), ::tolower);
        usrInput.erase(std::remove_if(usrInput.begin(), usrInput.end(), isspace), usrInput.end());
        this->process_helper(usrInput);
    }
}

//
//  BankServerInferface.hpp
//  final_project
//
//  Created by feiyang on 2018/5/27.
//

#ifndef BankServerInterface_hpp
#define BankServerInterface_hpp

#include <stdio.h>
#include <iostream>

#include "BankServer.hpp"
#include "PaxosDecorator.hpp"

class BankServerInferface {
private:
    BankServer *server;
    PaxosDecorator *paxos;

    bool exitFlag;
    void process_helper(std::string str);
public:
    BankServerInferface(BankServer *s, PaxosDecorator *p);
    void launch_inferface();
};


#endif /* BankServerInterface_hpp */

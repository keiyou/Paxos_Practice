//
//  ServerDecorator.cpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#include "ServerDecorator.hpp"

#include "Utility.hpp"


ServerDecorator::ServerDecorator(Server* s): Server(s){
    this->server = s;

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Construct Serve Decorator\n");
}

ServerDecorator::~ServerDecorator(){
    if(settings::DEBUG_FLAG)
        printf("DEBUG: Deconstruct Serve Decorator\n");
}

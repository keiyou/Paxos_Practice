//
//  ServerDecorator.hpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#ifndef ServerDecorator_hpp
#define ServerDecorator_hpp

#include <stdio.h>

#include "Server.hpp"

class ServerDecorator: public Server {
protected:
    Server* server;
public:
    ServerDecorator();
    ServerDecorator(Server* s);
    ~ServerDecorator();
};

#endif /* ServerDecorator_hpp */

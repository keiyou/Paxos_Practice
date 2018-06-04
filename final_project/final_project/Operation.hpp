//
//  Operation.hpp
//  final_project
//
//  Created by feiyang on 2018/5/20.
//

#ifndef Operation_hpp
#define Operation_hpp

#include <stdio.h>
#include <iostream>

#include "BankServer.hpp"

class Operation{
protected:
    Server* server;
public:
    Operation(Server *s);
    virtual ~Operation();
    
    virtual void execute(){};
    virtual std::string to_string(){return "";}
    virtual std::string to_json(){return "";}
    void assign_server(Server* s);
    static Operation* from_json(std::string strJson);
};

class TransferOperation final: public Operation {
private:
    int from;
    int to;
    int credit;
public:
    TransferOperation(BankServer* s, int f, int t, int c);
    ~TransferOperation();
    
    void execute();
    std::string to_string();
    std::string to_json();
};


#endif /* Operation_hpp */

//
//  Operation.cpp
//  final_project
//
//  Created by feiyang on 2018/5/20.
//

#include "Operation.hpp"
#include "Utility.hpp"

#include <iostream>
#include <cassert>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


Operation::Operation(Server *s){
    this->server = s;
}

Operation::~Operation(){};

Operation* Operation::from_json(std::string strJson){
    boost::property_tree::ptree pt;
    std::stringstream ss(strJson);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Operation Json Parse Failed\n");
        exit(1);
    }

    boost::property_tree::ptree::iterator it = pt.begin();
    std::string opType = it->first.data();
    assert(++it == pt.end());

    if(opType == "TransferOperation"){
        boost::property_tree::ptree op = pt.get_child("TransferOperation");
        int f = op.get<int>("From");
        int t = op.get<int>("To");
        int c = op.get<int>("Credit");
        TransferOperation* tranOp = new TransferOperation(NULL,f,t,c);
        return tranOp;
    }

    printf("ERROR: Unknown Operation Type\n");
    exit(1);
}

void Operation::assign_server(Server *s){
    this->server = s;
}


TransferOperation::TransferOperation(BankServer* s, int f, int t, int c): Operation(s){
    this->from = f;
    this->to = t;
    this->credit = c;
}

TransferOperation::~TransferOperation(){};

void TransferOperation::execute(){
    BankServer* s = (BankServer*)(this->server);
    s->transfer_money(this->from, this->to, this->credit);
}

std::string TransferOperation::to_string(){
    std::string str = "Transfer Operation: ";
    str += "From Site " + std::to_string(this->from) + ", ";
    str += "To Site " + std::to_string(this->to)+ ", ";
    str += "Credit: " + std::to_string(this->credit);
    return str;
}

std::string TransferOperation::to_json(){
    boost::property_tree::ptree root, operation;
    operation.put("From", this->from);
    operation.put("To", this->to);
    operation.put("Credit", this->credit);

    root.add_child("TransferOperation", operation);

    std::stringstream s;
    boost::property_tree::write_json(s, root, false);
    std::string out = s.str();
    return out;
}

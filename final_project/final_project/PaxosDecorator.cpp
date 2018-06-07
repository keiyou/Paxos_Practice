//
//  PaxosDecorator.cpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#include "PaxosDecorator.hpp"
#include "Utility.hpp"
#include "PaxosUtility.hpp"

#include <random>
#include <chrono>
#include <thread>
#include <cassert>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>



PaxosDecorator::PaxosDecorator(Server* s, int size): ServerDecorator(s){
    this->blockChain = new BlockChain<std::vector<Operation*>>();
    this->requestQueue = new LimitedQueue<Operation*>(size);

    this->ballotNum = std::make_tuple(0, this->get_site_number(), 0);

    this->acceptNum = std::make_tuple(0,0,0);
    this->highestNum = std::make_tuple(0,0,0);
    this->acceptVal = std::vector<Operation*>();
    this->acks = 0;
    this->accepts = 0;
    this->acked = false;
    this->accepted = false;
    this->prop = false;

    this->load_request_queue();

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Construct Paxos Decorator\n");
}

PaxosDecorator::~PaxosDecorator(){
    for(std::vector<Operation*>::iterator it = myVal.begin(); it != myVal.end(); it++){
        delete *it;
    }
    for(std::vector<Operation*>::iterator it = acceptVal.begin(); it != acceptVal.end(); it++){
        delete *it;
    }
    for(std::deque<Operation*>::iterator it = requestQueue->begin(); it != requestQueue->end(); it++){
        delete *it;
    }
    for(Block<std::vector<Operation*>>* i = blockChain->get_head(); i != NULL; i = i->next){
        for(std::vector<Operation*>::iterator it = (i->val).begin(); it != (i->val).end(); it++){
            delete *it;
        }
    }

    delete this->blockChain;
    delete this->requestQueue;

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Deconstruct Paxos Decorator\n");
}


void PaxosDecorator::queue_check_thread(){
    while(!this->shutDownFlag){
        std::unique_lock<std::mutex> ulk(this->proposing);
        if(prop){
            if(settings::DEBUG_FLAG)
                printf("DEBUG: In Proposing, Block\n");
            this->prop_cv.wait(ulk);
            if(settings::DEBUG_FLAG)
                printf("DEBUG: END Proposing, Unblock\n");
            continue;
        }

        int random_time = rand()%settings::RANDOM_DELAY_RANGE+1;
        std::chrono::seconds duration(random_time);
        std::this_thread::sleep_for(duration);

        if(settings::DEBUG_FLAG)
            printf("DEBUG: Thread Check Sleep for: %ds\n", random_time);

        if(this->requestQueue->empty()){
            if(settings::DEBUG_FLAG)
                printf("DEBUG: Request Queue Empty, Block\n");
            this->empty_cv.wait(ulk);
            if(settings::DEBUG_FLAG)
                printf("DEBUG: New Request, Unblock\n");
            continue;
        }

        this->myVal = std::vector<Operation*>();
        for(std::deque<Operation*>::iterator it = this->requestQueue->begin(); it != this->requestQueue->end(); it++){
            this->myVal.push_back(*it);
        }

        this->highestNum = std::make_tuple(0,0,0);
        this->acks = 0;
        this->acked = false;
        this->accepts = 0;
        this->accepted = false;
        this->prop = true;
        this->success = true;

        std::lock_guard<std::mutex> lk(this->idle);
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Start New Proposal\n");
        this->proposer_prepare();
    }

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Queue Check Thread Exit\n");
}

int PaxosDecorator::get_site_number(){
    return this->server->get_site_number();
}

void PaxosDecorator::proposer_prepare(){
    std::get<0>(this->ballotNum)++;
    std::get<1>(this->ballotNum)=this->server->get_site_number();
    std::get<2>(this->ballotNum)=this->blockChain->get_size();

    boost::property_tree::ptree root;
    root.put("MessageType", "Paxos_Prepare");
    root.put("Site", this->server->get_site_number());
    root.put("BallotNum", tuple_to_json(this->ballotNum));

    std::stringstream s;
    boost::property_tree::write_json(s, root, false);
    std::string prepare = s.str();

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Paxos Proposer Broadcast Prepare\n");

    this->server->broadcast(prepare);
}

void PaxosDecorator::accepter_ack(std::string msg){
    boost::property_tree::ptree pt;
    std::stringstream ss(msg);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Accepter Json Parse Failed\n");
        exit(1);
    }

    int site = std::stoi(pt.get<std::string>("Site"));
    std::tuple<int,int,int> bal = tuple_from_json(pt.get<std::string>("BallotNum"));

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Paxos Accepter Receive Prepare From Site:%d\n", site);

    if(compare(this->ballotNum, bal)){
        this->ballotNum = bal;
        boost::property_tree::ptree root;
        root.put("MessageType", "Paxos_Ack");
        root.put("Site", this->server->get_site_number());
        root.put("BallotNum", tuple_to_json(bal));
        root.put("AcceptNum", tuple_to_json(this->acceptNum));
        if(this->acceptVal.empty()){
            root.put("AcceptVal", "");
        } else {
            root.put("AcceptVal", vector_to_json(this->acceptVal));
        }

        std::stringstream s;
        boost::property_tree::write_json(s, root, false);
        std::string ack = s.str();

        if(settings::DEBUG_FLAG)
            printf("DEBUG: Paxos Accepter Send Ack to Site: %d\n", site);

        this->server->send_message(site, ack);
    } else if(settings::DEBUG_FLAG) {
        printf("DEBUG: Paxos Accepter: Ignore Prepare from Site: %d\n", site);
    }
}

void PaxosDecorator::proposer_accept(std::string msg){
    boost::property_tree::ptree pt;
    std::stringstream ss(msg);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Proposer Json Parse Failed\n");
        exit(1);
    }

    int site = std::stoi(pt.get<std::string>("Site"));
    std::string receivedVal = pt.get<std::string>("AcceptVal");

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Paxos Proposer Receive Ack From Site %d\n", site);

    std::tuple<int,int,int> b = tuple_from_json(pt.get<std::string>("BallotNum"));

    if(receivedVal != ""){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Paxos Proposer AcceptVal Not Empty!\n");

        this->success = false;
        if(compare(this->highestNum, b)){
            highestNum = b;
            for(std::vector<Operation*>::iterator it = myVal.begin(); it != myVal.end(); it++){
                // delete *it;
            }

            myVal = vector_from_json(receivedVal);
        }
    }

    this->acks++;
    if(!this->acked && this->acks >= (this->server->get_network_size()/2+1)){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Paxos Proposer Ack from Majority: %d\n", this->acks);

        this->acked = true;
        boost::property_tree::ptree root;
        root.put("MessageType", "Paxos_Accept");
        root.put("Site", this->server->get_site_number());
        root.put("BallotNum", tuple_to_json(b));
        root.put("AcceptVal", vector_to_json(this->myVal));

        std::stringstream s;
        boost::property_tree::write_json(s, root, false);
        std::string accept = s.str();

        if(settings::DEBUG_FLAG)
            printf("DEBUG: Paxos Proposer Broadcast Accept\n");

        this->server->broadcast(accept);
    }
}

void PaxosDecorator::accepter_accepted(std::string msg){
    boost::property_tree::ptree pt;
    std::stringstream ss(msg);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Accepter Json Parse Failed\n");
        exit(1);
    }

    int site = std::stoi(pt.get<std::string>("Site"));
    std::tuple<int,int,int> b = tuple_from_json(pt.get<std::string>("BallotNum"));

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Paxos Accepter Receive Accept From Site %d\n", site);

    if(compare(this->ballotNum, b)){
        for(std::vector<Operation*>::iterator it = acceptVal.begin(); it != acceptVal.end(); it++){
            delete *it;
        }

        this->acceptNum = b;
        this->acceptVal = vector_from_json(pt.get<std::string>("AcceptVal"));

        boost::property_tree::ptree root;
        root.put("MessageType", "Paxos_Accepted");
        root.put("Site", this->server->get_site_number());
        root.put("AcceptNum", tuple_to_json(this->acceptNum));
        root.put("AcceptVal", vector_to_json(this->acceptVal));

        std::stringstream s;
        write_json(s, root, false);
        std::string accepted = s.str();

        if(settings::DEBUG_FLAG)
            printf("DEBUG: Paxos Accepter Send Accepted to Site: %d\n", site);

        this->server->send_message(site, accepted);
    } else if(settings::DEBUG_FLAG) {
        printf("DEBUG: Paxos Accepter: Ignore Accept from Site: %d\n", site);
    }
}


void PaxosDecorator::proposer_decision(std::string msg){
    boost::property_tree::ptree pt;
    std::stringstream ss(msg);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Accepter Json Parse Failed\n");
        exit(1);
    }

    int site = std::stoi(pt.get<std::string>("Site"));
    // to do: add assertions

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Paxos Proposer Receive Accepted From Site %d\n", site);

    this->accepts++;
    if(!accepted && this->accepts >= (this->server->get_network_size()/2+1)){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Paxos Proposer Receive Accepted from Majority: %d\n", this->accepts);

        this->accepted = true;

        boost::property_tree::ptree root;
        root.put("MessageType", "Paxos_Decision");
        root.put("Site", this->server->get_site_number());
        root.put("AcceptNum", tuple_to_json(tuple_from_json(pt.get<std::string>("AcceptNum"))));
        root.put("AcceptVal", vector_to_json(vector_from_json(pt.get<std::string>("AcceptVal"))));

        std::stringstream s;
        boost::property_tree::write_json(s, root, false);
        std::string accepted = s.str();

        if(settings::DEBUG_FLAG)
            printf("DEBUG: Paxos Proposer Broadcast Decision\n");

        this->server->broadcast(accepted);

        if(success){
            for(std::vector<Operation*>::iterator it = this->myVal.begin(); it != this->myVal.end(); it++){
                delete(*it);
                this->requestQueue->pop();
            }
            this->save_request_queue();
        }
    }
}

void PaxosDecorator::learner_learn(std::string msg){
    boost::property_tree::ptree pt;
    std::stringstream ss(msg);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Learner Json Parse Failed\n");
        exit(1);
    }

    int site = std::stoi(pt.get<std::string>("Site"));

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Paxos Learner Receive Decision From Site %d\n", site);


    std::tuple<int,int,int> b = tuple_from_json(pt.get<std::string>("AcceptNum"));
    if(std::get<2>(b) < this->blockChain->get_size()){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Duplicate Decision, Ignore\n");
        return;
    }

    std::vector<Operation*> val = vector_from_json(pt.get<std::string>("AcceptVal"));
    for(std::vector<Operation*>::iterator it = val.begin(); it != val.end(); it++){
        (*it)->assign_server(this->server);
        (*it)->execute();
        if(settings::DEBUG_FLAG)
            printf("\tDEBUG: %s\n", (*it)->to_string().c_str());
    }

    this->blockChain->append(val);

    if(settings::DEBUG_FLAG)
        printf("DEBUG: New Block Appended\n");

    this->acceptNum = std::make_tuple(0,0,0);
    this->acceptVal = std::vector<Operation*>();
    this->prop = false;
    this->prop_cv.notify_one();
}


void PaxosDecorator::save_block(){
    if(settings::DEBUG_FLAG)
        printf("DEBUG: Save BlockChain\n");
    std::ofstream myfile;
    myfile.open(settings::B_SAVE_FILE);
    myfile << "1";
    myfile.close();
}

void PaxosDecorator::load_block(){
    std::ifstream myfile;
    std::string str;
    myfile.open(settings::B_SAVE_FILE);
    if(myfile.is_open()){
        if(getline(myfile, str)){
            if(str == "1")
                std::lock_guard<std::mutex> lk(this->idle);
                this->request_recovery();
                std::unique_lock<std::mutex> ulk(this->proposing);
                this->recover_cv.wait(ulk);
        }
        myfile.close();
    }
}

void PaxosDecorator::load_request_queue(){
    std::lock_guard<std::mutex> lk(this->idle);
    if(settings::DEBUG_FLAG)
        printf("DEBUG: Load Request Queue\n");
    std::ifstream myfile;
    myfile.open(settings::Q_SAVE_FILE);
    std::string strJson;
    if(myfile.is_open()){
        if(getline(myfile, strJson)){
            this->requestQueue = queue_from_json(strJson);
        }
        myfile.close();
        return;
    }
    if(settings::DEBUG_FLAG)
        printf("DEBUG: Request Queue File Not Exist\n");
}


void PaxosDecorator::save_request_queue(){
    std::ofstream myfile;
    myfile.open(settings::Q_SAVE_FILE);
    myfile << queue_to_json(this->requestQueue);
    myfile.close();
    if(settings::DEBUG_FLAG)
        printf("DEBUG: Save Request Queue\n");
}

void PaxosDecorator::request_recovery(){
    boost::property_tree::ptree root;
    root.put("MessageType", "Paxos_Recovery_Request");
    root.put("Site", this->server->get_site_number());
    std::stringstream s;
    boost::property_tree::write_json(s, root, false);
    std::string request = s.str();

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Broadcast Request Recovery\n");

    this->server->broadcast(request);
}

void PaxosDecorator::send_recovery(std::string msg){
    boost::property_tree::ptree pt;
    std::stringstream ss(msg);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Request Json Parse Failed\n");
        exit(1);
    }
    int site = std::stoi(pt.get<std::string>("Site"));

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Receive Request Recovery From Site %d\n", site);

    boost::property_tree::ptree root;
    root.put("MessageType", "Paxos_Recovery");
    root.put("BallotNum", tuple_to_json(this->ballotNum));
    root.put("BlockChain", chain_to_json(this->blockChain));
    std::stringstream s;
    boost::property_tree::write_json(s, root, false);
    std::string recovery = s.str();
    this->server->send_message(site, recovery);

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Send Recovery to Site: %d\n", site);
}

void PaxosDecorator::receive_recovery(std::string msg){
    if(settings::DEBUG_FLAG)
        printf("DEBUG: Receive Recovery\n");

    boost::property_tree::ptree pt;
    std::stringstream ss(msg);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Recovery Json Parse Failed\n");
        exit(1);
    }


    int site = std::stoi(pt.get<std::string>("Site"));
    if(site == this->server->get_site_number()){
        return;
    }

    std::string strJson = pt.get<std::string>("BlockChain");
    BlockChain<std::vector<Operation*>>* b = chain_from_json(strJson);

    int tempSize = this->blockChain->get_size();
    if(tempSize < b->get_size()){
        for(Block<std::vector<Operation*>>* i = blockChain->get_head(); i != NULL; i = i->next){
            for(std::vector<Operation*>::iterator it = (i->val).begin(); it != (i->val).end(); it++){
                delete *it;
            }
        }
        this->blockChain = b;

        std::tuple<int,int,int> bal = tuple_from_json(pt.get<std::string>("BallotNum"));
        this->ballotNum = bal;

        Block<std::vector<Operation*>>* iterator = this->blockChain->get_head();
        for(int i = 0; i < tempSize; i++){
            iterator = iterator->next;
        }

        while(iterator != NULL){
            for(std::vector<Operation*>::iterator it = iterator->val.begin(); it != iterator->val.end(); it++){
                (*it)->assign_server(this->server);
                (*it)->execute();
            }
            iterator = iterator->next;
        }
    }
    this->recover_cv.notify_one();
}

void PaxosDecorator::process_helper(std::string msgType, std::string msg){
    if(msgType == "Paxos_Prepare"){
        std::lock_guard<std::mutex> lk(this->idle);
        accepter_ack(msg);
        return;
    }
    if(msgType == "Paxos_Ack"){
        std::lock_guard<std::mutex> lk(this->idle);
        proposer_accept(msg);
        return;
    }
    if(msgType == "Paxos_Accept"){
        std::lock_guard<std::mutex> lk(this->idle);
        accepter_accepted(msg);
        return;
    }
    if(msgType == "Paxos_Accepted"){
        std::lock_guard<std::mutex> lk(this->idle);
        proposer_decision(msg);
        return;
    }
    if(msgType == "Paxos_Decision"){
        std::lock_guard<std::mutex> lk(this->idle);
        this->learner_learn(msg);
        return;
    }
    if(msgType == "Paxos_Recovery_Request"){
        std::lock_guard<std::mutex> lk(this->idle);
        this->send_recovery(msg);
        return;
    }
    if(msgType == "Paxos_Recovery"){
        std::lock_guard<std::mutex> lk(this->idle);
        this->receive_recovery(msg);
        return;
    }

    this->server->process_helper(msgType, msg);
}


void PaxosDecorator::start_server(Server* s){
    if(this->networkStatus)
        return;

    this->server->start_server(s);
    this->load_block();
    std::thread newThread (&PaxosDecorator::queue_check_thread, this);
    newThread.detach();

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Paxos Started\n");
}


void PaxosDecorator::shut_down_server(){
    this->server->shut_down_server();
    this->prop_cv.notify_one();
    this->empty_cv.notify_one();
    this->save_block();
    this->save_request_queue();
    if(settings::DEBUG_FLAG)
        printf("DEBUG: Paxos Shut Down\n");
}


int PaxosDecorator::new_request(Operation* val){
    if(settings::DEBUG_FLAG)
        printf("DEBUG: Receive New Request\n");

    if(!this->server->get_network_status()){
        this->requestQueue->push(val);
        this->empty_cv.notify_one();
        this->save_request_queue();
        return 1;
    }
    this->requestQueue->push(val);
    this->empty_cv.notify_one();
    this->save_request_queue();
    return 0;
}


void PaxosDecorator::print_blockchain(){
    std::lock_guard<std::mutex> lk(this->idle);

    Block<std::vector<Operation*>> *iterator = this->blockChain->get_head();
    std::string str = "";

    while(iterator != NULL){
        str += "\t";
        for(std::vector<Operation*>::iterator it = iterator->val.begin(); it != iterator->val.end(); it++){
            str += "["+(*it)->to_string()+"],";
        }
        str = str.substr(0, str.size()-1);
        str += "\n ";
        iterator = iterator->next;
    }
    str += "\n";

    printf("Current Block Chain:\n%s\n", str.c_str());
}


void PaxosDecorator::print_queue(){
    std::lock_guard<std::mutex> lk(this->idle);
    printf("Current Request Queue:\n\t%s\n", this->requestQueue->to_string().c_str());
}

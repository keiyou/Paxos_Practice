//
//  PaxosDecorator.hpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#ifndef PaxosDecorator_hpp
#define PaxosDecorator_hpp

#include "ServerDecorator.hpp"
#include "BlockChain.hpp"
#include "LimitedQueue.hpp"
#include "Operation.hpp"

#include <stdio.h>
#include <mutex>
#include <condition_variable>
#include <tuple>
#include <vector>


class PaxosDecorator: public ServerDecorator{
private:
    BlockChain<std::vector<Operation*>> *blockChain;
    LimitedQueue<Operation*> *requestQueue;

    std::tuple<int,int,int> ballotNum;
    std::tuple<int,int,int> acceptNum;
    std::tuple<int,int,int> highestNum;
    std::vector<Operation*> myVal;
    std::vector<Operation*> acceptVal;
    int acks;
    int accepts;
    bool acked;
    bool accepted;

    bool success;
    bool prop;

    std::mutex proposing;
    std::mutex idle;
    std::condition_variable empty_cv;
    std::condition_variable prop_cv;
    std::condition_variable recover_cv;

    void queue_check_thread();
    int get_site_number();

    void proposer_prepare();
    void accepter_ack(std::string msg);
    void proposer_accept(std::string msg);
    void accepter_accepted(std::string msg);
    void proposer_decision(std::string msg);
    void learner_learn(std::string msg);

    void save_block();
    void load_block();
    void save_request_queue();
    void load_request_queue();

    void request_recovery();
    void send_recovery(std::string msg);
    void receive_recovery(std::string msg);
public:
    PaxosDecorator(Server *s, int size);
    ~PaxosDecorator();

    void start_server(Server* s);
    void shut_down_server();
    void process_helper(std::string msgType, std::string msg);

    int new_request(Operation *val);

    void print_blockchain();
    void print_queue();
};




#endif /* PaxosDecorator_hpp */

#ifndef VOTE_SERVER_HPP
#define VOTE_SERVER_HPP

#include "Event.hpp"
#include "TimeTable.hpp"

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>


using namespace std;

class VoteServer{
private:
    TimeTable* tt;
    int serverNumber;
    int sendSockfd;
    int listenSockfd;

    vector<int> votes;
    vector<vector<Event>> log;

    mutex votesLock;
    mutex logLock;

    int synchronize_log(int siteNumber, vector<Event> l);
    void collect_garbage();

public:
    VoteServer(int size, int num);

    void start_server(string config_file);
    void send_synchronize_message(string config_file);
    void recv_synchronize_message();

    void print_dict();
    void print_log();
    void print_table();

    void voteTo(Candidate c);
};



#endif

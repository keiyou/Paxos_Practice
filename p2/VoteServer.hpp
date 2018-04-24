#ifndef VOTE_SERVER_HPP
#define VOTE_SERVER_HPP

#include "Event.hpp"
#include "TimeTable.hpp"

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#define BUFSIZE 512

using namespace std;

struct ServerNetConfig{
    string localIP;
    string localPort;
    string remoteIP;
    string remotePort;
};

class VoteServer{
private:
    TimeTable* tt;
    int serverNumber;

    vector<int> votes;
    vector<vector<Event>> log;

    mutex dictLock;
    mutex logLock;
    mutex tableLock;

    bool shutDownFlag;

    ServerNetConfig config;

    void send_synchronize_message();
    void recv_synchronize_message();
    void recv_message(int fd);

    void print_dict();
    void print_log();
    void print_table();

    void voteTo(Candidate c);

    int synchronize_log(int siteNumber, vector<Event> l);
    void collect_garbage();

    int server_interface();

public:
    VoteServer(int size, int num, string config_file);

    void start_server();
};



#endif

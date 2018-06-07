//
//  Server.hpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#ifndef Server_hpp
#define Server_hpp

#include <stdio.h>
#include <vector>
#include <condition_variable>
#include <iostream>

struct NetworkConfig{
    std::string ip;
    int port;
};

class Server{
private:
    std::vector<bool> netFlags;

    void listen_thread();
    void read_network_config(std::string filename);
    void listen_helper(int fd);
    void send_message_helper(int size, std::string msg);
protected:
    int siteNumber;
    int networkSize;
    static std::vector<NetworkConfig> networkConfigs;

    static bool shutDownFlag;
    static bool networkStatus;
public:
    Server();
    Server(Server* s);
    Server(int siteNumber, int networkSize, std::string configFile);
    virtual ~Server();

    virtual void start_server(Server* s);
    virtual void shut_down_server();
    virtual void process_helper(std::string msgType, std::string msg);

    int get_site_number();
    int get_network_size();
    bool get_network_status();

    void net_up(int i);
    void net_down(int i);

    void send_message(int site, std::string msg);
    void broadcast(std::string msg);
};


#endif /* Server_hpp */

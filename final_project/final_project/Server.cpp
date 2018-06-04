//
//  Server.cpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#include "Server.hpp"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <thread>
#include <cassert>
#include <chrono>

#include "Utility.hpp"


bool Server::shutDownFlag = false;
bool Server::networkStatus = false;
std::vector<NetworkConfig> Server::networkConfigs = std::vector<NetworkConfig>();

/* private helper function */

void Server::listen_thread(){
    int sockfd = -1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        fprintf(stderr, "Failed to Create Listen Socket!\n");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((this->networkConfigs)[siteNumber].port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);


    int err = bind(sockfd, (struct sockaddr*)(&serverAddr), sizeof(struct sockaddr_in));
    if(err < 0){
        fprintf(stderr, "Bind Error!\n");
        close(sockfd);
        exit(1);
    }

    err = listen(sockfd, SOMAXCONN);
    if(err < 0){
        fprintf(stderr, "Listen Error!\n");
        close(sockfd);
        exit(1);
    }

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Server Starts Listening\n");

    while(!shutDownFlag){
        struct sockaddr_in clientAddr;
        socklen_t clilen;
        int newfd = accept(sockfd, (struct sockaddr*)(&clientAddr), &clilen);
        if(newfd < 0){
            fprintf(stderr, "Accept Error!\n");
            exit(1);
        }
        std::thread newThread (&Server::listen_helper, this, newfd);
        newThread.detach();
    }

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Listen Tread Exit\n");
}


void Server::listen_helper(int fd){
    char buf[settings::BUFFER_SIZE];
    int readLen = read(fd, buf, sizeof(buf));
    std::string msg(buf, readLen);

    boost::property_tree::ptree pt;
    std::stringstream ss(msg);
    try{
        read_json(ss, pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Listen Json Parse Failed\n");
        exit(1);
    }

    std::string msgType = pt.get<std::string>("MessageType");
    this->process_helper(msgType, msg);
    close(fd);
}

void Server::process_helper(std::string msgType, std::string msg){
    if(msgType == "ShutDown"){return;}
    printf("Unknown Message Received! Check Message Format!\n");
}

void Server::read_network_config(std::string filename){
    std::ifstream ifs (filename);
    std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    int cnt = 0;

    boost::property_tree::ptree pt;
    std::stringstream ss(str);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Config Json Parse Failed\n");
        exit(1);
    }

    boost::property_tree::ptree array = pt.get_child("NetConfigs");
    for(boost::property_tree::ptree::iterator it = array.begin(); it != array.end(); it++){
        NetworkConfig temp;
        temp.ip = it->second.get<std::string>("IP");
        temp.port = it->second.get<int>("port");
        this->networkConfigs.push_back(temp);
        cnt++;
    }

    assert(cnt == this->networkSize);

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Network Configuration Read\n");
}

Server::Server(){};

Server::Server(Server* s){
    this->siteNumber = s->siteNumber;
    this->networkSize = s->networkSize;
}

/* public interfaces */
Server::Server(int siteNumber, int networkSize, std::string configFile){
    this->siteNumber = siteNumber;
    this->networkSize = networkSize;
    this->read_network_config(configFile);

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Construct Server\n");
}

void Server::start_server(Server* s){
    this->networkStatus = true;
    std::thread newThread (&Server::listen_thread, s);
    newThread.detach();

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Server Started\n");
}

Server::~Server(){
    if(settings::DEBUG_FLAG)
        printf("DEBUG: Deconstruct Server\n");
};

void Server::shut_down_server(){
    this->networkStatus = false;
    this->shutDownFlag = true;
    send_message(siteNumber, "{\"MessageType:\" : \"ShutDown\"}");

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Server Shut Down\n");
}

void Server::send_message(int site, std::string msg){
    int sockfd = -1;
    char buf[settings::BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        fprintf(stderr, "Failed to Create Connecting Socket\n");
        return;
    }

    struct sockaddr_in serverAddr;
    socklen_t addrSize = sizeof(serverAddr);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;

    std::cout <<( this->networkConfigs)[site].ip.c_str() << std::endl;

    serverAddr.sin_port = htons((this->networkConfigs)[site].port);
    serverAddr.sin_addr.s_addr = inet_addr((this->networkConfigs)[site].ip.c_str());

    int err = connect(sockfd, (struct sockaddr*)&serverAddr, addrSize);
    if(err < 0){
        fprintf(stderr, "Failed to Connect to the Server %d\n", site);
        close(sockfd);
        return;
    }

    if(settings::DEBUG_FLAG)
        printf("DEBUG: Connected to Server %d\n", site);

    std::chrono::seconds duration(settings::SIMULATE_DELAY);
    std::this_thread::sleep_for(duration);

    strncpy(buf, msg.c_str(), sizeof(buf));
    err = send (sockfd, buf, msg.size(), 0);
    if(err < 0){
        fprintf(stderr, "Send Error\n");
        return;
    }

    if(settings::DEBUG_FLAG)
        printf("DEBUG: MESSAGE SENT: %s\n", msg.c_str());

    close(sockfd);
}

void Server::broadcast(std::string msg){
    if(settings::DEBUG_FLAG)
        printf("DEBUG: BROADCAST\n");
    for(int i = 0; i < this->networkSize; i++){
        this->send_message(i, msg);
    }
}


int Server::get_network_size(){
    return networkSize;
}

int Server::get_site_number(){
    return siteNumber;
}

bool Server::get_network_status(){
   return networkStatus;
}

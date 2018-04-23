#include "VoteServer.hpp"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>



VoteServer::VoteServer(int size, int num){
    this->serverNumber = num;
    this->tt = new TimeTable(cnt, num);
    sendSockfd = -1;
    listenSockfd = -1;

    this->votes = vector<int> (NumofCandidate, 0);
    this->log = vector<vector<int>> (size, vector<Event>());
}

void VoteServer::start_server(string config_file);
void VoteServer::send_synchronize_message(string config_file);
void VoteServer::recv_synchronize_message();


int VoteServer::synchronize_log(int siteNumber, vector<Event> l);
void VoteServer::collect_garbage();

void VoteServer::print_dict(){
    printf("Current Dictionary:\n");
    for(int i = 0; i < NumofCandidate; i++){
        printf("\t%s: %d\n", CandidateString[i], votes[i]);
    }
}

void VoteServer::print_log(){
    printf("Current Log:\n");

    for(int i = 0; i < (this->log).size(); i++){
        printf("\tLog of Site %d: ", i);
        for(int j = 0; j < (this->log)[i].size(); j++){
            printf("%s ", (this->log)[i][j].to_string());
        }
        printf("\n");
    }

}

void VoteServer::print_table(){
    printf("Current Time Table:\n%s",this->tt->to_string);
}

void voteTo(Candidate c){
    (this->log)[serverNumber].append(Event(c, this->tt->get_time()));
    (this->votes)[c]++;
    this->tt->advance();
}

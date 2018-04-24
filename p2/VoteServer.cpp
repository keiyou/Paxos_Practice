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

#include <fstream>



VoteServer::VoteServer(int size, int num, string filename){
    this->serverNumber = num;
    this->tt = new TimeTable(cnt, num);

    this->votes = vector<int> (NumofCandidate, 0);
    this->log = vector<vector<int>> (size, vector<Event>());

    string line;
    ifstream myfile (filename);
    if(!myfile.is_open()){
        fprintf(stderr,"Cannot Open the Config File!\n");
        exit(1);
    }

    getline(myfile, line);
    (this->config).localIP = line;
    getline(myfile, line);
    (this->config).localPort = line;
    getline(myfile, line);
    (this->config).remoteIP = line;
    getline(myfile, line);
    (this->config).remotePort = line;

}


void VoteServer::send_synchronize_message(){
    int sockfd = -1;
    char buf[BUFSIZE];

    sockfd = socket(AF_NET, SOCK_STREAM, 0);
    if(sockfd < 0){
        fprintf(stderr,"Failed to Create Socket!\n");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    socklen_t addrSize = sizeof(serverAddr);
    memset(&servverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((this->config).remotePort);
    serverAddr.sin_addr.s_addr = inet_addr((this->config).remoteIP);

    int err = connect(socketfd, (struct sockaddr*)&serverAddr, addrSize);
    if(err < 0) {
        fprintf(stderr,"Failed to Connect to Server!\n");
        close(socketfd);
        exit(1);
    }

    printf("Connected to Server\n");

    /*
        FILL IN THE BUFFER
            HERE
    */

    err = send(socketfd, buf, BUFSIZE, 0);

    if(err < 0){
        printf("Failed to Send Sychronizing Message\n");
    }

    close(sockfd);
    return 0;
}


void VoteServer::recv_synchronize_message(){
    int sockfd = -1;
    char buf[BUFSIZE];

    sockfd = socket(AF_NET, SOCK_STREAM, 0);
    if(sockfd < 0){
        fprintf(stderr,"Failed to Create Socket!\n");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((this->config).localPort);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int err = bind(sockfd, (struct sockaddr*)(&serverAddr), sizeof(struct sockaddr_in));
    if(err < 0){
        fprintf(stderr,"Bind Error!\n");
        close(sockfd);
        exit(1);
    }

    err = listen(sockfd, 5);
    if(err < 0){
        fprintf(stderr,"Listen Error!\n");
        close(sockfd);
        exit(1);
    }

    printf("Server Started\n");

    while(true){
        struct sockaddr_in clientAddr;
        socklen_t clilen;
        int newfd = accept(socketfd, (struct sockaddr*)(&clientAddr), &clilen);
        if(newfd < 0){
            fprintf(stderr,"Accept Error!\n");
            exit(1);
        }
        thread newThread (recv_message, newfd);
        newThread.depatch();

        if(shutDownFlag)
            break;
    }

    printf("Receive Thread Terminate\n");
}


void VoteServer::recv_message(int fd);

int VoteServer::synchronize_log(int siteNumber, vector<Event> l);


void VoteServer::collect_garbage();



void VoteServer::print_dict(){
    dictLock.lock();
    printf("Current Dictionary:\n");
    for(int i = 0; i < NumofCandidate; i++){
        printf("\t%s: %d\n", CandidateString[i], votes[i]);
    }
    dictLock.unlock();
}

void VoteServer::print_log(){
    logLock.lock();
    printf("Current Log:\n");
    for(int i = 0; i < (this->log).size(); i++){
        printf("\tLog of Site %d: ", i);
        for(int j = 0; j < (this->log)[i].size(); j++){
            printf("%s ", (this->log)[i][j].to_string());
        }
        printf("\n");
    }
    logLock.unlock();
}

void VoteServer::print_table(){
    tableLock.lock();
    printf("Current Time Table:\n%s",this->tt->to_string);
    tableLock.unlock();
}

void voteTo(Candidate c){
    logLock.lock();
    (this->log)[serverNumber].append(Event(c, this->tt->get_time()));
    logLock.unlock();
    dictLock.lock();
    (this->votes)[c]++;
    dictLock.unlock();
    tableLock.lock();
    this->tt->advance();
    tableLock.unlock();
}



void VoteServer::start_server(){

    thread recv_thread (recv_synchronize_message, this->config_file);

    int choice = -1
    while(choice != 7){
        int choice = server_interface();
        switch (choice){
            case 1 : {voteTo(A);break;}
            case 2 : {voteTo(B);break;}
            case 3 : {send_synchronize_message(this->config_file);break;}
            case 4 : {print_dict();break;}
            case 5 : {print_log();break;}
            case 6 : {print_table();break;}
            default: {break;}
        }
    }

    recv_thread.join();
    printf("Server Shut Down\n");
}

int VoteServer::server_interface(){
    printf("**********************************\n");

    int userChoice = -1;
    printf("Please type in the request:\n");
    printf("1: Vote to A\n");
    printf("2: Vote to B\n");
    printf("3: Send sychronizing message to next site\n");
    printf("4: Print Dictionary\n");
    printf("5: Print Log\n");
    printf("6: Print Table\n");
    printf("7: Shut down server\n");
    cin >> userChoice;
    if(userChoice<1 || userChoice>7){
        printf("\n Unrecognized Input, Please Try Again");
        return server_interface();
    }

    return userChoice;
}

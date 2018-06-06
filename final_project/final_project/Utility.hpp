//
//  Utility.hpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#ifndef Utility_hpp
#define Utility_hpp

#include <stdio.h>
#include <string>


namespace settings{
    extern bool DEBUG_FLAG;
    const int SIMULATE_DELAY = 3;
    const int RANDOM_DELAY_RANGE = 9;
    const int BUFFER_SIZE = 2048;
    const std::string B_SAVE_FILE = "saved_block.txt";
    const std::string Q_SAVE_FILE = "saved_queue.txt";
}

namespace utility{
    const std::string SERVER_NAME[5] = {"A","B","C","D","E"};
    const int NETWORK_SIZE = 5;
    const int REQUEST_QUEUE_SIZE = 10;
    const int INITIAL_BALANCE = 100;
    const std::string NET_CONFIG_FILE = "netconfig.txt";
}


#endif /* Utility_hpp */

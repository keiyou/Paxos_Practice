//
//  PaxosUtility.hpp
//  final_project
//
//  Created by feiyang on 2018/6/3.
//

#ifndef PaxosUtility_hpp
#define PaxosUtility_hpp

#include "ServerDecorator.hpp"
#include "BlockChain.hpp"
#include "LimitedQueue.hpp"
#include "Operation.hpp"

#include <stdio.h>
#include <mutex>
#include <condition_variable>
#include <tuple>
#include <vector>

std::string tuple_to_json(std::tuple<int,int,int> t);
std::tuple<int,int,int> tuple_from_json(std::string strJson);
std::string vector_to_json(std::vector<Operation*> v);
std::vector<Operation*> vector_from_json(std::string strJson);
std::string queue_to_json(LimitedQueue<Operation*>* queue);
LimitedQueue<Operation*>* queue_from_json(std::string strJson);
std::string chain_to_json(BlockChain<std::vector<Operation*>>* chain);
BlockChain<std::vector<Operation*>>* chain_from_json(std::string strJson);

std::string vector_to_string(std::vector<Operation*> v);

bool compare(std::tuple<int,int,int> t1, std::tuple<int,int,int> t2);

#endif /* PaxosUtility_hpp */

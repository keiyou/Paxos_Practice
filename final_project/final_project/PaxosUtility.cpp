//
//  PaxosUtility.cpp
//  final_project
//
//  Created by feiyang on 2018/6/3.
//

#include "PaxosUtility.hpp"
#include "Utility.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>


std::string tuple_to_json(std::tuple<int,int,int> t){
    boost::property_tree::ptree root;
    boost::property_tree::ptree tuple;

    tuple.put("First", std::get<0>(t));
    tuple.put("Second", std::get<1>(t));
    tuple.put("Third", std::get<2>(t));

    root.add_child("tuple",tuple);

    std::stringstream s;
    boost::property_tree::write_json(s, root, false);
    std::string out = s.str();
    return out;
}

std::tuple<int,int,int> tuple_from_json(std::string strJson){
    boost::property_tree::ptree pt;
    std::stringstream ss(strJson);
    try{
        boost::property_tree::read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Tuple Json Parse Failed\n");
        exit(1);
    }

    int i,j,k;
    boost::property_tree::ptree tuple = pt.get_child("tuple");
    i = std::stoi(tuple.get<std::string>("First"));
    j = std::stoi(tuple.get<std::string>("Second"));
    k = std::stoi(tuple.get<std::string>("Third"));

    return std::make_tuple(i,j,k);
}

std::string vector_to_json(std::vector<Operation*> v){
    boost::property_tree::ptree root;
    boost::property_tree::ptree vector;

    for(typename std::vector<Operation*>::iterator it = v.begin(); it != v.end(); it++){
        boost::property_tree::ptree temp;
        temp.put("", (*it)->to_json());
        vector.push_back(std::make_pair("", temp));
    }

    root.add_child("Vector", vector);
    std::stringstream s;
    boost::property_tree::write_json(s, root, false);
    std::string out = s.str();
    return out;
}

std::vector<Operation*> vector_from_json(std::string strJson){
    boost::property_tree::ptree pt;
    std::stringstream ss(strJson);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Vector Json Parse Failed\n");
        exit(1);
    }

    std::vector<Operation*> v;
    boost::property_tree::ptree vector = pt.get_child("Vector");

    for(boost::property_tree::ptree::iterator it = vector.begin(); it != vector.end(); it++){
        Operation* val = Operation::from_json(it->second.data());
        v.push_back(val);

        if(val == NULL){
            std::cout << "WTFFFFFFFF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        }
    }

    return v;
}

std::string queue_to_json(LimitedQueue<Operation*>* queue){
    boost::property_tree::ptree root, array;
    for(std::deque<Operation*>::iterator it=queue->begin(); it!=queue->end(); it++){
        boost::property_tree::ptree temp;
        temp.put("", (*it)->to_json());
        array.push_back(std::make_pair("", temp));
    }
    root.add_child("LimitedQueue", array);
    std::stringstream s;
    write_json(s, root, false);
    std::string out = s.str();
    return out;
}

LimitedQueue<Operation*>* queue_from_json(std::string strJson){
    boost::property_tree::ptree pt;
    std::stringstream ss(strJson);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Queue Json Parse Failed\n");
        exit(1);
    }

    boost::property_tree::ptree array = pt.get_child("LimitedQueue");
    LimitedQueue<Operation*>* q = new LimitedQueue<Operation*>(10);
    for(boost::property_tree::ptree::iterator it = array.begin(); it != array.end(); it++){
        Operation* val = Operation::from_json(it->second.data());
        q->push(val);
    }

    return q;
}

std::string chain_to_json(BlockChain<std::vector<Operation*>>* block){
    boost::property_tree::ptree array;
    for(Block<std::vector<Operation*>> *i = block->get_head(); i != NULL; i = i->next){
        boost::property_tree::ptree temp;
        temp.put("", vector_to_json(i->val));
        array.push_back(std::make_pair("", temp));
    }
    std::stringstream s;
    boost::property_tree::write_json(s, array, false);
    std::string out = s.str();
    return out;

}

BlockChain<std::vector<Operation*>>* chain_from_json(std::string strJson){
    boost::property_tree::ptree pt;
    std::stringstream ss(strJson);
    try{
        read_json(ss,pt);
    } catch (boost::property_tree::ptree_error &e){
        if(settings::DEBUG_FLAG)
            printf("DEBUG: Chain Json Parse Failed\n");
        exit(1);
    }

    BlockChain<std::vector<Operation*>>* b = new BlockChain<std::vector<Operation*>>();
    boost::property_tree::ptree array = pt;
    for(boost::property_tree::ptree::iterator it = array.begin(); it != array.end(); it++){
        Block<std::vector<Operation*>>* temp = new Block<std::vector<Operation*>>();
        temp->val = vector_from_json(it->second.data());
        b->append(temp);
    }

    return b;
}


std::string vector_to_string(std::vector<Operation*> v){
    std::string str = "";
    for(std::vector<Operation*>::iterator it = v.begin(); it != v.end(); it++){
        str += (*it)->to_string();
        str += ",";
    }
    return str;
}


bool compare(std::tuple<int,int,int> t1, std::tuple<int,int,int> t2){
    if(std::get<2>(t1) > std::get<2>(t2)){
        return false;
    }

    if(std::get<0>(t1) > std::get<0>(t2)){
        return false;
    } else {
        if(std::get<0>(t1)==std::get<0>(t2) && std::get<1>(t1)>std::get<1>(t2)){
            return false;
        }
    }

    return true;
}

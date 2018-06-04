
#include <iostream>
#include "PaxosDecorator.hpp"

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

int main(){

    boost::property_tree::ptree root;
    root.put("MessageType", "Paxos_Prepare");
    root.put("Site", 1);
    root.put("BallotNum", tuple_to_json(std::make_tuple(1,1,1)));

    std::stringstream s;
    write_json(s, root, false);
    std::string prepare = s.str();

    std::cout << prepare << std::endl;

    boost::property_tree::ptree pt;
    std::stringstream ss(prepare);
    try{
        read_json(ss, pt);
    } catch (boost::property_tree::ptree_error &e){
        printf("DEBUG: Listen Json Parse Failed\n");
        exit(1);
    }
    printf("XXXXX\n");
}

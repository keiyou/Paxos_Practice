//
//  LimitedQueue.hpp
//  final_project
//
//  Created by feiyang on 2018/5/21.
//

#ifndef LimitedQueue_hpp
#define LimitedQueue_hpp

#include <stdio.h>
#include <iostream>
#include <deque>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

template <class T>
class LimitedQueue {
private:
    int size;
    std::deque<T> queue;
public:
    LimitedQueue();
    LimitedQueue(int size);

    bool empty();
    void push(T val);
    T front();
    T pop();

    typename std::deque<T>::iterator begin();
    typename std::deque<T>::iterator end();

    std::string to_string();
};


template <class T>
LimitedQueue<T>::LimitedQueue(){
    this->size = 0;
    this->queue = std::deque<T>();
}

template <class T>
LimitedQueue<T>::LimitedQueue(int size){
    this->size = size;
    this->queue = std::deque<T>();
}

template <class T>
bool LimitedQueue<T>::empty(){
    return this->queue.empty();
}

template <class T>
void LimitedQueue<T>::push(T val){
    if(this->queue.size() < this->size){
        this->queue.push_back(val);
    }
}

template <class T>
T LimitedQueue<T>::front(){
    return this->queue.front();
}

template <class T>
T LimitedQueue<T>::pop(){
    if(this->size >0){
        T temp = this->queue.front();
        this->queue.pop_front();
        return temp;
    }
    return T();
}

template <class T>
std::string LimitedQueue<T>::to_string(){
    std::string str = "";
    for(typename std::deque<T>::iterator it=queue.begin(); it!=queue.end(); it++){
        str += "[";
        str += (*it)->to_string();
        str += "], ";
    }
    return str;
}
template <class T>
typename std::deque<T>::iterator LimitedQueue<T>::begin(){
    return this->queue.begin();
}
template <class T>
typename std::deque<T>::iterator LimitedQueue<T>::end(){
    return this->queue.end();
}

#endif /* LimitedQueue_hpp */

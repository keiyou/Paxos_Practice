//
//  BlockChain.hpp
//  final_project
//
//  Created by feiyang on 2018/5/16.
//

#ifndef BlockChain_hpp
#define BlockChain_hpp

#include <stdio.h>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

template <class T>
struct Block{
    Block *prev;
    Block *next;
    T val;
};

template <class T>
class BlockChain{
private:
    Block<T> *head;
    Block<T> *tail;
    int size;

public:
    BlockChain();

    void append(T val);
    void append(Block<T>* block);
    void concat(BlockChain<T>* blockChain);

    Block<T>* get_head();
    int get_size();
};


template <class T>
BlockChain<T>::BlockChain(){
    this->head = NULL;
    this->tail = NULL;
    this->size = 0;
}


template <class T>
void BlockChain<T>::append(T v){
    Block<T>* block = new Block<T>();
    block->val = v;
    this->append(block);
}

template <class T>
void BlockChain<T>::append(Block<T>* block){
    this->size++;

    if(this->head==NULL){
        this->head = block;
        this->tail = block;
        return;
    }
    this->tail->next = block;
    block->prev = this->tail;
    this->tail = block;
}

template <class T>
void BlockChain<T>::concat(BlockChain<T>* block){
    if(this->head == NULL){
        this = block;
    }
    if(block->head == NULL){
        return;
    }

    this->tail->next = block->head;
    block->head->prev = this->tail;
}

template <class T>
Block<T>* BlockChain<T>::get_head(){
    return this->head;
}

template <class T>
int BlockChain<T>::get_size(){
    return this->size;
}

#endif /* BlockChain_hpp */

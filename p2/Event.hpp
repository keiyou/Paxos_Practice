#ifndef EVENT_HPP
#define EVENT_HPP

#include <iostream>
#include <vector>

using namespace std;

typedef enum Candidate {
    A = 0,
    B = 1
} Candidate;

#define NumofCandidate 2
const vector<string> CandidateString = {"A", "B"};

class Event{
private:
    Candidate vote;
    int time;

public:
    Event(Candidate v, int t): vote(v), time(t){}
    string to_string(){return "{Vote: "+CandidateString[vote]+"}";};
    Candidate get_vote(){return this->vote;};
    int get_time(){return this->time;};
};

#endif

#ifndef TIMETABLE_HPP
#define TIMETABLE_HPP

#include <iostream>
#include <vector>

using namespace std;

class TimeTable{
private:
    vector<vector<int>> table;
    int serverNumber;

public:
    TimeTable(int size, int number);

    void advance();
    string to_string();
    int get_server_time();
    int all_known_time(int siteNumber);

    vector<vector<int>> get_table();
    void merge_time_table(int siteNumber, vector<vector<int>>& tb);
};


#endif

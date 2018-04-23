#include "TimeTable.hpp"

#include <algorithm>
#include <limits>

TimeTable::TimeTable(int size, int number){
    this->serverNumber = number;
    table = vector<vector<int>> (size, vector<int>(size, 0));
}

void TimeTable::advance(){
    this->table[serverNumber][serverNumber]++;
}

string TimeTable::to_string(){
    string res = "";

    for(int i = 0; i < table.size(); i++){
        for(int j = 0; j < table.szie(); j++){
            res += to_string(table[i][j]) + " ";
        }
        res += "\n";
    }

    return res;
}

int TimeTable::get_server_time(){
    return this->table[serverNumber][serverNumber];
}

int TimeTable::all_known_time(int siteNumber){
    int t = std::numeric_limits<int>::max();
    for(int i = 0; i < (this->table).size(); i++){
        t = min(t, this->table[i][siteNumber]);
    }

    return min;
}


void TimeTable::merge_time_table(int siteNumber, vector<vector<int>>& tb){
    for(int i = 0; i < table.size(); i++){
        if(i != this->serverNumber){
            for(int j = 0; j < ttable.size(); j++){
                this->table[i][j] = max(this->table[i][j], tb[i][j]);
            }
        }
    }

    for(int i = 0; i < table.size(); i++){
        this->table[serverNumber][i] = max(this->table[serverNumber][i], tb[siteNumber][i]);
    }
}

vector<vector<int>> TimeTable::get_table(){
    return this->table;
}

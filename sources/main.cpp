#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

vector<string> split_string(const string &s, const char &delim)
{
    vector<string> values;
    string value;
    istringstream ss(s);

    while (getline(ss, value, delim))
        values.push_back(value);

    return values;
}

int convert_time_to_sec(string time)
{
    vector<string> hms = split_string(time, ':');

    int hour = stoi(hms[0]);
    int min = stoi(hms[1]);
    int sec = stoi(hms[2]);
    int result = hour * 3600 + min * 60 + sec;

    return result;
}

void latched_status_data_counter(const vector<string> &data,
                                 map<int,int> &histogram, int &prev_status,
                                 map<int,int> &time_spent, int &prev_time)
{
    int curr_time = convert_time_to_sec(data[1]);
    if (prev_status != -1) // skip first step
    {
        int time_dif = curr_time - prev_time;
        time_spent[prev_status] += time_dif;
    }

    prev_time = curr_time;

    int curr_status = stoi(data[4]);
    if (curr_status != prev_status) // if status code changed
    {
        histogram[curr_status]++;
        prev_status = curr_status;
    }
}

void print_results(const map<int, int> &histogram, const map<int, int> &time_spent)
{
    ofstream output_file("output.txt");

    output_file << "Latched status" << '\t' << "Times entered" << '\t' << "Seconds" << '\n';

    for(auto p : histogram)
        output_file << p.first << "\t\t" << p.second << "\t\t" << time_spent.at(p.first) << '\n';
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 1; // not enough arguments

    string file_name = argv[1];
    ifstream input_file(file_name);
    if (input_file.fail())
        return 2; // file not found

    map<int, int> histogram;
    int prev_status = -1;
    map<int, int> time_spent;
    int prev_time = -1;

    string row;
    getline(input_file, row); // skip csv header
    while (getline(input_file, row))
    {
        vector<string> data = split_string(row, ',');
        latched_status_data_counter(data, histogram, prev_status, time_spent, prev_time);
    }

    print_results(histogram, time_spent);

    return 0;
}

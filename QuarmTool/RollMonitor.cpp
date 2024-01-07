#include "RollMonitor.h"
#include <regex>
#include <iostream>
#include <ctime>
#include <chrono>
#include <array>

bool compareByResult(const roll_data& a, const roll_data& b) {
    return a.result > b.result;
}

roll_data extract_roll_data(const std::string& input, std::string player, std::time_t timestamp) {
    roll_data rd;
    std::regex pattern("\\b\\d+\\b");

    std::smatch match;
    std::array<int, 3> extractedNumbers{};

    auto it = input.cbegin();
    for (int i = 0; i < 3 && std::regex_search(it, input.cend(), match, pattern); ++i) {
        extractedNumbers[i] = std::stoi(match[0]);
        it = match.suffix().first;
    }
    rd.from = extractedNumbers[0];
    rd.to = extractedNumbers[1];
    rd.result = extractedNumbers[2];
    rd.player = player;
    rd.timestamp = timestamp;
    return rd;
}


RollMonitor::RollMonitor()
{

}
RollMonitor::~RollMonitor()
{

}

bool RollMonitor::validate_roll(int identifier)
{
    auto currentTime = std::chrono::system_clock::now();
    auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    bool should_remove = false;
    for (auto& r : roll_map[identifier])
    {
        if (r.player == current_roller && (currentTime_t - r.timestamp) < 120)
        {
            should_remove = true;
        }
    }

    if (should_remove)
    {
        roll_map[identifier].clear();
        return false;
    }
    return true;
}


void RollMonitor::parse_data(std::time_t timestamp, std::string data)
{
    auto currentTime = std::chrono::system_clock::now();
    auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    auto timeDifference = currentTime_t - timestamp;

    //cleanup expired roll vectors

    map_lock.lock();
    for (auto& [ident, vec] : roll_map)
    {
        if (vec.size() && currentTime_t - vec.front().timestamp > 120)
            vec.clear();
    }



    static std::string roll_start = "**A Magic Die is rolled by ";
    static std::string roll_data_start = "**It could have been any number from ";
    if (data.compare(0, roll_start.length(), roll_start) == 0) 
    {
        current_roller = data.substr(roll_start.length(), data.length() - 2 - roll_start.length());
    }
    else if (data.compare(0, roll_data_start.length(), roll_data_start) == 0)
    {
        roll_data rd = extract_roll_data(data, current_roller, timestamp);
        int ident = rd.from+rd.to;

        if (validate_roll(ident)) //make sure it wasn't a double roll with the same values
        {
            roll_map[ident].push_back(rd);
            std::sort(roll_map[ident].begin(), roll_map[ident].end(), compareByResult);
        }
    }
    
    map_lock.unlock();

    
}


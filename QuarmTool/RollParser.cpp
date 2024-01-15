#include "RollParser.h"
#include <regex>
#include <iostream>
#include <time.h>
#include <chrono>
#include <array>
#include "ImGui/imgui.h"

bool compareByResult(const roll_data& a, const roll_data& b) {
    return a.result > b.result;
}

bool compareByTime(const roll_data& a, const roll_data& b) {
    return a.timestamp > b.timestamp;
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


RollParser::RollParser()
{

}
RollParser::~RollParser()
{

}

bool RollParser::validate_roll(int identifier)
{
    auto currentTime = std::chrono::system_clock::now();
    auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    bool should_remove = false;
    for (auto& r : roll_map[identifier])
    {
        if (r.player == current_roller && (currentTime_t - r.timestamp) < 120)
        {
            return false;
        }
    }


    return true;
}

void RollParser::check_expired(int id)
{
    auto currentTime = std::chrono::system_clock::now();
    auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    std::deque<roll_data> time_sorted = roll_map[id];
    if (roll_map[id].size())
    {
        std::sort(time_sorted.begin(), time_sorted.end(), compareByTime);
        if (currentTime_t - time_sorted.front().timestamp > 120)
        {
            expired_rolls.push_front(roll_map[id]);
            roll_map[id].clear();
        }
    }
}

void RollParser::draw()
{
    auto currentTime = std::chrono::system_clock::now();
    auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    map_lock.lock();

    ImGui::BeginChild(std::string("MainRollFrame").c_str());
    int iin = 1;
    for (auto & [i, vec] :roll_map)
    {
        check_expired(i);
        if (vec.size())
        {
            
            std::tm localTime;
            localtime_s(&localTime, &vec.front().timestamp);

            // Format the time as a string
            char buffer[80]; // Adjust the buffer size as needed
            std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &localTime);

            ImGui::BeginChild(std::string("rollframe" + std::to_string(i) + "##" + std::to_string(i)).c_str(), { 160, 400 }, true);
            ImGui::Text("%i to %i (%s)", vec.front().from, vec.front().to, buffer);

            if (ImGui::BeginTable(std::string("roll##" + std::to_string(i)).c_str(), 2, ImGuiTableFlags_BordersInner | ImGuiTableFlags_Resizable))
            {

                for (auto& roll : vec)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text(roll.player.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%i", roll.result);
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();
            if (iin % 4 != 0)
            {
                ImGui::SameLine();
                iin++;
            }
            else if (vec.size())
                iin++;
        }
    }

    for (auto & vec : expired_rolls)
    {
        if (vec.size())
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(.3f, .3f, .3f, .5f));
            ImGui::BeginChild(std::string("rollframe" + std::to_string(vec.front().timestamp) + "##" + std::to_string(vec.front().timestamp)).c_str(), { 160, 400 }, true);
            ImGui::Text("%i to %i (expired)", vec.front().from, vec.front().to);

            if (ImGui::BeginTable(std::string("roll##" + std::to_string(vec.front().timestamp)).c_str(), 2, ImGuiTableFlags_BordersInner | ImGuiTableFlags_Resizable))
            {
                for (auto& roll : vec)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text(roll.player.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%i", roll.result);
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
            if (iin % 4 != 0)
            {
                ImGui::SameLine();
                iin++;
            }
            else if (vec.size())
                iin++;
        }
    }
    ImGui::EndChild();
    map_lock.unlock();
}

void RollParser::parse_data(std::time_t timestamp, std::string data)
{
    auto currentTime = std::chrono::system_clock::now();
    auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    auto timeDifference = currentTime_t - timestamp;

    //cleanup expired roll vectors

    map_lock.lock();
    //for (auto& [ident, vec] : roll_map)
    //{
    //    if (vec.size() && currentTime_t - vec.front().timestamp > 120)
    //        vec.clear();
    //}



    static std::string roll_start = "**A Magic Die is rolled by ";
    static std::string roll_data_start = "**It could have been any number from ";
    if (data.compare(0, roll_start.length(), roll_start) == 0) 
    {
        current_roller = data.substr(roll_start.length(), data.length() - 1 - roll_start.length());
    }
    else if (data.compare(0, roll_data_start.length(), roll_data_start) == 0)
    {
        roll_data rd = extract_roll_data(data, current_roller, timestamp);
        int ident = rd.from+rd.to;

        if (validate_roll(ident)) //make sure it wasn't a double roll with the same values
        {
            roll_map[ident].push_front(rd);
            std::sort(roll_map[ident].begin(), roll_map[ident].end(), compareByResult);
        }
    }
    
    map_lock.unlock();

    
}


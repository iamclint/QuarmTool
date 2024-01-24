#include "DKPParser.h"
#include "ImGui/imgui.h"
#include <sstream>
#include "QuarmTool.h"
#include <regex>
#include <chrono>
#include "ImGuiWidgets.h"
#include "ImGui/imgui_stdlib.h"

bool DKPParser::parseMessage(const std::string& message, DKPBid& bid)
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (!qt)
    {
        qt = QuarmTool::GetInst();
        return false;
    }
   if (qt->pSettings->dkp_regex == "")
        qt->pSettings->dkp_regex = base_regex;
    std::regex pattern(qt->pSettings->dkp_regex, std::regex_constants::icase);
    // Use std::smatch to store the matched groups
   std::smatch match;


    // Attempt to match the regular expression
    if (std::regex_match(message, match, pattern)) {
        // Access captured groups using match
        if (match.size() >= 5)
        {
            bid = DKPBid(match[4], match[2], match[3], match[5], bid.timestamp);
            return true;
        }
    }
    return false;
}



void DKPParser::draw()
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (!qt)
    {
        qt = QuarmTool::GetInst();
        return;
    }
    ImGui::BeginChild(std::string("DKPUI").c_str());
    ImGui::InputText("##dkpRegex", &qt->pSettings->dkp_regex);
    ImGui::SameLine();
    if (ImGui::Button("Save"))
        qt->pSettings->update<std::string>("dkp_regex", qt->pSettings->dkp_regex);
    if (wins.size() > 0)
    {
        if (ImGui::BeginTable("DKPTable##nq1do6qqhmNVJs3", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable, { 0.f,0.f }, 0.f))
        {
            static ImVec2 size_table1 = { 1,1 };
            ImVec2 cursor_pos = ImGui::GetCursorPos();

            for (auto& d : wins)
            {
                std::tm localTime;
                localtime_s(&localTime, &d.timestamp);

                // Format the time as a string
                char buffer[80]; // Adjust the buffer size as needed
                std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &localTime);

                ImGui::TableNextColumn();

                ImGui::Text("%s", buffer);
                ImGui::TableNextColumn();
                ImGui::Text(d.item.c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%i", d.dkp);
                ImGui::TableNextColumn();
                ImGui::Text(d.player.c_str());
                ImGui::TableNextColumn();
                ImGui::Text(d.comments.c_str());
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
}


void DKPParser::parse_data(std::time_t timestamp, std::string data)
{
    DKPBid bid;
    bid.timestamp = timestamp;
    if (parseMessage(data, bid))
    {
        wins.push_back(bid);
    }
}

DKPParser::DKPParser()
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (!qt)
    {
        qt = QuarmTool::GetInst();
        return;
    }
    if (qt->pSettings->dkp_regex == "")
        qt->pSettings->dkp_regex = base_regex;
}

DKPParser::~DKPParser()
{

}
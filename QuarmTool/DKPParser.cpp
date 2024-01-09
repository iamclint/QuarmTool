#include "DKPParser.h"
#include "ImGui/imgui.h"
#include <sstream>
#include "QuarmTool.h"
#include <regex>
#include <chrono>
#include "ImGuiWidgets.h"

bool parseMessage(const std::string& message, DKPBid& bid)
{
    std::regex pattern(R"(\s*([^;]+)\s*;\s*([^;]+)\s*;\s*(.*);\s*(.*))");
    // Use std::smatch to store the matched groups
    std::smatch matches;

   std::smatch match;

    // Attempt to match the regular expression
    if (std::regex_match(message, match, pattern)) {
        // Access captured groups using match
        if (matches.size() == 4)
        {
            bid = DKPBid(match[1], match[2], match[3], match[4]);
            return true;
        }
    }
    return false;
}



void DKPParser::draw()
{
    ImGui::BeginChild(std::string("DKPUI").c_str());
    if (wins.size() > 0)
    {
        if (ImGui::BeginTable("DKPTable##nq1do6qqhmNVJs3", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Resizable, { 0.f,0.f }, 0.f))
        {
            static ImVec2 size_table1 = { 1,1 };
            ImVec2 cursor_pos = ImGui::GetCursorPos();
            ImGui::TableSetupColumn("Item");
            ImGui::TableSetupColumn("Dkp");
            ImGui::TableSetupColumn("Player");
            ImGui::TableSetupColumn("Comments");
            ImGui::TableHeadersRow();
            for (auto& d : wins)
            {
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
    if (parseMessage(data, bid))
    {
        wins.push_back(bid);
    }
}

DKPParser::DKPParser()
{

}

DKPParser::~DKPParser()
{

}
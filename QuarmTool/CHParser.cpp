#include "CHParser.h"
#include <TlHelp32.h>
#include "ImGui/imgui.h"
#include <sstream>
#include "QuarmTool.h"
#include <regex>
#include <chrono>
#include "ImGuiWidgets.h"
#include "ImGui/imgui_stdlib.h"


bool CHParser::parseMessage(const std::string& message, CHCast& ref_cast)
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (!qt)
    {
        qt = QuarmTool::GetInst();
        return false;
    }
    if (qt->pSettings->ch_regex == "")
    {
        qt->pSettings->ch_regex = base_regex;
    }
    std::regex pattern(qt->pSettings->ch_regex, std::regex_constants::icase);
    //std::regex pattern(R"((\w+) (say(?:s)?,|tells the raid(?:s)?,|shouts(?:,)?|tells the guild(?:s)?,|says out of character(?:s)?,) '(\w+)\s+CH\s+.*?\s*(\w+)'\s*)");
    // Use std::smatch to store the matched groups
    std::smatch matches;

    // Try to match the pattern in the message
    if (std::regex_match(message, matches, pattern))
    {
        // Output the matched names
        if (matches.size() == 4)  // Ensure we have four captured groups
        {
            ref_cast.caster = matches[1];
            ref_cast.identifier = matches[2];
            ref_cast.target = matches[3];
            ref_cast.cast_time = spell::CompleteHeal;
            return true;
        }
    }
    return false;
}


void CHParser::parse_data(std::time_t timestamp, std::string data)
{
    CHCast tmp_cast;
    tmp_cast.casting_timestamp = timestamp;
    if (parseMessage(data, tmp_cast))
    {
        active_heals[tmp_cast.target].push_back(tmp_cast);
    }
}
void ChangeToToolWindow(HWND hwnd)
{
    // Get the current window styles
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    // Set the WS_EX_TOOLWINDOW style
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_TOOLWINDOW);

    // Update the window style
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

void animateButton(float& buttonX, std::chrono::time_point<std::chrono::steady_clock>& startTime, float animationDuration) {
    // Calculate elapsed time since animation started
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();


    // Calculate the percentage completion of the animation
    float percentage = static_cast<float>(elapsedTime) / animationDuration;

    // Update button position based on the percentage completion
    buttonX = ImGui::GetWindowWidth() - (percentage * ImGui::GetWindowWidth());
}

ImVec2 WindowTranslate(ImVec2 position)
{
    ImVec2 window_pos = ImGui::GetWindowPos();
    return { window_pos.x + position.x, window_pos.y + position.y };
}

std::string DurationStr(std::chrono::time_point<std::chrono::steady_clock> end_time, std::chrono::time_point<std::chrono::steady_clock> start_time)
{
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Extract seconds and milliseconds from the duration
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration - seconds);

    // Format the result as seconds.milliseconds
    std::ostringstream result;
    result << seconds.count() << "." << std::setfill('0') << std::setw(3) << milliseconds.count() << "s";
    return result.str();
}

void CHParser::draw_ui()
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (!qt)
    {
        qt = QuarmTool::GetInst();
        return;
    }
    auto currentTime = std::chrono::steady_clock::now();
    ImGui::BeginChild(std::string("CHUI").c_str());
    ImGui::InputText("##regex", &qt->pSettings->ch_regex);
    ImGui::SameLine();
    if (ImGui::Button("Save"))
        qt->pSettings->update<std::string>("ch_regex", qt->pSettings->ch_regex);
    if (ImGui::Checkbox("Draw ch overlay", &qt->pSettings->show_ch_overlay))
        qt->pSettings->update<bool>("ch_overlay", qt->pSettings->show_ch_overlay);


    for (int i = 0; auto & [heal_to, casters] : active_heals)
    {
        if (casters.size())
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(.1f, .1f, .1f, 1.f));
            ImGui::BeginChild((heal_to + "chui").c_str(), { 200, 300 }, true);
            ImGui::Text("Heals to %s (%i)", heal_to.c_str(), casters.size());
            if (ImGui::BeginTable((heal_to + "heallist").c_str(), 2, ImGuiTableFlags_SizingFixedFit))
            {
                for (auto& c : casters) {


                    auto end_time_ms = (c.startTime + std::chrono::milliseconds((int)c.cast_time));

                    // Calculate the time duration between current time and end time
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ms - currentTime);

                    // Calculate the total duration from start to end
                    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ms - c.startTime);

                    // Calculate the percentage of time left as a float
                    float percentageTimeLeft = (static_cast<float>(duration.count()) / totalDuration.count());
                    std::stringstream ss;
                    ss << std::chrono::duration_cast<std::chrono::seconds>(duration) << std::endl;
                    if (percentageTimeLeft > 0)
                    {
                        ImGui::TableNextColumn();
                        ImGui::Text(c.caster.c_str());
                        ImGui::TableNextColumn();
                        ImGui::EqProgressBar((heal_to + c.caster).c_str(), percentageTimeLeft, { 100, 15 }, ss.str().c_str(), ImGui::GenerateColorFromStr(c.caster));
                    }
                }
                ImGui::EndTable();
            }
            casters.erase(std::remove_if(casters.begin(), casters.end(), [](CHCast c) {
                auto currentTime = std::chrono::system_clock::now();
                auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
                auto timeDifference = currentTime_t - c.casting_timestamp;
                return timeDifference > 10;
                }
            ), casters.end());
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
    }
    ImGui::EndChild();

}

void CHParser::draw()
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (!qt)
    {
        qt = QuarmTool::GetInst();
        return;
    }
    if (!qt->pSettings->show_ch_overlay)
        return;
    if (qt->pGameWindow && qt->pGameWindow->Handle)
    {
        auto currentTime = std::chrono::system_clock::now();
        auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
        float width = qt->pGameWindow->Width;
        float height = qt->pGameWindow->Height;

        float heal_window_width = width * .55f;
        static bool has_ran = false;
        float vertical_padding = 10;
        float heal_height = 35;
        float base_top = qt->pGameWindow->Rect.bottom - height * .45f;

        for (int i = 0; auto& [heal_to, casters] : active_heals)
        {
            if (casters.size() == 0) //no active casts on target
                continue;


            if (IsWindowVisible(qt->pGameWindow->Handle))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
              //  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.486726, 0.486726, 0.486726, 1.0));
                
                
                ImGui::SetNextWindowBgAlpha(.85f);

                ImGui::SetNextWindowPos({ qt->pGameWindow->Rect.left + ((width - heal_window_width) / 2),  base_top + ((heal_height + vertical_padding) * i) }, ImGuiCond_Always);
                ImGui::SetNextWindowSize({ heal_window_width, heal_height }, ImGuiCond_Always);
                ImGui::Begin(heal_to.c_str(), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs);
                {
                    SetWindowPos((HWND)ImGui::GetWindowViewport()->PlatformHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

                    if (ImGui::IsWindowAppearing())
                        SetFocus(qt->pGameWindow->Handle);

                    if (casters.size())
                    {
                        auto currentTime = std::chrono::steady_clock::now();
                        CHCast cast = casters.front();
                        std::string time_left = DurationStr(cast.startTime + std::chrono::milliseconds((int)cast.cast_time), currentTime);
                        
                        if (std::chrono::duration_cast<std::chrono::milliseconds>((cast.startTime + std::chrono::milliseconds((int)cast.cast_time)) - currentTime).count() > 0)
                        {
                            ImVec2 text_size = ImGui::CalcTextSize(time_left.c_str());
                            ImGui::GetWindowDrawList()->AddText(WindowTranslate({ std::clamp(cast.current_pos / 2 - (text_size.x / 2),text_size.x + (ImGui::GetStyle().FramePadding.x*2), 1000000.f), (heal_height / 2) - text_size.y - 2 }), ImColor(0.0f, 0.5f, 0.0f, 1.f), time_left.c_str());
                            ImGui::GetWindowDrawList()->AddLine(WindowTranslate({ 0, heal_height / 2 }), WindowTranslate({ cast.current_pos, heal_height / 2 }), ImColor(0.0f, 0.5f, 0.0f, 1.f));
                        }
                    }


                    for (auto it = casters.begin(); it != casters.end(); ++it) {
                        // Access the current element
                        auto& cast = *it;

                        auto timeDifference = currentTime_t - cast.casting_timestamp; //time difference in seconds
                        if (cast.current_pos == -1)
                            cast.current_pos = ImGui::GetWindowWidth();
                        animateButton(cast.current_pos, cast.startTime, cast.cast_time);

                        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GenerateColorFromStr(cast.caster).Value);
                        ImGui::SetCursorPosX(cast.current_pos);
                        ImGui::Button(cast.identifier.c_str(), { 0, heal_height });
                        ImGui::PopStyleColor();
                        float w = ImGui::GetItemRectSize().x;
                        ImGui::SameLine();
                        auto currentTime = std::chrono::steady_clock::now();
                        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - cast.startTime).count();

                        // Access the next element if it exists
                        if (it + 1 != casters.end()) {
                            auto& next = *(it + 1);

                            std::string dur = DurationStr(next.startTime, cast.startTime);
                            ImVec2 text_size = ImGui::CalcTextSize(dur.c_str());
                            ImGui::GetWindowDrawList()->AddText(WindowTranslate({ cast.current_pos + (next.current_pos - cast.current_pos + w) / 2 - (text_size.x / 2), (heal_height / 2) - text_size.y - 2 }), ImColor(0.0f, 0.5f, 0.0f, 1.f), dur.c_str());
                            ImGui::GetWindowDrawList()->AddLine(WindowTranslate({ cast.current_pos + w, heal_height / 2 }), WindowTranslate({ next.current_pos, heal_height / 2 }), ImColor(0.0f, 0.5f, 0.0f, 1.f));
                        }
                        else
                        {
                            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - cast.startTime);
                            if (duration.count() <= (int)cast.cast_time)
                            {
                                std::string dur = DurationStr(currentTime, cast.startTime);
                                ImVec2 text_size = ImGui::CalcTextSize(dur.c_str());
                                ImGui::GetWindowDrawList()->AddText(WindowTranslate({ cast.current_pos + (ImGui::GetWindowWidth() - (cast.current_pos + w)) / 2 - (text_size.x / 2), (heal_height / 2) - text_size.y - 2 }), ImColor(0.0f, 0.5f, 0.0f, 1.f), dur.c_str());
                                ImGui::GetWindowDrawList()->AddLine(WindowTranslate({ cast.current_pos + w, heal_height / 2 }), WindowTranslate({ ImGui::GetWindowWidth(), heal_height / 2 }), ImColor(0.0f, 0.5f, 0.0f, 1.f));
                            }
                        }

                    }
                    ImGui::SetCursorPosX(0);
                    ImGui::Button(heal_to.c_str(), {0, heal_height});
                    ImGui::SameLine();
                }
                ImGui::End();
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
            }
            casters.erase(std::remove_if(casters.begin(), casters.end(), [](CHCast c) {
                auto currentTime = std::chrono::system_clock::now();
                auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
                auto timeDifference = currentTime_t - c.casting_timestamp;
                return timeDifference > 10;
            }
            ), casters.end()); //remove if the cast was more than 12 seconds ago
            i++;
        }

      
    }

}

CHParser::CHParser()
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (!qt)
    {
        qt = QuarmTool::GetInst();
        return;
    }
    if (qt->pSettings->ch_regex == "")
        qt->pSettings->ch_regex = base_regex;
}


CHParser::~CHParser()
{

}
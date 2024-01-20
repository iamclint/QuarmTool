#include "Timers.h"
#include "QuarmTool.h"
#include "ImGui/imgui.h"
#include "ImGuiWidgets.h"
#include <chrono>
void TimerManager::add(std::string& message, DWORD duration_ms)
{
    for (auto& t : timers)
    {
        if (t.message == message)
        {
            t.add(duration_ms);
            return;
        }
    }
    timers.push_back({ message, duration_ms });
}

std::string DurationStr(int milliseconds)
{
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::milliseconds)(milliseconds));

    // Extract seconds and milliseconds from the duration
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration-minutes);
    // Format the result as seconds.milliseconds
    std::ostringstream result;
    result << minutes.count() << "m " << seconds.count() << "s";
    return result.str();
}


void TimerManager::draw_ui()
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (!qt)
    {
        qt = QuarmTool::GetInst();
        return;
    }
    static int selected_index = -1;
    std::scoped_lock l(_lock);
    ImGui::BeginChild("TimersWindow##timerui");
    {
        if (timers.size())
        {
            //if (ImGui::BeginTable("timerstblbtns", 2, ImGuiTableFlags_SizingFixedSame))
           // {
            ///    ImGui::TableNextColumn();
                if (selected_index == -1)
                    ImGui::BeginDisabled();

                if (ImGui::Button("Delete"))
                {
                    timers.erase(timers.begin() + selected_index);
                    /*qt->pConfirmation->confirm("Confirm Deletion", "Are you sure you wish to delete timer: " + timers[selected_index].message, [this](bool t)
                        {
                            if (t)
                            {
                                timers.erase(timers.begin() + selected_index);
                            }
                            else
                            {
                                OutputDebugStringA("don't delete");
                            }
                            selected_index = -1;
                        });*/
                }

                if (selected_index == -1)
                   ImGui::EndDisabled();
             //   ImGui::EndTable();
           //}
            if (ImGui::BeginTable("timerstbl", 2, ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_Resizable))
            {
                for (int index = 0; auto & n : timers)
                {
                    float diff = n.end_time - GetTickCount64();
                    float perc = (diff / (float)(n.duration * 1000));
                    if (ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        n.end_time = GetTickCount64() - 1;
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(("##" + n.message).c_str(), selected_index == index, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        selected_index = index;
                    }
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Right))
                    {
                        selected_index = index;
                        timers.erase(timers.begin() + selected_index);
                    }
                    ImGui::SameLine(-1);
                    if (n.count > 1)
                        ImGui::Text("(%i) %s", n.count, n.message.c_str());
                    else
                        ImGui::Text("%s", n.message.c_str());
                    
                    ImGui::TableNextColumn();
                    ImGui::EqProgressBar(("timerprogress" + std::to_string(index)).c_str(), perc, { 150,8 }, DurationStr((int)diff).c_str(), ImGui::GenerateColorFromStr(n.message));
                    index++;
                }
                ImGui::EndTable();
            }
            timers.erase(std::remove_if(timers.begin(), timers.end(), [](timer n) {
                return (GetTickCount64() > n.end_time);
                }), timers.end());
        }
    }
}

void TimerManager::draw()
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (!qt)
    {
        qt = QuarmTool::GetInst();
        return;
    }
    std::scoped_lock l(_lock);
    if (qt->pGameWindow && qt->pGameWindow->Handle)
    {
        auto currentTime = std::chrono::system_clock::now();
        auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
        float width = qt->pGameWindow->Width;
        float height = qt->pGameWindow->Height;

        static bool has_ran = false;
        if (timers.size())
        {
            if (IsWindowVisible(qt->pGameWindow->Handle))
            {

                ImGui::SetNextWindowBgAlpha(.85f);
                float next_window_top = 0;
                for (int index = 0; auto & n : timers)
                {
                    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
                     ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
                    static float notify_height = 30.f;
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 3, 0 });
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.486726, 0.486726, 0.486726, 1.0));
                    if (next_window_top)
                        ImGui::SetNextWindowPos({ (float)qt->pGameWindow->Rect.left + 100, next_window_top }, ImGuiCond_Once);
                    else
                        ImGui::SetNextWindowPos({ (float)qt->pGameWindow->Rect.left + 100,  (float)(qt->pGameWindow->Rect.top) + 60 + (index * notify_height) }, ImGuiCond_Once);
                    ImGui::SetNextWindowSize({ 0, 0 }, ImGuiCond_Always);
                    ImGui::Begin(("TimersWindow##timer" + std::to_string(index)).c_str(), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing);
                    {
                        SetWindowPos((HWND)ImGui::GetWindowViewport()->PlatformHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                        if (ImGui::IsWindowAppearing())
                            SetFocus(qt->pGameWindow->Handle);
                        float diff = n.end_time - GetTickCount64();
                        float perc = (diff/(float)(n.duration*1000));
                        if (ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                            n.end_time = GetTickCount64() - 1;

                        ImGui::Text("%s", n.message.c_str());
                        ImGui::SameLine();
                        ImGui::EqProgressBar(("timerprogress" + std::to_string(index)).c_str(), perc, { 150,8 }, DurationStr((int)diff).c_str(), ImGui::GenerateColorFromStr(n.message));
                        index++;
                    }
                    next_window_top = ImGui::GetWindowPos().y + ImGui::GetWindowSize().y + 3;
                    ImGui::End();
                    ImGui::PopStyleVar(2);
                    ImGui::PopStyleColor();
                }
            }
            timers.erase(std::remove_if(timers.begin(), timers.end(), [](timer n) {
                return (GetTickCount64() > n.end_time);
                }), timers.end());
        }
    }
}

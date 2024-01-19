#include "Notifications.h"
#include "QuarmTool.h"
#include "ImGui/imgui.h"
#include <mutex>
void NotificationManager::add(std::string& message, DWORD duration_ms)
{
    for (auto& t : notifications)
    {
        if (t.message == message)
        {
            t.add(duration_ms);
            return;
        }
    }
    notifications.push_back({ message, duration_ms });
}
void NotificationManager::draw()
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
        if (notifications.size())
        {
            if (IsWindowVisible(qt->pGameWindow->Handle))
            {






                ImGui::SetNextWindowBgAlpha(.85f);
                float next_window_top = 0;
                for (int index = 0; auto & n : notifications)
                {
                   // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
                    //  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
                    static float notify_height=50.f;
                    
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.486726, 0.486726, 0.486726, 1.0));
                    if (next_window_top)
                        ImGui::SetNextWindowPos({ (float)qt->pGameWindow->Rect.left + 20, next_window_top }, ImGuiCond_Always);
                    else
                        ImGui::SetNextWindowPos({ (float)qt->pGameWindow->Rect.left + 20,  (float)(qt->pGameWindow->Rect.top) + 60 + (index * notify_height) }, ImGuiCond_Always);
                    ImGui::SetNextWindowSize({ 0, 0}, ImGuiCond_Always);
                    ImGui::Begin(("NotificationWindow##notify" + std::to_string(index)).c_str(), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs);
                    {
                        SetWindowPos((HWND)ImGui::GetWindowViewport()->PlatformHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                        ImGui::SetWindowFontScale(1.2);
                        if (ImGui::IsWindowAppearing())
                            SetFocus(qt->pGameWindow->Handle);

                        ImGui::Text("(%i) %s", n.count, n.message.c_str());
                        index++;
                    }
                    next_window_top = ImGui::GetWindowPos().y + ImGui::GetWindowSize().y + 5;
                    ImGui::End();

                    ImGui::PopStyleColor();
                }
            }
                notifications.erase(std::remove_if(notifications.begin(), notifications.end(), [](notification n) {
                    return (GetTickCount64() > n.end_time);
                    }), notifications.end());
        }
    }
}
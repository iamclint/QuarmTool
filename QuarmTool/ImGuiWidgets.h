#pragma once
#include "ImGui/imgui.h"
#include <iostream>
#include <string>
#include <functional>
namespace ImGui
{
    bool BeginChildWidget(const char* name, ImVec2 size, bool border, ImGuiWindowFlags flags);
    void EndChildWidget();
    bool ChannelSelection(int* flag);
	bool EqProgressBar(const char* id, float fraction, const ImVec2& size_arg, const char* overlay, ImU32 Color);
    ImColor GenerateColorFromStr(const std::string& inputStr);
       
}

#pragma once
#include "ImGui/imgui.h"
#include <iostream>
#include <string>
#include <functional>
namespace ImGui
{
	bool EqProgressBar(const char* id, float fraction, const ImVec2& size_arg, const char* overlay, ImU32 Color);
    ImColor GenerateColorFromStr(const std::string& inputStr);
       
}

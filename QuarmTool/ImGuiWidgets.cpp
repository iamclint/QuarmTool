#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "ImGuiWidgets.h"
#include "ImGui/imgui.h"
#include "imgui/imgui_internal.h"
namespace ImGui
{
	ImColor GenerateColorFromStr(const std::string& inputStr) {
		// Use a hash function to generate a unique numeric value for the input string
		std::hash<std::string> hashFunction;
		size_t hashValue = hashFunction(inputStr);

		// Use the hash value to determine the RGB components of the color
		int r = hashValue % 255;
		int g = (hashValue / 3) % 255;
		int b = (hashValue / 7) % 255;

		// Create an ImColor using the RGB components
		return ImColor(r, g, b);
	}
		bool EqProgressBar(const char* id, float fraction, const ImVec2& size_arg, const char* overlay, ImU32 Color)
		{

			ImGuiWindow* window = GetCurrentWindow();
			if (window->SkipItems)
				return false;

			ImGuiContext& g = *GImGui;
			const ImGuiStyle& style = g.Style;

			ImVec2 pos = window->DC.CursorPos;
			ImVec2 size = CalcItemSize(size_arg, CalcItemWidth(), g.FontSize);
			ImRect bb(pos, pos + size);
			ItemSize(size, style.FramePadding.y);
			ImGuiID item_id = window->GetID(id);
			if (!ItemAdd(bb, item_id))
				return false;
			bool hovered, held;
			bool pressed = ButtonBehavior(bb, item_id, &hovered, &held, 0);


			// Render
			fraction = ImSaturate(fraction);
			RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
			bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));
			const ImVec2 fill_br = ImVec2(ImLerp(bb.Min.x, bb.Max.x, fraction), bb.Max.y);
			RenderRectFilledRangeH(window->DrawList, bb, Color, 0.0f, fraction, style.FrameRounding);


			//window->DrawList->AddRect(bb.Min, bb.Max, ImColor(0.0f, 0.0f, 0.0f, 1.0f), style.FrameRounding, 1);
			for (int i = 0; i < 4; i++)
			{
				float d = (bb.Max.x - bb.Min.x) / 4; // size divided by 4
				float x = bb.Min.x + (i * d);
				window->DrawList->AddLine({ x, bb.Min.y }, { x, bb.Max.y }, ImColor(0.2f, 0.2f, 0.2f, .6f), 1);
			}
			for (int i = 0; i < 20; i++)
			{
				float d = (bb.Max.x - bb.Min.x) / 20; // size divided by 4
				float x = bb.Min.x + (i * d);
				window->DrawList->AddLine({ x, bb.Max.y - ((bb.Max.y - bb.Min.y) / 3) }, { x, bb.Max.y }, ImColor(0.2f, 0.2f, 0.2f, .6f), 1);
			}


			// Default displaying the fraction as percentage string, but user can override it
			char overlay_buf[32];
			if (!overlay)
			{
				ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", fraction * 100 + 0.01f);
				overlay = overlay_buf;
			}

			ImVec2 overlay_size = CalcTextSize(overlay, NULL);
			if (overlay_size.x > 0.0f)
				RenderText({ (bb.Max.x - ((bb.Max.x - bb.Min.x) / 2)) - (overlay_size.x / 2) - style.ItemInnerSpacing.x, bb.Min.y }, overlay, NULL, &overlay_size);
			//RenderTextClipped((bb.Max.x - ((bb.Max.x - bb.Min.x) / 2)) - (overlay_size.x / 2) - style.ItemInnerSpacing.x, bb.Min.y), bb.Max, overlay, NULL, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
			return pressed;
		}
}
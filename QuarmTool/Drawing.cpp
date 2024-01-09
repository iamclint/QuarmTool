#include "Drawing.h"
#include "QuarmTool.h"
#include "ImGui/imgui_stdlib.h"
#include <ShlObj.h>
#include <chrono>
LPCSTR Drawing::lpWindowName = "Quarm Tool";
ImVec2 Drawing::vWindowSize = { 450,  500};
ImGuiWindowFlags Drawing::WindowFlags = ImGuiWindowFlags_MenuBar;
bool Drawing::bDraw = true;

void Drawing::Active()
{
	bDraw = true;
}

bool Drawing::isActive()
{
	return bDraw == true;
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
	if (uMsg == BFFM_INITIALIZED) {
		// Set the initial folder path
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}

void FolderSelectQuarm()
{
	static QuarmTool* qt = QuarmTool::GetInst();
	if (!qt)
	{
		qt = QuarmTool::GetInst();
		return;
	}
	std::thread f = std::thread([]()
		{
			// Initialize the COM library
			CoInitialize(NULL);

			// Initialize the BROWSEINFO structure
			BROWSEINFOA bi = { 0 };
			bi.lpszTitle = "Select your quarm folder"; // Set the dialog title
			LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

			if (pidl != NULL) {
				// Get the full path from the PIDL
				char selectedPath[MAX_PATH];
				if (SHGetPathFromIDListA(pidl, selectedPath)) {
					qt->pSettings->game_path = selectedPath;
				}

				// Free the PIDL
				CoTaskMemFree(pidl);
			}

			// Uninitialize the COM library
			CoUninitialize();
		});
	f.detach();
}


void draw_debug()
{
	static QuarmTool* qt = QuarmTool::GetInst();
	ImGui::Text(qt->pSettings->game_path.c_str());
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	
}






void setup_scheme()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	
	colors[ImGuiCol_TitleBg] = ImVec4(0.0929204, 0.0929204, 0.0929204, .9);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.0929204, 0.0929204, 0.0929204, 1);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.1929204, 0.1929204, 0.1929204, .9);
	colors[ImGuiCol_TabHovered] = ImVec4(0.2929204, 0.2929204, 0.2929204, 1.0);
	colors[ImGuiCol_TabActive] = ImVec4(0.1929204, 0.1929204, 0.1929204, 1.0);

	//colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	//colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	//colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void Drawing::Draw()
{
	static QuarmTool* qt=QuarmTool::GetInst();
	if (!qt)
	{
		qt = QuarmTool::GetInst();
		return;
	}
	if (qt->pLogMonitor && qt->pLogMonitor->ch)
		qt->pLogMonitor->ch->draw();

	if (isActive())
	{
		static bool first_run = true;
		if (qt->pSettings->game_path == "" && first_run)
			FolderSelectQuarm();
		if (first_run)
		{
			//ImGui::PushStyleColor(ImGuiCol_TitleBg, );
			setup_scheme();
		}
		
		ImGui::SetNextWindowSize(vWindowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::Begin(lpWindowName, &bDraw, WindowFlags);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Select quarm folder", "Ctrl+O"))
					{
						FolderSelectQuarm();
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
			if (ImGui::BeginTabBar("Tabs"))
			{
				if (ImGui::BeginTabItem("Debug"))
				{
					draw_debug();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Rolls"))
				{
					qt->pLogMonitor->rolls->draw();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("CH Chain"))
				{
					qt->pLogMonitor->ch->draw_ui();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("DKP"))
				{
					qt->pLogMonitor->dkp->draw();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}


		}
		ImGui::End();

		first_run = false;
	}

	#ifdef _WINDLL
	if (GetAsyncKeyState(VK_INSERT) & 1)
		bDraw = !bDraw;
	#endif
}

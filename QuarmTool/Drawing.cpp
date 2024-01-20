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
					qt->pSettings->update("game_path", selectedPath);
					qt->pLogMonitor->UpdateFolder();
				}

				// Free the PIDL
				CoTaskMemFree(pidl);
			}

			// Uninitialize the COM library
			CoUninitialize();
		});
	f.detach();
}

void SelectLog()
{
	std::thread f = std::thread([]()
		{
			static QuarmTool* qt = QuarmTool::GetInst();
			if (!qt)
			{
				qt = QuarmTool::GetInst();
				return;
			}
			// Initialize the COM library
			CoInitialize(NULL);

			char currentDirectory[MAX_PATH];
			GetCurrentDirectoryA(MAX_PATH, currentDirectory);
			// Initialize the OPENFILENAME structure
			OPENFILENAMEA ofn = { 0 };
			char szFile[MAX_PATH] = { 0 };

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = "Log Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrTitle = "Select a log file";
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

			// Set the initial directory
			ofn.lpstrInitialDir = qt->pSettings->game_path.c_str();  // Set your desired start path here

			// Show the File Dialog
			if (GetOpenFileNameA(&ofn))
			{

				qt->pLogMonitor->ReadLogFile(szFile);
			}
			SetCurrentDirectoryA(currentDirectory);
			// Uninitialize the COM library
			CoUninitialize();
		});
	f.detach();
}


void draw_debug()
{
	static QuarmTool* qt = QuarmTool::GetInst();
	ImGui::Text(qt->pSettings->game_path.c_str());
	ImGui::Text(qt->pLogMonitor->active_log.c_str());
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	if (ImGui::Button("Sort Logs"))
	{
		qt->pLogMonitor->SortLog();
	}
	if (ImGui::Button("Read Log"))
	{
		SelectLog();
	}
}






void setup_scheme()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.09f, 0.94f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.18f, 0.94f);
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
	colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 0.90f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);// ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.90f);
	colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.25f, 0.90f);// ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);// ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.90f);
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
			qt->pLogMonitor->active_log = qt->pSettings->last_log_file;
		//	if (qt->pSettings->auto_sort_logs)
		//		qt->pLogMonitor->SortLog();
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
					if (ImGui::MenuItem("Load Log", "Ctrl+L"))
					{
						SelectLog();
					}
					ImGui::EndMenu();
				}
				ImGui::Text("Log Monitoring:");
				ImGui::SameLine();
				if (qt->pLogMonitor->is_monitoring)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0.0f, 0.5f, 0.0f).Value);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					if (ImGui::Button("Enabled"))
						qt->pLogMonitor->is_monitoring = false;
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Click to disable active monitoring of your log");
					ImGui::PopStyleColor();
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0.5f, 0.0f, 0.0f).Value);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					if (ImGui::Button("Disabled"))
						qt->pLogMonitor->is_monitoring = true;
					if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Click to enable active monitoring of your log");
					ImGui::PopStyleColor();
				}
				ImGui::EndMenuBar();
			}
		
			static int tab_sel = 0;
			ImVec2 cursor_pos = ImGui::GetCursorPos();
			ImGui::BeginChild("TabSelect##Xdrs46VxVhqmBXf", { 200.f,0.f }, 1, 0);
			{
				static ImVec2 size_child_window1 = { 1,1 };
				ImVec2 cursor_pos = ImGui::GetCursorPos();
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5, 0.5));
				if (ImGui::Selectable("/rand", tab_sel == 0, 0, { 0.f,40.f }))
				{
					tab_sel = 0;
				}
				if (ImGui::Selectable("dkp", tab_sel==1, 0, { 0.f,40.f }))
				{
					tab_sel = 1;
				}
				if (ImGui::Selectable("complete heal", tab_sel == 2, 0, { 0.f,40.f }))
				{
					tab_sel = 2;
				}
				if (ImGui::Selectable("user parses", tab_sel == 4, 0, { 0.f,40.f }))
				{
					tab_sel = 4;
				}
				if (ImGui::Selectable("timers", tab_sel == 5, 0, { 0.f,40.f }))
				{
					tab_sel = 5;
				}
				if (ImGui::Selectable("settings", tab_sel == 3, 0, { 0.f,40.f }))
				{
					tab_sel = 3;
				}


				if (ImGui::Selectable("debug", tab_sel == -1, 0, { 0.f,40.f }))
				{
					tab_sel = -1;
				}
				ImGui::EndChild();
			}
			ImGui::SameLine();
			ImGui::BeginChild("child window##TTAfzgjBodFBR6y", { 0.f,0.f }, 1, 0);
			{
				ImVec2 cursor_pos = ImGui::GetCursorPos();
				switch (tab_sel)
				{
				case 0:
					qt->pLogMonitor->rolls->draw();
					break;
				case 1:
					qt->pLogMonitor->dkp->draw();
					break;
				case 2:
					qt->pLogMonitor->ch->draw_ui();
					break;
				case 3:
					qt->pSettings->draw();
					break;
				case 4:
					qt->pLogMonitor->user->draw_ui();
					break;
				case 5:
					qt->pTimer->draw_ui();
					break;
				case -1:
					draw_debug();
					break;
				default:
					qt->pLogMonitor->rolls->draw();
					break;
				}
				ImGui::EndChild();
			}

		}
		qt->pConfirmation->draw();
		ImGui::End();
		qt->pNotify->draw();
	//	qt->pTimer->draw();
		first_run = false;
	}

	#ifdef _WINDLL
	if (GetAsyncKeyState(VK_INSERT) & 1)
		bDraw = !bDraw;
	#endif
}

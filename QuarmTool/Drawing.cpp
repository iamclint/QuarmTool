#include "Drawing.h"
#include "QuarmTool.h"
#include "ImGui/imgui_stdlib.h"
#include <ShlObj.h>
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

void draw_rolls()
{
	static QuarmTool* qt = QuarmTool::GetInst();

	qt->pLogMonitor->rolls->map_lock.lock();
	for (int iin = 1; auto& [i, vec] : qt->pLogMonitor->rolls->roll_map)
	{
		if (vec.size())
		{
			ImGui::BeginChild(std::string("rollframe" + std::to_string(i) + "##" + std::to_string(i)).c_str(), { 160, 400 }, true);
			ImGui::Text("%i to %i", vec.front().from, vec.front().to);
			
			if (ImGui::BeginTable(std::string("roll##" + std::to_string(i)).c_str(), 2, ImGuiTableFlags_BordersInner | ImGuiTableFlags_Resizable))
			{

				for (auto& roll : vec)
				{
					ImGui::TableNextColumn();
					ImGui::Text(roll.player.c_str());
					ImGui::TableNextColumn();
					ImGui::Text("%i", roll.result);
				}
				ImGui::EndTable();
			}
			ImGui::EndChild();
			if (iin % 4 != 0)
				ImGui::SameLine();
			iin++;
		}
	}
	qt->pLogMonitor->rolls->map_lock.unlock();
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
					draw_rolls();
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

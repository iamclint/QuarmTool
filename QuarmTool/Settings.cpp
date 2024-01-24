#include "Settings.h"
#include "QuarmTool.h"
#include "ImGui/imgui.h"
Settings::Settings()
{
	game_path = read<std::string>("game_path");
	auto_sort_logs = read<bool>("auto_sort");
	last_log_file = read<std::string>("last_log");
	show_ch_overlay = read<bool>("ch_overlay");
	ch_regex = read<std::string>("ch_regex");
	dkp_regex = read<std::string>("dkp_regex");
 //   ui->Entities->is_open = zeal->settings->read<bool>("Entities_Window");
}

void Settings::draw()
{
	static QuarmTool* qt = QuarmTool::GetInst();
	ImGui::BeginChild(std::string("Settings").c_str());
	if (ImGui::Checkbox("Automatically sort logs on launch", &auto_sort_logs))
		update<bool>("auto_sort", auto_sort_logs);

	
	if (ImGui::Button("Sort Logs"))
		qt->pLogMonitor->SortLog();
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Sort your log into seperate date files inside \\sorted logs folder");
	ImGui::EndChild();
}
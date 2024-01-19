#include "UserGeneratedParser.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
#include "QuarmTool.h"
#include <regex>
UserGeneratedParser::UserGeneratedParser()
{
	std::ifstream fileIn("UserGenerated.json");

	if (!fileIn.is_open()) {
		return;
	}

	nlohmann::json jsonVec = get_vec();

	for (const auto& jsonInfo : jsonVec)
	{
		ParseInfo newInfo;
		newInfo.from_json(jsonInfo);
		parses.push_back(newInfo);
	}
	fileIn.close();
}

void UserGeneratedParser::write_vec()
{
	std::ofstream outputFile("UserGenerated.json");
	nlohmann::json jsonvec;
	for (const auto& info : parses)
	{
		nlohmann::json jsonInfo;
		info.to_json(jsonInfo);
		jsonvec.push_back(jsonInfo);
	}
	outputFile << jsonvec.dump(4) << std::endl;
	outputFile.close();
}

nlohmann::json UserGeneratedParser::get_vec()
{
	std::ifstream fileIn("UserGenerated.json");

	if (!fileIn.is_open()) {
		return {};
	}

	nlohmann::json jsonData;
	fileIn >> jsonData;
	fileIn.close();
	return jsonData;
}


void SelectSound()
{
	static QuarmTool* qt = QuarmTool::GetInst();
	if (!qt)
	{
		qt = QuarmTool::GetInst();
		return;
	}

	std::thread f = std::thread([]()
		{
			static QuarmTool* qt = QuarmTool::GetInst();

			// Initialize the COM library
			CoInitialize(NULL);

			// Initialize the OPENFILENAME structure
			OPENFILENAMEA ofn = { 0 };
			char szFile[MAX_PATH] = { 0 };

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = "Sound Files (*.wav;*.mp3)\0*.wav;*.mp3\0All Files (*.*)\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrTitle = "Select a sound file";
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

			// Set the initial directory
			ofn.lpstrInitialDir = qt->pSettings->game_path.c_str();  // Set your desired start path here

			// Show the File Dialog
			if (GetOpenFileNameA(&ofn))
			{
				qt->pLogMonitor->user->active_parse->sound_path = szFile;
			}

			// Uninitialize the COM library
			CoUninitialize();
		});
	f.detach();
}


UserGeneratedParser::~UserGeneratedParser()
{

}

std::vector<std::string> splitString(const std::string& input) {
	std::vector<std::string> result;
	std::istringstream iss(input);
	std::string token;

	while (iss >> token) {
		result.push_back(token);
	}

	return result;
}


void UserGeneratedParser::parse_data(std::time_t timestamp, std::string data)
{
	static QuarmTool* qt = QuarmTool::GetInst();
	if (!qt)
	{
		qt = QuarmTool::GetInst();
		return;
	}
	for (auto& p : parses)
	{
		try
		{
			if (p.match_type == MatchType_::MatchType_regex)
			{
				std::regex pattern(p.pattern, std::regex_constants::icase);
				// Use std::smatch to store the matched groups
				std::smatch match;
				// Attempt to match the regular expression
				if (std::regex_match(data, match, pattern)) {
					// Iterate through the vector and replace occurrences of "{1}", "{2}", etc.
					std::string tmp_str = p.display;
					for (size_t i = 0; i < match.size(); ++i)
					{
						// Construct the placeholder string, e.g., "{1}"
						std::string placeholder = "{" + std::to_string(i) + "}";

						// Find the position of the placeholder in the original string
						size_t pos = tmp_str.find(placeholder);

						// Replace the placeholder with the corresponding string from the vector
						while (pos != std::string::npos) {
							tmp_str.replace(pos, placeholder.length(), match[i]);
							pos = tmp_str.find(placeholder);
						}
					}
					if (p.event_type & MatchEvent_NotifyWindow)
					{
						std::scoped_lock notifies(qt->pNotify->_lock);
						qt->pNotify->add(tmp_str, 3);
					}
					if (p.event_type & MatchEvent_TimerBar)
					{
						std::scoped_lock timers(qt->pTimer->_lock);
						qt->pTimer->add(tmp_str, p.duration);

					}
				}
			}
			else if (p.match_type == MatchType_::MatchType_string)
			{
				if (data.find(p.pattern) != std::string::npos)
				{
					std::vector<std::string> sp_data = splitString(data);
					std::string tmp_str = p.display;
					for (size_t i = 0; i < sp_data.size(); ++i)
					{
						// Construct the placeholder string, e.g., "{1}"
						std::string placeholder = "{" + std::to_string(i) + "}";

						// Find the position of the placeholder in the original string
						size_t pos = tmp_str.find(placeholder);

						// Replace the placeholder with the corresponding string from the vector
						while (pos != std::string::npos) {
							tmp_str.replace(pos, placeholder.length(), sp_data[i]);
							pos = tmp_str.find(placeholder);
						}
					}
					if (p.event_type & MatchEvent_NotifyWindow)
					{
						std::scoped_lock notifies(qt->pNotify->_lock);
						qt->pNotify->add(tmp_str, 3);
					}
					if (p.event_type & MatchEvent_TimerBar)
					{
						std::scoped_lock timers(qt->pTimer->_lock);
						qt->pTimer->add(tmp_str, p.duration);

					}
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Exception caught: " << e.what() << std::endl;
		}
	}
}

void UserGeneratedParser::draw()
{

}


void UserGeneratedParser::draw_ui()
{
	static QuarmTool* qt = QuarmTool::GetInst();
	if (!qt)
	{
		qt = QuarmTool::GetInst();
		return;
	}
	static ParseInfo tmp_parse;
	if (!active_parse) 
		active_parse = &tmp_parse;
	static int selected = -1;
	static bool editing = false;
	ImGui::BeginChild("UserGeneratedParses");

	
	if (ImGui::BeginTable("EditingTable", 3, ImGuiTableFlags_SizingFixedSame))
	{
		ImGui::TableNextColumn();
		if (ImGui::BeginPopup("AddEditParse", ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::BeginChild("DataInput", { 600.f,400.f }, 1, 0);
			{
				static const char* types = "None\0Regex\0String";
				ImGui::InputText("name", &active_parse->name);
				if (ImGui::Combo(types, (int*)&active_parse->match_type, types))
				{
					std::stringstream ss;
					ss << (int)active_parse->match_type << std::endl;
					OutputDebugStringA(ss.str().c_str());
				}
				if (active_parse->match_type == MatchType_::MatchType_regex)
					ImGui::InputTextWithHint("pattern", "(\w+) (looks tranquil.)", &active_parse->pattern);
				else
					ImGui::InputTextWithHint("pattern", "looks tranquil.", &active_parse->pattern);
				ImGui::CheckboxFlags("sound", (int*)&active_parse->event_type, (int)MatchEvent_Playsound);
				ImGui::CheckboxFlags("notification", (int*)&active_parse->event_type, (int)MatchEvent_NotifyWindow);
				ImGui::CheckboxFlags("timer", (int*)&active_parse->event_type, (int)MatchEvent_TimerBar);
				if (active_parse->event_type & MatchEvent_Playsound)
				{
					ImGui::InputText("Sound File", &active_parse->sound_path);
					ImGui::SameLine();
					if (ImGui::Button("..."))
						SelectSound();
				}

				if (active_parse->event_type & MatchEvent_TimerBar)
				{
					ImGui::InputInt("duration", &active_parse->duration);
				}
				ImGui::InputText("display", &active_parse->display);
				if (editing)
				{
					if (ImGui::Button("Save"))
					{
						write_vec();
						tmp_parse = {};
						ImGui::CloseCurrentPopup();
					}
				}
				else
				{
					if (ImGui::Button("Add"))
					{
						parses.push_back(*active_parse);
						write_vec();
						tmp_parse = {};
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndChild();
			ImGui::SameLine();
			ImGui::BeginChild("TestArea", { 500, 0 });
			{

				ImGui::InputTextWithHint("Sample Data", "Frewil looks tranquil.", &active_parse->test_data);
			
				try
				{
					std::string tmp_str = active_parse->display;
					if (active_parse->match_type == MatchType_::MatchType_regex)
					{
						std::regex pattern(active_parse->pattern, std::regex_constants::icase);
						// Use std::smatch to store the matched groups
						std::smatch match;
						// Attempt to match the regular expression
						if (std::regex_match(active_parse->test_data, match, pattern)) {
							for (int match_index = 0; auto & m : match)
							{
								ImGui::Text("{%i} %s", match_index, m.str().c_str());
								match_index++;
							}

							for (size_t i = 0; i < match.size(); ++i)
							{
								// Construct the placeholder string, e.g., "{1}"
								std::string placeholder = "{" + std::to_string(i) + "}";

								// Find the position of the placeholder in the original string
								size_t pos = tmp_str.find(placeholder);

								// Replace the placeholder with the corresponding string from the vector
								while (pos != std::string::npos) {
									tmp_str.replace(pos, placeholder.length(), match[i]);
									pos = tmp_str.find(placeholder);
								}
							}
						}
					}
					else if (active_parse->match_type == MatchType_::MatchType_string)
					{
						if (active_parse->test_data.find(active_parse->pattern) != std::string::npos)
						{
							std::vector<std::string> sp_data = splitString(active_parse->test_data);
							for (int match_index=0; auto& e : sp_data)
							{
								ImGui::Text("{%i} %s", match_index, e.c_str());
								match_index++;
							}

							for (size_t i = 0; i < sp_data.size(); ++i)
							{
								// Construct the placeholder string, e.g., "{1}"
								std::string placeholder = "{" + std::to_string(i) + "}";

								// Find the position of the placeholder in the original string
								size_t pos = tmp_str.find(placeholder);

								// Replace the placeholder with the corresponding string from the vector
								while (pos != std::string::npos) {
									tmp_str.replace(pos, placeholder.length(), sp_data[i]);
									pos = tmp_str.find(placeholder);
								}
							}
						}
					}
					ImGui::Text("Display: %s", tmp_str.c_str());


				} catch(const std::exception& e) {
					// Catch and handle the exception
					ImGui::Text("Invalid Regex");
					//std::cerr << "Exception caught: " << e.what() << std::endl;
				}

			}
			ImGui::EndChild();
			ImGui::EndPopup();
		}

		if (ImGui::Button("New"))
		{
			editing = false;
			active_parse = &tmp_parse;
			ImGui::OpenPopup("AddEditParse");
		}
		ImGui::TableNextColumn();
		if (selected < 0)
			ImGui::BeginDisabled();
		if (ImGui::Button("Edit"))
		{
			active_parse = &parses[selected];
			editing = true;
			ImGui::OpenPopup("AddEditParse");
		}
		ImGui::TableNextColumn();
		if (ImGui::Button("Delete"))
		{
			qt->pConfirmation->confirm("Confirm Deletion", "Are you sure you wish to delete: " + parses[selected].name, [this](bool t)
				{
					if (t)
					{
						parses.erase(parses.begin() + selected);
						write_vec();
					}
					else
					{
						OutputDebugStringA("don't delete");
					}
					selected = -1;
				});


		}
		if (selected < 0)
			ImGui::EndDisabled();
		ImGui::EndTable();
	}


	if (ImGui::BeginTable("Parses", 6, ImGuiTableFlags_Resizable))
	{
		ImGui::TableSetupColumn("name");
		ImGui::TableSetupColumn("match");
		ImGui::TableSetupColumn("events");
		ImGui::TableSetupColumn("match type");
		ImGui::TableSetupColumn("display");
		ImGui::TableSetupColumn("extra");
		ImGui::TableHeadersRow();
		for (int index = 0; auto & e : parses)
		{
			ImGui::TableNextColumn();
			if (ImGui::Selectable(("##" + std::to_string(index) + e.name).c_str(), selected == index, ImGuiSelectableFlags_SpanAllColumns))
				selected = index;
			ImGui::SameLine(-1);
			ImGui::Text("%s", e.name.c_str());
			ImGui::TableNextColumn();
			ImGui::Text("%s", e.pattern.c_str());
			ImGui::TableNextColumn();
			std::stringstream ss;
			if (e.event_type & MatchEvent_::MatchEvent_Playsound)
				ss << "Sound ";
			if (e.event_type & MatchEvent_::MatchEvent_NotifyWindow)
				ss << "Notify ";
			if (e.event_type & MatchEvent_::MatchEvent_TimerBar)
				ss << "Timer ";
			ImGui::Text("%s", ss.str().c_str());
			ImGui::TableNextColumn();
			if (e.match_type == MatchType_::MatchType_regex)
				ImGui::Text("regular expression");
			if (e.match_type == MatchType_::MatchType_string)
				ImGui::Text("string");
			ImGui::TableNextColumn();
			ImGui::Text("%s", e.display.c_str());
			ImGui::TableNextColumn();
			if (e.event_type & MatchEvent_Playsound)
				ImGui::Text("%s", e.sound_path.c_str());
			if (e.event_type & MatchEvent_TimerBar)
				ImGui::Text("%i", e.duration);

			index++;
		}
		ImGui::EndTable();
	}

	ImGui::EndChild();

	
}
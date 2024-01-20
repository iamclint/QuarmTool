#include "UserGeneratedParser.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
#include "QuarmTool.h"
#include <regex>
#pragma comment(lib, "Winmm.lib")
std::vector<std::string> splitString(const std::string& input) {
	std::vector<std::string> result;
	std::istringstream iss(input);
	std::string token;

	while (iss >> token) {
		result.push_back(token);
	}

	return result;
}

void CopyToClipboard(const std::string& text) {
	// Open the clipboard
	if (OpenClipboard(NULL)) {
		// Empty the clipboard
		EmptyClipboard();

		// Allocate global memory for the string
		HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
		if (hClipboardData != NULL) {
			// Lock the memory and copy the string
			char* pClipboardData = static_cast<char*>(GlobalLock(hClipboardData));
			if (pClipboardData != NULL) {
				strcpy_s(pClipboardData, text.size() + 1, text.c_str());
				GlobalUnlock(hClipboardData);

				// Set the clipboard data
				SetClipboardData(CF_TEXT, hClipboardData);
			}
		}

		// Close the clipboard
		CloseClipboard();
	}
}

std::string ReadFromClipboard() {
	std::string clipboardData;

	// Open the clipboard
	if (OpenClipboard(NULL)) {
		// Get the clipboard data in CF_TEXT format
		HANDLE hClipboardData = GetClipboardData(CF_TEXT);
		if (hClipboardData != NULL) {
			// Lock the memory and retrieve the text
			char* pClipboardData = static_cast<char*>(GlobalLock(hClipboardData));
			if (pClipboardData != NULL) {
				clipboardData = pClipboardData;
				GlobalUnlock(hClipboardData);
			}
		}

		// Close the clipboard
		CloseClipboard();
	}

	return clipboardData;
}

bool is_valid_json(const std::string& jsonString) {
	try {
		// Attempt to parse the JSON string
		nlohmann::json jsonData = nlohmann::json::parse(jsonString);
		if (!jsonData.is_object() || !jsonData.contains("match") || !jsonData.contains("name") || !jsonData.contains("display") || !jsonData.contains("sound_path") || !jsonData["duration"].is_number() ||
			!jsonData["event_type"].is_number() || !jsonData["type"].is_number() || !jsonData["enabled"].is_boolean()) {
			return false; // Structure doesn't match expectations
		}

		// The JSON string is valid and conforms to the structure
		return true;
	}
	catch (const nlohmann::json::parse_error&) {
		// Parsing failed, not valid JSON
		return false;
	}
}

UserGeneratedParser::UserGeneratedParser()
{
	std::ifstream fileIn("UserGenerated.json");
	active_parse = nullptr;
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
	if (outputFile.is_open())
	{
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
	else
	{
		std::cout << "Error opening UserGenerated.json" << std::endl;
	}
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

std::vector<std::string> get_parse_matches(ParseInfo& parse, std::string& data, std::string& display_data)
{
	std::vector<std::string> rvec;
	if (parse.match_type == MatchType_::MatchType_regex)
	{
		std::regex pattern(parse.pattern, std::regex_constants::icase);
		std::smatch match;
		if (std::regex_match(data, match, pattern)) 
		{
			for (auto& m : match)
			{
				rvec.push_back(m.str());
			}
		}
	}
	else if (parse.match_type == MatchType_::MatchType_string)
	{
		if (data.find(parse.pattern) != std::string::npos)
		{
			rvec = splitString(data);
		}
	}

	for (size_t i = 0; i < rvec.size(); ++i)
	{
		// Construct the placeholder string, e.g., "{1}"
		std::string placeholder = "{" + std::to_string(i) + "}";

		// Find the position of the placeholder in the original string
		size_t pos = display_data.find(placeholder);

		// Replace the placeholder with the corresponding string from the vector
		while (pos != std::string::npos) {
			display_data.replace(pos, placeholder.length(), rvec[i]);
			pos = display_data.find(placeholder);
		}
	}

	return rvec;
}



void SelectSound()
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

			SetCurrentDirectoryA(currentDirectory);

			// Uninitialize the COM library
			CoUninitialize();
		});
	f.detach();
}


UserGeneratedParser::~UserGeneratedParser()
{

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
			if (!p.enabled)
				continue;
			std::string tmp_str = p.display;
			std::vector<std::string> matches = get_parse_matches(p, data, tmp_str);
			if (matches.size() > 0)
			{
				if (p.event_type & MatchEvent_Playsound)
				{
					if (!p.sound_path.length())
						break;
					std::stringstream ss;
					
					if (p.sound_path.substr(p.sound_path.length() - 3, 3) == "wav")
					{
						ss << "open \"" << p.sound_path << "\" type waveaudio alias mp3";
					}
					else
					{
						ss << "open \"" << p.sound_path << "\" type mpegvideo alias mp3";
					}
					mciSendStringA(ss.str().c_str(), NULL, 0, NULL);
					mciSendStringA("play mp3", NULL, 0, NULL);
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
					std::cout << ss.str() << std::endl;
				}
				if (active_parse->match_type == MatchType_::MatchType_regex)
					ImGui::InputTextWithHint("pattern", "(\\w+) (looks tranquil.)", &active_parse->pattern);
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
					ImGui::InputInt("duration in seconds", &active_parse->duration);
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
					std::vector<std::string> matches = get_parse_matches(*active_parse, active_parse->test_data, tmp_str);
					for (int index = 0 ; auto& m : matches)
					{
						ImGui::Text("{%i} %s", index, m.c_str());
						index++;
					}
					ImGui::Text("Display: %s", tmp_str.c_str());


				} catch(const std::exception& e) {
					// Catch and handle the exception
					
					ImGui::Text("Invalid Regex: %s", e.what());
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
						std::cout << "delete was cancelled" << std::endl;
					}
					selected = -1;
				});


		}
		if (selected < 0)
			ImGui::EndDisabled();
		ImGui::EndTable();
	}

	bool item_context = false;
	if (ImGui::BeginTable("Parses", 8, ImGuiTableFlags_Resizable))
	{
		ImGui::TableSetupColumn("enabled");
		ImGui::TableSetupColumn("name");
		ImGui::TableSetupColumn("match");
		ImGui::TableSetupColumn("events");
		ImGui::TableSetupColumn("match type");
		ImGui::TableSetupColumn("display");
		ImGui::TableSetupColumn("duration");
		ImGui::TableSetupColumn("path");
		ImGui::TableHeadersRow();
		for (int index = 0; auto & e : parses)
		{
			ImGui::TableNextColumn();
			if (ImGui::Checkbox(("##" + e.name).c_str(), &e.enabled))
				write_vec();
			ImGui::TableNextColumn();
			if (ImGui::Selectable(("##" + std::to_string(index) + e.name).c_str(), selected == index, ImGuiSelectableFlags_SpanAllColumns))
				selected = index;

			if (ImGui::BeginPopupContextItem()) {
				selected = index;
				item_context = true;
				if (ImGui::MenuItem("Copy")) {
					nlohmann::json userparse;
					e.to_json(userparse);
					CopyToClipboard(userparse.dump());
				}
				ImGui::EndPopup();
			}
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
			if (e.event_type & MatchEvent_TimerBar)
				ImGui::Text("%i", e.duration);
			ImGui::TableNextColumn();
			if (e.event_type & MatchEvent_Playsound)
				ImGui::Text("%s", e.sound_path.c_str());



			index++;
		}
		ImGui::EndTable();
	}
	
	if (!item_context && ImGui::BeginPopupContextWindow()) {

		static std::string data = "";
		if (ImGui::IsWindowAppearing()) //only read the clipboard once per opening of this context window
		 data = ReadFromClipboard();
		if (is_valid_json(data))
		{
			if (ImGui::MenuItem("Paste")) {
				ParseInfo nparse;
				nlohmann::json jsonData = nlohmann::json::parse(data);
				nparse.from_json(jsonData);
				parses.push_back(nparse);
				write_vec();
			}
		}
		else
			ImGui::MenuItem("No valid parse to paste");
		ImGui::EndPopup();
	}
	ImGui::EndChild();

	
}
#include "UserGeneratedParser.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
UserGeneratedParser::UserGeneratedParser()
{
	std::ifstream fileIn("UserGenerated.xml");

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
	std::ofstream outputFile("UserGenerated.xml");
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
	std::ifstream fileIn("UserGenerated.xml");

	if (!fileIn.is_open()) {
		return {};
	}

	nlohmann::json jsonData;
	fileIn >> jsonData;
	fileIn.close();
	return jsonData;
}


UserGeneratedParser::~UserGeneratedParser()
{

}

void UserGeneratedParser::parse_data(std::time_t timestamp, std::string data)
{

}

void UserGeneratedParser::draw()
{

}

void UserGeneratedParser::draw_ui()
{

	ImGui::BeginChild("UserGeneratedParses");
	if (ImGui::BeginPopupModal("AddParse", 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static ParseInfo tmp_parse;
		static const char* types = "None\0Regex\0String";
		ImGui::InputText("name", &tmp_parse.name);
		ImGui::Combo(types, (int*)&tmp_parse.type, types);
		ImGui::InputText("match", &tmp_parse.match);
		ImGui::CheckboxFlags("sound", (int*)&tmp_parse.event_type, (int)MatchEvent_Playsound);
		ImGui::CheckboxFlags("notification", (int*)&tmp_parse.event_type, (int)MatchEvent_NotifyWindow);
		ImGui::CheckboxFlags("timer", (int*)&tmp_parse.event_type, (int)MatchEvent_TimerBar);
		if (tmp_parse.event_type & MatchEvent_TimerBar)
		{
			ImGui::InputInt("duration", &tmp_parse.duration);
		}
		ImGui::InputText("display", &tmp_parse.display);
		if (ImGui::Button("Add"))
		{
			parses.push_back(tmp_parse);
			write_vec();
			tmp_parse = {};
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
		

	}
	if (ImGui::Button("Add Parse"))
	{
		ImGui::OpenPopup("AddParse");
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
		for (auto& e : parses)
		{
			ImGui::TableNextColumn();
			ImGui::Text("%s", e.name.c_str());
			ImGui::TableNextColumn();
			ImGui::Text("%s", e.match.c_str());
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
			if (e.type == MatchType::regex)
				ImGui::Text("regular expression");
			if (e.type == MatchType::string)
				ImGui::Text("string");
			ImGui::TableNextColumn();
			ImGui::Text("%s", e.display.c_str());
			ImGui::TableNextColumn();
			if (e.event_type & MatchEvent_Playsound)
				ImGui::Text("%s", e.sound_path.c_str());
			if (e.event_type & MatchEvent_TimerBar)
				ImGui::Text("%i", e.duration);
		}
		ImGui::EndTable();
	}

	ImGui::EndChild();
}
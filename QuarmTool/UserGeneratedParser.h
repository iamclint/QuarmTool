#pragma once
#include <string>
#include <ctime>
#include "nlohmann/json.hpp"
enum class MatchType
{
    none,
	regex,
	string
};

enum MatchEvent_
{
    MatchEvent_None = 0,
    MatchEvent_Playsound = 1 << 0,   // Play sound on match
    MatchEvent_NotifyWindow = 1 << 1,   // Display a notify window
    MatchEvent_TimerBar = 1 << 2,   // Display a timer bar
};


struct ParseInfo
{
	std::string match;
    std::string name;
	MatchType type;
	MatchEvent_ event_type;
    std::string display;
    std::string sound_path;
    int duration;
    bool enabled;
    // Default constructor
    ParseInfo() : type(MatchType::regex), event_type(MatchEvent_None), enabled(true) {}

    ParseInfo(const std::string& name_, const std::string& match_, const std::string& display_, int duration_, const std::string& sound_path_, MatchType type_, MatchEvent_ event_type_, bool enabled_)
        : match(match_), type(type_), event_type(event_type_), enabled(enabled_), name(name_), display(display_), sound_path(sound_path_), duration(duration_) {}

    ~ParseInfo() {}

    // Serialize to JSON
    void to_json(nlohmann::json& j) const
    {
        j = {
            {"name", name},
            {"match", match},
            {"display", display},
            {"sound_path", sound_path},
            {"type", static_cast<int>(type)},
            {"event_type", static_cast<int>(event_type)},
            {"enabled", enabled},
            {"duration", duration}
        };
    }

    // Deserialize from JSON
    void from_json(const nlohmann::json& j)
    {
        j.at("match").get_to(match);
        j.at("type").get_to(type);
        j.at("event_type").get_to(event_type);
        j.at("enabled").get_to(enabled);
        j.at("name").get_to(name);
        j.at("display").get_to(display);
        j.at("sound_path").get_to(sound_path);
        j.at("duration").get_to(duration);
    }
};


class UserGeneratedParser
{
public:
    std::vector<ParseInfo> parses;
	void parse_data(std::time_t timestamp, std::string data);
	void draw_ui();
	void draw();
    nlohmann::json get_vec();
    void write_vec();
    UserGeneratedParser();
    ~UserGeneratedParser();
};


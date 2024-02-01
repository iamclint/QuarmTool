#pragma once
#include <string>
#include <ctime>
#include "nlohmann/json.hpp"
#include <functional>
#include "LogMonitor.h"
enum MatchType_
{
    MatchType_none,
    MatchType_regex,
    MatchType_string
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
	std::string pattern;
    std::string name;
    MatchType_ match_type;
	MatchEvent_ event_type;
    std::string display;
    std::string sound_path;
    std::string test_data;
    int duration;
    bool enabled;
    int channels;
    // Default constructor
    ParseInfo() : match_type(MatchType_::MatchType_regex), event_type(MatchEvent_None), enabled(true), duration(0), channels(255) {}

    ParseInfo(const std::string& name_, const std::string& pattern_, const std::string& display_, int duration_, const std::string& sound_path_, MatchType_ type_, MatchEvent_ event_type_, bool enabled_)
        : pattern(pattern_), match_type(type_), event_type(event_type_), enabled(enabled_), name(name_), display(display_), sound_path(sound_path_), duration(duration_), channels(255) {}

    ~ParseInfo() {}

    // Serialize to JSON
    void to_json(nlohmann::json& j) const
    {
        j = {
            {"name", name},
            {"match", pattern},
            {"display", display},
            {"sound_path", sound_path},
            {"type", static_cast<int>(match_type)},
            {"event_type", static_cast<int>(event_type)},
            {"enabled", enabled},
            {"duration", duration},
            {"channels", channels}
        };
    }

    // Deserialize from JSON
    void from_json(const nlohmann::json& j)
    {
        j.at("match").get_to(pattern);
        j.at("type").get_to(match_type);
        j.at("event_type").get_to(event_type);
        j.at("enabled").get_to(enabled);
        j.at("name").get_to(name);
        j.at("display").get_to(display);
        j.at("sound_path").get_to(sound_path);
        j.at("duration").get_to(duration);
        if (j.contains("channels"))
            j.at("channels").get_to(channels);
        else
            channels = 255;
    }
};


class UserGeneratedParser
{
public:
    std::vector<ParseInfo> parses;
    void parse_data(class LineData& ld);
	void draw_ui();
	void draw();
    nlohmann::json get_vec();
    void write_vec();
    ParseInfo* active_parse;
    UserGeneratedParser();
    ~UserGeneratedParser();
private:
    void confirm_delete();
    int selected = -1;
};


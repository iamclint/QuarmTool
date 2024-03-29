#pragma once
#include "LogMonitor.h"
#include <windows.h>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>

enum spell
{
	CompleteHeal=10000,
	Sieve=3000
};

struct CHCast
{
	std::string caster;
	std::string target;
	std::string identifier;
	std::time_t casting_timestamp;
	float cast_time;
	std::chrono::time_point<std::chrono::steady_clock> startTime;
	float current_pos;
	CHCast() : current_pos(-1), startTime(std::chrono::steady_clock::now()), cast_time(10000) {};
	CHCast(std::string _caster, std::string _target, std::string _identifier, std::time_t timestamp, float _cast_time)
		: caster(_caster), target(_target), identifier(_identifier), casting_timestamp(timestamp), current_pos(-1), startTime(std::chrono::steady_clock::now()), cast_time(_cast_time) {};
};
class CHParser
{
public:
	void parse_data(class LineData& ld);
	void draw();
	void draw_ui();
	int channels;
	std::unordered_map<std::string, std::vector<CHCast>> active_heals;
	CHParser();
	~CHParser();
private:
	const std::string base_regex = R"((\w+) ch [\W]? ?(\w+))";
	//const std::string base_regex = R"((\w+) (say(?:s)?,|tell(?:s)? the raid,|shout(?:s)?,|tell(?:s)? the guild,|say(?:s)? out of character,) '(\w+) CH .*?\s*(\w+)'\s*)";
	bool parseMessage(class LineData& ld , CHCast& ref_cast);
};


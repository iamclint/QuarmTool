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
	void parse_data(std::time_t timestamp, std::string data);
	void draw();
	void draw_ui();
	std::unordered_map<std::string, std::vector<CHCast>> active_heals;
	CHParser();
	~CHParser();
private:

};


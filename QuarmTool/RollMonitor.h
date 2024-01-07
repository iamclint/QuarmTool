#pragma once
#include "LogMonitor.h"
#include <windows.h>
#include <unordered_map>
#include <mutex>

struct roll_data
{
	int from;
	int to;
	int result;
	std::time_t timestamp;
	std::string player;
	roll_data(int f, int t, int r, std::time_t time, std::string p)
		: from(f), to(t), result(r), timestamp(time), player(p) {}
	roll_data() {};
};

class RollMonitor
{
public:
	void parse_data(std::time_t timestamp, std::string data);
	std::unordered_map<int, std::vector<roll_data>> roll_map;
	bool validate_roll(int identifier);
	std::mutex map_lock;
	RollMonitor();
	~RollMonitor();
private:
	std::string current_roller = "";

	
};


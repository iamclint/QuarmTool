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
	bool expired;
	roll_data(int f, int t, int r, std::time_t time, std::string p)
		: from(f), to(t), result(r), timestamp(time), player(p), expired(false) {}
	roll_data() {};
};

class RollParser
{
public:
	void parse_data(std::time_t timestamp, std::string data);
	std::unordered_map<int, std::vector<roll_data>> roll_map;
	std::unordered_map<int, std::vector<roll_data>> expired_rolls;
	bool validate_roll(int identifier);
	std::mutex map_lock;
	void check_expired(int id);
	void draw();
	RollParser();
	~RollParser();
private:
	std::string current_roller = "";

	
};


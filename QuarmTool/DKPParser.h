#pragma once
#include "LogMonitor.h"
#include <windows.h>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>

struct DKPBid
{
	std::string player;
	std::string item;
	int dkp;
	std::string comments;
	// Constructor to initialize the struct members
	DKPBid() : dkp(0) {};
	DKPBid(const std::string& player, const std::string& item, int dkp, const std::string& comments)
		: player(player), item(item), dkp(dkp), comments(comments) {}
	DKPBid(const std::string& player, const std::string& item, const std::string& dkp, const std::string& comments)
		: player(player), item(item), dkp(std::stoi(dkp)), comments(comments) {}
};

class DKPParser
{
public:
	void parse_data(std::time_t timestamp, std::string data);
	void draw();
	std::vector<DKPBid> wins;
	DKPParser();
	~DKPParser();
};


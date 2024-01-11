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
        : player(player), item(item), comments(comments) {

        try {
            // Attempt to convert the string to an integer
            this->dkp = std::stoi(dkp);
        }
        catch (const std::invalid_argument& e) {
            // Handle the case where dkp is not a valid integer
            std::cerr << "Invalid DKP value: " << e.what() << std::endl;
            // You may want to set a default value or handle the error in some other way
            this->dkp = 0;  // Default value, adjust as needed
        }
        catch (const std::out_of_range& e) {
            // Handle the case where dkp is out of the valid range for int
            std::cerr << "DKP value out of range: " << e.what() << std::endl;
            // You may want to set a default value or handle the error in some other way
            this->dkp = 0;  // Default value, adjust as needed
        }
    }
};

class DKPParser
{
public:
	void parse_data(std::time_t timestamp, std::string data);
	void draw();
    std::string current_regex = "";
	std::vector<DKPBid> wins;
	DKPParser();
	~DKPParser();
private:
    const std::string base_regex = R"((\w+) (?:tells|say(?:s)?,).+?'(\w.+?) ?; ?(\d+) ?; ?(\w+) ?(?:Gratss)? ?(?:;)?(?: )?(.+?)'\s)";
    bool parseMessage(const std::string& message, DKPBid& bid);
};


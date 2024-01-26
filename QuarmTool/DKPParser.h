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
    std::time_t timestamp;
	// Constructor to initialize the struct members
	DKPBid() : dkp(0) {};
	DKPBid(const std::string& player, const std::string& item, int dkp, const std::string& comments, const std::time_t timestamp)
		: player(player), item(item), dkp(dkp), comments(comments), timestamp(timestamp) {}

    DKPBid(const std::string& player, const std::string& item, const std::string& dkp, const std::string& comments, const std::time_t timestamp)
        : player(player), item(item), comments(comments), timestamp(timestamp) {

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
	void parse_data(class LineData& ld);
	void draw();
	std::vector<DKPBid> wins;
    int channels;
	DKPParser();
	~DKPParser();
private:
    //const std::string base_regex = R"((\w+) \b[^']*'(\w.+?) ?; ?(\d+) ?; ?(\w+) ?(?:Gratss)? ?(?:;)?(?: )?(.+?)')";
    const std::string base_regex = R"((\w+) [^']*'([\w\s]+); ?(\d+) ?; ?(\w+)[\w\s]+;? ?([\w\W]+)')";
    bool parseMessage(const std::string& message, DKPBid& bid);
};


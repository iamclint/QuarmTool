#pragma once
#include <windows.h>
#include <thread>
#include <string>
#include <iostream>
#include <unordered_map>
#include <ctime>
#include "RollParser.h"
#include "CHParser.h"
#include "DKPParser.h"
#include "UserGeneratedParser.h"

enum channel_
{
	channel_none = 0,
	channel_say = 1 << 0, //bitwise flagging
	channel_ooc = 1 << 1,
	channel_shout = 1 << 2,
	channel_auction = 1 << 3,
	channel_guild = 1 << 4,
	channel_raid = 1 << 5,
	channel_tell = 1 << 6,
	channel_emote = 1 << 7, //emotes/damage/ect
	channel_any = channel_emote | channel_tell | channel_raid | channel_guild | channel_auction | channel_shout | channel_ooc | channel_say
};
class LineData
{
public:
	//includes the timestamp string
	std::string full_msg;
	std::time_t timestamp;
	//full message but without the timestamp
	std::string msg;
	//the message without the tells the guild and quotes
	//if msg  = Frewil tells the guild, 'CCC CH - Shade'  
	//then the channel msg would be CCC CH - Shade
	std::string channel_msg;
	//who sent the message
	std::string message_sender;
	//the chat channel the message was in (none would be emotes, damage, fizzles ect)
	channel_ channel;
	LineData(const std::string& full_msg);
private:
	void ParseChannelData();
	void ParseTimestamp();
};

class LogMonitor
{
public:

	void UpdateFolder();

	std::shared_ptr<class RollParser> rolls;
	std::shared_ptr<class CHParser> ch;
	std::shared_ptr<class DKPParser> dkp;
	std::shared_ptr<class UserGeneratedParser> user;
	std::string active_log = "";
	void ReadLogFile(std::string path);
	void SortLog();
	bool is_monitoring = true;
	LogMonitor();
	~LogMonitor();

private:
	std::unordered_map<std::string,std::streampos> last_read_pos;
	bool should_monitor;
	HANDLE FolderHandle;
	void TruncateLog(int line);
	void UpdateThread();
	void HandleFileChange(std::string filename);
	void HandleNewLine(const std::string& line, bool visuals = true);
	
	std::thread eventloop;
	OVERLAPPED overlapped;
	
};


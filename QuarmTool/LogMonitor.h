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


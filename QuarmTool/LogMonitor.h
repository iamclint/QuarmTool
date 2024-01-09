#pragma once
#include <windows.h>
#include <thread>
#include <string>
#include <iostream>
#include <unordered_map>
#include "RollParser.h"
#include "CHParser.h"
#include "DKPParser.h"
class LogMonitor
{
public:

	void UpdateFolder();

	std::shared_ptr<class RollParser> rolls;
	std::shared_ptr<class CHParser> ch;
	std::shared_ptr<class DKPParser> dkp;

	LogMonitor();
	~LogMonitor();

private:
	std::unordered_map<std::string,std::streampos> last_read_pos;
	bool should_monitor;
	HANDLE FolderHandle;
	void UpdateThread();
	void HandleFileChange(std::string filename);
	void HandleNewLine(const std::string& line);
	std::thread eventloop;
	OVERLAPPED overlapped;
	
};


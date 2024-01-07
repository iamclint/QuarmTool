#pragma once
#include <windows.h>
#include <thread>
#include <string>
#include <iostream>
#include <unordered_map>
#include "RollMonitor.h"
#include "CHMonitor.h"
class LogMonitor
{
public:

	void UpdateFolder();

	std::shared_ptr<class RollMonitor> rolls;
	std::shared_ptr<class CHMonitor> ch;

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


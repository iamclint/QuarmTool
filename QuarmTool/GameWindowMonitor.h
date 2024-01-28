#pragma once
#include <Windows.h>
#include <thread>
class GameWindowMonitor
{
public:
	HWND Handle = 0;
	HWND Handle_p99 = 0;
	RECT Rect = {};
	float Width = 0;
	float Height = 0;
	GameWindowMonitor();
	~GameWindowMonitor();
private:
	bool end_thread=false;
	std::thread monitor_process_thread;
	void monitor_process();
};


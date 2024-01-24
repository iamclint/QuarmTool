#pragma once
#include <string>
#include <vector>
#include <ctime>
#include <Windows.h>
#include <mutex>
struct timer
{
    std::string message;
    ULONGLONG end_time;
    ULONGLONG duration;
    int count;
    void add(DWORD milliseconds)
    {
        end_time = GetTickCount64() + static_cast<ULONGLONG>(milliseconds * 1000);
        count++;
    }
    // Default constructor
    timer() : message(""), end_time(0), count(1), duration(1000) {}

    // Parameterized constructor with message and milliseconds duration
    timer(const std::string& msg, DWORD milliseconds) : message(msg)
    {
        end_time = GetTickCount64() + static_cast<ULONGLONG>(milliseconds * 1000);
        duration = milliseconds;
        count = 1;
    }
};

class TimerManager
{
public:
    std::mutex _lock;
	void add(std::string& message, DWORD duration_ms);
	void draw();
    void draw_ui();
private:
	std::vector<timer> timers;
};


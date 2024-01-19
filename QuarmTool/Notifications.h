#pragma once
#include <string>
#include <vector>
#include <ctime>
#include <Windows.h>
#include <mutex>
struct notification
{
    std::string message;
    ULONGLONG end_time;
    int count;
    // Default constructor
    notification() : message(""), end_time(0) {}
    void add(DWORD milliseconds)
    {
        end_time = GetTickCount64() + static_cast<ULONGLONG>(milliseconds * 1000);
        count++;
    }
    // Parameterized constructor with message and milliseconds duration
    notification(const std::string& msg, DWORD milliseconds) : message(msg)
    {
        end_time = GetTickCount64() + static_cast<ULONGLONG>(milliseconds*1000);
        count = 1;
    }
};

class NotificationManager
{
public:
    std::mutex _lock;
	void add(std::string& message, DWORD duration_ms);
	void draw();
private:
	std::vector<notification> notifications;
};


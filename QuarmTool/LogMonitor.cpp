#include "LogMonitor.h"
#include "QuarmTool.h"
#include <iostream>
#include <Windows.h>
#include <limits>
#include <regex>
#include "ImGui/imgui.h"
namespace fs = std::filesystem;

LogMonitor::LogMonitor()
{
    
    rolls = std::shared_ptr<RollParser>(new RollParser);
    ch = std::shared_ptr<CHParser>(new CHParser);
    dkp = std::shared_ptr<DKPParser>(new DKPParser);
    user = std::shared_ptr<UserGeneratedParser>(new UserGeneratedParser);
    UpdateFolder();
}


void LineData::ParseChannelData()
{
    try
    {
        std::regex pattern(R"(\] (\w+)(.*?),)", std::regex_constants::icase);
        // Use std::smatch to store the matched groups
        std::string channel_str = "";
        std::smatch match;
        if (std::regex_search(full_msg, match, pattern)) {
            // Access captured groups using match
            if (match.size() >= 2)
            {
                message_sender = match[1];
                channel_str = match[2];
            }
        }
        if (channel_str != "")
        {
            if (channel_str.find("guild") != std::string::npos)
                channel = channel_::channel_guild;
            else if (channel_str.find("raid") != std::string::npos)
                channel = channel_::channel_raid;
            else if (channel_str.find("character") != std::string::npos) //ooc
                channel = channel_::channel_ooc;
            else if (channel_str.find("auctions") != std::string::npos)
                channel = channel_::channel_auction;
            else if (channel_str.find("shouts") != std::string::npos)
                channel = channel_::channel_auction;
            else if (channel_str.find("tells you") != std::string::npos)
                channel = channel_::channel_tell;
            else if (channel_str.find("say") != std::string::npos)
                channel = channel_::channel_say;
            else
                channel = channel_::channel_emote;
        }
        else
            channel = channel_::channel_emote;
        if (channel == channel_::channel_emote)
        {
            std::regex pattern(R"(\] (\w+))", std::regex_constants::icase);
            std::string channel_str = "";
            std::smatch match;
            if (std::regex_search(full_msg, match, pattern) && match.size() > 0)
                message_sender = match[1];
        }
        else
        {
            std::regex pattern("'(.*)'", std::regex_constants::icase);
            std::string channel_str = "";
            std::smatch match;
            if (std::regex_search(full_msg, match, pattern) && match.size() > 0)
                channel_msg = match[1];
        }
    }
    catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

}

void LineData::ParseTimestamp()
{
    msg = full_msg.substr(27, full_msg.length() - 27);
    std::string timestampStr = full_msg.substr(1, 24);
    // Creating an input string stream for parsing
    std::istringstream iss(timestampStr);
    // Formatting to match the timestamp format
    std::tm tmp = {};
    iss >> std::get_time(&tmp, "%a %b %d %H:%M:%S %Y");

    if (iss.fail()) {
        std::cout << "Failed to parse timestamp" << std::endl;
    }
    // Convert the parsed timestamp to a time_t
    timestamp = std::mktime(&tmp);
}

LineData::LineData(const std::string& data)
{
    channel = channel_::channel_none;
    full_msg = data;
    ParseTimestamp();
    ParseChannelData();
}

bool areSameDay(time_t time1, time_t time2) {
    // Convert time_t to tm structures
    
    struct tm tm1;
    struct tm tm2;
    gmtime_s(&tm1, &time1);
    gmtime_s(&tm2, &time2);

    // Compare year, month, and day components
    return (tm1.tm_year == tm2.tm_year) &&
        (tm1.tm_mon == tm2.tm_mon) &&
        (tm1.tm_mday == tm2.tm_mday);
}

bool areSameDay(std::tm time1, std::tm time2) {
    // Compare year, month, and day components
    return (time1.tm_year == time2.tm_year) &&
        (time1.tm_mon == time2.tm_mon) &&
        (time1.tm_mday == time2.tm_mday);
}

void log_data(std::tm& timestamp, const std::string& data)
{
    static QuarmTool* qt = QuarmTool::GetInst();
    std::string path = qt->pSettings->game_path + "\\" + "sorted logs";
    if (!(fs::exists(path) && fs::is_directory(path)))
    {
        fs::create_directories(path);
    }
    std::stringstream ss;
    ss << path << "\\" << timestamp.tm_mon+1 << "-" << timestamp.tm_mday << "-" << 1900+timestamp.tm_year << ".txt";
    std::ofstream out_file;
    out_file.open(ss.str(), std::ios::app);
    out_file << data << std::endl;
    out_file.close();
}


void LogMonitor::TruncateLog(int line_index)
{
    static QuarmTool* qt = QuarmTool::GetInst();
    const std::string inputFileName = qt->pSettings->game_path + "\\" + active_log;
    const std::string outputFileName = qt->pSettings->game_path + "\\" + active_log;

    std::ifstream inputFile(inputFileName);
    std::vector<std::string> lines;

    // Read the file content
    std::string line;
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }

    inputFile.close();  // Close the input file

    // Open the output file for writing
    std::ofstream outputFile(outputFileName, std::ios::trunc);

    // Write the lines back to the file, excluding the first 1000 lines
    for (size_t i = line_index; i < lines.size(); ++i) {
        outputFile << lines[i] << "\n";
    }

    outputFile.close();  // Close the output file
}

void LogMonitor::SortLog()
{
    if (active_log == "")
        return;
   // auto currentTime = std::chrono::system_clock::now();
   // auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);

    // Get the current time in seconds since the epoch
    std::time_t currentTime = std::time(nullptr);

    // Convert the time to the local time using std::localtime
    std::tm localTime; 
    localtime_s(&localTime, & currentTime);


    static QuarmTool* qt = QuarmTool::GetInst();
    std::ifstream file(qt->pSettings->game_path + "\\" + active_log, std::ios::binary);
    while (!file.is_open())
    {
        file = std::ifstream(qt->pSettings->game_path + "\\" + active_log, std::ios::binary);
        Sleep(10);
    }
    
    if (file.is_open()) {
        // Determine the size of the file
        //file.seekg(0, std::ios::end);
        std::streamsize fileSize = file.tellg();
        std::string line;
        std::tm prev_time = {};
        std::stringstream current_data;
        bool new_day = true;
        int line_index = 1;
        while (std::getline(file, line, '\n'))
        {
            if (line.length() > 24)
            {
                std::string timestampStr = line.substr(1, 24);

                // Creating an input string stream for parsing
                std::istringstream iss(timestampStr);

                // Formatting to match the timestamp format
                std::tm tm_time = {};
                iss >> std::get_time(&tm_time, "%a %b %d %H:%M:%S %Y");
                time_t line_timestamp = std::mktime(&tm_time);

                if (areSameDay(tm_time, prev_time) || new_day)
                {
                    current_data << line << "\n";
                    new_day = false;
                }
                else
                {
                    log_data(prev_time, current_data.str());
                    current_data.clear();
                    current_data.str("");
                    new_day = true;
                }

                if (areSameDay(localTime, tm_time))
                {
                    break;
                }
                prev_time = tm_time;
                line_index++;
            }
       }

        file.close();
        TruncateLog(line_index);

    }
    else {
        std::cerr << "Error opening file: " << active_log << std::endl;
    }
}
void LogMonitor::ReadLogFile(std::string path)
{
    is_monitoring = false;
    auto currentTime = std::chrono::system_clock::now();
    auto currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    static QuarmTool* qt = QuarmTool::GetInst();
    std::ifstream file(path, std::ios::binary);
    while (!file.is_open())
    {
        file = std::ifstream(path, std::ios::binary);
        Sleep(10);
    }

    if (file.is_open()) {
        std::streamsize fileSize = file.tellg();
        std::string line;
        while (std::getline(file, line, '\r'))
        {
            line.erase(std::remove_if(line.begin(), line.end(),
                [](char c) { return c == '\r' || c == '\n'; }),
                line.end());

                HandleNewLine(line, false);
        }

    }
    else {
        std::cerr << "Error opening file: " << active_log << std::endl;
    }
}


std::string ConvertWideStringToAnsi(const wchar_t* wideStr, size_t length) {
    int bufferSize = WideCharToMultiByte(CP_ACP, 0, wideStr, static_cast<int>(length), NULL, 0, NULL, NULL);

    if (bufferSize == 0) {
        // Handle error
        return std::string();
    }

    char* buffer = new char[bufferSize + 1];  // +1 for null-termination

    if (WideCharToMultiByte(CP_ACP, 0, wideStr, static_cast<int>(length), buffer, bufferSize, NULL, NULL) == 0) {
        // Handle error
        delete[] buffer;
        return std::string();
    }

    buffer[bufferSize] = '\0';  // Null-terminate the string

    std::string result(buffer);
    delete[] buffer;

    return result;
}
void LogMonitor::HandleNewLine(const std::string& data, bool visuals)
{
    // Extracting the timestamp substring
    if (data.length() < 27)
        return;

    LineData ld(data);

    rolls->parse_data(ld);
    dkp->parse_data(ld);
    if (visuals)
    {
        user->parse_data(ld);
        ch->parse_data(ld);
    }
    

}
void LogMonitor::HandleFileChange(std::string filename)
{
    if (!is_monitoring)
        return;
    static QuarmTool* qt = QuarmTool::GetInst();
    if (filename.find("eqlog_") == 0 && filename.rfind("_pq.proj.txt") == (filename.length() - strlen("_pq.proj.txt")))
    { 
        if (filename != active_log) {
            active_log = filename;

            qt->pSettings->update("last_log", active_log);
        }
        //a project quarm log file has been modified
        std::ifstream file(qt->pSettings->game_path + "\\" + filename, std::ios::in);


        while (!file.is_open())
        {
            file = std::ifstream(qt->pSettings->game_path + "\\" + filename, std::ios::in);
            Sleep(10);
        }

        if (file.is_open()) {
            // Determine the size of the file
            file.seekg(0, std::ios::end);
            std::streamsize fileSize = file.tellg();

            auto it = last_read_pos.find(filename);
            if (it == last_read_pos.end()) {
                last_read_pos[filename] = fileSize;
                return;
            }
                
            
            file.seekg(last_read_pos[filename], std::ios::beg);
            last_read_pos[filename] = fileSize;

            // Read the contents into a vector of lines
            std::string line;
            while (std::getline(file, line)) {
                line.erase(std::remove_if(line.begin(), line.end(),
                    [](char c) { return c == '\r' || c == '\n'; }),
                    line.end());
                // Process the new line
                HandleNewLine(line);
            }
        
        }
        else {
            std::cerr << "Error opening file: " << filename << std::endl;
        }
    }
}


void LogMonitor::UpdateThread()
{
    QuarmTool* qt = QuarmTool::GetInst();

    OVERLAPPED overlapped = { 0 };
    DWORD bytes_transferred;
    uint8_t change_buf[1024];

    while (should_monitor) {
        // Use the file handle as the event handle
        overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        if (!overlapped.hEvent) {
            // Handle error and break the loop
            break;
        }

        // Queue the initial event
        if (!ReadDirectoryChangesW(
            FolderHandle, change_buf, sizeof(change_buf), TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
            NULL, &overlapped, NULL)) {
            // Handle error and break the loop
            CloseHandle(overlapped.hEvent);
            break;
        }

        // Wait for the overlapped operation to complete
        DWORD wait_result = WaitForSingleObject(overlapped.hEvent, INFINITE);

        if (wait_result == WAIT_OBJECT_0) {
            if (!GetOverlappedResult(FolderHandle, &overlapped, &bytes_transferred, FALSE)) {
                // Handle error and break the loop
                break;
            }

            FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)change_buf;

            do {
                DWORD name_len = event->FileNameLength / sizeof(wchar_t);

                switch (event->Action) {
                    case FILE_ACTION_ADDED: {
                    } break;

                    case FILE_ACTION_REMOVED: {
                    } break;

                    case FILE_ACTION_MODIFIED: {
                        HandleFileChange(ConvertWideStringToAnsi(event->FileName, name_len));
                        
                    } break;

                    case FILE_ACTION_RENAMED_OLD_NAME: {
                    } break;

                    case FILE_ACTION_RENAMED_NEW_NAME: {
                    } break;

                    default: {
                    } break;
                }


                // Move to the next entry if available
                if (event->NextEntryOffset) {
                    event = (FILE_NOTIFY_INFORMATION*)((LPBYTE)event + event->NextEntryOffset);
                }
                else {
                    break;
                }
            } while (true);

            // Reset the event
            ResetEvent(overlapped.hEvent);

            //// Queue the next event
            //if (!ReadDirectoryChangesW(
            //    FolderHandle, change_buf, sizeof(change_buf), TRUE,
            //    FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
            //    NULL, &overlapped, NULL)) {
            //    // Handle error and break the loop
            //    break;
            //}
        }
        else if (wait_result == WAIT_FAILED) {
            // Handle WaitForSingleObject error and break the loop
            break;
        }
    }

    // Cleanup
    CloseHandle(overlapped.hEvent);
}


void LogMonitor::UpdateFolder()
{

    QuarmTool* qt = QuarmTool::GetInst();
    should_monitor = false;
    if (eventloop.joinable())
        eventloop.join();
    should_monitor = true;

    if (FolderHandle)
        CloseHandle(FolderHandle);

    FolderHandle = CreateFileA(
        qt->pSettings->game_path.c_str(), // Replace with your directory path
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );
    if (FolderHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening directory: " << GetLastError() << std::endl;
        return;
    }

    eventloop = std::thread([this]() { UpdateThread(); });
    eventloop.detach();

}

LogMonitor::~LogMonitor()
{
    if (FolderHandle)
        CloseHandle(FolderHandle);
}


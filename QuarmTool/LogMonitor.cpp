#include "LogMonitor.h"
#include "QuarmTool.h"
#include <iostream>
#include <Windows.h>

LogMonitor::LogMonitor()
{
    
    rolls = std::shared_ptr<RollMonitor>(new RollMonitor);
    ch = std::shared_ptr<CHMonitor>(new CHMonitor);
    UpdateFolder();
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
void LogMonitor::HandleNewLine(const std::string& data)
{
    // Extracting the timestamp substring
    if (data.length() < 27)
        return;

    std::string timestampStr = data.substr(1, 24); 

    // Creating an input string stream for parsing
    std::istringstream iss(timestampStr);

    // Formatting to match the timestamp format
    std::tm timestamp = {};
    iss >> std::get_time(&timestamp, "%a %b %d %H:%M:%S %Y");

    if (iss.fail()) {
        OutputDebugStringA("Failed to parse timestamp");
        std::cerr << "Failed to parse timestamp" << std::endl;
    }

    // Convert the parsed timestamp to a time_t
    std::time_t timestamp_t = std::mktime(&timestamp);


    std::string removed_stamp_data = data.substr(27, data.length() - 27);

    rolls->parse_data(timestamp_t, removed_stamp_data);
    ch->parse_data(timestamp_t, removed_stamp_data);

}
void LogMonitor::HandleFileChange(std::string filename)
{
    static QuarmTool* qt = QuarmTool::GetInst();
    if (filename.find("eqlog_") == 0 && filename.rfind("_pq.proj.txt") == (filename.length() - strlen("_pq.proj.txt")))
    { 
        //a project quarm log file has been modified
        std::ifstream file(qt->pSettings->game_path + "\\" + filename, std::ios::binary);


        while (!file.is_open())
        {
            file = std::ifstream(qt->pSettings->game_path + "\\" + filename, std::ios::binary);
            Sleep(10);
        }

        if (file.is_open()) {
            // Determine the size of the file
            file.seekg(0, std::ios::end);
            std::streamsize fileSize = file.tellg();

            // Read only the new content since the last check
            if (!last_read_pos[filename])
            {
                int pos = file.tellg();
                // Start from the end and move backward until a newline character is found

                for (int i = 0; i < 2; i++)
                {
                    file.seekg(-2, std::ios::cur); // Move one character back from the end
                    while (file.tellg() > 0 && file.peek() != '\n') {
                        file.seekg(-1, std::ios::cur);
                    }
                }
                //last_read_pos[filename] = file.tellg(); //got a new base to start reading from
            }
            else
                file.seekg(last_read_pos[filename], std::ios::beg);

            // Read the contents into a vector of lines
            std::string line;
            while (std::getline(file, line)) {
                // Process the new line
                HandleNewLine(line);
            }
            last_read_pos[filename] = fileSize;
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
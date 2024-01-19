#pragma once
#include <functional>
#include <string>

class ImConfirmation
{
public:
    bool confirm(const std::string& title_, const std::string& message_, std::function<void(bool)> callback_);
    void draw();
private:
    std::string title;
    std::string message;
    std::function<void(bool)> callback;
    bool wait_confirm = false;
    bool do_popup = false;
};
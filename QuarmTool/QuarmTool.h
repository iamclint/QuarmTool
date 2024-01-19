#pragma once
#include <Windows.h>
#include "Settings.h"
#include "LogMonitor.h"
#include "GameWindowMonitor.h"
#include "ImConfirmation.h"
#include "Timers.h"
#include "Notifications.h"
class QuarmTool
{
public:
	static QuarmTool* pInst;
	static QuarmTool* GetInst();
	std::shared_ptr<GameWindowMonitor> pGameWindow;
	std::shared_ptr<Settings> pSettings;
	std::shared_ptr<LogMonitor> pLogMonitor;
	std::shared_ptr<ImConfirmation> pConfirmation;
	std::shared_ptr<NotificationManager> pNotify;
	std::shared_ptr<TimerManager> pTimer;

	QuarmTool();
	~QuarmTool();
};


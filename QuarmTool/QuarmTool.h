#pragma once
#include <Windows.h>
#include "Settings.h"
#include "LogMonitor.h"
#include "GameWindowMonitor.h"
class QuarmTool
{
public:
	static QuarmTool* pInst;
	static QuarmTool* GetInst();
	std::shared_ptr<GameWindowMonitor> pGameWindow;
	std::shared_ptr<Settings> pSettings;
	std::shared_ptr<LogMonitor> pLogMonitor;

	QuarmTool();
	~QuarmTool();
};


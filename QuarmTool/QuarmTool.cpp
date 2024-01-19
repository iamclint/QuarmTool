#include "QuarmTool.h"
QuarmTool* QuarmTool::pInst = nullptr;
QuarmTool::QuarmTool()
{
	pInst = this;
	pGameWindow = std::shared_ptr<GameWindowMonitor>(new GameWindowMonitor);
	pSettings = std::shared_ptr<Settings>(new Settings);
	pLogMonitor = std::shared_ptr<LogMonitor>(new LogMonitor);
	pConfirmation = std::shared_ptr<ImConfirmation>(new ImConfirmation);
	pNotify = std::shared_ptr<NotificationManager>(new NotificationManager);
	pTimer = std::shared_ptr<TimerManager>(new TimerManager);
}
QuarmTool::~QuarmTool()
{
	//delete pInst;
}

QuarmTool* QuarmTool::GetInst()
{
	return pInst;
}
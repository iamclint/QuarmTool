#include "QuarmTool.h"
QuarmTool* QuarmTool::pInst = nullptr;
QuarmTool::QuarmTool()
{
	pInst = this;
	pGameWindow = std::shared_ptr<GameWindowMonitor>(new GameWindowMonitor);
	pSettings = std::shared_ptr<Settings>(new Settings);
	pLogMonitor = std::shared_ptr<LogMonitor>(new LogMonitor);
	
}
QuarmTool::~QuarmTool()
{
	delete pInst;
}

QuarmTool* QuarmTool::GetInst()
{
	return pInst;
}
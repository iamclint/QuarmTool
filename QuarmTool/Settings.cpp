#include "Settings.h"
#include "QuarmTool.h"
Settings::Settings()
{
	game_path = read<std::string>("game_path");
	
 //   ui->Entities->is_open = zeal->settings->read<bool>("Entities_Window");
}
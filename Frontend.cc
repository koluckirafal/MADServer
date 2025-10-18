#include <cstdlib>
#include <iostream>
#include <unistd.h>

#include "Backend.h"
#include "GameVariables.h"
#include "Logger.h"
#include "consts.h"
#include "utils.h"
// #include "build.h"

#include "Frontend.h"
#include "Logger.h"

Frontend::Frontend(std::string config_path)
{
    config_path_ = config_path;

    if (!FindRezFiles())
    {
        LOG_ERROR << "Couldn't find any REZ files!";
        exit(1);
    }

    // server_ = Backend();
    // game_vars_ = GameVariables();

    // START INIT

    // NetStart_DoWizard(g_hInst, &g_ServerInfo, &g_ServerOptions, &g_NetGame, bNoDlgs, configFile);
    // NetStart.cpp:355

    game_vars_.tractor_beam = true;
    game_vars_.double_jump = true;
    game_vars_.ramming_damage = true;
    game_vars_.world_time_speed = -1.0f;
    game_vars_.run_speed = 1.0;
    game_vars_.missile_speed = 1.0;
    game_vars_.respawn_scale = 1.0;
    game_vars_.heal_scale = 1.0;
    game_vars_.world_night_color = "0.5 0.5 0.5";

    // NetStart.cpp:369
    game_vars_.server_name = DEFAULTSERVERNAME;

    LOG_INFO << "Loading config file";

    // NetStart.cpp:383
    server_.LoadConfigFile(config_path);

    LOG_INFO << "Initializing networking";

    // NetStart.cpp:401
    bool net_init = server_.InitNetworking();
    if (!net_init)
    {
        LOG_ERROR << "Couldn't initialize networking";
        exit(1);
    }

    LOG_INFO << "Loading console variables";

    // NetStart.cpp:413
    // NetStart_LoadConsoleVars();
    game_vars_.Load(server_);

    LOG_DEBUG << "Loading level list";

    // NetStart.cpp:415
    // LoadLevelStrings();
    std::vector<std::string> levels = GetLevels(server_);

    LOG_DEBUG << "Loading REZ";

    // NetStart.cpp:416
    // LoadRezStrings();
    std::vector<std::string> rez_files = GetRezFiles(server_);

    // TODO: NetStart_GoNoDialogs

    // END INIT

    // TODO: CGameServDlg
    // dlg.SetConfigFilename(configFile);
}

Frontend::~Frontend()
{
    // Save config vars and file
    game_vars_.Save(server_);
    server_.SaveConfigFile(config_path_);
}

int Frontend::Loop()
{
    LOG_INFO << "This is supposed to be a main loop...";
    int end_frags = 0;
    end_frags = server_.GetGameVar("EndFrags", end_frags);
    LOG_INFO << "Number of players: " << end_frags;
    return 0;
}

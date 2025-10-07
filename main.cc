#include <cstdlib>
#include <iostream>
#include <unistd.h>

#include "GameVariables.h"
#include "Logger.h"
#include "MADServer.h"
#include "Utils.h"
#include "build.h"
#include "consts.h"

#define DEFAULT_CONFIG_FILE "~/.hyperion/Shogo/ShogoSrv.cfg"

void usage(std::string progname)
{
    std::clog << "Usage: " << progname << " [-e] [-c config] [-v]" << std::endl;
    std::clog << "\t-e\t\tStop server after all players leave the game" << std::endl;
    std::clog << "\t-c config\tLoad config file (default: " << DEFAULT_CONFIG_FILE << ")" << std::endl;
    std::clog << "\t-v\t\tPrint the version and quit" << std::endl;
}

int main(int argc, char *argv[])
{
    bool exit_when_empty = false;
    bool only_print_version = false;
    std::string config_path = DEFAULT_CONFIG_FILE;

    char c;
    while ((c = getopt(argc, argv, "c:ev")) != -1)
    {
        switch (c)
        {
        case 'c':
            config_path = std::string(optarg);
            break;
        case 'e':
            exit_when_empty = true;
            break;
        case 'v':
            only_print_version = true;
            break;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    LOG_INFO << "MADServer " << BUILD_REVISION;
    LOG_INFO << "Copyright (c) 2025, BluRaf / Team Zlomformatyk";
    if (only_print_version)
        return 0;

    if (!FindRezFiles())
    {
        return 1;
    }

    MADServer server;
    GameVariables game_vars;

    // START INIT

    // NetStart_DoWizard(g_hInst, &g_ServerInfo, &g_ServerOptions, &g_NetGame, bNoDlgs, configFile);
    // NetStart.cpp:355

    game_vars.tractor_beam = true;
    game_vars.double_jump = true;
    game_vars.ramming_damage = true;
    game_vars.world_time_speed = -1.0f;
    game_vars.run_speed = 1.0;
    game_vars.missile_speed = 1.0;
    game_vars.respawn_scale = 1.0;
    game_vars.heal_scale = 1.0;
    game_vars.world_night_color = "0.5 0.5 0.5";

    // NetStart.cpp:369
    game_vars.server_name = DEFAULTSERVERNAME;

    LOG_INFO << "Loading config file";

    // NetStart.cpp:383
    server.LoadConfigFile(config_path);

    LOG_INFO << "Initializing networking";

    // NetStart.cpp:401
    bool net_init = server.InitNetworking();
    if (!net_init)
    {
        LOG_ERROR << "Couldn't initialize networking";
        return 1;
    }

    LOG_INFO << "Loading console variables";

    // NetStart.cpp:413
    // NetStart_LoadConsoleVars();
    game_vars.Load(server);

    LOG_DEBUG << "Loading level list";

    // NetStart.cpp:415
    // LoadLevelStrings();
    std::vector<std::string> levels = GetLevels(server);

    LOG_DEBUG << "Loading REZ";

    // NetStart.cpp:416
    // LoadRezStrings();
    std::vector<std::string> rez_files = GetRezFiles(server);

    // TODO: NetStart_GoNoDialogs

    // END INIT

    // TODO: CGameServDlg
    // dlg.SetConfigFilename(configFile);
    server.Loop();

	// Save config vars and file
	game_vars.Save(server);
	server.SaveConfigFile(config_path);

    return 0;
}

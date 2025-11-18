#include <cstdlib>
#include <iostream>
#include <unistd.h>

#include "Backend.h"
#include "EventQueue.h"
#include "GameVariables.h"
#include "Logger.h"
#include "consts.h"
#include "utils.h"
#include <arpa/inet.h>
// #include "build.h"

#include "Frontend.h"
#include "Logger.h"

Frontend::Frontend(std::string config_path) : server_(queue_)
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

    continue_loop_ = true;
}

Frontend::~Frontend()
{
    // Save config vars and file
    game_vars_.Save(server_);
    server_.SaveConfigFile(config_path_);
}

void Frontend::HandleShellMessage(std::string msg)
{
    // TODO: Implement processing shell messages (GameServDlg.cpp:944)
    LOG_DEBUG << "Shell message: " << msg;
}

void Frontend::HandlePacket(const std::vector<unsigned char> data, const struct sockaddr_in sender)
{
    // TODO: Implement processing unknown packets for GameSpy support (GameServDlg.cpp:1394)
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &sender.sin_addr, ip, sizeof(ip));

    LOG_DEBUG << "Unknown packet from " << ip << ':' << ntohs(sender.sin_port) << ", size: " << data.size();
}

int Frontend::Loop()
{
    while (true == continue_loop_)
    {
        Event ev = queue_.waitAndPop();

        switch (ev.type)
        {
        case Event::NONE:
            break;

        case Event::SHELL_MESSAGE:
            HandleShellMessage(ev.shell.text);
            break;

        case Event::UNKNOWN_PACKET:
            HandlePacket(ev.packet.data, ev.packet.sender);
            break;
        }
    }

    return 0;
}

void Frontend::Stop()
{
    continue_loop_ = false;
}

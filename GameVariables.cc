#include "GameVariables.h"
#include "Logger.h"
#include "MADServer.h"

void GameVariables::Load(MADServer &server)
{
    server_name = server.GetGameVar("ServerName", server_name);
    service_name = server.GetGameVar("ServiceName", service_name);

    game_type = (enum GameType)server.GetGameVar("GameType", game_type);
    end_type = (enum EndType)server.GetGameVar("EndType", end_type);
    end_frags = server.GetGameVar("EndFrags", end_frags);
    end_time = server.GetGameVar("EndTime", end_time);
    net_service = server.GetGameVar("NetService", net_service);
    max_players = server.GetGameVar("MaxPlayers", max_players);
    update_info = server.GetGameVar("UpdateInfo", update_info);
    server_reg = server.GetGameVar("ServerReg", server_reg);
    use_gamespy = server.GetGameVar("UseGameSpy", use_gamespy);

    dash_go_info = server.GetGameVar("DashGoInfo", dash_go_info);

    port = server.GetGameVar("Port", port);

    tractor_beam = server.GetGameVar("TractorBeam", tractor_beam);
    double_jump = server.GetGameVar("DoubleJump", double_jump);
    ramming_damage = server.GetGameVar("RammingDamage", ramming_damage);

    world_night_color = server.GetGameVar("WorldColorNight", world_night_color);

    run_speed = server.GetGameVar("RunSpeed", run_speed);
    missile_speed = server.GetGameVar("MissileSpeed", missile_speed);
    world_time_speed = server.GetGameVar("WorldTimeSpeed", world_time_speed);
    respawn_scale = server.GetGameVar("RespawnScale", respawn_scale);
    heal_scale = server.GetGameVar("HealScale", heal_scale);
}

void GameVariables::Save(MADServer &server)
{
    server.SetGameVar("ServerName", server_name);
    server.SetGameVar("ServiceName", service_name);

    server.SetGameVar("GameType", game_type);
    server.SetGameVar("EndType", end_type);
    server.SetGameVar("EndFrags", end_frags);
    server.SetGameVar("EndTime", end_time);
    server.SetGameVar("NetService", net_service);
    server.SetGameVar("MaxPlayers", max_players);
    server.SetGameVar("UpdateInfo", update_info);
    server.SetGameVar("ServerReg", server_reg);
    server.SetGameVar("UseGameSpy", use_gamespy);
    server.SetGameVar("DashGoInfo", 1);
    server.SetGameVar("Port", port);

    server.SetGameVar("TractorBeam", tractor_beam);
    server.SetGameVar("DoubleJump", double_jump);
    server.SetGameVar("RammingDamage", ramming_damage);
    server.SetGameVar("WorldColorNight", world_night_color);

    server.SetGameVar("RunSpeed", run_speed);
    server.SetGameVar("MissileSpeed", missile_speed);
    server.SetGameVar("WorldTimeSpeed", world_time_speed);
    server.SetGameVar("RespawnScale", respawn_scale);
    server.SetGameVar("HealScale", heal_scale);
}

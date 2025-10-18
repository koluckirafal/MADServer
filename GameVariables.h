#ifndef _GAMEVARIABLES_H
#define _GAMEVARIABLES_H

#include "Backend.h"

enum GameType
{
    kSingle = 0,
    kDeathmatch = 1,
    kCaptureTheFlag = 2,
    kCooperative = 3
};

enum EndType
{
    kFrags = 1,
    kTime = 2,
    kFragsAndTime = 3,
    kNever = 4
};

class GameVariables
{
  public:
    std::string server_name;  // 256
    std::string service_name; // 256
    enum GameType game_type;
    enum EndType end_type;
    int end_frags;
    int end_time;
    int net_service; // hardcoded to ???
    int max_players;
    bool update_info;
    bool server_reg;
    bool use_gamespy;
    bool dash_go_info;
    int port;
    bool tractor_beam;
    bool double_jump;
    bool ramming_damage;
    std::string world_night_color; // 32
    float run_speed;
    float missile_speed;
    float world_time_speed;
    float respawn_scale;
    float heal_scale;
    GameVariables()
        : game_type(kDeathmatch), end_type(kFrags), end_frags(25), end_time(10), net_service(0), max_players(16),
          update_info(true), server_reg(true), use_gamespy(true), dash_go_info(false), port(0) {};
    void Load(Backend &server);
    void Save(Backend &server);
};

#endif // _GAMEVARIABLES_H

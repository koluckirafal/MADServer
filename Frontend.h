#ifndef _FRONTEND_H
#define _FRONTEND_H

#include "Backend.h"
#include "EventQueue.h"
#include "GameVariables.h"
#include <iostream>
#include <netinet/in.h>

class Frontend
{
  private:
    Backend server_;
    EventQueue queue_;
    GameVariables game_vars_;
    std::string config_path_;
    bool continue_loop_;

  public:
    Frontend(std::string config_path);
    ~Frontend();
    int Loop();
    void Stop();
    void HandleShellMessage(const std::string msg);
    void HandlePacket(const std::vector<unsigned char> data, const struct sockaddr_in sender);
};

#endif // _FRONTEND_H

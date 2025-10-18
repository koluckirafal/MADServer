#ifndef _FRONTEND_H
#define _FRONTEND_H

#include "Backend.h"
#include "GameVariables.h"
#include <iostream>

class Frontend
{
  private:
    Backend server_;
    GameVariables game_vars_;
    std::string config_path_;

  public:
    Frontend(std::string config_path);
    ~Frontend();
    int Loop();
};

#endif // _FRONTEND_H

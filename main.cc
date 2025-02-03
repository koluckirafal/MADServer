#include "Logger.h"
#include "MADServer.h"
#include <iostream>

int main(int argc, char *argv[])
{
    LOG_INFO << "MADServ says hello!";
    MADServer server;
    LOG_INFO << "Server is ready!";

    // HCONSOLEVAR hVar = NULL;
    // std::cout << "main: uwaga rakieta!!" << std::endl;
    // DRESULT dr = server_intf->GetConsoleVar("WebRegUpdate", &hVar, NULL);
    // std::cout << "main: jeblo " << dr << " und " << hVar << std::endl;

    // char test[] = "debuglevel 0";
    // server_intf->RunConsoleString(test);

    return 0;
}

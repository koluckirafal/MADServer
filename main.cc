#include <unistd.h>

#include "build.h"
#include "Logger.h"
#include "MADServer.h"


int main(int argc, char *argv[])
{
    bool exit_when_empty = false;
    std::string config_file = "ShogoServ.cpp";

    char c;
    while ((c = getopt (argc, argv, "c:e")) != -1) {
        switch (c) {
            case 'c':
                config_file = std::string(optarg);
                break;
            case 'e':
                exit_when_empty = true;
                break;
            default:
                std::clog << "usage: " << argv[0] << " [-e] [-c ShogoServ.cfg]" << std::endl;
                return EXIT_FAILURE;
        }
    }

    LOG_INFO << "MADServer " << BUILD_REVISION;
    LOG_INFO << "Copyright (c) 2025, BluRaf / Team Zlomformatyk";

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

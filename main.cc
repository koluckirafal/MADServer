#include <cstdlib>
#include <iostream>
#include <unistd.h>

#include "Logger.h"
#include "MADServer.h"
#include "Utils.h"
#include "build.h"

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

    if (!FindRezFiles()) {
        return 1;
    }

    MADServer server;

    server.Setup(config_path);
    server.Loop();

    return 0;
}

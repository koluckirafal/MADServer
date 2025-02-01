#include "MADServer.h"
#include "MADServerIO.h"
#include <iostream>

int main(int argc, char *argv[])
{
    MADServer server;
    MADServerIO server_io;

    std::cout << "main: Test!" << std::endl;
    HCONSOLEVAR hVar = NULL;
    std::cout << "main: uwaga rakieta!!" << std::endl;
    //DRESULT dr = server.getServerManager()->GetConsoleVar("WebRegUpdate", &hVar, NULL);
    //std::cout << "main: jeblo " << dr << " und " << hVar << std::endl;

    server.getServerManager()->SetAppHandler(&server_io);
    char test[] = "debuglevel 0";
    server.getServerManager()->RunConsoleString(test);

    return 0;
}

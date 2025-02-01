#include "MADServer.h"
#include "MADServerIO.h"
#include <iostream>

int main(int argc, char *argv[])
{
    MADServer server;
    MADServerIO server_io;
    // TODO: Wrap all ServerInterface functions in MADServer class

    // ServerInterface * server_intf = server.getServerManager();

    // server_intf->SetAppHandler(&server_io);

    // HCONSOLEVAR hVar = NULL;
    // std::cout << "main: uwaga rakieta!!" << std::endl;
    // DRESULT dr = server_intf->GetConsoleVar("WebRegUpdate", &hVar, NULL);
    // std::cout << "main: jeblo " << dr << " und " << hVar << std::endl;

    // char test[] = "debuglevel 0";
    // server_intf->RunConsoleString(test);

    return 0;
}

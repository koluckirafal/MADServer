#include "MADServer.h"
#include <iostream>

int main(int argc, char *argv[])
{
    MADServer server;
    std::cout << "Hello from main.cpp!" << std::endl;

    // HCONSOLEVAR hVar = NULL;
    // std::cout << "main: uwaga rakieta!!" << std::endl;
    // DRESULT dr = server_intf->GetConsoleVar("WebRegUpdate", &hVar, NULL);
    // std::cout << "main: jeblo " << dr << " und " << hVar << std::endl;

    // char test[] = "debuglevel 0";
    // server_intf->RunConsoleString(test);

    return 0;
}

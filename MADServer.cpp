#include <dlfcn.h>
#include <iostream>
#include "MADServer.h"

DGUID GAMEGUID = { /* 87EEDE80-0ED4-11D2-BA96-006008904776 */
	0x87eede80,	0xed4, 0x11d2, 0xba, 0x96, 0x0, 0x60, 0x8, 0x90, 0x47, 0x76
};

MADServer::MADServer()
{
    std::cout << "MADServer says hello!" << std::endl;

    // Load server.dll and import server functions
    try {
        server_dll = dlopen("./server.dll", RTLD_LAZY);
        if (server_dll == nullptr) {
            throw 1;
        }
        s_fnCreateServer = (CreateServerFn)(dlsym(server_dll, "CreateServer"));
        if (s_fnCreateServer == nullptr) {
            throw 2;
        }
        s_fnDeleteServer = (DeleteServerFn)(dlsym(server_dll, "DeleteServer"));
        if (s_fnCreateServer == nullptr) {
            throw 3;
        }
    }
    catch (int e)
    {
        std::cout << "MADServer: server.dll loading failed with exception " << e << std::endl;
        std::cout << "MADServer: dlerror: " << dlerror() << std::endl;
    }

    std::cout << "MADServer: server.dll at " << server_dll << std::endl;
    std::cout << "MADServer: CreateServer at " << s_fnCreateServer << std::endl;
    std::cout << "MADServer: DeleteServer at " << s_fnDeleteServer << std::endl;

    // Run the god-damn server
    try {
        s_pServerMgr = nullptr;
        SI_CREATESTATUS status = s_fnCreateServer(SI_VERSION, GAMEGUID, &s_pServerMgr);
        if (status != 0) {
            throw 4;
        }
    }
    catch (int e)
    {
        std::cout << "MADServer: server creating failed with exception " << e << std::endl;
    }
    std::cout << "MADServer: s_pServerMgr at " << s_pServerMgr << std::endl;
}

ServerInterface* MADServer::getServerManager()
{
    return s_pServerMgr;
}

MADServer::~MADServer()
{
    std::cout << "MADServer says good buy!" << std::endl;
    s_fnDeleteServer();
    int refs = 0;
    if (server_dll != nullptr) refs = dlclose(server_dll);
    if (refs != 0) {
        std::cout << "MADServer: " << refs << " references to server.dll left in memory!" << std::endl;
    }
    std::cout << "MADServer *tips fedora*" << std::endl;
}

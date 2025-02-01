#include <iostream>
#include <dlfcn.h>

#include "server_interface.h"

#include "MADServer.h"


DGUID GAMEGUID = { /* 87EEDE80-0ED4-11D2-BA96-006008904776 */
	0x87eede80,	0xed4, 0x11d2, 0xba, 0x96, 0x0, 0x60, 0x8, 0x90, 0x47, 0x76
};


#define THROW_DYNLOADEXCEPTION(num) { \
    std::cout << "MADServer::Impl_: dlerror: " << dlerror() << std::endl; \
    throw num; \
}

class MADServer::Impl_
{
private:
    void* server_dll;
    CreateServerFn CreateServer;
    DeleteServerFn DeleteServer;
    ServerInterface* server_mgr;
    ServerInterface* GetServerManager()
    {
        if (server_mgr == 0) throw 1;    // Server not initialized!

        return server_mgr;
    };

public:
    Impl_()
    {
        std::cout << "MADServer::Impl_: CONSTRUCTOR START" << std::endl;

        // Load server.dll and import server functions
        server_dll = dlopen("./server.dll", RTLD_LAZY);
        if (server_dll == 0) {
            THROW_DYNLOADEXCEPTION(1);
        }

        std::cout << "MADServer::Impl_: server.dll at " << server_dll << std::endl;

        CreateServer = (CreateServerFn)(dlsym(server_dll, "CreateServer"));
        if (CreateServer == 0) {
            THROW_DYNLOADEXCEPTION(2);
        }
        int refs = 0;
        DeleteServer = (DeleteServerFn)(dlsym(server_dll, "DeleteServer"));
        if (DeleteServer == 0) {
            THROW_DYNLOADEXCEPTION(3);
        }

        // Create the server instance

        SI_CREATESTATUS status = CreateServer(SI_VERSION, GAMEGUID, &server_mgr);
        if (status != 0) {
            std::cout << "MADServer::Impl_: Couldn't create server" << std::endl;
            throw 4;
        }
        std::cout << "MADServer::Impl_: server_mgr at " << server_mgr << std::endl;

        std::cout << "MADServer::Impl_: CONSTRUCTOR END" << std::endl;
    };

    ~Impl_()
    {
        std::cout << "MADServer::Impl_: DESTRUCTOR START" << std::endl;

        DeleteServer();
        server_mgr = 0;

        int refs = 0;
        if (server_dll != 0) refs = dlclose(server_dll);
        if (refs != 0) {
            std::cout << "MADServer::Impl_: " << refs << " references to server.dll left in memory!" << std::endl;
        }

        std::cout << "MADServer::Impl_: DESTRUCTOR END" << std::endl;
    };
};


MADServer::MADServer()
{
    std::cout << "MADServer: CONSTRUCTOR START" << std::endl;
    p_impl_ = new Impl_();
    std::cout << "MADServer: CONSTRUCTOR END" << std::endl;
}


MADServer::~MADServer()
{
    std::cout << "MADServer: DESTRUCTOR START" << std::endl;
    delete p_impl_;
    std::cout << "MADServer: DESTRUCTOR END" << std::endl;
}

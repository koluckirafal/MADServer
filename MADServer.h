#ifndef _MADSERVER_H
#define _MADSERVER_H

#include "server_interface.h"

class MADServer
{
    ServerInterface* s_pServerMgr;
private:
    void * server_dll;
    CreateServerFn s_fnCreateServer;
    DeleteServerFn s_fnDeleteServer;
public:
    MADServer();
    ServerInterface* getServerManager();
    ~MADServer();
};

#endif

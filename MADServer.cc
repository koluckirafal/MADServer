#include <iostream>
#include <vector>
#include <string>
#include <dlfcn.h>
#include <errno.h>

#include "server_interface.h"
#include "MADServer.h"


DGUID GAMEGUID = { /* 87EEDE80-0ED4-11D2-BA96-006008904776 */
	0x87eede80,	0xed4, 0x11d2, 0xba, 0x96, 0x0, 0x60, 0x8, 0x90, 0x47, 0x76
};


#define THROW_DYNLOADEXCEPTION(num) { \
    std::cout << "MADServer::Impl_: dlerror: " << dlerror() << std::endl; \
    throw num; \
}

#define SHELLMESSAGEPTR typedef unsigned long (MADServer::*ShellMessagePtr)(std::string &msg)
#define PROCESSPACKETPTR typedef unsigned long (MADServer::*ProcessPacketPtr)(std::vector<unsigned char> &data, unsigned char src_addr[4], unsigned short src_port)

class MADServer::Callbacks : public ServerAppHandler
{
private:
    MADServer& madserv_ref;
    SHELLMESSAGEPTR;
    PROCESSPACKETPTR;
    ShellMessagePtr smsg_ptr;
    ProcessPacketPtr procpacket_ptr;

public:
    Callbacks(MADServer &madserv, ShellMessagePtr smsg, ProcessPacketPtr procpacket)
        : madserv_ref(madserv), smsg_ptr(smsg), procpacket_ptr(procpacket) {}

	DRESULT	ShellMessageFn(char *pInfo) {
        if (pInfo == 0) throw 1;

        std::string shell_msg(pInfo);

        return (madserv_ref.*smsg_ptr)(shell_msg);
    }

	DRESULT ConsoleOutputFn(char *pMsg) {
        if (pMsg == 0) throw 2;
        // TODO: Replace with logger
        std::cout << "server.dll: " << pMsg << std::endl;
        return 0;
    }

	DRESULT	OutOfMemory() {
        // TODO: Replace with logger
        std::cout << "server.dll: Memory exhausted, exiting immediately!" << std::endl;
        exit(ENOMEM);

        return 1;
    }

	DRESULT ProcessPacket(char *pData, DDWORD dataLen, DBYTE senderAddr[4], D_WORD senderPort) {
        if (pData == 0) throw 4;

        std::vector<unsigned char> data(dataLen);
        for (int i = 0; i < dataLen; i++) data[i] = static_cast<unsigned char>(pData[i]);

        return (madserv_ref.*procpacket_ptr)(data, senderAddr, senderPort);
    }
};


class MADServer::Impl_
{
private:
    void* server_dll;
    CreateServerFn CreateServer;
    DeleteServerFn DeleteServer;
    ServerInterface* server_mgr;
    MADServer::Callbacks* server_cb;
    SHELLMESSAGEPTR;
    PROCESSPACKETPTR;
    ServerInterface* GetServerManager()
    {
        if (server_mgr == 0) throw 1;    // Server not initialized!

        return server_mgr;
    };


public:
    Impl_(MADServer& madserv, ShellMessagePtr smsg, ProcessPacketPtr procpacket)
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

        // Set callbacks
        server_cb = new Callbacks(madserv, smsg, procpacket);
        GetServerManager()->SetAppHandler(server_cb);

        std::cout << "MADServer::Impl_: CONSTRUCTOR END" << std::endl;
    };

    ~Impl_()
    {
        std::cout << "MADServer::Impl_: DESTRUCTOR START" << std::endl;

        DeleteServer();
        server_mgr = 0;

        delete server_cb;
        server_cb = 0;

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
    p_impl_ = new Impl_(*this, &MADServer::ProcessShellMessage, &MADServer::ProcessPacket);
    std::cout << "MADServer: CONSTRUCTOR END" << std::endl;
}


MADServer::~MADServer()
{
    std::cout << "MADServer: DESTRUCTOR START" << std::endl;
    delete p_impl_;
    std::cout << "MADServer: DESTRUCTOR END" << std::endl;
}


unsigned long MADServer::ProcessShellMessage(std::string &msg)
{
    // TODO: Implement processing shell messages (GameServDlg.cpp:944)
}


unsigned long MADServer::ProcessPacket(std::vector<unsigned char> &data, unsigned char src_addr[4], unsigned short src_port)
{
    // TODO: Implement processing unknown packets for GameSpy support (GameServDlg.cpp:1394)
}

// TODO: Wrap all ServerInterface functions in MADServer class

/*
unsigned long MADServer::RunConsoleString(char *pStr)
{

}

unsigned long MADServer::GetConsoleVar(char *pName, HCONSOLEVAR *hVar, char *pDefaultVal)
{

}

unsigned long MADServer::GetVarValueFloat(HCONSOLEVAR hVar, float *val)
{

}

unsigned long MADServer::GetVarValueString(HCONSOLEVAR hVar, char *pStr, DDWORD maxLen)
{

}

unsigned long MADServer::LoadConfigFile(char *pFilename)
{

}

unsigned long MADServer::SaveConfigFile(char *pFilename)
{

}

unsigned long MADServer::SendToServerShell(char *pInfo)
{

}

char MADServer::AddResources(char **pResources, DDWORD nResources)
{

}

struct FileEntry_t* MADServer::GetFileList(char *pDirName)
{

}

void MADServer::FreeFileList(struct FileEntry_t *pList)
{

}

char MADServer::StartWorld(StartGameRequest *pRequest)
{

}

int	 MADServer::GetNumClients()
{

}

char MADServer::GetClientName(int index, char *pName, int maxChars)
{

}

char MADServer::GetClientInfo(int index, ClientInfo* pInfo)
{

}

char MADServer::BootClient(DDWORD dwClientID)
{

}

int MADServer::GetErrorCode()
{

}

void MADServer::GetErrorString(char *pString, int maxLen)
{

}	

char MADServer::Update(long flags)
{

}

char MADServer::InitNetworking(char *pDriver, DDWORD dwFlags)
{

}

char MADServer::GetServiceList(NetService *&pListHead)
{

}

char MADServer::FreeServiceList(NetService *pListHead)
{

}

char MADServer::SelectService(HNETSERVICE hNetService)
{

}

char MADServer::UpdateSessionName(const char* sName)
{

}

char MADServer::HostGame(NetHost* pHostInfo)
{

}

char MADServer::GetTcpIpAddress(char* sAddress, DDWORD dwBufferSize, unsigned short &hostPort)
{

}

unsigned long MADServer::SendTo(const void *pData, DDWORD len, const char *sAddr, DDWORD port)
{

}
*/
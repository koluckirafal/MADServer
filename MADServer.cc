#include <dlfcn.h>
#include <errno.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Logger.h"
#include "Utils.h"
#include "server_interface.h"
#include "MADServer.h"
#include "consts.h"

DGUID GAMEGUID = {/* 87EEDE80-0ED4-11D2-BA96-006008904776 */
                  0x87eede80, 0xed4, 0x11d2, 0xba, 0x96, 0x0, 0x60, 0x8, 0x90, 0x47, 0x76};

#define THROW_DYNLOADEXCEPTION(num)                                                                                    \
    {                                                                                                                  \
        LOG_FATAL << "dlerror: " << dlerror();                                                                         \
        throw num;                                                                                                     \
    }

#define SHELLMESSAGEPTR typedef unsigned long (MADServer::*ShellMessagePtr)(std::string & msg)
#define PROCESSPACKETPTR                                                                                               \
    typedef unsigned long (MADServer::*ProcessPacketPtr)(std::vector<unsigned char> & data, unsigned char src_addr[4], \
                                                         unsigned short src_port)

// Server callbacks
class MADServer::Callbacks_ : public ServerAppHandler
{
  private:
    MADServer &madserv_ref;
    SHELLMESSAGEPTR;
    PROCESSPACKETPTR;
    ShellMessagePtr smsg_ptr;
    ProcessPacketPtr procpacket_ptr;

  public:
    Callbacks_(MADServer &madserv, ShellMessagePtr smsg, ProcessPacketPtr procpacket)
        : madserv_ref(madserv), smsg_ptr(smsg), procpacket_ptr(procpacket)
    {
    }

    DRESULT ShellMessageFn(char *pInfo)
    {
        if (pInfo == 0)
            throw 1;

        std::string shell_msg(pInfo);

        return (madserv_ref.*smsg_ptr)(shell_msg);
    }

    DRESULT ConsoleOutputFn(char *pMsg)
    {
        if (pMsg == 0)
            throw 2;
        LOG_INFO << "server.dll: " << pMsg;
        return 0;
    }

    DRESULT OutOfMemory()
    {
        LOG_FATAL << "server.dll: Memory exhausted, exiting immediately!";
        exit(ENOMEM);

        return 1;
    }

    DRESULT ProcessPacket(char *pData, DDWORD dataLen, DBYTE senderAddr[4], D_WORD senderPort)
    {
        if (pData == 0)
            throw 4;

        std::vector<unsigned char> data(dataLen);
        for (int i = 0; i < dataLen; i++)
            data[i] = static_cast<unsigned char>(pData[i]);

        return (madserv_ref.*procpacket_ptr)(data, senderAddr, senderPort);
    }
};

// Implementation - contains platform-specific server.dll interfacing
class MADServer::Impl_
{
  private:
    void *server_dll_;
    CreateServerFn CreateServer_;
    DeleteServerFn DeleteServer_;
    ServerInterface *server_mgr_;

  public:
    Impl_()
    {
        // Load server.dll and import server functions
        server_dll_ = dlopen("./server.dll", RTLD_LAZY);
        if (server_dll_ == 0)
        {
            THROW_DYNLOADEXCEPTION(1);
        }

        LOG_DEBUG << "server.dll at " << server_dll_;

        CreateServer_ = (CreateServerFn)(dlsym(server_dll_, "CreateServer"));
        if (CreateServer_ == 0)
        {
            THROW_DYNLOADEXCEPTION(2);
        }
        int refs = 0;
        DeleteServer_ = (DeleteServerFn)(dlsym(server_dll_, "DeleteServer"));
        if (DeleteServer_ == 0)
        {
            THROW_DYNLOADEXCEPTION(3);
        }

        // Create the server instance

        SI_CREATESTATUS status = CreateServer_(SI_VERSION, GAMEGUID, &server_mgr_);
        if (status != 0)
        {
            LOG_ERROR << "Couldn't create server";
            throw 4;
        }
        LOG_DEBUG << "server_mgr_ at " << server_mgr_;
    };

    ~Impl_()
    {
        DeleteServer_();
        server_mgr_ = 0;

        int refs = 0;
        if (server_dll_ != 0)
            refs = dlclose(server_dll_);
        if (refs != 0)
        {
            LOG_DEBUG << refs << " references to server.dll left in memory!";
        }
    };

    ServerInterface *GetServerManager()
    {
        if (server_mgr_ == 0)
            throw 1; // Server not initialized!

        return server_mgr_;
    };
};

MADServer::MADServer()
{
    // Create server implementation
    p_impl_ = new Impl_();

    // Create callbacks
    server_cb_ = new Callbacks_(*this, &MADServer::ProcessShellMessage, &MADServer::ProcessPacket);

    // Set callbacks
    p_impl_->GetServerManager()->SetAppHandler(server_cb_);
}

MADServer::~MADServer()
{
    // Destroy server implementation first,
    // because server.dll will use callback to announce its departure
    delete p_impl_;
    delete server_cb_;
}

unsigned long MADServer::ProcessShellMessage(std::string &msg)
{
    // TODO: Implement processing shell messages (GameServDlg.cpp:944)
    return 0;
}

unsigned long MADServer::ProcessPacket(std::vector<unsigned char> &data, unsigned char src_addr[4],
                                       unsigned short src_port)
{
    // TODO: Implement processing unknown packets for GameSpy support (GameServDlg.cpp:1394)
    return 0;
}

int MADServer::Setup(std::string config_path)
{
    // NetStart_DoWizard(g_hInst, &g_ServerInfo, &g_ServerOptions, &g_NetGame, bNoDlgs, configFile);
    // NetStart.cpp:355
    /*
    server_options_->m_bTractorBeam    = TRUE;
	server_options_->m_bDoubleJump     = TRUE;
	server_options_->m_bRammingDamage  = TRUE;
	server_options_->m_fWorldTimeSpeed = -1.0f;
	server_options_->m_fRunSpeed       = 1.0;
	server_options_->m_fMissileSpeed   = 1.0;
	server_options_->m_fRespawnScale   = 1.0;
	server_options_->m_fHealScale      = 1.0;
	strcpy(server_options_->m_sWorldNightColor, "0.5 0.5 0.5");

    // NetStart.cpp:369
    server_name_ = DEFAULTSERVERNAME;
    */

    // NetStart.cpp:383
    p_impl_->GetServerManager()->LoadConfigFile((char *)config_path.c_str());

    // NetStart.cpp:401
    bool net_init = p_impl_->GetServerManager()->InitNetworking(NULL, 0);
    if (!net_init) {
        LOG_ERROR << "Couldn't initialize networking";
        return 1;
    }

    // NetStart.cpp:411
}

int MADServer::Loop(void)
{
    LOG_INFO << "This is supposed to be a main loop...";
}

void MADServer::SetGameVar(const std::string key, std::string value)
{
    std::string command = "\"+" + key + "\" \"" + value + "\"";
	p_impl_->GetServerManager()->RunConsoleString((char *)command.c_str());
}

void MADServer::SetGameVar(const std::string key, float value)
{
	SetGameVar(key, FloatToString(value));
}

void MADServer::SetGameVar(const std::string key, int value)
{
	SetGameVar(key, IntToString(value));
}

std::string MADServer::GetGameVar(const std::string key, std::string* init_value)
{
	HCONSOLEVAR var_handle;
    std::string value;
    char raw_value[MAX_STRING_SIZE+1];
    memset(raw_value, '\0', MAX_STRING_SIZE);

    if (init_value != NULL) {
        value = *init_value;
        strncpy(raw_value, value.c_str(), MAX_STRING_SIZE);
    }

	if (p_impl_->GetServerManager()->GetConsoleVar((char *)key.c_str(), &var_handle, raw_value) == 0)
	{
		if (p_impl_->GetServerManager()->GetVarValueString(var_handle, raw_value, MAX_STRING_SIZE) == 0) {
            value = raw_value;
        }
        else {
            throw 2;
        }
	}
	else
	{
		throw 1;
	}

    return value;
}

int MADServer::GetGameVar(const std::string key, int* init_value)
{
	HCONSOLEVAR var_handle;
	std::string value;

    if (init_value != NULL) {
        value = IntToString(*init_value);
    }

    value = GetGameVar(key, &value);

	return StringToInt(value);
}

float MADServer::GetGameVar(const std::string key, float* init_value)
{
	HCONSOLEVAR var_handle;
	std::string value;

    if (init_value != NULL) {
        value = FloatToString(*init_value);
    }

    value = GetGameVar(key, &value);

	return StringToFloat(value);
}

// TODO: Wrap all ServerInterface functions in MADServer class

/*
unsigned long MADServer::RunConsoleString(std::string string)
{
    return p_impl_->GetServerManager()->RunConsoleString(string.c_str());
}

unsigned long MADServer::GetConsoleVar(char *pName, HCONSOLEVAR *hVar, char *pDefaultVal)
{
    return p_impl_->GetServerManager()->
}

unsigned long MADServer::GetVarValueFloat(HCONSOLEVAR hVar, float *val)
{
    return p_impl_->GetServerManager()->
}

unsigned long MADServer::GetVarValueString(HCONSOLEVAR hVar, char *pStr, DDWORD maxLen)
{
    return p_impl_->GetServerManager()->
}

unsigned long MADServer::LoadConfigFile(char *pFilename)
{
    return p_impl_->GetServerManager()->
}

unsigned long MADServer::SaveConfigFile(char *pFilename)
{
    return p_impl_->GetServerManager()->
}

unsigned long MADServer::SendToServerShell(char *pInfo)
{
    return p_impl_->GetServerManager()->
}

char MADServer::AddResources(char **pResources, DDWORD nResources)
{
    return p_impl_->GetServerManager()->
}

struct FileEntry_t* MADServer::GetFileList(char *pDirName)
{
    return p_impl_->GetServerManager()->
}

void MADServer::FreeFileList(struct FileEntry_t *pList)
{
    return p_impl_->GetServerManager()->
}

char MADServer::StartWorld(StartGameRequest *pRequest)
{
    return p_impl_->GetServerManager()->
}

int	 MADServer::GetNumClients()
{
    return p_impl_->GetServerManager()->
}

char MADServer::GetClientName(int index, char *pName, int maxChars)
{
    return p_impl_->GetServerManager()->
}

char MADServer::GetClientInfo(int index, ClientInfo* pInfo)
{
    return p_impl_->GetServerManager()->
}

char MADServer::BootClient(DDWORD dwClientID)
{
    return p_impl_->GetServerManager()->
}

int MADServer::GetErrorCode()
{
    return p_impl_->GetServerManager()->
}

void MADServer::GetErrorString(char *pString, int maxLen)
{
    return p_impl_->GetServerManager()->
}

char MADServer::Update(long flags)
{
    return p_impl_->GetServerManager()->
}

char MADServer::InitNetworking(char *pDriver, DDWORD dwFlags)
{
    return p_impl_->GetServerManager()->
}

char MADServer::GetServiceList(NetService *&pListHead)
{
    return p_impl_->GetServerManager()->
}

char MADServer::FreeServiceList(NetService *pListHead)
{
    return p_impl_->GetServerManager()->
}

char MADServer::SelectService(HNETSERVICE hNetService)
{
    return p_impl_->GetServerManager()->
}

char MADServer::UpdateSessionName(const char* sName)
{
    return p_impl_->GetServerManager()->
}

char MADServer::HostGame(NetHost* pHostInfo)
{
    return p_impl_->GetServerManager()->
}

char MADServer::GetTcpIpAddress(char* sAddress, DDWORD dwBufferSize, unsigned short &hostPort)
{
    return p_impl_->GetServerManager()->
}

unsigned long MADServer::SendTo(const void *pData, DDWORD len, const char *sAddr, DDWORD port)
{
    return p_impl_->GetServerManager()->
}
*/

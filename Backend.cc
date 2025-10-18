#include <dlfcn.h>
#include <errno.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Backend.h"
#include "Logger.h"
#include "consts.h"
#include "server_interface.h"
#include "utils.h"

// clang-format off
DGUID GAMEGUID = {/* 87EEDE80-0ED4-11D2-BA96-006008904776 */
                  0x87eede80, 0xed4, 0x11d2, {0xba, 0x96, 0x0, 0x60, 0x8, 0x90, 0x47, 0x76}};
// clang-format on

#define THROW_DYNLOADEXCEPTION(num)                                                                                    \
    {                                                                                                                  \
        LOG_FATAL << "dlerror: " << dlerror();                                                                         \
        throw num;                                                                                                     \
    }

#define SHELLMESSAGEPTR typedef unsigned long (Backend::*ShellMessagePtr)(std::string & msg)
#define PROCESSPACKETPTR                                                                                               \
    typedef unsigned long (Backend::*ProcessPacketPtr)(std::vector<unsigned char> & data, unsigned char src_addr[4],   \
                                                       unsigned short src_port)

// Server callbacks
class Backend::Callbacks_ : public ServerAppHandler
{
  private:
    Backend &madserv_ref;
    SHELLMESSAGEPTR;
    PROCESSPACKETPTR;
    ShellMessagePtr smsg_ptr;
    ProcessPacketPtr procpacket_ptr;

  public:
    Callbacks_(Backend &madserv, ShellMessagePtr smsg, ProcessPacketPtr procpacket)
        : madserv_ref(madserv), smsg_ptr(smsg), procpacket_ptr(procpacket)
    {
    }

    virtual ~Callbacks_() {};

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
        for (size_t i = 0; i < dataLen; i++)
            data[i] = static_cast<unsigned char>(pData[i]);

        return (madserv_ref.*procpacket_ptr)(data, senderAddr, senderPort);
    }
};

// Implementation - contains platform-specific server.dll interfacing
class Backend::Impl_
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
        //  int refs = 0;
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

Backend::Backend()
{
    // Create server implementation
    p_impl_ = new Impl_();

    // Create callbacks
    // TODO: move callbacks outsite Backend
    server_cb_ = new Callbacks_(*this, &Backend::ProcessShellMessage, &Backend::ProcessPacket);

    // Set callbacks
    p_impl_->GetServerManager()->SetAppHandler(server_cb_);
}

Backend::~Backend()
{
    // Destroy server implementation first,
    // because server.dll will use callback to announce its departure
    delete p_impl_;
    delete server_cb_;
}

unsigned long Backend::ProcessShellMessage(std::string &msg)
{
    // TODO: Implement processing shell messages (GameServDlg.cpp:944)
    return 0;
}

unsigned long Backend::ProcessPacket(std::vector<unsigned char> &data, unsigned char src_addr[4],
                                     unsigned short src_port)
{
    // TODO: Implement processing unknown packets for GameSpy support (GameServDlg.cpp:1394)
    return 0;
}

void Backend::SetGameVar(const std::string key, std::string value)
{
    std::string command = "\"+" + key + "\" \"" + value + "\"";
    p_impl_->GetServerManager()->RunConsoleString((char *)command.c_str());
}

void Backend::SetGameVar(const std::string key, float value)
{
    SetGameVar(key, FloatToString(value));
}

void Backend::SetGameVar(const std::string key, int value)
{
    SetGameVar(key, IntToString(value));
}

std::string Backend::GetGameVar(const std::string key, std::string init_value)
{
    HCONSOLEVAR var_handle;
    std::string value;

    value = init_value;

    const char *raw_key = key.c_str();
    char raw_value[MAX_STRING_SIZE + 1];
    memset(raw_value, '\0', MAX_STRING_SIZE);
    strncpy(raw_value, value.c_str(), MAX_STRING_SIZE - 1);

    // LOG_DEBUG << "server.dll::GetConsoleVar(\"" << key << "\", \"" << value << "\");";
    if (p_impl_->GetServerManager()->GetConsoleVar((char *)raw_key, &var_handle, raw_value) == 0)
    {
        // LOG_DEBUG << "server.dll::GetVarValueString(" << value << ", " << STRINGIZE(MAX_STRING_SIZE) << ");";
        if (p_impl_->GetServerManager()->GetVarValueString(var_handle, raw_value, MAX_STRING_SIZE) == 0)
        {
            value = raw_value;
        }
        else
        {
            LOG_DEBUG << "Variable exists, but couldn't get value?";
        }
    }
    else
    {
        SetGameVar(key, value);
    }

    LOG_DEBUG << key << " = " << value;
    return value;
}

int Backend::GetGameVar(const std::string key, int init_value)
{
    std::string value;

    value = IntToString(init_value);

    value = GetGameVar(key, value);

    return StringToInt(value);
}

float Backend::GetGameVar(const std::string key, float init_value)
{
    std::string value;

    value = FloatToString(init_value);

    value = GetGameVar(key, value);

    return StringToFloat(value);
}

// TODO: Wrap all ServerInterface functions in Backend class

unsigned long Backend::LoadConfigFile(std::string config_path)
{
    return p_impl_->GetServerManager()->LoadConfigFile((char *)(config_path.c_str()));
}

unsigned long Backend::SaveConfigFile(std::string config_path)
{
    return p_impl_->GetServerManager()->SaveConfigFile((char *)(config_path.c_str()));
}

char Backend::InitNetworking(void)
{
    return p_impl_->GetServerManager()->InitNetworking(NULL, 0);
}

/*

unsigned long Backend::SendToServerShell(char *pInfo)
{
    return p_impl_->GetServerManager()->
}

char Backend::AddResources(char **pResources, DDWORD nResources)
{
    return p_impl_->GetServerManager()->
}

struct FileEntry_t* Backend::GetFileList(char *pDirName)
{
    return p_impl_->GetServerManager()->
}

void Backend::FreeFileList(struct FileEntry_t *pList)
{
    return p_impl_->GetServerManager()->
}

char Backend::StartWorld(StartGameRequest *pRequest)
{
    return p_impl_->GetServerManager()->
}

int	 Backend::GetNumClients()
{
    return p_impl_->GetServerManager()->
}

char Backend::GetClientName(int index, char *pName, int maxChars)
{
    return p_impl_->GetServerManager()->
}

char Backend::GetClientInfo(int index, ClientInfo* pInfo)
{
    return p_impl_->GetServerManager()->
}

char Backend::BootClient(DDWORD dwClientID)
{
    return p_impl_->GetServerManager()->
}

int Backend::GetErrorCode()
{
    return p_impl_->GetServerManager()->
}

void Backend::GetErrorString(char *pString, int maxLen)
{
    return p_impl_->GetServerManager()->
}

char Backend::Update(long flags)
{
    return p_impl_->GetServerManager()->
}

char Backend::GetServiceList(NetService *&pListHead)
{
    return p_impl_->GetServerManager()->
}

char Backend::FreeServiceList(NetService *pListHead)
{
    return p_impl_->GetServerManager()->
}

char Backend::SelectService(HNETSERVICE hNetService)
{
    return p_impl_->GetServerManager()->
}

char Backend::UpdateSessionName(const char* sName)
{
    return p_impl_->GetServerManager()->
}

char Backend::HostGame(NetHost* pHostInfo)
{
    return p_impl_->GetServerManager()->
}

char Backend::GetTcpIpAddress(char* sAddress, DDWORD dwBufferSize, unsigned short &hostPort)
{
    return p_impl_->GetServerManager()->
}

unsigned long Backend::SendTo(const void *pData, DDWORD len, const char *sAddr, DDWORD port)
{
    return p_impl_->GetServerManager()->
}
*/

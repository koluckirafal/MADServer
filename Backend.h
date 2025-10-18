#ifndef _Backend_H
#define _Backend_H

#include <memory>
#include <string>
#include <vector>

class Backend
{
  private:
    unsigned long ProcessShellMessage(std::string &msg);
    unsigned long ProcessPacket(std::vector<unsigned char> &data, unsigned char src_addr[4], unsigned short src_port);
    class Impl_;
    Impl_ *p_impl_;
    class Callbacks_;
    Callbacks_ *server_cb_;

  public:
    Backend();
    ~Backend();
    void SetGameVar(const std::string key, std::string value);
    void SetGameVar(const std::string key, float value);
    void SetGameVar(const std::string key, int value);
    std::string GetGameVar(const std::string key, std::string init_value);
    int GetGameVar(const std::string key, int init_value);
    float GetGameVar(const std::string key, float init_value);
    unsigned long LoadConfigFile(std::string config_path);
    unsigned long SaveConfigFile(std::string config_path);
    char InitNetworking(void);
    /*
    --unsigned long RunConsoleString(char *pStr);   // internal?
    --unsigned long GetConsoleVar(char *pName, HCONSOLEVAR *hVar, char *pDefaultVal);
    --unsigned long GetVarValueFloat(HCONSOLEVAR hVar, float *val); // unused?
    --unsigned long GetVarValueString(HCONSOLEVAR hVar, char *pStr, DDWORD maxLen);
    unsigned long SendToServerShell(char *pInfo);
    char AddResources(char **pResources, DDWORD nResources);
    struct FileEntry_t* GetFileList(char *pDirName);
    void FreeFileList(struct FileEntry_t *pList);
    char StartWorld(StartGameRequest *pRequest);
    int	 GetNumClients();
    char GetClientName(int index, char *pName, int maxChars);
    char GetClientInfo(int index, ClientInfo* pInfo);
    char BootClient(DDWORD dwClientID);
    int GetErrorCode();
    void GetErrorString(char *pString, int maxLen);
    char Update(long flags);
    char GetServiceList(NetService *&pListHead);
    char FreeServiceList(NetService *pListHead);
    char SelectService(HNETSERVICE hNetService);
    char UpdateSessionName(const char* sName);
    char HostGame(NetHost* pHostInfo);
    char GetTcpIpAddress(char* sAddress, DDWORD dwBufferSize, unsigned short &hostPort);
    unsigned long SendTo(const void *pData, DDWORD len, const char *sAddr, DDWORD port);
    */
};

#endif // _Backend_H

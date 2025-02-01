#ifndef _MADSERVERIO_H
#define _MADSERVERIO_H

#include "server_interface.h"

class MADServerIO : public ServerAppHandler
{
public:
	DRESULT ShellMessageFn(char* pInfo);
	DRESULT ConsoleOutputFn(char* pInfo);
	DRESULT	OutOfMemory();
	DRESULT ProcessPacket(char *pData, DDWORD dataLen, DBYTE senderAddr[4], D_WORD senderPort);
};

#endif
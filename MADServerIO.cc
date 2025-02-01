#include <iostream>
#include "MADServerIO.h"

//#include "server_interface.h"

DRESULT MADServerIO::ShellMessageFn(char* pInfo)
{
	return LT_OK;
}

DRESULT MADServerIO::ConsoleOutputFn(char* pInfo)
{
	std::cout << "MADServerIO::ConsoleOutputFn: " << pInfo << std::endl;

	return LT_OK;
}

DRESULT MADServerIO::OutOfMemory()
{
    std::cout << "MADServerIO::OutOfMemory: lol not enough mem" << std::endl;
	return LT_OK;
}

DRESULT MADServerIO::ProcessPacket(char *pData, DDWORD dataLen, DBYTE senderAddr[4], D_WORD senderPort)
{
	return LT_OK;
}
#ifndef _UTILS_H
#define _UTILS_H

#include "MADServer.h"
#include <sstream>
#include <string>
#include <vector>

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

bool CheckRez(std::string name);
bool FindRezFiles(void);

int StringToInt(std::string &str);
float StringToFloat(std::string &str);
std::string IntToString(int &value);
std::string FloatToString(float &value);
std::vector<std::string> GetLevels(MADServer &server);
void SaveLevels(MADServer &server, std::vector<std::string> levels);

#endif // _UTILS_H
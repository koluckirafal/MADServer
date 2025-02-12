#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <sstream>

bool CheckRez(std::string name);
bool FindRezFiles(void);

int StringToInt(std::string &str);
float StringToFloat(std::string &str);
std::string IntToString(int &value);
std::string FloatToString(float &value);

#endif // _UTILS_H
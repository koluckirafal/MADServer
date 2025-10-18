#include "Backend.h"
#include "Logger.h"
#include "build.h"
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

bool CheckRez(std::string name)
{
    struct stat sb;
    std::string filename = name + ".rez";

    bool rez_exists = 0;

    if (stat(filename.c_str(), &sb) != -1)
    {
        if (S_ISREG(sb.st_mode) != 0)
        {
            rez_exists = 1;
        }
    }

    if (!rez_exists && (stat(name.c_str(), &sb) != -1))
    {
        if (S_ISDIR(sb.st_mode) != 0)
        {
            rez_exists = 1;
        }
    }

    if (!rez_exists)
    {
        LOG_ERROR << "Couldn't find resouce \"" << name << "\"";
    }

    return rez_exists;
}

bool FindRezFiles(void)
{
    bool all_ok = 1;

    if (all_ok)
        all_ok = CheckRez(LITH_GAME);
    if (all_ok)
        all_ok = CheckRez("sound");

    return all_ok;
}

int StringToInt(std::string &str)
{
    std::stringstream ss(str);
    int result;

    ss >> result;
    if (ss.fail())
    {
        LOG_ERROR << "Couldn't convert string \"" << str << "\" to int";
    }

    return result;
}

float StringToFloat(std::string &str)
{
    std::stringstream ss(str);
    float result;

    ss >> result;
    if (ss.fail())
    {
        LOG_ERROR << "Couldn't convert string \"" << str << "\" to int";
    }

    return result;
}

std::string IntToString(int value)
{
    std::stringstream ss;

    ss << value;

    return ss.str();
}

std::string IntToString(size_t value)
{
    std::stringstream ss;

    ss << value;

    return ss.str();
}

std::string FloatToString(float value)
{
    std::stringstream ss;

    ss << value;

    return ss.str();
}

std::vector<std::string> GetLevels(Backend &server)
{
    int count = 0;
    std::vector<std::string> levels;

    count = server.GetGameVar("NumLevels", 0);

    for (int i = 0; i < count; i++)
    {
        std::string level = server.GetGameVar("Level" + IntToString(i), "");

        if (!level.empty())
        {
            levels.push_back(level);
        }
    }

    return levels;
}

void SaveLevels(Backend &server, std::vector<std::string> &levels)
{
    for (size_t i = 0; i < levels.size(); i++)
    {
        const std::string key = "Level" + IntToString(i);
        server.SetGameVar(key, levels[i]);
    }

    server.SetGameVar("NumLevels", (int)(levels.size()));
}

std::vector<std::string> GetRezFiles(Backend &server)
{
    int count = 0;
    std::vector<std::string> rez_files;

    count = server.GetGameVar("NumRezFiles", 0);
    if (count < 0)
    {
        LOG_ERROR << "Negative number of REZ files?";
    }

    for (size_t i = 0; i < static_cast<size_t>(count); i++)
    {
        std::string rez_file = server.GetGameVar("RezFile" + IntToString(i), "");

        if (!rez_file.empty())
        {
            rez_files.push_back(rez_file);
        }
    }

    return rez_files;
}

void SaveRezList(Backend &server, std::vector<std::string> &rez_files)
{
    for (size_t i = 0; i < rez_files.size(); i++)
    {
        server.SetGameVar("RezFile" + IntToString(i), rez_files[i]);
    }

    server.SetGameVar("NumRezFiles", (int)(rez_files.size()));
}

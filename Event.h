#ifndef _EVENT_H
#define _EVENT_H

#include <netinet/in.h>
#include <string>
#include <vector>

struct Event
{
    enum Type
    {
        NONE,
        SHELL_MESSAGE,
        UNKNOWN_PACKET
    } type;

    // Payloads
    struct ShellMsg
    {
        std::string text;
    };

    struct Packet
    {
        std::vector<unsigned char> data;
        struct sockaddr_in sender;
    };

    // Can't put STL containers in POD union, so we're doing it this way
    ShellMsg shell;
    Packet packet;

    Event() : type(Event::NONE)
    {
    }

    static Event MakeShell(const std::string &msg)
    {
        Event e;
        e.type = SHELL_MESSAGE;
        e.shell.text = msg;
        return e;
    }

    static Event MakePacket(const std::vector<unsigned char> &data, const struct sockaddr_in sender)
    {
        Event e;
        e.type = UNKNOWN_PACKET;
        e.packet.data = data;
        e.packet.sender = sender;
        return e;
    }
};

#endif // _EVENT_H

#ifndef _MADSERVER_H
#define _MADSERVER_H

class MADServer
{
private:
    class Impl_;
    Impl_* p_impl_;
public:
    MADServer();
    ~MADServer();
};

#endif

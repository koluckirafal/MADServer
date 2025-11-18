#ifndef _EVENTQUEUE_H
#define _EVENTQUEUE_H

#include "Event.h"
#include <pthread.h>
#include <queue>

class EventQueue
{
  private:
    std::queue<Event> queue_;
    pthread_mutex_t mutex_;
    pthread_cond_t condition_;

  public:
    EventQueue()
    {
        pthread_mutex_init(&mutex_, 0);
        pthread_cond_init(&condition_, 0);
    }

    ~EventQueue()
    {
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&condition_);
    }

    void push(const Event &ev)
    {
        pthread_mutex_lock(&mutex_);
        queue_.push(ev);
        pthread_cond_signal(&condition_);
        pthread_mutex_unlock(&mutex_);
    }

    Event waitAndPop()
    {
        pthread_mutex_lock(&mutex_);
        while (queue_.empty())
        {
            pthread_cond_wait(&condition_, &mutex_);
        }
        Event ev = queue_.front();
        queue_.pop();
        pthread_mutex_unlock(&mutex_);
        return ev;
    }
};

#endif // _EVENTQUEUE_H

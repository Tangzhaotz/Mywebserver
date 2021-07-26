#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h> //epoll_ctl()
#include <fcntl.h>  // fcntl()
#include <unistd.h> // close()
#include <assert.h> // close()
#include <vector>
#include <errno.h>

class Epoller {
public:
    explicit Epoller(int maxEvent = 1024);  //构造函数，参数是表示最多的事件的数量

    ~Epoller();

    bool AddFd(int fd, uint32_t events);  //往epoll添加要检测的事件
 
    bool ModFd(int fd, uint32_t events);  //修改epoll里面的要检测的事件

    bool DelFd(int fd);  //删除要检测的事件

    int Wait(int timeoutMs = -1);  //等待检测，让内核去检测

    int GetEventFd(size_t i) const;  //获取事件的fd

    uint32_t GetEvents(size_t i) const;  
        
private:
    int epollFd_;   // epoll_create()创建一个epoll对象，返回值就是epollFd_

    std::vector<struct epoll_event> events_;     // 检测到的事件的集合,实际发生变化的事件的集合，最后需要返回给用户区
};

#endif //EPOLLER_H
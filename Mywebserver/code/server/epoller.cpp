#include "epoller.h"

// 创建epoll对象 epoll_create(512)
Epoller::Epoller(int maxEvent):epollFd_(epoll_create(512)), events_(maxEvent){  //events_表示返回的事件的集合
    assert(epollFd_ >= 0 && events_.size() > 0);
}

Epoller::~Epoller() {
    close(epollFd_);
}

// 添加文件描述符到epoll中进行管理
bool Epoller::AddFd(int fd, uint32_t events) {
    if(fd < 0) return false;
    epoll_event ev = {0};  //开始的时候将事件初始化为0
    /*
    struct epoll_event
    {
        uint32_t events;	// Epoll events 
        epoll_data_t data;	// User data variable
    }__EPOLL_PACKED;


    typedef union epoll_data
    {
        void *ptr;
        int fd;
        uint32_t u32;
        uint64_t u64;
    } epoll_data_t;


    */
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev);  //往内添加事件，添加成功返回0
}

// 修改
bool Epoller::ModFd(int fd, uint32_t events) {
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev);  //修改事件，修改成功返回0
}

// 删除
bool Epoller::DelFd(int fd) {
    if(fd < 0) return false;
    epoll_event ev = {0};
    return 0 == epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &ev);//删除成功返回0
}

// 调用epoll_wait()进行事件检测
int Epoller::Wait(int timeoutMs) {
    return epoll_wait(epollFd_, &events_[0], static_cast<int>(events_.size()), timeoutMs);  //timeouts是阻塞时间，该函数成功返回的是变化的文件描述符的个数
}

//当内核程序向用户返回信息的时候，不仅需要返回的是内核检测到的文件描述符的信息，同时还需要返回的是该文件描述符发生的事件的类型，具体是哪一类事件，
//所以需要下面的两个函数

// 获取产生事件的文件描述符
int Epoller::GetEventFd(size_t i) const {
    assert(i < events_.size() && i >= 0);
    return events_[i].data.fd;   //返回的是发生变化的文件描述符
}

// 获取事件
uint32_t Epoller::GetEvents(size_t i) const {
    assert(i < events_.size() && i >= 0);
    return events_[i].events;  //获取发生变化的文件描述符的事件
}
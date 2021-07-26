#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <sys/types.h>
#include <sys/uio.h>     // readv/writev
#include <arpa/inet.h>   // sockaddr_in
#include <stdlib.h>      // atoi()
#include <errno.h>      

#include "../log/log.h"
#include "../pool/sqlconnRAII.h"
#include "../buffer/buffer.h"
#include "httprequest.h"
#include "httpresponse.h"

// Http连接类，其中封装了请求和响应对象
class HttpConn {
public:
    HttpConn();  //构造函数

    ~HttpConn();  //析构函数

    void init(int sockFd, const sockaddr_in& addr);  //初始化，第一个参数为套接字，第二个参数为相应的地址

    ssize_t read(int* saveErrno);   //read函数，如果是ET模式的话，全部将数据读入到
 
    ssize_t write(int* saveErrno);  //写函数

    void Close();  //关闭

    int GetFd() const;   //获取文件描述符

    int GetPort() const;   //获取端口号

    const char* GetIP() const;   //获取IP地址
    
    sockaddr_in GetAddr() const;  //获取地址
    
    bool process();   //判断是否在处理过程中

    int ToWriteBytes() {    //计算可以写入的字节的长度
        return iov_[0].iov_len + iov_[1].iov_len; 
    }

    bool IsKeepAlive() const {   //判断是否保持活跃
        return request_.IsKeepAlive();
    }

    static bool isET;
    static const char* srcDir;  // 资源的目录
    static std::atomic<int> userCount; // 总共的客户单的连接数
    
private:
   
    int fd_;   //文件描述符
    struct  sockaddr_in addr_;   //套接字的地址结构体

    bool isClose_;   // 判断是否关闭连接
    
    int iovCnt_;    // 分散内存的数量
    struct iovec iov_[2];   // 分散内存
    
    Buffer readBuff_;   // 读(请求)缓冲区，保存请求数据的内容
    Buffer writeBuff_;  // 写(响应)缓冲区，保存响应数据的内容

    HttpRequest request_;   // 请求对象
    HttpResponse response_; // 响应对象
};


#endif //HTTP_CONN_H
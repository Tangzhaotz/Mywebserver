#ifndef BUFFER_H
#define BUFFER_H
#include <cstring>   //perror
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readv
#include <atomic>
#include <assert.h>
class Buffer {
public:
    Buffer(int initBuffSize = 1024);
    ~Buffer() = default;

    size_t WritableBytes() const;   //可写的字节数,size_t是一种记录大小的数字类型
    size_t ReadableBytes() const ;  //可读的字节数
    size_t PrependableBytes() const;  //可以追加的字节数

    const char* Peek() const;    //读取数据
    void EnsureWriteable(size_t len);  //确保缓冲中是否可以继续写数据，传入的参数len和剩余的空间位置做比较
    void HasWritten(size_t len);  //已经写完的数据，从开始写的位置加上写入的长度

    void Retrieve(size_t len);  //计算看是否可以继续读取数据
    void RetrieveUntil(const char* end);

    void RetrieveAll() ;  //初始化装数据的位置的vector，全部初始化为0
    std::string RetrieveAllToStr();

    const char* BeginWriteConst() const;
    char* BeginWrite();  //开始写数据

    //追加数据
    void Append(const std::string& str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    void Append(const Buffer& buff);

    ssize_t ReadFd(int fd, int* Errno); 
    ssize_t WriteFd(int fd, int* Errno);

private:
    char* BeginPtr_();              // 获取内存起始位置，是一根指针
    const char* BeginPtr_() const;  // 获取内存起始位置
    void MakeSpace_(size_t len);    // 创建空间，因为默认是1024的大小，装不下的时候就需要重新申请空间

    std::vector<char> buffer_;  // 具体装数据的vector
    std::atomic<std::size_t> readPos_;  // 读到的位置
    std::atomic<std::size_t> writePos_; // 写的位置
};

#endif //BUFFER_H
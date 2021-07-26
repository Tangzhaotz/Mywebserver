 #include "buffer.h"

//构造函数，初始化一个缓冲区，缓冲的大小就是1024字节，开始的时候读指针的位置和写指针的位置都是初始化为0，即开始的位置
Buffer::Buffer(int initBuffSize) : buffer_(initBuffSize), readPos_(0), writePos_(0) {}

// 可以读的数据的大小  写位置 - 读位置，中间的数据就是可以读的大小
size_t Buffer::ReadableBytes() const {  
    return writePos_ - readPos_;
}

// 可以写的数据大小，缓冲区的总大小 - 写位置
size_t Buffer::WritableBytes() const {
    return buffer_.size() - writePos_;
}

// 前面可以用的空间，当前读取到哪个位置，就是前面可以用的空间大小
size_t Buffer::PrependableBytes() const {  //读取完的内容就不再需要了，那么空间就可以腾出来
    return readPos_;
}

const char* Buffer::Peek() const {
    return BeginPtr_() + readPos_;  //返回的是开始的位置到读指针指向的位置，得到的是读取到的数据的长度
}

void Buffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());  //判断传入的要读取的数据的长度和可以读取的数据的长度的大小，如果要读的长度小于可读的长度的话就继续读取数据
    readPos_ += len;  //计算读取的数据的大小
}

//buff.RetrieveUntil(lineEnd + 2);
void Buffer::RetrieveUntil(const char* end) {
    assert(Peek() <= end );
    Retrieve(end - Peek());
}

void Buffer::RetrieveAll() {
    bzero(&buffer_[0], buffer_.size());
    readPos_ = 0;
    writePos_ = 0;
}

std::string Buffer::RetrieveAllToStr() {  //将读取到的数据转换为字符
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}

const char* Buffer::BeginWriteConst() const {  //开始写入数据
    return BeginPtr_() + writePos_;  //返回的是写入的数据的长度
}

char* Buffer::BeginWrite() {  // 开始写数据，返回的是开始指针的位置加上写完后写指针指向的位置
    return BeginPtr_() + writePos_;
}

void Buffer::HasWritten(size_t len) {
    writePos_ += len;  //开始写的位置加上写的长度
} 

void Buffer::Append(const std::string& str) {  //追加数据
    Append(str.data(), str.length());
}

void Buffer::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}

//  Append(buff, len - writable);   buff临时数组，len-writable是临时数组中的数据个数
void Buffer::Append(const char* str, size_t len) {
    assert(str);
    EnsureWriteable(len);  //判读可写的字节数与len的大小
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const Buffer& buff) {
    Append(buff.Peek(), buff.ReadableBytes());
}

void Buffer::EnsureWriteable(size_t len) {
    if(WritableBytes() < len) {  //如果可以写的字节数小于要写入的字节的长度，那么就扩充内存
        MakeSpace_(len);  //扩容
    }
    assert(WritableBytes() >= len);
}

ssize_t Buffer::ReadFd(int fd, int* saveErrno) {  //从文件描述符把数据写到缓冲区
    
    char buff[65535];   // 临时的数组，保证能够把所有的数据都读出来
    
    struct iovec iov[2];
    /*
    iovec结构体包含两个部分，一个是缓冲的地址，一个是缓冲的大小，上面创建的是两块缓存数组
    */
    const size_t writable = WritableBytes();  //计算可以写的数据的大小
    
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = BeginPtr_() + writePos_;  //数组的第一个指针指向的位置
    iov[0].iov_len = writable;  //数组的第一块缓冲可以读取的数据的大小
    iov[1].iov_base = buff;  //数组的第二块缓冲的起始位置
    iov[1].iov_len = sizeof(buff);  //数组的第二块缓冲的大小

    const ssize_t len = readv(fd, iov, 2);  //fd是文件描述符，iov是iovec数组，2表示的是需要浏览两个iovec结构体变量
    if(len < 0) {
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable) {
        writePos_ += len;   //因为要将数据加到内存中，所以先判断将要加入的数据是否小于可以写的字节数，如果小于的话就加入进去，同时更新写指针的位置
    }
    else {
        writePos_ = buffer_.size();  //说明已经满了，写不进去
        Append(buff, len - writable);   //第一块缓冲区的数据已经写满了，就继续向第二块缓冲区写数据，写入的数据的大小为总的长度减去刚才已经写入到第一块中的大小
    }
    return len;  //写完数据，返回的是写入的数据的大小
}

ssize_t Buffer::WriteFd(int fd, int* saveErrno) {  //从缓冲区读取数据
    size_t readSize = ReadableBytes();  //计算可以读取的数据的大小
    ssize_t len = write(fd, Peek(), readSize);  //fd表示的是文件描述符，peek（）表示的是读指针的位置，第三个参数表示的是读的数据的大小
    if(len < 0) {
        *saveErrno = errno;
        return len;
    } 
    readPos_ += len;  //读取到数据，读指针的位置改变
    return len;
}

char* Buffer::BeginPtr_() {
    return &*buffer_.begin();
}

const char* Buffer::BeginPtr_() const {
    return &*buffer_.begin();
}

void Buffer::MakeSpace_(size_t len) {
    if(WritableBytes() + PrependableBytes() < len) {  //判断前面空出来的空间和后面的剩下的空间的总和和字节的长度作比较
        buffer_.resize(writePos_ + len + 1);  //不够装的话，重新调整大小
    } 
    else {  //剩余的空间总和够装的话
        size_t readable = ReadableBytes();  //计算可以读的数据的大小
        std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());  //将当前要读的数据拷贝到最前面，因为前面读完的数据留下空间，这样就可以实现添加
        readPos_ = 0;
        writePos_ = readPos_ + readable;  //获取写指针的位置
        assert(readable == ReadableBytes());
    }
}
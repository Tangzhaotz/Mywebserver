#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>     
#include <mysql/mysql.h>  //mysql

#include "../buffer/buffer.h"
#include "../log/log.h"
#include "../pool/sqlconnpool.h"
#include "../pool/sqlconnRAII.h"

class HttpRequest {
public:
    enum PARSE_STATE {   //请求的状态信息
        REQUEST_LINE,   // 正在解析请求首行
        HEADERS,        // 头
        BODY,           // 体
        FINISH,         // 完成
    };

    enum HTTP_CODE {  //HTTP请求得状态码
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };
    
    HttpRequest() { Init(); }   //构造函数，请求初始化
    ~HttpRequest() = default;    //析构函数

    void Init();  //初始化
    bool parse(Buffer& buff);  //是否解析

    std::string path() const;   //路径
    std::string& path();
    std::string method() const;  //方法
    std::string version() const;  //版本号
    std::string GetPost(const std::string& key) const;
    std::string GetPost(const char* key) const;

    bool IsKeepAlive() const;  //是否保持连接

private:
    bool ParseRequestLine_(const std::string& line);   //解析请求行
    void ParseHeader_(const std::string& line);   //解析请求头
    void ParseBody_(const std::string& line);  //解析请求体

    void ParsePath_();
    void ParsePost_();
    void ParseFromUrlencoded_();

    static bool UserVerify(const std::string& name, const std::string& pwd, bool isLogin);

    PARSE_STATE state_;     // 解析的状态
    std::string method_, path_, version_, body_;    // 请求方法，请求路径，协议版本，请求体
    std::unordered_map<std::string, std::string> header_;   // 请求头
    std::unordered_map<std::string, std::string> post_;     // post请求表单数据

    static const std::unordered_set<std::string> DEFAULT_HTML;  // 默认的网页
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG; 
    static int ConverHex(char ch);  // 将十六进制字符转换成十进制整数
};


#endif //HTTP_REQUEST_H
# Mywebserver
用C++实现的高性能WEB服务器,经过压力测试可以实现上千的QPS（因为使用的是阿里云的服务器，所以配置可能低一点）
# 功能
* 利用IO复用技术Epoll与线程池实现多线程的Reactor高并发模型；

* 利用正则与状态机解析HTTP请求报文，实现处理静态资源的请求；

* 利用标准库容器封装char，实现自动增长的缓冲区；

* 基于小根堆实现的定时器，关闭超时的非活动连接；

* 利用单例模式与阻塞队列实现异步的日志系统，记录服务器运行状态；

* 利用RAII机制实现了数据库连接池，减少数据库连接建立与关闭的开销，同时实现了用户注册登录功能。
# 环境要求:
* Linux系统,本项目主要是在ubuntu18.04上面运行和测试,可以自行安装虚拟机或者双系统或者购买服务器都可以.
* MySQL数据库
* C++
# 目录树
```
.
├── bin
│   └── server
├── build
│   └── Makefile
├── code
│   ├── buffer
│   │   ├── buffer.cpp
│   │   └── buffer.h
│   ├── config
│   │   └── config.h
│   ├── http
│   │   ├── httpconn.cpp
│   │   ├── httpconn.h
│   │   ├── httprequest.cpp
│   │   ├── httprequest.h
│   │   ├── httpresponse.cpp
│   │   └── httpresponse.h
│   ├── log
│   │   ├── blockqueue.h
│   │   ├── log.cpp
│   │   └── log.h
│   ├── main.cpp
│   ├── pool
│   │   ├── sqlconnpool.cpp
│   │   ├── sqlconnpool.h
│   │   ├── sqlconnRAII.h
│   │   └── threadpool.h
│   ├── server
│   │   ├── epoller.cpp
│   │   ├── epoller.h
│   │   ├── webserver.cpp
│   │   └── webserver.h
│   └── timer
│       ├── heaptimer.cpp
│       └── heaptimer.h
├── LICENSE
├── log
│   └── 2021_07_26.log
├── Makefile
├── readme.assest
│   └── 压力测试.png
├── readme.md
├── resources
│   ├── 400.html
│   ├── 403.html
│   ├── 404.html
│   ├── 405.html
│   ├── css
│   │   ├── animate.css
│   │   ├── bootstrap.min.css
│   │   ├── font-awesome.min.css
│   │   ├── magnific-popup.css
│   │   └── style.css
│   ├── error.html
│   ├── fonts
│   │   ├── FontAwesome.otf
│   │   ├── fontawesome-webfont.eot
│   │   ├── fontawesome-webfont.svg
│   │   ├── fontawesome-webfont.ttf
│   │   ├── fontawesome-webfont.woff
│   │   └── fontawesome-webfont.woff2
│   ├── images
│   │   ├── favicon.ico
│   │   ├── instagram-image1.jpg
│   │   ├── instagram-image2.jpg
│   │   ├── instagram-image3.jpg
│   │   ├── instagram-image4.jpg
│   │   ├── instagram-image5.jpg
│   │   └── profile-image.jpg
│   ├── index.html
│   ├── js
│   │   ├── bootstrap.min.js
│   │   ├── custom.js
│   │   ├── jquery.js
│   │   ├── jquery.magnific-popup.min.js
│   │   ├── magnific-popup-options.js
│   │   ├── smoothscroll.js
│   │   └── wow.min.js
│   ├── login.html
│   ├── picture.html
│   ├── register.html
│   ├── video
│   │   └── xxx.mp4
│   ├── video.html
│   └── welcome.html
├── test
│   ├── Makefile
│   ├── readme.md
│   ├── test
│   ├── test.cpp
│   ├── testlog1
│   │   ├── 2021_07_26-1.log
│   │   └── 2021_07_26.log
│   ├── testlog2
│   │   ├── 2021_07_26-1.log
│   │   └── 2021_07_26.log
│   └── testThreadpool
│       ├── 2021_07_26-1.log
│       ├── 2021_07_26-2.log
│       ├── 2021_07_26-3.log
│       └── 2021_07_26.log
└── webbench-1.5
    ├── Makefile
    ├── socket.c
    ├── webbench
    ├── webbench.c
    └── webbench.o


```

# 项目启动
进入数据库,开启mysql
![image](https://user-images.githubusercontent.com/53050401/126958943-e5bee91a-9b2a-49f5-a0c2-b56bc777b11e.png)

```bash
// 建立yourdb库
create database yourdb;

// 创建user表
USE yourdb;
CREATE TABLE user(
    username char(50) NULL,
    password char(50) NULL
)ENGINE=InnoDB;

// 添加数据
INSERT INTO user(username, password) VALUES('name', 'password');
```
```bash
make
![image](https://user-images.githubusercontent.com/53050401/126959218-ae11f833-4d4a-4312-b84c-3f22b334f861.png)

./bin/server
![image](https://user-images.githubusercontent.com/53050401/126959293-c045eb15-9781-490d-8cc3-a431a293c540.png)

```

## 单元测试
```bash
cd test
make
./test
```

## 压力测试
```
./webbench-1.5/webbench -c 100 -t 10 http://ip:port/
./webbench-1.5/webbench -c 1000 -t 10 http://ip:port/
./webbench-1.5/webbench -c 5000 -t 10 http://ip:port/
./webbench-1.5/webbench -c 10000 -t 10 http://ip:port/
```
## 测试环境
阿里云服务器1核2GB
QPS:上千

# 访问服务器
* 阿里云开启相应的端口,设置数据库的端口,用户名和密码.并开启数据库
![image](https://user-images.githubusercontent.com/53050401/126961041-eeb582c8-6082-4922-ba0e-a38ede73afdb.png)
* 通过端口和ip地址访问
![image](https://user-images.githubusercontent.com/53050401/126961112-5815b740-3655-404a-8b9d-59da3491ede0.png)
* 注册用户![image](https://user-images.githubusercontent.com/53050401/126961416-5497f143-5435-4cfc-918c-0fe43fc26a57.png)
数据库中可以显示注册的用户信息
![image](https://user-images.githubusercontent.com/53050401/126961453-73f1e9d8-b010-46c2-88ea-80ffca1ce71d.png)
# TODO
...


 

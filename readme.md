# My Analytics

2020 春季学期 XMU 信息学院 《Linux 使用与编程入门》课程期末设计 ，使用 C++ 编写的基于 CGI 的网页访问统计。

![result.png](https://i.loli.net/2020/06/09/LXIdOAjrG9JpE5c.png)

## 需求

>经过学习，我们可以为 Apache (or Nginx) 等 Web 服务器写一个网页计数器的 CGI 程序。  
可是，学海无涯，网页计数器可以更进一步，演变为网站统计工具，如同百度统计、Google Analytics 一般，能对访问网站的用户进行分类统计，了解来自某个地区的、某个操作系统的、某个浏览器的用户是多少等功能。  
安装 Linux 虚拟机中并搭建 Web 服务器运行环境（或基于 Docker 搭建环境），使用 C/C++ 实现上述的网站统计工具，并撰写任务报告。  
首先，任务报告内容包含从安装虚拟机、Web 服务器（或 Docker 环境），到网站统计工具的设计方案、实现过程、效果演示等。  
其次，C/C++ 的代码应该：  
- 实现上述的网站统计工具的功能；
- 并遵守 Makefile 的代码规范。
- 使用到自己编写的数据结构的相关库

调用别人写好的成熟的cgic的库，例如：cgic library by Thomas Boutell

## 说明 & 特性

- 基于 C++ & CGI 编写
- 环境使用 Docker 部署
- 支持多站点、多域名统计
- IP 地区库使用 ipip.net 的[离线数据库](https://www.ipip.net/product/ip.html#ipv4city)
- 提供 RESTful API 接口
- 界面美观（？）
- <s>应付期末作业而已还要什么特性</s>

## 技术栈

### 运行环境

- Linux (在 Ubuntu 18.04 和 ArchLinux 下测试)
- Docker
- Apache 2 (bitnami/apache)
- MySQL (MariaDB)

### 后端: C++11 & CGI

- CGI 环境：[boutell/cgic](https://github.com/boutell/cgic)
- JSON 解析：[tencent/rapidjson](https://github.com/tencent/rapidjson)
- 数据库操作（自封装）：[kirainmoe/ksm](https://github.com/kirainmoe/my-analytics/tree/master/lib/mysql)

### 前端

- 框架： React & create-react-app
- 组件库： Ant Design
- 表格库： ECharts

## 部署

### 安装依赖

需要 Docker、docker-compose 和 libmysqlclient-dev.

#### 安装 Docker 和 docker-compose

```bash
#  配置从 https 安装
sudo dpkg --configure -a
sudo apt-get install apt-transport-https ca-certificates curl software-properties-common

# 下载 docker 的 apt-key
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -

# 添加 docker for ubuntu 的软件源
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"

# 更新 apt 缓存
sudo apt-get update

# 安装 docker 社区版和 docker-compose
sudo apt-get install docker-ce docker-compose

# 启动 Docker
systemctl start docker
```

#### 下载源码并生成

```bash
# 克隆源码
git clone https://github.com/kirainmoe/my-analytics

# 进入目录
cd my-analytics

# 编译并生成
make deploy
```

#### 运行

在源码目录下，直接使用 `docker-compose` 运行：

```bash
sudo docker-compose up -d
```

## 统计

以下假设 `my-analytics` 被部署在 `http://localhost`.

### 初始化数据库

部署完成后，首先访问 `http://localhost/cgi-bin/init_database.cgi` 建立数据库和数据表结构。当看到如下图所示的结果时即初始化完成。

![create-db.png](https://i.loli.net/2020/06/09/haKwVk7yC3f9rli.png)

### 网页访问统计

将以下代码插入到你要统计访问量的网页中：

```html
<script type="text/javascript" src="http://localhost/cgi-bin/inject_script.cgi"></script>
```

这样当用户访问该网页的时候，会自动加载该 JS 并向 `http://localhost/cgi-bin/record.cgi` 发送请求，后端会记录请求内容到数据库。

### 查看访问报告

访问 `http://localhost?domain={domain}` 来查看统计报告，其中 `{domain}` 是你要统计的域名。

#### RESTful API

GET `http://localhost/cgi-bin/analytics.cgi`

| param | description |
|-------|-------------|
| domain | 要统计的域名 |



## License

MIT Licensed.
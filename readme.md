# My Analytics

2020 春季学期 XMU 信息学院 《Linux 使用与编程入门》课程期末设计 ，使用 C++ 编写的基于 CGI 的网页访问统计。

## 技术栈

### 后端: C++ & CGI

- [boutell/cgic](https://github.com/boutell/cgic)
- [tencent/rapidjson](https://github.com/tencent/rapidjson)
- libmysqlclient
- Docker

### 前端

- React
- ECharts

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
git clone https://github.com/kirainmoe/my-analytics
cd my-analytics
make deploy
```

#### 运行

在源码目录下，直接使用 `docker-compose` 运行：

```bash
sudo docker-compose up -d
```
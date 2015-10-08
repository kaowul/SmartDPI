# 项目介绍

AP设备以UDP向平台上报用户上网行为，包括URL，客户端mac，设备类型等信息

# 目录结构介绍
* asset: 项目所需附件
* nginx: nginx中间件，采用透明代理时使用
* readfifo: 项目核心，用于读取管道内容，解析及行为上报
* xtstring: netfilter内核模块，用于向管道文件发送上网记录

# 依赖：
* wifidog register模块: 用于生成wifidog.conf文件
* wifidog smartwifi模块: 可对本进程进行启动或关闭
* luci模块:读取ssid
* nginx(可选):透明代理方式读取url
* netiflter内核模块(可选)：内核模块方式读取url

# 安装(采用透明代理方式模拟)：

## openwrt编译readfifo并安装
* make package/readfifo/compile V=99
* opkg install readfifo.ipk

## openwrt编译Nginx并安装：
* make package/nginx/compile V=99
* opkg install nginx.ipk


# 示例(采用透明代理方式模拟)：

## 执行注册，生成wifidog.conf至/etc目录下

## 启动透明代理：
* 关闭80端口占用 /etc/init.d/uhttpd stop
* mkdir /var/log/nginx
* mkdir /var/lib/nginx
* /etc/init.d/nginx start

## 开启透明代理：
* 修改nginx.conf配置：参见asset目录下nginx.conf
* 将80端口流量发往8000：
* iptables –t nat –A PREROUTING –i br-lan –p tcp --src 192.168.1.0/24 --dport 80 –j DNAT --to 192.168.1.1:8000

## 将上网行为LOG输出到管道
* 建立管道 mkfifo /tmp/tp
* 将上网行为LOG输出到管道 tail -f /var/log/nginx/access.log > /tmp/tp


## 流量上报
执行readfifo，将从wifidog.conf读取平台地址，并完成行为上报

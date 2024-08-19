# etc

在Linux系统中，/etc目录是用来存放所有的系统管理所需要的配置文件和子目录。它基本上包含所有硬件和软件配置文件。

一些主要的文件和目录包括：

- /etc/resolv.conf：这是DNS配置文件，用于在网卡配置文件中进行配置。
- /etc/hostname：配置主机名。
- /etc/hosts：存储ip与域名对应的关系，用于解析域名（主机名），可以用/etc/hosts搭建网站的测试环境（虚拟机）。
- /etc/fstab：文件系统挂载表，开机的时候设备与入口对应关系开机自动挂载列表。
- /etc/rc.local：开机自启。
- /etc/inittab：运行级别的配置文件。
- /etc/profile：环境变量配置文件。
- /etc/bashrc：命令别名。
- /etc/motd：文件中的内容会在用户登录系统之后显示出来。
- /etc/issue和/etc/issue.net：文件中的内容会在用户登录系统之前显示出来

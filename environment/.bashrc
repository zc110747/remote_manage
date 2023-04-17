
################################################################################
# This file is parsed by bash whenever a new bash shell is opened; its contents
# configure the shell environment.
#
################################################################################

#项目私有配置，需要在项目下载后修改为本地配置
export ENV_PATH_ROOT="/mnt/d/user_project/git/remote_manage"
export ENV_KERNEL_DIR="/home/center/application/project/linux"

export EMBED_DEVICE_IPADDRESS="192.168.2.99"

#构建项目内部目录，除非项目结构更改，否则固定
export ENV_PATH_LOWER_APP="$ENV_PATH_ROOT/lower_app"
export ENV_PATH_LOWER_APP_MANAGE="$ENV_PATH_LOWER_APP/manage"
export ENV_PATH_LOWER_APP_SERVER="$ENV_PATH_LOWER_APP/server"
export ENV_PATH_UPPER_APP="$ENV_PATH_ROOT/upper_app"
export ENV_PATH_PACKAGE="$ENV_PATH_ROOT/package"
export ENV_PATH_KERNAL_MOD="$ENV_PATH_ROOT/kernal_mod"


#alias global define
alias ll="ls -alF"

#定义项目内不同代码路径
alias cdl="cd $ENV_PATH_LOWER_APP"
alias cdlm="cd $ENV_PATH_LOWER_APP_MANAGE"
alias cdls="cd $ENV_PATH_LOWER_APP_SERVER"

#编译项目功能实现
function SysBuildFirmware()
{
	echo frimware path:$ENV_PATH_ROOT
	echo kernal dir:$ENV_KERNEL_DIR

	#asio库以压缩包形式进行上传,首次使用需要解压
	if [ ! -d "$ENV_PATH_LOWER_APP_MANAGE/lib/aiso" ]; then
		cd $ENV_PATH_LOWER_APP_MANAGE/lib/
		tar -xvf asio.tar.bz2
		echo "create asio library for manage sucess"
	else
		echo "already exist asio, can build"
	fi

	cd $ENV_PATH_ROOT
	if [ -d "$ENV_KERNEL_DIR" ]; then
		echo "start bulid kernal driver"
		cd $ENV_PATH_KERNAL_MOD/led/
		make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
		mv kernal_led.ko ../../package/driver/
		make clean

		cd $ENV_PATH_KERNAL_MOD/beep/
		make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
		mv kernal_beep.ko ../../package/driver/
		make clean

		cd $ENV_PATH_KERNAL_MOD/i2c_ap/
		make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
		mv kernal_i2c_ap.ko ../../package/driver/
		make clean

		cd $ENV_PATH_KERNAL_MOD/spi_icm/
		make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
		mv kernal_spi_icm.ko ../../package/driver/
		make clean

		cd $ENV_PATH_KERNAL_MOD/key/
		make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
		mv kernal_key.ko ../../package/driver/
		make clean
	else
		echo "without linux kernal, not build kernal module"
	fi

	#覆盖配置文件
	cp -f $ENV_PATH_PACKAGE/config.json $ENV_PATH_LOWER_APP_MANAGE/config.json
	cp -f $ENV_PATH_PACKAGE/config.json $ENV_PATH_LOWER_APP_SERVER/config.json

	#编译程序
	cd $ENV_PATH_LOWER_APP_MANAGE
	make -j4
	cd $ENV_PATH_ROOT
	cp lower_app/Executables/app_demo package/

	#打包服务器程序(包含node的后台和前端页面),拷贝到输出目录
	cd lower_app/
	tar -vcjf server.tar.bz2 server/ 
	cd ../
	mv lower_app/server.tar.bz2 package/

	#将打包后的系统通过ssh提交到服务器中
	cd package/
	tar -vcjf remote_manage.tar.bz2 *
	scp -r remote_manage.tar.bz2 root@$EMBED_DEVICE_IPADDRESS:/tmp/
	rm remote_manage.tar.bz2 server.tar.bz2
}

alias SysBuildFirmware='SysBuildFirmware'

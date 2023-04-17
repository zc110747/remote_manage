################################################################################
# This file is parsed by bash whenever a new bash shell is opened; its contents
# configure the shell environment.
#
################################################################################

echo
echo "Loading CDE Plugin..."
echo "-------------------------------------------------------------------------"
echo "Load Plugin Success!"
echo "Can use command 'SysHelpCommand' for more helps."
echo "Update the Plugin by filepath /home/[root]/.bashrc."
echo "-------------------------------------------------------------------------"

#-------------------------------------------------------------------------------
# This section defines the private config for the program.
# if download the package, first modify when download.
#-------------------------------------------------------------------------------
export ENV_PATH_ROOT="/mnt/d/user_project/git/remote_manage"
export ENV_KERNEL_DIR="/home/center/application/project/linux"
export EMBED_DEVICE_IPADDRESS="192.168.2.99"
export FMWARE_VESRION="1.0.0.7"

#-------------------------------------------------------------------------------
# This section defines the compiler and compiler config.
# Only Change if need.
#-------------------------------------------------------------------------------
export COMPILE_APPLICATION=arm-linux-gnueabihf-
export LIB_APPLICATION=arm-

#-------------------------------------------------------------------------------
# This section defines the path for the program
# Not need Change unless program change.
#-------------------------------------------------------------------------------
export ENV_PATH_LOWER_APP="$ENV_PATH_ROOT/lower_app"
export ENV_PATH_LOWER_APP_MANAGE="$ENV_PATH_LOWER_APP/manage"
export ENV_PATH_LOWER_APP_SERVER="$ENV_PATH_LOWER_APP/server"
export ENV_PATH_UPPER_APP="$ENV_PATH_ROOT/upper_app"
export ENV_PATH_PACKAGE="$ENV_PATH_ROOT/package"
export ENV_PATH_KERNAL_MOD="$ENV_PATH_ROOT/kernal_mod"

#-------------------------------------------------------------------------------
# This section defines the alias of the command.
# The command abbreviation for use.
#-------------------------------------------------------------------------------
alias ll="ls -alF"
alias cdr="cd $ENV_PATH_ROOT"
alias cdl="cd $ENV_PATH_LOWER_APP"
alias cdlm="cd $ENV_PATH_LOWER_APP_MANAGE"
alias cdls="cd $ENV_PATH_LOWER_APP_SERVER"

#-------------------------------------------------------------------------------
# This section defines the function for program management.
# The Extend Command for program compiler, package, push.
#-------------------------------------------------------------------------------
function SysBuildApplication()
{
	echo frimware path:$ENV_PATH_ROOT
	echo kernal dir:$ENV_KERNEL_DIR

	#asio库以压缩包形式进行上传,首次使用需要解压
	if [ ! -d "$ENV_PATH_LOWER_APP_MANAGE/lib/asio" ]; then
		cd $ENV_PATH_LOWER_APP_MANAGE/lib/
		tar -xvf asio.tar.bz2
		echo "create asio library for manage sucess"
	else
		echo "already exist asio, can build"
	fi

	#编译驱动地址
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
	tar -vcjf remote_manage_v$FMWARE_VESRION.tar.bz2 *
	rm server.tar.bz2

	#回到项目的地址
	cdr
}

alias SysBuildApplication='SysBuildApplication'

#推送项目到嵌入式软件端
function SysPushFirmware()
{
	echo filepath:$ENV_PATH_PACKAGE, ipaddress:$EMBED_DEVICE_IPADDRESS
	scp -r $ENV_PATH_PACKAGE/remote_manage_v$FMWARE_VESRION.tar.bz2 root@$EMBED_DEVICE_IPADDRESS:/tmp/
}
alias SysPushFirmware='SysPushFirmware'

function SysCleanFirmware()
{
	rm $ENV_PATH_PACKAGE/remote_manage_v$FMWARE_VESRION.tar.bz2
	cd $ENV_PATH_LOWER_APP_MANAGE
	make clean

	#回到项目的地址
	cdr
}
alias SysCleanFirmware='SysCleanFirmware'

#显示帮助信息
function SysHelpCommand()
{
	echo 'SysBuildApplication'
	echo '    Build the Application, package firmware.'
	echo 'SysPushFirmware'
	echo '    Push the frimware to remote embed device.'
	echo 'SysHelpCommand'
	echo '    Show the help command.'
}
alias SysHelpCommand='SysHelpCommand'
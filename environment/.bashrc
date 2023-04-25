################################################################################
# This file is parsed by bash whenever a new bash shell is opened; its contents
# configure the shell environment.
# 在使用该文件时，仅修改ENV_PATH_ROOT到项目下载目录，然后复制到根目录下即可。
# 根目录地址为 '/home/[用户名]'
################################################################################

echo
echo "Loading CDE Plugin..."
echo "-------------------------------------------------------------------------"
echo "Load Plugin Success!"
echo "Can use command 'SysHelpCommand' for more helps."
echo "Update the Plugin by filepath /home/[root]/.bashrc."

#-------------------------------------------------------------------------------
# This section defines the private config for the program.
# if download the package, first modify when download.
#-------------------------------------------------------------------------------
export ENV_PATH_ROOT="/home/center/application/remote_manage/"
if [ ! -d "$ENV_PATH_ROOT" ]; then 
    export ENV_PATH_ROOT="/mnt/d/user_project/git/remote_manage"
fi

echo "Root Path:$ENV_PATH_ROOT"
export ENV_PATH_ENVIRONMENT="$ENV_PATH_ROOT/environment"

#-------------------------------------------------------------------------------
# execute the script to init global variable
#-------------------------------------------------------------------------------
if [ -f "$ENV_PATH_ENVIRONMENT/defineEnvVars" ]; then
    source $ENV_PATH_ENVIRONMENT/defineEnvVars
fi

#-------------------------------------------------------------------------------
# execute the script to init global command
#-------------------------------------------------------------------------------
if [ -f "$ENV_PATH_ENVIRONMENT/defineAlias" ]; then
    source $ENV_PATH_ENVIRONMENT/defineAlias
fi

#-------------------------------------------------------------------------------
# end of the bashrc.
#-------------------------------------------------------------------------------
echo "-------------------------------------------------------------------------"
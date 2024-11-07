# boot script file

echo "boot script loaded!"

setenv serverip "192.168.2.29" 
setenv ipaddr "192.168.2.99"
setenv gateway "192.168.2.1"
setenv netmask "255.255.255.0" 
setenv nfspath "/home/freedom/Desktop/user_git/sdk/nfs"
setenv bootargs "console=tty1 console=ttymxc0,115200 root=/dev/nfs nfsroot=${serverip}:${nfspath},proto=tcp,nfsvers=3 rw ip=${ipaddr}:${serverip}:${gateway}:${netmask}::eth0:off earlyprintk"
setenv netboot_cmd "tftp 80800000 zImage; tftp 83000000 ${fdt_file}; bootz 80800000 - 83000000;"

echo "boot script execute success!"

run netboot_cmd
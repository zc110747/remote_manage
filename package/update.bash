
#rm the application
rm -rf app_demo driver/ server/ startApp *.md *.json
rm -rf remote_manage* server.tar.bz2

#move data to current path
mv /tmp/remote_manage* ./

#tar the package
tar -xvf remote_manage*.tar.bz2
tar -xvf server.tar.bz2
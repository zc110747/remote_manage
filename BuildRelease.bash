
#build manage and move to demo
cd lower_app/manage/
make
cd ../../
cp lower_app/Executables/app_demo demo/

#cp 
cd lower_app/
tar -vcjf server.tar.bz2 server/ 
cd ../
mv lower_app/server.tar.bz2 demo/

cd demo/
scp -r app_demo server.tar.bz2 root@192.168.2.99:/tmp/

common_path	= $(ENV_PATH_ROOT)/embed/common

#cpp 
cpp_objects	+= 	${common_path}/fifo/fifo_manage.o
cpp_objects += 	${common_path}/jsonconfig/jsonconfig.o 
cpp_objects += 	${common_path}/logger/logger_server.o	
cpp_objects	+= 	${common_path}/server/asio_server.o
cpp_objects	+= 	${common_path}/calculate.o 
cpp_objects	+= 	${common_path}/time_manage.o 

#include
INCLUDES += -I ${common_path}/
INCLUDES += -I ${common_path}/event/
INCLUDES += -I ${common_path}/fifo/
INCLUDES += -I ${common_path}/jsonconfig/
INCLUDES += -I ${common_path}/logger/
INCLUDES += -I ${common_path}/server/

common_path=$(ENV_PATH_ROOT)/embed/common

cpp_objects += 	${common_path}/jsonconfig.o \
				${common_path}/logger.o	\
				${common_path}/fifo_manage.o \
				${common_path}/time_manage.o \
				${common_path}/calculate.o \
				${common_path}/asio_server.o

INCLUDES += -I ${common_path}/
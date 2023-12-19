
#include "mqtt_process.hpp"

mqtt_process::mqtt_process(const mqtt_info &info) 
:mosquittopp(info.id.c_str())
{
	info_ = info;
};

mqtt_process::~mqtt_process()
{
}

void mqtt_process::mqtt_run()
{
	mosqpp::lib_init();

	PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Mqtt start run!");
	connect(info_.host.c_str(), info_.port, info_.keepalive);

	while(1)
	{
		loop_forever();
	}

	mosqpp::lib_cleanup();
}

bool mqtt_process::start()
{
	thread_ = std::thread(std::bind(&mqtt_process::mqtt_run, this));
    thread_.detach();

	return true;
}

void mqtt_process::on_connect(int rc)
{
	PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Connected with code:%d!", rc);
	if(rc == 0)
	{
		/* Only attempt to subscribe on a successful connect. */
		subscribe(NULL, info_.sub_topic.c_str(), info_.qos);
		is_connet_ = true;
	}
}

void mqtt_process::on_message(const struct mosquitto_message *message)
{
	double temp_celsius, temp_fahrenheit;
	char buf[51];

	if(!strcmp(message->topic, info_.sub_topic.c_str()))
	{
		memset(buf, 0, sizeof(buf));
		/* Copy N-1 bytes to ensure always 0 terminated. */
		memcpy(buf, message->payload, message->payloadlen);
		buf[message->payloadlen] = '\0';
		PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "rx_buffer:%s.", buf);
	}
}

void mqtt_process::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "subscription succeeded!");
}

int mqtt_process::publish_msg(const std::string &str)
{
	int ret = 0;

	if(is_connet_ && str.length() > 0)
	{
		ret = publish(NULL, info_.pub_topic.c_str(), str.length(), str.c_str(), info_.qos);
		PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "publisher, topic:%s, ret:%d!", info_.pub_topic.c_str(), ret);
	}
	
	return ret;
}

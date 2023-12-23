
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

	PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "Mqtt start run!");
	connect(info_.host.c_str(), info_.port, info_.keepalive);

	while (1)
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
	PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "Connected with code:%d!", rc);
	if (rc == 0)
	{
		/* Only attempt to subscribe on a successful connect. */
		subscribe(NULL, info_.sub_topic.c_str(), info_.qos);
		is_connet_ = true;
	}
}

void mqtt_process::on_message(const struct mosquitto_message *message)
{
	char buf[51];

	if (!strcmp(message->topic, info_.sub_topic.c_str()))
	{
		memset(buf, 0, sizeof(buf));
		/* Copy N-1 bytes to ensure always 0 terminated. */
		memcpy(buf, message->payload, message->payloadlen);
		buf[message->payloadlen] = '\0';
		PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "rx_buffer:%s.", buf);
	}
}

void mqtt_process::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "subscription topic:%s, succeeded!", info_.sub_topic.c_str());
}

int mqtt_process::publish_msg(const std::string &str)
{
	int ret = 0;

	if (is_connet_ && str.length() > 0)
	{
		ret = publish(NULL, info_.pub_topic.c_str(), str.length(), str.c_str(), info_.qos);
		PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "publisher, topic:%s, ret:%d!", info_.pub_topic.c_str(), ret);
	}
	
	return ret;
}

static std::unique_ptr<mqtt_process> mqtt_process_ptr{nullptr};

bool mqtt_init(void)
{
	bool ret = false;
	try
	{
		const MqttDeivceInfo mqtt_config = system_config::get_instance()->get_mqtt_config();

		//mqtt subscribe init
		mqtt_info mqtt_process_info = {
			id:mqtt_config.id,
			host:system_config::get_instance()->get_ipaddress(),
			port:mqtt_config.port,
			sub_topic:mqtt_config.sub_topic,
			pub_topic:mqtt_config.pub_topic,
			keepalive:mqtt_config.keepalive,
			qos:mqtt_config.qos
		};
		mqtt_process_ptr = std::make_unique<mqtt_process>(mqtt_process_info);
		ret = mqtt_process_ptr->start();

		return ret;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}
}

int mqtt_publish(const std::string &str)
{
	int ret = -1;

	if (mqtt_process_ptr != nullptr)
	{
		ret = mqtt_process_ptr->publish_msg(str);
	}
	return ret;
}

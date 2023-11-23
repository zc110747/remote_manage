_Pragma("once")

#include <mosquittopp.h>
#include "common_unit.hpp"

typedef struct 
{
	std::string id;
	std::string host;
	int port;
	std::string sub_topic;
	std::string pub_topic;
	int keepalive{60};
	int qos{1};
}mqtt_info;

class mqtt_process : public mosqpp::mosquittopp
{
public:
	mqtt_process(const mqtt_info &info);
	~mqtt_process();

	void on_connect(int rc);
	void on_message(const struct mosquitto_message *message);
	void on_subscribe(int mid, int qos_count, const int *granted_qos);
	int publish_msg(const std::string &str);
	bool start();

private:
	mqtt_info info_;

	/// \brief is_connet_
    /// - wheather mqtt success connect
	bool is_connet_{false};

	/// \brief thread_
    /// - mqtt run thread object 
	std::thread thread_;

 	/// \brief mqtt_run
    /// - mqtt loop run thread.
	void mqtt_run();
};
int mqtt_publish(const std::string &str);
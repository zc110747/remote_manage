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
    /// \brief constructor
	/// \param info -- info used to initialize the mqtt service.
	mqtt_process(const mqtt_info &info);

	/// \brief destructor
	~mqtt_process();

    /// \brief on_connect
    /// - This method is used do mqtt connect
    /// \param rc -- state for mqtt connect, 0 is success.
	void on_connect(int rc);

    /// \brief on_message
    /// - This method is used do mqtt message receive
    /// \param message -- memssage when receive from subscribe topic
	void on_message(const struct mosquitto_message *message);

    /// \brief on_subscribe
    /// - This method is used do subscribe success.
    /// \param mid --
	/// \param qos_count -- 
	/// \param granted_qos -- 
	void on_subscribe(int mid, int qos_count, const int *granted_qos);

    /// \brief publish_msg
    /// - This method is publish msg str.
    /// \param str -- memssage when receive by subscribe
	/// \return publish msg process.
	int publish_msg(const std::string &str);

    /// \brief start
    /// - This method is used to start mqtt process.
	bool start();

private:
	/// \brief info_
    /// - info used to store mqtt config.
	mqtt_info info_;

	/// \brief is_connet_
    /// - wheather mqtt success connect.
	bool is_connet_{false};

	/// \brief thread_
    /// - mqtt run thread object. 
	std::thread thread_;

 	/// \brief mqtt_run
    /// - mqtt loop run thread.
	void mqtt_run();
};

bool mqtt_init(void);
int mqtt_publish(const std::string &str);
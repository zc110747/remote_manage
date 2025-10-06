import paho.mqtt.client as mqtt
import json
import time

message_list = [
    "A0{\"command\":\"req_getstat\",\"source\":\"SRC_DESKTOP\"}",
]

# 连接成功回调
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    
    # 订阅主题
    client.subscribe("/info/winform_topic")
    
    # 发送 JSON 格式请求
    client.publish("/info/mp_topic", message_list[0])

# 收到消息回调
def on_message(client, userdata, msg):
    print(f"{msg.topic} {msg.payload.decode()}")

# 创建 MQTT 客户端实例
client = mqtt.Client()
# 设置连接成功回调函数
client.on_connect = on_connect
# 设置收到消息回调函数
client.on_message = on_message

# 连接到 MQTT 服务器
client.connect("172.27.83.254", 1883, 60)

# 启动单线程循环处理网络流量
client.loop_start()

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    print("Exiting...")
    client.loop_stop()
    client.disconnect()
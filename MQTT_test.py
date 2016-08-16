import paho.mqtt.publish as publish
import time

host = "localhost"
topic = "Bean"

print("Sending 0...")
publish.single(topic, "0", hostname=host)
time.sleep(10)
print("Sending 1...")
publish.single(topic, "1", hostname=host)
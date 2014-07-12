#ifndef __PUBSUB_H__
#define  __PUBSUB_H__

 MQTT_MAX_PACKET_SIZE  Maximum packet size
#define MQTT_MAX_PACKET_SIZE 128

 MQTT_KEEPALIVE  keepAlive interval in Seconds
#define MQTT_KEEPALIVE 15

#define MQTTPROTOCOLVERSION 3
#define MQTTCONNECT     1  4   Client request to connect to Server
#define MQTTCONNACK     2  4   Connect Acknowledgment
#define MQTTPUBLISH     3  4   Publish message
#define MQTTPUBACK      4  4   Publish Acknowledgment
#define MQTTPUBREC      5  4   Publish Received (assured delivery part 1)
#define MQTTPUBREL      6  4   Publish Release (assured delivery part 2)
#define MQTTPUBCOMP     7  4   Publish Complete (assured delivery part 3)
#define MQTTSUBSCRIBE   8  4   Client Subscribe request
#define MQTTSUBACK      9  4   Subscribe Acknowledgment
#define MQTTUNSUBSCRIBE 10  4  Client Unsubscribe request
#define MQTTUNSUBACK    11  4  Unsubscribe Acknowledgment
#define MQTTPINGREQ     12  4  PING Request
#define MQTTPINGRESP    13  4  PING Response
#define MQTTDISCONNECT  14  4  Client is Disconnecting
#define MQTTReserved    15  4  Reserved

#define MQTTQOS0        (0  1)
#define MQTTQOS1        (1  1)
#define MQTTQOS2        (2  1)

class PubSubClient {
private
   Client _client;
   TCPClient _client;  CH 14Jan2014 - changed Client to TCPClient
   uint8_t buffer[MQTT_MAX_PACKET_SIZE];
   uint16_t nextMsgId;
   unsigned long lastOutActivity;
   unsigned long lastInActivity;
   bool pingOutstanding;
   void (callback)(char,uint8_t,unsigned int);
   uint16_t readPacket(uint8_t);
   uint8_t readByte();
   bool write(uint8_t header, uint8_t buf, uint16_t length);
   uint16_t writeString(char string, uint8_t buf, uint16_t pos);
   uint8_t ip;
   char domain;
   uint16_t port;
public
   PubSubClient();
   PubSubClient(uint8_t , uint16_t, void()(char,uint8_t,unsigned int),TCPClient& client);  CH 14Jan2014 - changed Client& to TCPClient&
   PubSubClient(char , uint16_t, void()(char ,uint8_t,unsigned int),TCPClient&  client);  CH 14Jan2014 - changed Client& to TCPClient&
   bool connect(const char );
   bool connect(char );
   bool connect(char , char , char );
   bool connect(char , char , uint8_t, uint8_t, char );
   bool connect(char , char , char , char , uint8_t, uint8_t, char );
   void disconnect();
   bool publish(char , char );
   bool publish(char , uint8_t , unsigned int);
   bool publish(char , uint8_t , unsigned int, bool);
   bool subscribe(char );
   bool subscribe(char , uint8_t qos);
   bool unsubscribe(char );
   bool puback(uint16_t msgId);
   bool loop();
   bool connected();
};
#endif  __PUBSUB_H__
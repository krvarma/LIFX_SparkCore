/*
  PubSubClient.cpp - A simple client for MQTT.
  Original Code - Nicholas O'Leary
  http://knolleary.net

  Adapted for Spark Core by Chris Howard - chris@kitard.com
  Based on PubSubClient 1.9.1

  Changes
  - Added gcc pragma to avoid warnings being thrown as errors (deprecated conversion from string constant to 'char*')
  - publish_P function removed due to lack of Arduino PROGMEN support on the Spark Core
  - Obvious includes commented out
  - Using Spark TCPClient instead of Arduino EthernetClient

*/

#define ARDUINO_H
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "application.h"
#include "PubSubClient.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

PubSubClient::PubSubClient() {
   this->_client = NULL;
}

PubSubClient::PubSubClient(uint8_t *ip, uint16_t port, void (*callback)(char*,uint8_t*,unsigned int), TCPClient& client) { // CH 14Jan2014 - Changed Client& to TCPClient&
   this->_client = &client;
   this->callback = callback;
   this->ip = ip;
   this->port = port;
   this->domain = NULL;
}

PubSubClient::PubSubClient(char* domain, uint16_t port, void (*callback)(char*,uint8_t*,unsigned int), TCPClient& client) { // CH 14Jan2014 - Changed Client& to TCPClient&
   this->_client = &client;
   this->callback = callback;
   this->domain = domain;
   this->port = port;
}


// CONNECT

//bool PubSubClient::connect(const char *id) {
bool PubSubClient::connect(char *id) {
   return connect(id,NULL,NULL,0,0,0,0);
}

bool PubSubClient::connect(char *id, char *user, char *pass) {
   return connect(id,user,pass,0,0,0,0);
}

bool PubSubClient::connect(char *id, char* willTopic, uint8_t willQos, uint8_t willRetain, char* willMessage)
{
   return connect(id,NULL,NULL,willTopic,willQos,willRetain,willMessage);
}

bool PubSubClient::connect(char *id, char *user, char *pass, char* willTopic, uint8_t willQos, uint8_t willRetain, char* willMessage) {
   if (!connected()) {
      int result = 0;

      if (domain != NULL) {
        result = _client->connect(this->domain, this->port);
      } else {
        result = _client->connect(this->ip, this->port);
      }

      if (result) {
         nextMsgId = 1;
         uint8_t d[9] = {0x00,0x06,'M','Q','I','s','d','p',MQTTPROTOCOLVERSION};
         // Leave room in the buffer for header and variable length field
         uint16_t length = 5;
         unsigned int j;
         for (j = 0;j<9;j++) {
            buffer[length++] = d[j];
         }

         uint8_t v;
         if (willTopic) {
            v = 0x06|(willQos<<3)|(willRetain<<5);
         } else {
            v = 0x02;
         }

         if(user != NULL) {
            v = v|0x80;

            if(pass != NULL) {
               v = v|(0x80>>1);
            }
         }

         buffer[length++] = v;

         buffer[length++] = ((MQTT_KEEPALIVE) >> 8);
         buffer[length++] = ((MQTT_KEEPALIVE) & 0xFF);
         length = writeString(id,buffer,length);
         if (willTopic) {
            length = writeString(willTopic,buffer,length);
            length = writeString(willMessage,buffer,length);
         }

         if(user != NULL) {
            length = writeString(user,buffer,length);
            if(pass != NULL) {
               length = writeString(pass,buffer,length);
            }
         }

         write(MQTTCONNECT,buffer,length-5);

         lastInActivity = lastOutActivity = millis();

         while (!_client->available()) {
            unsigned long t = millis();
            if (t-lastInActivity > MQTT_KEEPALIVE*1000UL) {
               _client->stop();
               return false;
            }
         }
         uint8_t llen;
         uint16_t len = readPacket(&llen);

         if (len == 4 && buffer[3] == 0) {
            lastInActivity = millis();
            pingOutstanding = false;
            return true;
         }
      }
      _client->stop();
   }
   return false;
}

uint8_t PubSubClient::readByte() {
   while(!_client->available()) {}
   return _client->read();
}

uint16_t PubSubClient::readPacket(uint8_t* lengthLength) {
   uint16_t len = 0;
   buffer[len++] = readByte();
   uint32_t multiplier = 1;
   uint16_t length = 0;
   uint8_t digit = 0;
   do {
      digit = readByte();
      buffer[len++] = digit;
      length += (digit & 127) * multiplier;
      multiplier *= 128;
   } while ((digit & 128) != 0);
   *lengthLength = len-1;
   for (uint16_t i = 0;i<length;i++)
   {
      if (len < MQTT_MAX_PACKET_SIZE) {
         buffer[len++] = readByte();
      } else {
         readByte();
         len = 0; // This will cause the packet to be ignored.
      }
   }

   return len;
}

bool PubSubClient::loop() {
   if (connected()) {
      unsigned long t = millis();
      if ((t - lastInActivity > MQTT_KEEPALIVE*1000UL) || (t - lastOutActivity > MQTT_KEEPALIVE*1000UL)) {
         if (pingOutstanding) {
            _client->stop();
            return false;
         } else {
            buffer[0] = MQTTPINGREQ;
            buffer[1] = 0;
            _client->write(buffer,2);
            lastOutActivity = t;
            lastInActivity = t;
            pingOutstanding = true;
         }
      }
      if (_client->available()) {
         uint8_t llen;
         uint16_t len = readPacket(&llen);
         uint16_t msgId = 0;
         uint8_t *payload;
         if (len > 0) {
            lastInActivity = t;
            uint8_t type = buffer[0]&0xF0;
            if (type == MQTTPUBLISH) {
               if (callback) {
                  uint16_t tl = (buffer[llen+1]<<8)+buffer[llen+2];
                  char topic[tl+1];
                  for (uint16_t i=0;i<tl;i++) {
                     topic[i] = buffer[llen+3+i];
                  }
                  topic[tl] = 0;
                  // msgId only present for QOS>0
                  if (buffer[0]&MQTTQOS1) {
                    msgId = (buffer[llen+3+tl]<<8)+buffer[llen+3+tl+1];
                    payload = buffer+llen+3+tl+2;
                    callback(topic,payload,len-llen-3-tl-2);
                    puback(msgId);
                  } else {
                    payload = buffer+llen+3+tl;
                    callback(topic,payload,len-llen-3-tl);
                  }
               }
            } else if (type == MQTTPINGREQ) {
               buffer[0] = MQTTPINGRESP;
               buffer[1] = 0;
               _client->write(buffer,2);
            } else if (type == MQTTPINGRESP) {
               pingOutstanding = false;
            }
         }
      }
      return true;
   }
   return false;
}


// PUBLISH

bool PubSubClient::publish(char* topic, char* payload) {
   return publish(topic,(uint8_t*)payload,strlen(payload),false);
}

bool PubSubClient::publish(char* topic, uint8_t* payload, unsigned int plength) {
   return publish(topic, payload, plength, false);
}

bool PubSubClient::publish(char* topic, uint8_t* payload, unsigned int plength, bool retained) {
   if (connected()) {
      // Leave room in the buffer for header and variable length field
      uint16_t length = 5;
      length = writeString(topic,buffer,length);
      uint16_t i;
      for (i=0;i<plength;i++) {
         buffer[length++] = payload[i];
      }
      uint8_t header = MQTTPUBLISH;
      if (retained) {
         header |= 1;
      }
      Serial.println("Publishing...");
      
      return write(header,buffer,length-5);
   }
   
   Serial.println("Not connected...");
   
   return false;
}

bool PubSubClient::write(uint8_t header, uint8_t* buf, uint16_t length) {
   uint8_t lenBuf[4];
   uint8_t llen = 0;
   uint8_t digit;
   uint8_t pos = 0;
   uint8_t rc;
   uint8_t len = length;
   do {
      digit = len % 128;
      len = len / 128;
      if (len > 0) {
         digit |= 0x80;
      }
      lenBuf[pos++] = digit;
      llen++;
   } while(len>0);

   buf[4-llen] = header;
   for (int i=0;i<llen;i++) {
      buf[5-llen+i] = lenBuf[i];
   }
   rc = _client->write(buf+(4-llen),length+1+llen);
   
   lastOutActivity = millis();
   return (rc == 1+llen+length);
}

bool PubSubClient::subscribe(char* topic) {
  return subscribe(topic, 0);
}

// SUBSCRIBE

bool PubSubClient::subscribe(char* topic, uint8_t qos) {
   if (qos < 0 || qos > 1)
     return false;

   if (connected()) {
      // Leave room in the buffer for header and variable length field
      uint16_t length = 5;
      nextMsgId++;
      if (nextMsgId == 0) {
         nextMsgId = 1;
      }
      buffer[length++] = (nextMsgId >> 8);
      buffer[length++] = (nextMsgId & 0xFF);
      length = writeString(topic, buffer,length);
      buffer[length++] = qos;
      return write(MQTTSUBSCRIBE|MQTTQOS1,buffer,length-5);
   }
   return false;
}

bool PubSubClient::puback(uint16_t msgId) {
  if(connected()) {
    // Leave room in the buffer for header and variable length field
    uint16_t length = 5;
    buffer[length++] = (msgId >> 8);
    buffer[length++] = (msgId & 0xFF);
    return write(MQTTPUBACK,buffer,length-5);
  }
  return false;
}

// HELPERS

//bool PubSubClient::unsubscribe(char* topic) {
bool PubSubClient::unsubscribe(char* topic) {
   if (connected()) {
      uint16_t length = 5;
      nextMsgId++;
      if (nextMsgId == 0) {
         nextMsgId = 1;
      }
      buffer[length++] = (nextMsgId >> 8);
      buffer[length++] = (nextMsgId & 0xFF);
      length = writeString(topic, buffer,length);
      return write(MQTTUNSUBSCRIBE|MQTTQOS1,buffer,length-5);
   }
   return false;
}

void PubSubClient::disconnect() {
   buffer[0] = MQTTDISCONNECT;
   buffer[1] = 0;
   _client->write(buffer,2);
   _client->stop();
   lastInActivity = lastOutActivity = millis();
}

uint16_t PubSubClient::writeString(char* string, uint8_t* buf, uint16_t pos) {
   char* idp = string;
   uint16_t i = 0;
   pos += 2;
   while (*idp) {
      buf[pos++] = *idp++;
      i++;
   }
   buf[pos-i-2] = (i >> 8);
   buf[pos-i-1] = (i & 0xFF);
   return pos;
}


bool PubSubClient::connected() {
   bool rc;
   if (_client == NULL ) {
      rc = false;
   } else {
      rc = (int)_client->connected();
      if (!rc) _client->stop();
   }
   return rc;
}
// This #include statement was automatically added by the Spark IDE.
#include "PubSubClient.h"

int LED = D7; // for demo only

TCPClient tcpClient;
PubSubClient client("iot.eclipse.org", 1883, NULL, tcpClient);
bool isConnected = false;

int controlLifx(String arg){
    if(client.connected()){
        char szCmd[32];
        
        arg.toCharArray(szCmd, 32);
        
        if(client.publish("lifxcmd", szCmd)){
            Spark.publish("lifxcmd", szCmd);            
        }
        
        digitalWrite(LED, HIGH);
        delay(100);
        digitalWrite(LED, LOW);
    }
    else{
        Serial.println("Not connected.");
    }
}


void setup(){
    Serial.begin(57600);
    pinMode(LED, OUTPUT);
    
    Spark.function("lifxcmd", controlLifx);

    client.connect("Spark");
}

void loop(){
    if(!client.connected()){
        Serial.println("Connecting...");
        
        client.connect("Spark");
    }
    
    delay(1000);
}
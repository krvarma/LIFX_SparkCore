// This #include statement was automatically added by the Spark IDE.
#include "PubSubClient.h"

// This #include statement was automatically added by the Spark IDE.
#include "SFE_TSL2561.h"

SFE_TSL2561 tsl = SFE_TSL2561();
TCPClient tcpClient;
PubSubClient client("iot.eclipse.org", 1883, NULL, tcpClient);
bool isConnected = false;
unsigned char id;
boolean gain;
unsigned int ms, data1, data2;

int lowerLimit = 8;
boolean isOn = false;

int setLowerLimit(String args){
    lowerLimit = args.toInt();
    
    return lowerLimit;
}

void setup() {
    pinMode(D7, OUTPUT);
    Serial.begin(9600);
    
    tsl.begin();
    tsl.getID(id);
    tsl.setTiming(0, 2, ms);
    tsl.setPowerUp();
    
    Spark.function("setLightLowerLimit", setLowerLimit);
    
    client.connect("Spark");
}

void loop() {
    if(!client.connected()){
        Serial.println("Connecting...");
        
        client.connect("Spark");
    }
    
    tsl.getData(data1, data2);
    
    double lux;
    boolean good = tsl.getLux(gain,ms,data1,data2,lux);
    
    char szInfo[32];
    
    sprintf(szInfo, "Current value %.2f (lux) - ", lux);
    Serial.print(szInfo);
    
    if(lux < lowerLimit){
        if(!isOn){
            //turnOnLifx(true);
            setLifxColor("255,0,0");
        }
        
        isOn = true;
        digitalWrite(D7, HIGH);
        Serial.println("Bad Lighting Condition.");
    }
    else{
        if(isOn){
            //turnOnLifx(false);    
            setLifxColor("0,255,0");
        }
        
        isOn = false;
        
        digitalWrite(D7, LOW);
        Serial.println("Good Lighting Condition.");
    }
    
    delay(1000);
}

void turnOnLifx(bool isOn){
    if(client.connected()){
        char szCmd[9];
        
        sprintf(szCmd, "lights=%d", (isOn ? 1 : 0));
        
        client.publish("lifxcmd", szCmd);
    }
    else{
        Serial.println("Client not connected :(");
    }
}

void setLifxColor(char* color){
    if(client.connected()){
        char szCmd[22];
        
        sprintf(szCmd, "color=%s", color);
        
        client.publish("lifxcmd", szCmd);
    }
    else{
        Serial.println("Client not connected :(");
    }
}
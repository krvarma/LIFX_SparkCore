Control LIFX Bulb using Spark Core
-----------------------------

**Introduction**

Here I am sharing a project to control LIFX WiFi bulb with Spark Core. As of now LIFX cannot be controlled/accessed from outside network/Internet, it can only be accessed from the network in which it is connected to. One method to access it from Internet is to use an intermediate server running on the same network it is connected to and let the server control the LIFX. This server acts as a bridge between the bulb and outside world. LIFX provides some [SDKs][1] to control the bulb. There are also lot of reverse engineered SDKs available, one such is the LIFX.js library for node.js. 

**Communication Between Spark Core and Intermediate server**

We can use different methods to communicate between Spark Core and Intermediate Server, such as MQTT, Sparknode, etc... For this project I used MQTT and Sparknode. There is no reason to use both, but just for exploring I used both MQTT and Sparknode. 

The project works using MQTT as message broker. The Spark Core uses this [MQTT Library by Chris Howard][2] to connect to MQTT server and publish messages. The node.js server use this [MQTT Library by Adam Rudd][3]. For controlling LIFX bulb I am using the [LIFX.js library by Kevin Bowman (magicmonkey)][4]. I am using 
[Eclipse MQTT Sandbox][5] as MQTT Server. I have this node.js server running on my Raspberry PI which is connected to the same network to which the LIFX is connected. 

**Sample applications**


There are two samples, one is the to play with the colors, turn on, blink, etc... For this sample I am using for both MQTT and Sparknode. Both achieve the same results but with different ways. The second example uses TSL2561 Light sensor to watch the light level and turn the LIFX red when it is below a particular level and green when above the level. 
The first sample one comes with a Web Page to control the bulb. You can use this web page to set the color, turn on/off and blink the bulb (don't forget to replace the *deviceid* and *accesstoken* with actual values).

**Installation**

 1. Download and Install Node.js from [this link][6] 
 2. Download the node.js server from https://github.com/krvarma/LIFX_SparkCore and extract 
 3. Goto the folder and run `npm install`. Since it contains the package.json file, it will install all the dependencies 
 4. Run `node lifx.js` 
 5. Flash the Spark Core with one of the example 
 6. Connect LIFX and wait for the bulb to connect to the network

**Conclusion**

Merely controlling the bulb from outside network is not that exciting, but Spark Core can do lot of amazing things and controlling LIFX from Spark Core is good idea. We can use the LIFX as a notification device, say you are monitoring the temperature of a room and if the temperature is not within the limit Spark Core can blink the bulb or change the color, etc...

**Screenshots**

*Web Page*

![Web Page][7]

*Spark Core Sample Two* 

![Spark Core Sample Two][8]

**Limitations**

 1. In this sample I am considering only a single LIFX bulb, when there
    are more than one, the application can easily changed to support
    multiple bulbs or a group of bulbs. Spark Core try to get how many
    lights and groups are connected and fetch the data from node.js
    server. After this information retrieved we can control individual
    bulb.

 2. The lifx.js library does not provide methods to retrieve current
        color of the bulb. This requires some amount of work to try
        understanding the protocol involved and implement in the library


  [1]: https://github.com/LIFX
  [2]: https://community.spark.io/t/mqtt-library-and-sample/2111
  [3]: https://github.com/adamvr/MQTT.js/
  [4]: https://github.com/magicmonkey/lifxjs
  [5]: http://iot.eclipse.org/sandbox.html
  [6]: http://nodejs.org/
  [7]: https://raw.githubusercontent.com/krvarma/LIFX_SparkCore/master/screenshots/web.jpg
  [8]: https://raw.githubusercontent.com/krvarma/LIFX_SparkCore/master/screenshots/spark.JPG
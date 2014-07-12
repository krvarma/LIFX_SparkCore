Control LIFX Bulb using Spark Core
-----------------------------

Here I am sharing a project to control LIFX WiFi bulb with Spark Core, not directly but using an intermediate node.js server. 

The project works using MQTT as message broker. The Spark Core uses this [MQTT Library by Chris Howard][1] to connect to MQTT server and publish messages. The node.js server use this [MQTT Library by Adam Rudd][2]. For controlling LIFX bulb I am using the [LIFX.js library by Kevin Bowman (magicmonkey)][3]. I am using 
[Eclipse MQTT Sandbox][4] as MQTT Server.

I have this node.js server running on my Raspberry PI which is connected to the same network to which the LIFX is connected. This is very important because as of now LIFX cannot be controlled from outside network but only form same network in which the bulb is connected. Since the Spark Core can be accessed from Internet this setup gives you the ability to access the bulb form outside network also.

Merely controlling the bulb from outside network is not that exciting, but Spark Core can do lot of amazing things and controlling LIFX from Spark Core is good idea. We can use the LIFX as a notification device, say you are monitoring the temperature of a room and if the temperature is not within the limit Spark Core can blink the bulb or change the color, etc...

**Installation**

 1. Download and Install Node.js from [this link][5] 
 2. Download the node.js server from https://github.com/krvarma/LIFX_SparkCore and extract 
 3. Goto the folder and run `npm install`. Since it contains the package.json file, it will install all the dependencies 
 4. Run `node lifx.js` 
 5. Flash the Spark Core with one of the example 
 6. Connect LIFX and wait for the bulb to connect to the network

There are two samples, one is the to play with the colors, turn on, blink, etc... Another example uses TSL2561 Light sensor to watch te light level and turn the LIFX red when it is below a particular level and turn green when above the level. The sample one comes with a Web Page to control the bulb. You can use this web page to set the color, turn on/off and blink the bulb (don't forget to replace the *deviceid* and *accesstoken* with actual values).

**Screenshots**

*Web Page*

![Web Page][6]

*Spark Core Sample Two* 

![Spark Core Sample Two][7]

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

  [1]: https://community.spark.io/t/mqtt-library-and-sample/2111
  [2]: https://github.com/adamvr/MQTT.js/
  [3]: https://github.com/magicmonkey/lifxjs
  [4]: http://iot.eclipse.org/sandbox.html
  [5]: http://nodejs.org/
  [6]: https://raw.githubusercontent.com/krvarma/LIFX_SparkCore/master/screenshots/web.jpg
  [7]: https://raw.githubusercontent.com/krvarma/LIFX_SparkCore/master/screenshots/spark.JPG
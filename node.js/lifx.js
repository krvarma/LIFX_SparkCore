var mqtt = require('mqtt')
var lifx = require("lifx");
var bulbFound = false;

var lx = lifx.init();
var client = mqtt.createClient(1883, 'iot.eclipse.org');

client.subscribe('lifxcmd');

lx.on('bulb', function(b){
	bulbFound = true;
	
	console.log("LIFX Bulb Found");
});

client.on('message', function (topic, message) {
	console.log(message);
  
	var msg = message.split('=');
	
	console.log(msg[0]);
	console.log(msg[1]);
	
	if(bulbFound){
		if("lights" == msg[0]){
			if("1" == msg[1]){
				lx.lightsOn();
			}
			else{
				lx.lightsOff();
			}
		}
		else if("color" == msg[0]){
			var clrarray = msg[1].split(",");
			
			console.log(clrarray);
			
			var clr = rgbToHsl(clrarray[0], clrarray[1], clrarray[2]);
			
			console.log(clr);
					
			var h = (clr[0] * 65535);
			var s = (clr[1]* 65535);
			var b = (clr[2] * 65535);
						
			console.log(parseInt(h));
			console.log(parseInt(s));
			console.log(parseInt(b));
			
			lx.lightsColour(parseInt(h), parseInt(s), parseInt(b), 0x0dac, 0x0513);
		}
		else if("blink" == msg[0]){
			var times = msg[1];
			
			for(var i=0; i<times; ++i){
				lx.lightsOff();
				wait(1000);
				lx.lightsOn();
				wait(1000);
			}
		}
		else if("blinkrgb" == msg[0]){
			var clr, h, s, l;
			
			clr = rgbToHsl(255, 0, 0);
			h = (clr[0] * 65535);
			s = (clr[1]* 65535);
			b = (clr[2] * 65535);
			lx.lightsColour(parseInt(h), parseInt(s), parseInt(b), 0x0dac, 0x0513);
			wait(2000);
			
			clr = rgbToHsl(0, 255, 0);
			h = (clr[0] * 65535);
			s = (clr[1]* 65535);
			clrb = (clr[2] * 65535);
			lx.lightsColour(parseInt(h), parseInt(s), parseInt(b), 0x0dac, 0x0513);
			wait(2000);
			
			clr = rgbToHsl(0, 0, 255);
			h = (clr[0] * 65535);
			s = (clr[1]* 65535);
			b = (clr[2] * 65535);
			lx.lightsColour(parseInt(h), parseInt(s), parseInt(b), 0x0dac, 0x0513);
			wait(2000);
			
			clr = rgbToHsl(255, 255, 255);
			h = (clr[0] * 65535);
			s = (clr[1]* 65535);
			b = (clr[2] * 65535);
			lx.lightsColour(parseInt(h), parseInt(s), parseInt(b), 0x0dac, 0x0513);
			wait(2000);
		}
	}
	else{
		console.log("Bulb not connected");
	}
});

function rgbToHsl(r, g, b){
    r /= 255, g /= 255, b /= 255;
    var max = Math.max(r, g, b), min = Math.min(r, g, b);
    var h, s, l = (max + min) / 2;
	
    if(max == min){
        h = s = 0; // achromatic
    }else{
        var d = max - min;
        s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
        switch(max){
            case r: h = (g - b) / d + (g < b ? 6 : 0); break;
            case g: h = (b - r) / d + 2; break;
            case b: h = (r - g) / d + 4; break;
        }
        h /= 6;
    }
	
    return [h, s, l];
}

function wait(timeout){
	var start = new Date().getTime();
	
	while(((new Date).getTime() - start) <= timeout){
		;
	}
}
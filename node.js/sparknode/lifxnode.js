var spark = require('sparknode');
var lifx = require("lifx");

// Initialize LIFX
var bulbFound = false;
var lx = lifx.init();

// Spark Core credentials
var core = new spark.Core({
	accessToken: "a67780d614cd4453eabcbba48d77e5dd29029288",
	id: "55ff6c065075555323331487"
});

// Called when connected bulb found
lx.on('bulb', function(b){
	bulbFound = true;
	
	console.log("LIFX Bulb Found");
});

// Subscribe Spark Core events
core.on('lifxcmd', function(info) {
	console.log(info);
	console.log(info.data);
  
	var msg = info.data.split('=');
	if(bulbFound){
		// LIGHT command found, second argument will be 1 or 0, 1 for on and 0 for off
		if("lights" == msg[0]){
			if("1" == msg[1]){
				lx.lightsOn();
			}
			else{
				lx.lightsOff();
			}
		}
		// COLOR command found, second argument will be R,G,B. Parse RGB and set LIFX color
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
		// BLINK command found, second argument is number of times to blink
		else if("blink" == msg[0]){
			var times = msg[1];
			
			for(var i=0; i<times; ++i){
				lx.lightsOff();
				wait(1000);
				lx.lightsOn();
				wait(1000);
			}
		}
		// BLINKRGB, a simple test command, not much usefull
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

// Convert RGB to HSL
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

// A simple wait method
function wait(timeout){
	var start = new Date().getTime();
	
	while(((new Date).getTime() - start) <= timeout){
		;
	}
}
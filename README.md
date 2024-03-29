For any documentation/updated info please visit: https://github.com/suletom/PJsonstore/

# PJsonstore - minimal PHP array like structure with JSON serialization/deserialization

This project is minimalistic header library for arduino. The library uses a modified version of jsmn to parse JSON data and it can hold and modify the data in memory. Of course it can write data back to JSON. The object can be used similar to PHP array structure. The main purpuse was to create a global configuration object for an ESP8266 device to create webUI and store configured data to flash.

# Usage/examples

1. Download the [latest](https://github.com/suletom/PJsonstore/releases/download/v1.0.1/PJsonstore.zip) release, and extract to your Arduino libraries directory!
2. Include the lib in your sketch. See the example!
3. Basic usage example:
  
		#include <PJsonstore.h>

		pjsonstore config;

		//handy macro to define json object string
		String confdef = R"({
		"wifipw": ["12345678","WIFI password"],
		"ssid": ["device","SSID"]
		})";
	  
		void setup() {

			String output;

			Serial.begin(9600);
	  
			//provide the expected number of json tokens,
			if (config.from_json(64, confdef.c_str())){
				//success
				Serial.print("Get pw value: ");
				Serial.println(config["wifipw"][0].get_value().pj_string);

				Serial.print("Convert back to json: ");
				config.to_json(output);
				Serial.println(output);
				output="";
		  
				config["wifipw"][0]="new string value";

				Serial.print("Convert back, after modifying pw value: ");
				config.to_json(output);
				Serial.println(output);  
		        
			}else{
				//failure, perhaps you need more tokens?
			}

		}
	  
		void loop() {
	
		} 	 
  
# Known issues
- No support for "\uXXX" escaped characters (needs much more code than available)

  
  
  

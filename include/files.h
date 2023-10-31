#ifndef FILES_H
#define FILES_H

#include <Arduino.h>
#include <SPIFFS.h> 
#include <ArduinoJson.h>
#include "hbtfmqtt.h"
 
class FILES
{
	private:
		static FILES *instance;
    	FILES(){};
		
		int callCount = 0;

	public:
		static FILES *getInstance() 
    	{
      		if (!instance)
      		instance = new FILES;
      		return instance;
    	}

		bool reset = false;

    	void setup();
    	void format();
		DynamicJsonDocument getConfig();
		void setConfig(DynamicJsonDocument config);
		void delConfig();
    	bool fileExists(String filename);

    	size_t sizeGet(String filename);
    	String fileRead(String filename);
    	void fileWrite(String filename, String content);
    	void incRestartCounter();
    	void resetRestartCounter(int count);
};

#endif
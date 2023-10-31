#ifndef WLAN_H
#define WLAN_H

#include <WiFi.h>
#include <DNSServer.h>
#include "hbtfmqtt.h"

class WLAN
{
	private:
		static WLAN *instance;
    	WLAN(){};

    	unsigned long apStartMillis   = 0;
    	int wlanFails                 = 0;
    	IPAddress ip;
    	IPAddress subnet;
		IPAddress dns1;
		IPAddress dns2;
		IPAddress gw;

  	public:
		static WLAN *getInstance() 
    	{
      		if (!instance)
      		instance = new WLAN;
      		return instance;
    	}
		
    	DNSServer dns;    
    	IPAddress ipAddress;
    	bool ready    = false;
    	bool apStart  = false;
    	void setup();
    	void loop();

	    void initWLAN();
};

#endif
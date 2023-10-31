#define TFBLEGW_DEBUG
#include "wlan.h"

WLAN *WLAN::instance = 0;

void WLAN::setup()
{
  	initWLAN();
}

void WLAN::loop()
{
  	if(apStart)
  	{
    	dns.processNextRequest();
  	}

	initWLAN();
}

void WLAN::initWLAN()
{
	if(WiFi.status() != WL_CONNECTED)
	{
    	if(!apStart)
    	{
      		ready = false;
			FILES* files = FILES::getInstance();
            DynamicJsonDocument config 	= files->getConfig();
			config.shrinkToFit();

			if(!config["wlanName"].isNull() && !config["wlanPassword"].isNull())
			{
				while(wlanFails < WLAN_FAILS)
				{
					DEBUG_PRINTLN(F("try to connect wlan..."));
					WiFi.mode(WIFI_STA);
					if((!config.containsKey("useDHCP") || !config["useDHCP"].as<bool>()) && (config.containsKey("ipAddress") && config.containsKey("subnetMask") && config.containsKey("dns1") && config.containsKey("gateway") && config["ipAddress"].as<String>().length() > 1 && config["subnetMask"].as<String>().length() > 1 && config["dns1"].as<String>().length() > 1 && config["gateway"].as<String>().length() > 1))
    				{
						if(ip.fromString(config["ipAddress"].as<String>()) && subnet.fromString(config["subnetMask"].as<String>()) && dns1.fromString(config["dns1"].as<String>()) && gw.fromString(config["gateway"].as<String>()))
						{
							if(config.containsKey("dns2") && config["dns2"].as<String>() != "")
							{
								dns2.fromString(config["dns2"].as<String>());
							}
							WiFi.config(ip, gw, subnet, dns1, dns2);
						}
					}
					WiFi.begin(config["wlanName"].as<const char*>(), config["wlanPassword"].as<const char*>());
					delay(WLAN_INTERVAL);
					if(WiFi.status() == WL_CONNECTED)
					{
            			ipAddress = WiFi.localIP();
            			ready     = true;

            			DEBUG_PRINTLN(F("wlan connected"));
						//DEBUG_PRINT(F("MAC: "));
						//DEBUG_PRINTLN(WiFi.macAddress());
						DEBUG_PRINT(F("IP: "));
						DEBUG_PRINTLN(ipAddress);
						break;
          			}
          			wlanFails++;
        		}
      		}
      		else
      		{
        		wlanFails = 100;
      		}
      		if(wlanFails >= WLAN_FAILS)
      		{
        		DEBUG_PRINTLN(F("start AP..."));
        		if(WiFi.softAP((String(DEVICETYPE)+"-"+DEVICEID).c_str()))
        		{
          			WiFi.softAPConfig(IPAddress(4,3,2,1), IPAddress(4,3,2,1), IPAddress(255, 255, 255, 0));
          			dns.setErrorReplyCode(DNSReplyCode::NoError);
          			dns.start(53, "*", IPAddress(4,3,2,1));
          			DEBUG_PRINTLN(F("AP started"));
          			//DEBUG_PRINT(F("MAC: "));
          			//DEBUG_PRINTLN(WiFi.macAddress());
          			DEBUG_PRINT(F("IP: "));
          			DEBUG_PRINTLN(ipAddress);
          			apStartMillis   = millis();
          			apStart         = true;
          			ready           = true;
        		}
      		}
    	}
    	else
    	{
      		if(millis() - apStartMillis >= AP_TIMEOUT)
      		{
        		DEBUG_PRINTLN(F("AP reboot..."));
        		ESP.restart();
      		}
    	}
  	}
}
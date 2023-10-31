#define HBTFMQTT_DEBUG
#include "websrv.h"

WEBSRV *WEBSRV::instance = 0;
/*
bool _webAuth(AsyncWebServerRequest *request)
{
    DynamicJsonDocument config = files.getConfig();
  	if(config.containsKey("webAuth") && config["webAuth"].as<String>() != "")
  	{
    	if(!request->authenticate(config["webAuth"].as<const char*>()))
    	{
			request->requestAuthentication(DEVICETYPE, true);
      		return false;
    	}
    	return true;
  	}
  	else
  	{
		return true;
  	}
}
*/

class CaptiveRequestHandler : public AsyncWebHandler 
{
	public:
    	CaptiveRequestHandler() {}
  		virtual ~CaptiveRequestHandler() {}

  		bool canHandle(AsyncWebServerRequest *request)
		{
    		return true;
  		}
  		
		void handleRequest(AsyncWebServerRequest *request) 
  		{
    		request->send_P(200, "text/html", settingsSimple_html);
  		}
};

void WEBSRV::start(int port)
{
	if(!ready)
  	{
    	_server  = new AsyncWebServer(port);
    	_ws      = new AsyncWebSocket("/ws");
    	
		if(_server && _ws)
    	{
      		ready = true;
    	}
    	else
    	{
      		ready = false;
    	}
    	
		_ws->onEvent(std::bind(&WEBSRV::_wsEvent, this
        	, std::placeholders::_1
        	, std::placeholders::_2
        	, std::placeholders::_3
        	, std::placeholders::_4
        	, std::placeholders::_5
        	, std::placeholders::_6));
    	_server->addHandler(_ws);

    	_server->on("/hbtfmqtt.css", HTTP_GET, [](AsyncWebServerRequest *request)
    	{
			//if(!_webAuth(request)) return;
      		request->send_P(200, "text/css", hbtfmqtt_css);
    	}).setFilter(ON_STA_FILTER);
  
    	_server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    	{
			//if(!_webAuth(request)) return;
      		request->send_P(200, "text/html", index_html);
    	}).setFilter(ON_STA_FILTER);

		_server->on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
    	{
			//if(!_webAuth(request)) return;
      		request->send_P(200, "text/html", settings_html);
    	}).setFilter(ON_STA_FILTER);

    	_server->on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
    	{
			//if(!_webAuth(request)) return;
      		request->send(SPIFFS, "/config.cfg", "text/json");
    	});
   		_server->onNotFound([](AsyncWebServerRequest *request){
			//if(!_webAuth(request)) return;
      		request->redirect("/");
    	});

    	_server->addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    	_server->begin();
  	}
}

void WEBSRV::_wsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
	/*
	if(type == WS_EVT_CONNECT)
  	{
    	DEBUG_PRINT(F("Client: "));
    	DEBUG_PRINT(client->id());
    	DEBUG_PRINTLN(F(" connect"));
  	} 
  	else if(type == WS_EVT_DISCONNECT)
  	{
    	DEBUG_PRINT(F("Client: "));
    	DEBUG_PRINT(client->id());
    	DEBUG_PRINTLN(F(" disconnect"));
  	}
	*/
  	if(type == WS_EVT_DATA)
  	{   
      	DEBUG_PRINT(F("Data received: "));
      	for(int i=0; i < len; i++) 
      	{
        	DEBUG_PRINT((char) data[i]);
      	}
      	DEBUG_PRINTLN("");
    
    	AwsFrameInfo *info = (AwsFrameInfo *)arg;
    	if (info->final && info->index == 0 && info->len == len)
    	{
      		if (info->opcode == WS_TEXT)
      		{
        		wsClientID = client->id();
				DynamicJsonDocument wsData(512); // websocket data

				deserializeJson(wsData, (char*)data);
				wsData.shrinkToFit();
        		if(wsData.containsKey("dataType")) 
        		{
					if(wsData["dataType"].as<String>() == "indexGet")
          			{
            			DynamicJsonDocument sData(128);
						/*
    			        CLOUD *cloud = CLOUD::getInstance();
						
						sData["deviceId"] 	= DEVICEID;
						sData["state"]		= cloud->state;
						sData["fVersion"] 	= FVERSION;
						*/
            
						this->jSend("indexGet", sData, client->id());

          			}
          			// INDEX-GET end
					// WLANSCAN start
          			else if(wsData["dataType"].as<String>() == "wlanScan")
          			{
						WiFi.disconnect();
						WiFi.scanNetworks(true, false);
          			}
          			else if(wsData["dataType"].as<String>() == "wlanConnect")
          			{
						DEBUG_PRINTLN(wsData["data"]["wlanName"].as<String>());
						FILES* files = FILES::getInstance();
                        DynamicJsonDocument config 	= files->getConfig();
						config.shrinkToFit();
						config["wlanName"]       	= wsData["data"]["wlanName"];
            			config["wlanPassword"]   	= wsData["data"]["wlanPassword"];
						files->setConfig(config);
            			//ESP.restart();
					}
          			// WLANSCAN end
					// SETTINGS-GET start
          			else if(wsData["dataType"].as<String>() == "settingsGet")
          			{
						FILES* files = FILES::getInstance();
						DynamicJsonDocument config 	= files->getConfig();
            			config.shrinkToFit();
            			this->jSend("settingsGet", config);
          			}
          			// SETTINGS-GET end
          			// SETTINGS-SET start
					else if(wsData["dataType"].as<String>() == "settingsIp")
          			{
						if(wsData["data"]["action"].as<String>() == "settingsSet")
						{
							FILES* files = FILES::getInstance();
							DynamicJsonDocument config 	= files->getConfig();
							config.shrinkToFit();
							if(wsData["data"]["ipAddress"].as<String>() != "")
							{
								config["ipAddress"] = wsData["data"]["ipAddress"];
							}

							if(wsData["data"]["subnetAddress"].as<String>() != "")
							{
								config["subnetAddress"] = wsData["data"]["subnetAddress"];
							}

							if(wsData["data"]["gwAddress"].as<String>() != "")
							{
								config["gwAddress"]    = wsData["data"]["gwAddress"];
							}

							if(wsData["data"]["dnsAddress"].as<String>() != "")
							{
								config["dnsAddress"]    = wsData["data"]["dnsAddress"];
							}
							files->setConfig(config);
						}
					}
					else if(wsData["dataType"].as<String>() == "settingsMqtt")
          			{
						if(wsData["data"]["action"].as<String>() == "settingsSet")
						{
							FILES* files = FILES::getInstance();
                        	DynamicJsonDocument config 	= files->getConfig();
							config.shrinkToFit();
							if(wsData["data"]["mqttHost"].as<String>() != "")
            				{
              					config["mqttHost"]    = wsData["data"]["mqttHost"];
            				}

							if(wsData["data"]["mqttPort"].as<String>() != "")
            				{
              					config["mqttPort"]    = wsData["data"]["mqttPort"];
            				}

							if(wsData["data"]["mqttTopic"].as<String>() != "")
            				{
              					config["mqttTopic"]    = wsData["data"]["mqttTopic"];
            				}

							if(wsData["data"]["mqttUsername"].as<String>() != "")
            				{
              					config["mqttUsername"] = wsData["data"]["mqttUsername"];
            				}

            				if(wsData["data"]["mqttPassword"].as<String>() != "")
            				{
              					config["mqttPassword"] = wsData["data"]["mqttPassword"];
            				}
  
            				files->setConfig(config);
						}
          			}
          			// SETTINGS-SET end
          			// REBOOT start
          			else if(wsData["dataType"].as<String>() == "reboot")
          			{
            			ESP.restart();
          			}
          			// REBOOT end
        		}
      		}
        }
  	}
}

void WEBSRV::jSend(String dataType, DynamicJsonDocument sendData, int clientId)
{
	if(ready)
	{
        String jsonString;
        serializeJson(sendData, jsonString);
        sendData.clear();

        String sendString = "{\"dataType\":\""+dataType+"\", \"data\":"+jsonString+"}";
        jsonString.clear();

        DynamicJsonDocument send(256);
        deserializeJson(send, sendString);
        send.shrinkToFit();

		if(clientId > 0 && !_ws->getClients().isEmpty())
		{
			if(!_ws->getClients().isEmpty())
			{
				_ws->text(clientId, sendString);
			}
		}
		else
		{
			_ws->textAll(sendString);
		}
        sendString.clear();
        send.clear();
    }
	else
	{
		sendData.clear();
	}
}

void WEBSRV::dataSendAll(String dataType, String dataString)
{
	if(ready && !_ws->getClients().isEmpty())
  	{
    	String jsonString;
    	DynamicJsonDocument json(256);
    	json["dataType"]  = dataType;
    	json["data"]      = dataString;
    	serializeJson(json, jsonString);
    	_ws->textAll(jsonString);
    	json.clear();
    	jsonString.clear();
  	}
}

void WEBSRV::clientSend(uint32_t clientID, String dataType, String dataString)
{
	String jsonString;
  	DynamicJsonDocument json(CONF_SIZE);
  	//json.shrinkToFit();
  	json["dataType"]  = dataType;
  	json["data"]      = dataString;
  	serializeJson(json, jsonString);
  	_ws->text(clientID, jsonString);
  	json.clear();
  	jsonString.clear();
}

void WEBSRV::loop()
{
	if(WiFi.scanComplete() == WIFI_SCAN_RUNNING)
	{
		wlanScanActive = true;
	}
	
	if(wlanScanActive)
  	{
		int n = WiFi.scanComplete();
		if(n != -2 && n > 0)
		{
      		DEBUG_PRINT("N: ");
      		DEBUG_PRINTLN(String(n));
      		String wsSendDataString;
      		DynamicJsonDocument wsSendDataJson(512);
      		JsonObject root   = wsSendDataJson.to<JsonObject>();
      		root["dataType"]  = "wlanStations";
      		wlanScanActive    = false;
			JsonObject data = root.createNestedObject("data");
			for (int i = 0; i < n; ++i)
			{
				String wlanName = String(WiFi.SSID(i));
				String rssi     = String(WiFi.RSSI(i));
				String ch       = String(WiFi.channel(i));
				if(wlanName != "" && rssi != "" && ch != "")
				{
					JsonObject station  = data.createNestedObject(wlanName);
					station["rssi"]     = rssi;
					station["channel"]  = ch;
				}
			}
      		WiFi.scanDelete();
      		serializeJson(root, wsSendDataString);
            wsSendDataJson.shrinkToFit();
      		_ws->textAll(wsSendDataString);
      		wsSendDataJson.clear();
      		wsSendDataString.clear();
      		root.clear();
    	}
  	}
	_ws->cleanupClients();
}

void WEBSRV::stop()
{
	if(ready)
	{
    	_server->end();
    	_ws->closeAll();
    	ready = false;
  	}
}
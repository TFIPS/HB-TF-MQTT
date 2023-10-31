#ifndef WEBSRV_H
#define WEBSRV_H


#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "hbtfmqtt.h"

#include "pages_html.h"

class WEBSRV
{
	private:
    	static WEBSRV *instance;
    	WEBSRV(){};

    	AsyncWebServer *_server;
    
    	bool wlanScanActive = false;

		bool _webAuth(AsyncWebServerRequest *request);
    	void _wsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

	public:
		static WEBSRV *getInstance() 
    	{
      		if (!instance)
      		instance = new WEBSRV;
      		return instance;
    	}
    	AsyncWebSocket *_ws;
    
    	bool ready        = false;
    	int wsClientID    = 0;

    	void start(int port);
    	void loop();
    	void stop();
		void jSend(String dataType, DynamicJsonDocument sendData, int clientId = 0);
    	void dataSendAll(String dataType, String dataString);
    	void clientSend(uint32_t clientID, String dataType, String dataString);
};

#endif
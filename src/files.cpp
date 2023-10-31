#define HBTFMQTT_DEBUG
#include "files.h"
FILES *FILES::instance = 0;

void FILES::setup()
{
    bool fsReady = false;
    fsReady = SPIFFS.begin();
    if(!fsReady)
    {
        DEBUG_PRINTLN(F("DEBUG: filesystem error"));
        format();
        SPIFFS.begin();
    }
    else
    {
        DEBUG_PRINTLN(F("DEBUG: filesystem ok"));
    }
}

void FILES::format()
{
    DEBUG_PRINTLN(F("DEBUG: format filesystem"));
    SPIFFS.format();
}

DynamicJsonDocument FILES::getConfig()
{
    DynamicJsonDocument config(CONF_SIZE);
    if(fileExists(CONF_FILE))
    {
        File configFile = SPIFFS.open(CONF_FILE, FILE_READ);
        deserializeJson(config, configFile);
        configFile.close();
    }
    return config;
}

void FILES::setConfig(DynamicJsonDocument config)
{
    config.shrinkToFit();
    String configString;
    serializeJson(config, configString);
    config.clear();
    File configFile = SPIFFS.open(CONF_FILE, FILE_WRITE);
    configFile.print(configString);
    DEBUG_PRINT(F("Schreibe in Config: "));
    DEBUG_PRINTLN(configString);
    configString.clear();
    configFile.close();
}

void FILES::delConfig()
{
    if(fileExists(CONF_FILE))
    {	
        SPIFFS.remove(CONF_FILE);
    }
}

bool FILES::fileExists(String filename)
{
    if(SPIFFS.exists(filename))
    {
        return true;
    }
    else
    {
        return false;
    }
}

size_t FILES::sizeGet(String filename)
{
    size_t fileSize = 0;
    
    if(fileExists(filename))
    {
        File fileHandler    = SPIFFS.open(filename, FILE_READ);
        fileSize            = fileHandler.size();
        fileHandler.close();
    }
    
    return fileSize;
}

String FILES::fileRead(String filename)
{
    String fileString;
    if(fileExists(filename))
    {
        File fileHandler    = SPIFFS.open(filename, FILE_READ);
        fileString          = fileHandler.readString();
        fileHandler.close();
        return fileString;
    }
    return fileString;
}

void FILES::fileWrite(String filename, String content)
{
    File fileHandler1 = SPIFFS.open(filename, FILE_WRITE);
    fileHandler1.print(content);
    fileHandler1.close();
}

void FILES::incRestartCounter()
{
    DynamicJsonDocument config(CONF_SIZE);
    deserializeJson(config, fileRead(CONF_FILE));
    config["restart"] = config["restart"].as<unsigned int>() + 1;
    String configString;
    serializeJson(config, configString);
    fileWrite(CONF_FILE, configString);

    if(config["restart"].as<unsigned int>() >= 5)
    {
       format();
       ESP.restart();
    }
    config.clear();
}

void FILES::resetRestartCounter(int count)
{
    if(!reset)
    {
        callCount++;

        if(callCount >= count)
        {
            callCount   = 0;
            reset       = true;
            DEBUG_PRINTLN(F("DEBUG: Reset RestartCounter"));
            DynamicJsonDocument config(CONF_SIZE);
            deserializeJson(config, fileRead(CONF_FILE));
            if(config["restart"].as<unsigned int>() > 0)
            {
                config["restart"] = 0;
                String configString;
                serializeJson(config, configString);
                fileWrite(CONF_FILE, configString);
            }
            config.clear();
        }
    }
}
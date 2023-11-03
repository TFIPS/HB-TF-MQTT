#define HBTFMQTT_DEBUG
#include "hbtfmqtt.h"

FILES	*files  = FILES::getInstance();
WLAN 	*wlan   = WLAN::getInstance();
WEBSRV	*websrv	= WEBSRV::getInstance();
MQTT 	*mqtt   = MQTT::getInstance();

using namespace as;

const struct DeviceInfo PROGMEM devinfo = {
    {0x54,0x46,0x01},       	// Device ID
    "TFMQTT0001",           	// Device Serial
    {0x54,0x46},            	// Device Model
    0x10,                   	// Firmware Version
    as::DeviceType::THSensor, 	// Device Type
    {0x01, 0x00}               	// Info Bytes
};

typedef LibSPI<CC1101_CS_PIN> RadioSPI;
typedef AskSinRTC<StatusLed<LED_PIN>,NoBattery,Radio<RadioSPI,CC1101_GDO0_PIN> > Hal;
Hal hal;

DEFREGISTER(UReg0, MASTERID_REGS, DREG_BURSTRX, 0x21, 0x22)
class UList0 : public RegList0<UReg0> 
{
	public:
    	UList0 (uint16_t addr) : RegList0<UReg0>(addr) {}
    	bool Sendeintervall (uint16_t value) const 
		{
      		return this->writeRegister(0x21, (value >> 8) & 0xff) && this->writeRegister(0x22, value & 0xff);
    	}
    	uint16_t Sendeintervall () const 
		{
      		return (this->readRegister(0x21, 0) << 8) + this->readRegister(0x22, 0);
    	}
    	void defaults () 
		{
      		clear();
      		Sendeintervall(180);
    	}
};

class WeatherEventMsg : public Message 
{
	public:
    	void init(uint8_t msgcnt, int16_t* temp, int8_t* hum) 
		{
			uint8_t t1 = (temp[0] >> 8) & 0x7f;
			uint8_t t2 = temp[0] & 0xff;
			Message::init(11+7, msgcnt, 0x70, BIDI | WKMEUP, t1, t2);
			pload[0] = hum[0];

			pload[1] = (temp[1] >> 8) & 0x7f;
			pload[2] = temp[1] & 0xff;
			pload[3] = hum[1];

			pload[4] = (temp[2] >> 8) & 0x7f;
			pload[5] = temp[2] & 0xff;
			pload[6] = hum[2];
    	}
};

class WeatherChannel : public Channel<Hal, List1, EmptyList, List4, PEERS_PER_CHANNEL, UList0> 
{
	public:
    	WeatherChannel () : Channel() {}
    	virtual ~WeatherChannel () {}

    	void configChanged() {}

    	uint8_t status () const 
		{
      		return 0;
    	}

    	uint8_t flags () const 
		{
      		return 0;
    	}
};


class UType : public MultiChannelDevice<Hal, WeatherChannel, MAX_SENSORS, UList0> 
{
	class SensorArray : public Alarm 
	{
        UType& dev;

    	public:
        	int16_t	sensors[MAX_SENSORS];
			int8_t  humidity[MAX_SENSORS];
        	SensorArray (UType& d) : Alarm(0), dev(d) {}

        	virtual void trigger (__attribute__ ((unused)) AlarmClock& clock) 
			{
          		tick = delay();
          		sysclock.add(*this);
				sensors[0] 	= mqtt->mTemp1.toInt();
				humidity[0]	= mqtt->mHum1.toInt();
				sensors[1]	= mqtt->mTemp2.toInt();
				humidity[1]	= mqtt->mHum2.toInt();
				sensors[2]	= mqtt->mTemp3.toInt();
				humidity[2]	= mqtt->mHum3.toInt();

          		WeatherEventMsg& msg = (WeatherEventMsg&)dev.message();
          		msg.init(dev.nextcount(), sensors, humidity);
				//dev.sendPeerEvent(msg, *this);
				dev.broadcastEvent(msg);
          		//dev.send(msg, dev.getMasterID());
				DPRINT("SENDE");
        	}

        	uint32_t delay () 
			{
          		uint16_t _txMindelay = 180;
          		_txMindelay = dev.getList0().Sendeintervall();
          		if (_txMindelay == 0) _txMindelay = 180;
          		return seconds2ticks(_txMindelay);
        	}

    } sensarray;
	

	public:
    	typedef MultiChannelDevice<Hal, WeatherChannel, MAX_SENSORS, UList0> TSDevice;
    	UType(const DeviceInfo& info, uint16_t addr) : TSDevice(info, addr), sensarray(*this) {}
    	virtual ~UType () {}

    	virtual void configChanged () {
      	TSDevice::configChanged();
      	DPRINTLN("Config Changed List0");
      	DPRINT("Sendeintervall: "); DDECLN(this->getList0().Sendeintervall());
    }

    void init (Hal& hal) 
	{
    	TSDevice::init(hal);
      	sensarray.set(seconds2ticks(5));
      	sysclock.add(sensarray);
    }
};

UType sdev(devinfo, 0x20);
ConfigButton<UType> cfgBtn(sdev);

void setup () 
{
	DINIT(57600,ASKSIN_PLUS_PLUS_IDENTIFIER);
	sdev.init(hal);
  	buttonISR(cfgBtn, CONFIG_BUTTON_PIN);
  	sdev.initDone();

	files->setup();
	wlan->setup();
	mqtt->setup();
}

void loop() 
{
	wlan->loop();
	if(wlan->ready)
  	{
    	websrv->start(80);
    	websrv->loop();
  	}
  	else
  	{
    	websrv->stop();
	}

	if(wlan->ready && !wlan->apStart)
	{
		mqtt->loop();
	}
	bool worked = hal.runready();
  	bool poll = sdev.pollRadio();
}
#pragma once
#include <xsensdeviceapi.h>
#include <iostream>
#include <list>
#include "XSensCallbackHandler.hpp"

#include<thread>

extern std::string resultFolder;

class XSensGPS
{
private:
	//XsDevice * m_device;
	const XsPortInfo* m_portInfo;
	mutable xsens::Mutex  m_mutex;
	std::string fname;
	size_t m_maxNumberOfPacketsInBuffer;
	size_t m_numberOfPacketsInBuffer;
	std::list<XsDataPacket> m_packetBuffer;
protected:
	
	//virtual void onLiveDataAvailable(XsDevice*, const XsDataPacket* packet);
public:

	XsDevice * m_device;
    XSensCallbackHandler* m_CallBackHandler;

	~XSensGPS();
	XSensGPS(XsDevice* device, const XsPortInfo* portInfo, size_t);

	void addCallback();
	int openLogFile();
	XsDataPacket popOldestPacket();
	XsDataPacket getLatestPacket();
	bool configure();
	bool gotoMeasurement();
	bool packetAvailable() const;
	void operator()();
	
};


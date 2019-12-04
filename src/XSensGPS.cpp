
#include "XSensGPS.hpp"

using namespace std;

auto static handleError = [](string errorString)
{

    cout << errorString << endl;
    cout << "Press [ENTER] to continue." << endl;
    cin.get();
    return -1;
};

XSensGPS::XSensGPS(XsDevice* device, const XsPortInfo* portInfo, size_t maxBufferSize = 10) : m_maxNumberOfPacketsInBuffer(maxBufferSize), m_numberOfPacketsInBuffer(0) {
	this->m_device = device;
	this->m_portInfo = portInfo;
	
	/*XsDeviceCallbackHandler m_CallBackHandler(5);
	this->m_device->addCallbackHandler(&m_CallBackHandler);*/
	this->configure();
	

	std::cout << "Device:" + this->m_device->deviceId().toString().toStdString() + " is successfully configured." << std::endl;
    ostringstream os;
    //time_t now = time(0);
    //tm *ltm = localtime(&now);
    ////char dt[40];
    //strftime(dt, sizeof(dt), "%F_%H_%M", ltm);
    int64_t startTime = XsTime::timeStampNow();
    os << "GPS_" << this->m_device->deviceId().toString().toStdString() <<"_"<<startTime<< ".mtb";
    this->fname=os.str();
//    string logFileName = "logfile.mtb";


}

void XSensGPS::addCallback()
{
	
	

//	os << "GPS_" << this->m_device->deviceId().toString().toStdString() << ".txt";

    XSensCallbackHandler* tmp=new XSensCallbackHandler();
	m_CallBackHandler = tmp;
	m_device->addCallbackHandler(tmp);
}

XsDataPacket XSensGPS::getLatestPacket() {
    return m_CallBackHandler->getLatestPacket();
}
XsDataPacket XSensGPS::popOldestPacket() {
	/*XsMutexLocker lock(m_mutex);
	XsDataPacket oldestPacket(m_packetBuffer.front()); 
	m_packetBuffer.pop_front(); 
	--m_numberOfPacketsInBuffer; 
	return oldestPacket; */
	return m_CallBackHandler->getNextPacket();
}
// Lambda function for error handling

bool XSensGPS::configure()
{
    // Put the device into configuration mode before configuring the device
    cout << "Putting device into configuration mode..." << endl;
    if (!m_device->gotoConfig())
        return handleError("Could not put device into configuration mode. Aborting.");

    cout << "Configuring the device..." << endl;
    XsOutputConfigurationArray configArray;
    configArray.push_back(XsOutputConfiguration(XDI_PacketCounter, 0));
    configArray.push_back(XsOutputConfiguration(XDI_SampleTimeFine, 0));

    if (m_device->deviceId().isImu())
    {
        configArray.push_back(XsOutputConfiguration(XDI_DeltaV, 0));
        configArray.push_back(XsOutputConfiguration(XDI_DeltaQ, 0));
        configArray.push_back(XsOutputConfiguration(XDI_MagneticField, 0));
    }
    else if (m_device->deviceId().isVru() || m_device->deviceId().isAhrs())
    {
        configArray.push_back(XsOutputConfiguration(XDI_Quaternion, 0));
    }
    else if (m_device->deviceId().isGnss())
    {
        configArray.push_back(XsOutputConfiguration(XDI_Quaternion, 0));
        configArray.push_back(XsOutputConfiguration(XDI_LatLon, 0));
        configArray.push_back(XsOutputConfiguration(XDI_AltitudeEllipsoid, 0));
        configArray.push_back(XsOutputConfiguration(XDI_VelocityXYZ, 0));
    }
    else
    {
        return handleError("Unknown device while configuring. Aborting.");
    }

    if (!m_device->setOutputConfiguration(configArray))
        return handleError("Could not configure MTi device. Aborting.");

    return true;
}

bool XSensGPS::gotoMeasurement()
{
	if (!m_device->gotoMeasurement())
	{
		throw std::runtime_error("Could not put device into measurement mode. Aborting.");
	}
	return true;
}
bool XSensGPS::packetAvailable() const
{
	
		//XsMutexLocker lock(m_mutex);
		return m_CallBackHandler->packetAvailable();
	
}
void XSensGPS::operator()()
{
	std::cout << "from thread" << std::endl;
}
XSensGPS::~XSensGPS()
{
    m_device->closeLogFile();

}

int XSensGPS::openLogFile() {
    if (m_device->createLogFile(this->fname) != XRV_OK)
        return handleError("Failed to create a log file. Aborting.");
    else
        cout << "Created a log file: " << this->fname.c_str() << endl;
    return 1;
}

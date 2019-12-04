//
// Created by saurav on 04/11/2019.
//

#ifndef UASL_IMAGE_ACQUISITION_XSENSGPSCONTROLLER_H
#define UASL_IMAGE_ACQUISITION_XSENSGPSCONTROLLER_HPP

#include "XSensGPS.hpp"
typedef std::vector<XSensGPS*> ListOfGPS;
typedef ListOfGPS::iterator GPSIterator;
typedef std::map<XsDevice *, XsPortInfo const *> DeviceInfo;
typedef DeviceInfo::iterator DeviceInfoIterator;

class XSensGPSController {
private:
    DeviceInfo gpsDevices;

    GPSIterator itr;
    DeviceInfoIterator pos;
    XsControl* control;
public:
    ListOfGPS gpsList;
//    Search for Xsens GPS_enabled devices and create object for each of them.
// Most probably there is only one connected
    bool initGPS();
    bool startMeasurement();
    bool startRecording();
    bool stopRecording();
    ~XSensGPSController();
};


#endif //UASL_IMAGE_ACQUISITION_XSENSGPSCONTROLLER_H

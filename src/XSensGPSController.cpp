//
// Created by saurav on 04/11/2019.
//

#include "XSensGPSController.hpp"

bool XSensGPSController::initGPS() {
    try {
        control = XsControl::construct();
        assert(control != 0);
        std::cout << "XsControl object constructed" << std::endl;
        // Scan for connected devices
        std::cout << "Scanning for devices..." << std::endl;
        XsPortInfoArray portInfoArray = XsScanner::scanPorts();
        size_t deviceCounter = 0;
        XsDevice* detectedDevice;
        for (XsPortInfoArray::const_iterator portInfoIter = portInfoArray.begin();
             portInfoIter != portInfoArray.end(); ++portInfoIter) {
            ++deviceCounter;
            if (!control->openPort(portInfoIter->portName(), portInfoIter->baudrate())) {
                throw std::runtime_error("Error opening port " + portInfoIter->portName().toStdString());
            }
            XsPortInfo const  portInfo = *portInfoIter;
            detectedDevice = control->device(portInfoIter->deviceId());
            //gpsDevices.insert(DeviceInfo::value_type(detectedDevice, &portInfo));
            XSensGPS *gps = new XSensGPS(detectedDevice, &portInfo, 10);
            gps->openLogFile();
            gps->addCallback();
            gpsList.push_back(gps);
            std::cout << "All the devices detected are configured" << std::endl;
        }

    }
    catch (std::runtime_error err) {
        std::cout << "Cannot start GPS_enabled" << std::endl;
        return false;
    }
    return true;
}

bool XSensGPSController::startMeasurement() {
    try {
        for(GPSIterator gpsIterator=gpsList.begin(); gpsIterator!=gpsList.end();++gpsIterator){
            (*gpsIterator)->gotoMeasurement();
        }

    }catch (std::runtime_error err){
        std::cout<<"Cannot goto measurement"<<std::endl;
        return false;
    }


    return true;
}
bool XSensGPSController::startRecording() {
    try {
        for(GPSIterator gpsIterator=gpsList.begin(); gpsIterator!=gpsList.end();++gpsIterator){
            (*gpsIterator)->m_device->startRecording();
        }

    }catch (std::runtime_error err){
        std::cout<<"Cant start recording"<<std::endl;
        return false;
    }


    return true;
}
bool XSensGPSController::stopRecording() {
    try {
        for(GPSIterator gpsIterator=gpsList.begin(); gpsIterator!=gpsList.end();++gpsIterator){
            (*gpsIterator)->m_device->stopRecording();
        }

    }catch (std::runtime_error err){
        std::cout<<"Cannot stop Recording "<<std::endl;
        return false;
    }


    return true;
}

XSensGPSController::~XSensGPSController() {
    try {
        for(GPSIterator gpsIterator=gpsList.begin(); gpsIterator!=gpsList.end();++gpsIterator){
            (*gpsIterator)->m_device->stopRecording();
            (*gpsIterator)->m_device->closeLogFile();
        }
//        for (XsPortInfoArray::const_iterator portInfoIter = portInfoArray.begin();
//             portInfoIter != portInfoArray.end(); ++portInfoIter) {
//
//        }
    control->destruct();

    }catch (std::runtime_error err){
        std::cout<<"Cannot stop Recording "<<std::endl;

    }

}

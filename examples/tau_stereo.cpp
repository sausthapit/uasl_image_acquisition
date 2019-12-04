//
// Created by saurav on 04/11/2019.
//
#include <iostream>
#include <fstream>
#include "XSensGPS.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
//#include <getopt.h>
#include "acquisition.hpp" //The main class
#include "camera_tau2.hpp" //You need the header corresponding to the specific camera you are using
#include "util_signal.hpp" //For the signal handling
#include "XSensGPSController.hpp"
//#define GPS_enabled

struct{

    std::string dir;
    bool save=false;
    bool trigger= false;
    bool equalise=false;
}params;

#define IMAGE_FORMAT CV_16U

int main(int argc, char** argv) {
    char c;
    while ((c = getopt(argc, argv, "etsd:")) != -1) {
        switch (c) {
            case 'd':
                params.dir = optarg;
                break;
            case 's':
                params.save = true;
                break;
            case 't':
                params.trigger=true;
                break;
            case 'e':
                params.equalise= true;
                break;
            default:
                std::cout << "Invalid option: -" << c << std::endl;
                return 1;
        }
    }

    if (params.save && params.dir.empty()) {
        std::cerr << "please provide directory (using option -d \"pathToDir\")" << std::endl;
        exit(-1);
    }

    //saving image timestamps

    std::ofstream fimage;

    if (params.save) {
        fimage.open(params.dir + "/image_data.csv");

    }
    cam::SigHandler sig_handle;

    cam::Acquisition acq;
//    acq.add_camera<cam::tau2>();


    acq.add_camera<cam::tau2>("FT2HKAW5");
    acq.add_camera<cam::tau2>("FT2HK5UG");


    for (int camera=0;camera<2;++camera) {
        dynamic_cast<cam::Tau2Parameters &>(acq.get_cam_params(camera)).set_pixel_format(IMAGE_FORMAT);
        dynamic_cast<cam::Tau2Parameters &>(acq.get_cam_params(camera)).set_image_roi(0, 16, 640, 480);
    }
    std::vector<cv::Mat> img_vec;

#ifdef GPS_enabled
    XSensGPSController gpsController;
    if(gpsController.initGPS()) {
        gpsController.startMeasurement();
        gpsController.startRecording();
    }
#endif
//start acquisition
    acq.setUseTrigger(params.trigger);
    if(params.trigger)
    {
        acq.set_trigger_port_name("/dev/ttyTRIGGER");
    }

    acq.start_acq();

    unsigned int img_nb = 0;
    while(sig_handle.check_term_sig() && acq.is_running())
    {
        int64_t ret_acq = acq.get_images(img_vec);
        if(ret_acq > 0)
        {

            std::string img_nb_s = std::to_string(img_nb);
            for(unsigned i = 0;i<img_vec.size();++i)
            {
                cv::Mat img = img_vec[i].clone();
                if(params.equalise)
				{ // equalisation for infrared image
                    double m = 20.0/64.0;
                    double mean_img = cv::mean(img)[0];
                    img = img * m + (127-mean_img* m);
                    img.convertTo(img, IMAGE_FORMAT);
				}
                if(params.save)
                    cv::imwrite(params.dir+"/cam"+std::to_string(i)+"_image"+std::string(5-img_nb_s.length(),'0')+img_nb_s+".png", img);
                else
                    cv::imshow("cam "+std::to_string(i),img);
            }
            if(fimage.is_open())
                fimage << img_nb_s <<  "," << ret_acq;
#ifdef GPS_enabled
            if(gpsController.gpsList[0]->packetAvailable())
            {
                XsDataPacket packet =gpsController.gpsList[0]->getLatestPacket();
                if(fimage.is_open())
                    fimage <<","<<packet.sampleTime64();
                else
                    std::cout << " Time:" << packet.sampleTime64();
                if (packet.containsLatitudeLongitude())
                {
                    XsVector latLon = packet.latitudeLongitude();

                    if(fimage.is_open())
                        fimage <<","<<latLon[0]<<","<<latLon[1];
                    else
                        std::cout << " |Lat:" << latLon[0]
                                  << ", Lon:" << latLon[1];

                }
                if(packet.containsUtcTime()){
                    XsTimeInfo xsTimeInfo=packet.utcTime();
                    std::cout<<"Time:  "<<xsTimeInfo.m_year<<"/"<<xsTimeInfo.m_month<<"/"<<xsTimeInfo.m_day<<"|"<<xsTimeInfo.m_hour<<":"<<xsTimeInfo.m_minute<<":"<<xsTimeInfo.m_second;
                }
//                std::cout<<"Time:"<<packet.sampleTime64();


            }
#endif
            if(fimage.is_open())
                fimage <<std::endl;
            else
//                std::cout<<std::endl;
            cv::waitKey(1);
            img_nb++;
        }
        else
            std::cerr << "[Error] could not retreive images" << std::endl;
    }
#ifdef GPS_enabled
    gpsController.stopRecording();

#endif
    if(fimage.is_open())
        fimage.close();

    return 0;
}
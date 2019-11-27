#ifndef UASL_IMAGE_ACQUISITION_CAMERA_SEQUENTIAL_HPP
#define UASL_IMAGE_ACQUISITION_CAMERA_SEQUENTIAL_HPP

#include "cond_var_package.hpp"
#include <memory>
#include <string>

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2
#include <opencv2/core/core.hpp>
#elif CV_MAJOR_VERSION >= 3
#include <opencv2/core.hpp>
#endif

namespace cam
{

enum CameraType {bluefox,tau2};

class Camera_params
{
	public:
	Camera_params(Cond_var_package& package_) : package(package_) {}
	virtual ~Camera_params() {}
	
	protected:
	Cond_var_package& package;
};

//Note : if your camera has parameters, you should pass a Cond_var_package& to this struct, this can be done by passing it to the constructor of Camera_seq
class Camera_seq
{
	public:
    virtual ~Camera_seq() {}
    virtual int retrieve_image(cv::Mat& img) = 0;
	virtual int start_acq(bool only_one_camera) = 0;
    virtual int stop_acq() = 0;    
    virtual Camera_params& get_params() = 0; 
    
    template <CameraType T> 
    static std::unique_ptr<Camera_seq> get_instance(Cond_var_package& package, const std::string& cam_id)
    {
    	return nullptr;				
    }    
}; //class Camera_seq

} //namespace cam

#endif

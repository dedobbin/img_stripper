#ifndef ___IMG_STRIPPER__
#define ___IMG_STRIPPER__

#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
 #include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

extern "C" {
	#include <wav_hammer/wav_hammer.h>
}

Raw_wave* mat_to_wav(cv::Mat img);
cv::Mat wav_to_mat(Raw_wave* wav, int w, int h);
bool save_mat_as_wav(cv::Mat img, std::string output_path);
bool save_wav_as_img(Raw_wave* wav, int w, int h, std::string output_path);

#endif
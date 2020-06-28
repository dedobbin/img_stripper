#include "errors.hpp"
#include "img_stripper.hpp"

std::string mat_str(cv::Mat img)
{
	return std::to_string(img.cols) + "x" + std::to_string(img.rows) + " pixels";
}

std::string wav_str(Raw_wave* wav)
{
	return "bitdepth: " + std::to_string(bits_per_sample(wav)) + ", " + std::to_string(num_channels(wav)) + " channel(s), " + std::to_string(num_samples(wav)) + " samples";
}

Raw_wave* mat_to_wav(cv::Mat img)
{
	int img_channels = 3;
	Raw_wave* w = create_header(41100, 24, 1);
	uint8_t* buffer = (uint8_t*)malloc(img.cols * img.rows * img_channels);	//expects 24bit pixels

	uint8_t* ptr = buffer;
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			cv::Vec3b bgr = img.at<cv::Vec3b>(i,j);
			memcpy(ptr, &bgr, img.channels());
			ptr += img_channels;
		}
	}
	w->data_chunk->audiodata = buffer;
	set_datasize(w, img.cols * img.rows);
	return w;
}

cv::Mat wav_to_mat(Raw_wave* wav, int w, int h)
{
	cv::Mat img(h, w, CV_8UC3, wav->data_chunk->audiodata);
	return img;
}

bool save_mat_as_wav(cv::Mat img, std::string output_path)
{
	//In a perfect world this would be dynamic but then should get rid of Vec3b in this function
	int img_channels = 3;
	Raw_wave* wav = create_header(41100, 24, 1);
 
	uint8_t* buffer = (uint8_t*)malloc(img.cols * img.rows * img_channels);	//expects 24bit pixels
	uint8_t* ptr = buffer;
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			cv::Vec3b bgr = img.at<cv::Vec3b>(i,j);
			memcpy(ptr, &bgr, img.channels());
			ptr += img_channels;
		}
	}

	wav->data_chunk->audiodata = buffer;
	set_datasize(wav, img.cols * img.rows * 3);

	std::cout << "Input: img (" << mat_str(img) << ")" << std::endl; 
	std::cout << "Ouput: wav (" << wav_str(wav) << ")" << std::endl;

	return write_wave(wav, output_path.c_str()) > 0;
}

bool save_wav_as_img(Raw_wave* wav, int w, int h, std::string output_path)
{	
	if (bits_per_sample(wav) / 8 != 3){
		std::cout << "Currently only supports wavs with 24bit samples" << std::endl;
		exit(ERROR_UNSUPPORTED_WAV);
	}

	int wav_data_size = datasize(wav);
	int expected_data_size = w * h * (bits_per_sample(wav) / 8);
	if (wav_data_size < expected_data_size){
		if (wav_data_size < w * 3){
			std::cout << "Not enough data for image size,try smaller width" << std::endl;
		} else {
			int proposed_h = wav_data_size / (w * 3);
			std::cout << "Not enough data for image size, try height " << proposed_h << " or smaller width" << std::endl;
		}
		exit(ERROR_INVALID_IMAGE_DIMENSIONS);
	} else if (wav_data_size > expected_data_size){
		std::cout << "Too much data for image size, "<< wav_data_size - expected_data_size << " bytes lost" << std::endl;
	}

	cv::Mat img = cv::Mat(h, w, CV_8UC3, wav->data_chunk->audiodata);
	
	std::cout << "Input: wav (" << wav_str(wav) << ")" << std::endl;
	std::cout << "Output: img (" << mat_str(img) << ")" << std::endl; 

	return cv::imwrite(output_path, img);
}
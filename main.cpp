#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
 #include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <regex>
#include <unordered_map>
#include <string>
#include <algorithm>



#define ERROR_INVALID_PARAMS 1
#define MALFORMED_DIMENSION_PARAM 2
#define UNSUPPORTED_TYPE 3
#define UNSUPPORTED_WAV 4
#define INVALID_IMAGE_DIMENSIONS 5

extern "C" {
	#include <wav_hammer/wav_hammer.h>
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


static std::vector<std::string> validImgExtensions = {"jpg", "jpeg", "png", "tiff"};

static std::string wav_extension = ".wav";


enum param_type_enum {
	UNSUPPORTED,
	IMG,
	WAV,
};

std::unordered_map <param_type_enum, std::string> param_type_str ={
	{param_type_enum::IMG, "image"},
	{param_type_enum::WAV, "wav"},
};


struct param_types_t{
	param_type_enum input = param_type_enum::UNSUPPORTED;
	param_type_enum output = param_type_enum::UNSUPPORTED;
};


bool has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}


std::string to_lower(std::string in)
{
	std::string out = "";
	for (auto c : in){
		out+= std::tolower(c);
	}
	return out;
}


param_types_t determine_param_types(std::string input_path, std::string output_path)
{
	std::string lc_input =  to_lower(input_path);
	std::string lc_output =  to_lower(output_path);

	//Default to unsupported, so if not overwritten, input is not recognized as supported type
	param_types_t param_types = {param_type_enum::UNSUPPORTED, param_type_enum::UNSUPPORTED};	

	for (auto extension : validImgExtensions){ 
		if (has_suffix(lc_input, extension)){
			param_types.input = param_type_enum::IMG;
		}

		if (has_suffix(lc_output, extension)){
			param_types.output = param_type_enum::IMG;
		}
	}

	if (has_suffix(lc_input, wav_extension)){
		param_types.input = param_type_enum::WAV;
	}

	if (has_suffix(lc_output, wav_extension)){
		param_types.output = param_type_enum::WAV;
	}

	return param_types;
}

std::string mat_str(cv::Mat img)
{
	return std::to_string(img.cols) + "x" + std::to_string(img.rows) + " pixels";
}

std::string wav_str(Raw_wave* wav)
{
	return "bitdepth: " + std::to_string(bits_per_sample(wav)) + ", " + std::to_string(num_channels(wav)) + " channel(s), " + std::to_string(num_samples(wav)) + " samples";
}

void save_mat_as_wav(cv::Mat img, std::string output_path)
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

	write_wave(wav, output_path.c_str());
}

void save_wav_as_img(Raw_wave* wav, int w, int h, std::string output_path)
{	
	if (bits_per_sample(wav) / 8 != 3){
		std::cout << "Currently only supports wavs with 24bit samples" << std::endl;
		exit(UNSUPPORTED_WAV);
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
		exit(INVALID_IMAGE_DIMENSIONS);
	} else if (wav_data_size > expected_data_size){
		std::cout << "Too much data for image size, "<< wav_data_size - expected_data_size << " bytes lost" << std::endl;
	}

	cv::Mat img = cv::Mat(h, w, CV_8UC3, wav->data_chunk->audiodata);
	
	std::cout << "Input: wav (" << wav_str(wav) << ")" << std::endl;
	std::cout << "Output: img (" << mat_str(img) << ")" << std::endl; 

	cv::imwrite(output_path, img);
}

std::pair<int, int> get_dimensions(std::string in)
{
	std::string x, y;
	try {
		std::regex re("([0-9]+)(x)([0-9]+)");
		std::smatch match;
		if (std::regex_search(in, match, re) && match.size() > 1) {
			x = match.str(1);
			y = match.str(3);
		}
	} catch (std::regex_error& e) {
		std::cout << "Malformed image dimension param" << std::endl;
		exit(MALFORMED_DIMENSION_PARAM);
	}

	std::pair<int, int> res = {0, 0};
	try {
		res = {stoi(x) , stoi(y)};
	} catch (const std::invalid_argument&) {
		std::cout << "Malformed image dimension param" << std::endl;
		exit(MALFORMED_DIMENSION_PARAM);
	}

	return res;
}

int main(int argc, char* argv[])
{	
	//TODO: when loading and storing check if succeeded
	if (argc < 3){
		std::cout << "Use params  'input file' 'output file'" << std::endl;
		std::cout << "If output is image, also provide dimensions in format 'wxh' (example: input.wav output.img 320x640)";
		return ERROR_INVALID_PARAMS;
	}

	std::string input_path = argv[1];
	std::string output_path = argv[2];
	
	param_types_t param_types = determine_param_types(input_path, output_path);

	if (param_types.input == param_types.output){
		std::cout << "Invalid params, input and output is " << param_type_str.at(param_types.input) << std::endl;
		exit(ERROR_INVALID_PARAMS);
	}

	std::pair<int, int> dimensions; 
	if (param_types.output == param_type_enum::IMG){
		if (argc < 4){
			std::cout << "also provide dimensions in format 'wxh' (example: input.wav output.img 320x640)";
			exit(ERROR_INVALID_PARAMS);
		}
		dimensions = get_dimensions(argv[3]);

	}

	if (param_types.input == param_type_enum::UNSUPPORTED || param_types.output == param_type_enum::UNSUPPORTED){
		std::cout << "Unsupported input type (based on file extensions)" << std::endl;
		exit(UNSUPPORTED);
	}

	cv::Mat img;
	Raw_wave* wav = NULL;
	
	if (param_types.input == param_type_enum::IMG){
		img = cv::imread(input_path);
	} else if (param_types.input == param_type_enum::WAV){
		load_wave(&wav, input_path.c_str());
	}

	if (param_types.output == param_type_enum::WAV){
		save_mat_as_wav(img, output_path);
	} else if (param_types.output == param_type_enum::IMG){
		save_wav_as_img(wav, dimensions.first, dimensions.second, output_path);
		}

	destroy_wave(&wav);
}
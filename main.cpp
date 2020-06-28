#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <unordered_map>
#include <string>
#include "errors.hpp"
#include "params.hpp"
#include "img_stripper.hpp"

int main(int argc, char* argv[])
{	
	cv::Mat img;
	Raw_wave* wav = NULL;
	
	input_t input = parse_params(argc, argv);
	std::string input_path = input.input_path;
	std::string output_path = input.output_path;
	param_types_t param_types = input.param_types;
	std::pair<int, int> output_image_dimensions = input.output_image_dimensions;

	bool load_success = false;
	if (param_types.input == param_type_enum::IMG){
		img = cv::imread(input_path);
		load_success = !img.empty();
	} else if (param_types.input == param_type_enum::WAV){
		load_success = load_wave(&wav, input_path.c_str()) > 0;
	}

	if (!load_success){
		std::cout << "Failed to load " << input_path << std::endl;
		exit(ERROR_LOAD_MEDIA_FAIL);
	}

	bool write_succes = false;
	if (param_types.output == param_type_enum::WAV){
		write_succes = save_mat_as_wav(img, output_path);
	} else if (param_types.output == param_type_enum::IMG){
		write_succes = save_wav_as_img(wav, output_image_dimensions.first, output_image_dimensions.second, output_path);
	}

	if (!write_succes){
		std::cout << "Failed to write " << output_path << std::endl;
		exit(ERROR_WRITE_MEDIA_FAIL);
	}

	destroy_wave(&wav);
	return 0;
}
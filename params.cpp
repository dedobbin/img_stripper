#include <regex>
#include <iostream>

#include "errors.hpp"
#include "params.hpp"
#include "helpers.hpp"

static std::vector<std::string> validImgExtensions = {".jpg", ".jpeg", ".png", ".tiff"};

static std::string wav_extension = ".wav";


bool has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
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
		exit(ERROR_MALFORMED_DIMENSION_PARAM);
	}

	std::pair<int, int> res = {0, 0};
	try {
		res = {stoi(x) , stoi(y)};
	} catch (const std::invalid_argument&) {
		std::cout << "Malformed image dimension param" << std::endl;
		exit(ERROR_MALFORMED_DIMENSION_PARAM);
	}

	return res;
}

input_t parse_params(int argc, char* argv[])
{
	if (argc < 3){
		std::cout << "Use params  'input file' 'output file'" << std::endl;
		std::cout << "If output is image, also provide dimensions in format 'wxh' (example: input.wav output.img 320x640)";
		exit(ERROR_INVALID_PARAMS);
	}

	std::string input_path = argv[1];
	std::string output_path = argv[2];
	
	param_types_t param_types = determine_param_types(input_path, output_path);

	if (param_types.input == param_type_enum::UNSUPPORTED || param_types.output == param_type_enum::UNSUPPORTED){
		std::cout << "Unsupported input type (based on file extensions): ";
		if (param_types.input == param_type_enum::UNSUPPORTED  && param_types.output == param_type_enum::UNSUPPORTED){
			std::cout << input_path << ", " << output_path << std::endl;
		} else if (param_types.input == param_type_enum::UNSUPPORTED) {
			std::cout << input_path << std::endl;
		} else if (param_types.output == param_type_enum::UNSUPPORTED){
			std::cout << output_path << std::endl;
		}
		exit(ERROR_UNSUPPORTED_TYPE);
	}

	if (param_types.input == param_types.output){
		std::cout << "Invalid params, input and output is same media type (based on extensions)" << std::endl;
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

	return {input_path, output_path, param_types, dimensions};
}

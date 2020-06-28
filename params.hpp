#ifndef __PARAMS_HPP__
#define __PARAMS_HPP__

#include <vector>
#include <string>

enum param_type_enum {
	UNSUPPORTED,
	IMG,
	WAV,
};

struct param_types_t{
	param_type_enum input = param_type_enum::UNSUPPORTED;
	param_type_enum output = param_type_enum::UNSUPPORTED;
};

struct input_t {
	std::string input_path;
	std::string output_path;
	param_types_t param_types;
	std::pair<int, int> output_image_dimensions;
};

/**
 * Returns struct of input params.
 * output_image_dimensions is only set when output is image
 */
input_t parse_params(int argc, char* argv[]);

#endif
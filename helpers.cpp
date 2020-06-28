#include "helpers.hpp"

std::string to_lower(std::string in)
{
	std::string out = "";
	for (auto c : in){
		out+= std::tolower(c);
	}
	return out;
}
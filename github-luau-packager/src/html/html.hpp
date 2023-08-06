/*
*	html.hpp
* 
*	hierarchy-like html parser
*/

#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace html
{

struct element
{
	std::string tag_name;
	element* parent;
	
};

} // namespace html


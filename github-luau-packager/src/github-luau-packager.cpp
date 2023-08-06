/*
*	github-luau-packager.cpp
* 
*	packager
*/

#include "./allocator/allocator.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include <algorithm>

std::string replace_all( std::string str, const std::string& from, const std::string& to )
{
	std::string result;
	result.reserve( str.length() );

	std::string::size_type lastPos = 0;
	std::string::size_type findPos;

	while ( std::string::npos != ( findPos = str.find( from, lastPos ) ) )
	{
		result.append( str, lastPos, findPos - lastPos );
		result += to;
		lastPos = findPos + from.length();
	}

	// Care for the rest after last occurrence
	result += str.substr( lastPos );

	return result;
}

std::string parse_path( allocator& allocator, std::filesystem::path path, uint32_t depth )
{
	const std::string indent = std::string( depth, '\t' );

	std::stringstream stream;

	stream << std::vformat( "{:s}[\"{:s}\"] = {:s}\n", std::make_format_args( indent, path.filename().string(), "{" ) );

	for ( const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator( path ) )
	{
		if ( entry.is_directory() )
		{
			stream << std::vformat( "{:s}", std::make_format_args( parse_path( allocator, entry.path(), depth + 1 ) ) );
		}
		else if ( entry.is_regular_file() )
		{
			if ( entry.path().extension() != ".lua" )
				continue;

			std::ifstream file{ entry.path().string() };

			const size_t size = entry.file_size();

			char* memory = reinterpret_cast<char*>( allocator.allocate( size ) );

			file.read( memory, size );

			std::string content = std::string{ memory, static_cast<size_t>( size ) };

			stream << std::vformat( "\n{:s}[\"{:s}\"] = function()\n{:s}{:s}\n{:s}end,\n", std::make_format_args( indent, entry.path().filename().generic_string(), std::string( depth + 1, '\t' ), replace_all( content, "\n", std::string( "\n" ) + std::string( depth + 1, '\t' ) ), indent ) );
		}
	}

	stream << std::vformat( "{:s}{:s}\n", std::make_format_args( indent, "}," ) );

	return stream.str();
};


int main()
{
	allocator allocator;

	const std::string parsed = parse_path( allocator, std::filesystem::current_path(), 0 );

	std::cout << parsed << std::endl;

	std::cin.get();
}


/*
*	parser.hpp
* 
*	converts path into code
*/

#pragma once

#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <format>
#include <string>

namespace parser
{

std::string main_file_buffer = {};

void replace_all( std::string& source, const std::string& from, const std::string& to )
{
	std::string::size_type position = 0u;

	while ( ( position = source.find( from, position ) ) != std::string::npos )
	{
		source.replace( position, from.length(), to );
		position += to.length();
	}

	return;
}

template <typename... Ty>
std::string fmt( const std::string& fmt, Ty&&... args )
{
	return std::vformat( fmt, std::make_format_args( std::forward<Ty>( args )... ) );
}

std::string parse( const std::filesystem::path& path, const std::unordered_set<std::string>& extentions, int depth )
{
	// make code look good
	const auto create_indent = [&]( int depth ) -> std::string
	{
		return std::string( depth, '\t' );
	};

	std::stringstream stream;

	stream << "{\n";

	for ( const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator( path ) )
	{
		if ( entry.path().filename().string() == "output.lua" || entry.path().filename().string() == ".git" )
			continue;

		if ( entry.is_directory() )
		{
			const std::string children = parse( entry.path(), extentions, depth + 1 );

			stream << fmt( "{:s}[\"{:s}\"] = {:s},\n", create_indent( depth + 1 ), entry.path().filename().string(), children );

			continue; // just go to next
		}

		if ( extentions.find( entry.path().extension().string() ) == extentions.end() )
			continue;

		// atp we know we have a file
		std::ifstream file{ entry.path().string(), std::ios::binary };

		if ( !file.is_open() )
			continue; // skip

		const uintmax_t size = entry.file_size();
		
		// unique buffer
		auto buffer = std::make_unique<char[]>( size );

		// read file
		file.read( buffer.get(), size);

		// this is terrible
		std::string strbuffer = std::string( buffer.get(), size );

		if ( entry.path().filename().string() == "main.lua" )
		{
			main_file_buffer.swap( strbuffer );

			continue;
		}

		// indent file content
		replace_all( strbuffer, "\n", std::string( "\n" ) + create_indent( depth + 2 ) );

		stream << fmt( "{:s}[\"{:s}\"] = function()\n{:s}{:s}\n{:s}end,\n", create_indent( depth + 1 ), entry.path().filename().generic_string(), create_indent( depth + 2 ), strbuffer, create_indent( depth + 1 ) );
	}

	stream << create_indent( depth ) << "}";

	return stream.str();
}

} // namespace parser


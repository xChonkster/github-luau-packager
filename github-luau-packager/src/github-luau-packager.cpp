/*
*	github-luau-packager.cpp
* 
*	packager
*/

#include "./parser/parser.hpp"

#include <iostream>
#include <filesystem>

const char* message = R"(
Press [ENTER] to package the script
Press [CTRL + C] to exit this app
Code must contain a "main.lua" (loader) file

Tree name is GITHUB_LUAU_TREE
Import function name is GITHUB_LUAU_IMPORT
)";

const char* importer = R"(

local function GITHUB_LUAU_IMPORT(name)
	local value = GITHUB_LUAU_TREE

	for _, index in string.split(name, '/') do
		value = value[index]
	end

	if typeof(value) == "function" then
		value = value()
	end

	return value
end
)";

int main()
{
	std::unordered_set<std::string> extentions = { ".lua", ".ttf", ".txt" };
	const std::filesystem::path current_path = std::filesystem::current_path();

	// print instructions
	std::printf( "%s\nSupported extentions: ", message );

	for ( const std::string& extention : extentions )
		std::printf( "%s ", extention.c_str() );

	std::printf( "\n" );

	// loop
	while ( true )
	{
		std::cout << ">>> ";

		std::cin.get();

		const std::string hierarchy = parser::parse( current_path, extentions, 0 );

		const std::string final = parser::fmt( "local GITHUB_LUAU_TREE = {:s}{:s}\n{:s}\n", hierarchy, importer, parser::main_file_buffer );

		std::ofstream output{ current_path.string() + "\\output.lua", std::ios::binary };

		output.write( final.data(), final.length() );

		std::printf( "output written to \"output.lua\"\n" );
	}
}


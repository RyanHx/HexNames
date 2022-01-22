#include <iostream>
#include <filesystem>
#include <stdio.h>
#include <fstream>
#include <iterator>
#include <vector>

namespace fs = std::filesystem;

std::string extract_name(const std::string path, uint32_t offset)
{
	std::vector<unsigned char> bytes;
	std::ifstream input;

	input.open(path, std::ios::binary);
	if (input.fail())
	{
		std::cout << "Failed to open " << path << '\n';
		return "";
	}
	input.seekg(offset, std::ios::beg);
	while (!input.eof())
	{
		unsigned char byte;
		input >> byte;
		if (input.fail())
		{
			return "";
		}
		if (byte < 0x20 || byte > 0x7F) // range of ASCII printable characters (excluding extended)
		{
			break;
		}
		bytes.push_back(byte);
	}
	input.close();

	return std::string(bytes.begin(), bytes.end());
}

std::string replace_with_extract(const std::string path, const std::string name)
{
	fs::path file_path{ path };
	std::string ext{ file_path.extension().string() };
	std::string new_path{ file_path.replace_filename(name).string() };
	return new_path + ext;
}

int main(int argc, char* argv[])
{	
	uint32_t offset;
	std::cout << "Enter offset: ";
	std::cin >> std::hex >> offset;
	for (int i{ 1 }; i < argc; ++i)
	{
		if (fs::exists(argv[i]))
		{
			std::string extracted_name{ extract_name(argv[i], offset) };
			if (extracted_name.empty())
			{
				std::cout << "Failed to find name for " << argv[i] << '\n';
				continue;
			}
			fs::rename(argv[i], replace_with_extract(argv[i], extracted_name));
		}		
	}
	std::cout << "Operation finished." << '\n';
	static_cast<void>(getchar());
}
#include <iostream>
#include <filesystem>
#include <stdio.h>
#include <fstream>
#include <iterator>
#include <vector>

namespace fs = std::filesystem;

std::string extract_name(const fs::path path, uint32_t offset)
{
	std::vector<unsigned char> bytes;
	std::ifstream file_stream;

	file_stream.open(path, std::ios::binary);
	if (file_stream.fail())
	{
		std::cout << "Failed to open " << path << '\n';
		return "";
	}
	file_stream.seekg(offset, std::ios::beg);
	while (!file_stream.eof())
	{
		unsigned char byte;
		file_stream >> byte;
		if (file_stream.fail())
		{
			return "";
		}
		if (byte < 0x20 || byte > 0x7F) // range of ASCII printable characters (excluding extended)
		{
			break;
		}
		bytes.push_back(byte);
	}
	file_stream.close();

	return std::string(bytes.begin(), bytes.end());
}

int main(int argc, char* argv[])
{
	uint32_t offset;
	std::cout << "Enter offset: ";
	std::cin >> std::hex >> offset;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	for (int i{ 1 }; i < argc; ++i)
	{
		fs::path file_path{ argv[i] };
		if (!fs::exists(file_path))
			continue;

		std::string extracted_name{ extract_name(file_path, offset) };
		if (extracted_name.empty())
		{
			std::cout << "Failed to find name for " << file_path.string() << '\n';
			continue;
		}
		fs::path new_file_path{ file_path.replace_filename(extracted_name).string() + file_path.extension().string() };
		fs::rename(file_path, new_file_path);
	}

	std::cout << "Operation finished." << '\n';
	static_cast<void>(getchar());
}
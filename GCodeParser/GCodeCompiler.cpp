// GCodeParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include <charconv>
#include <iomanip>

#include "..\GCodeLib\inc\Lexer.h"
#include "..\GCodeLib\inc\Parser.h"
#include "..\GCodeLib\inc\Encoder.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: gcode_compiler <input.gcode> [output.bin]\n";
    }
    
    const std::filesystem::path inPath = argv[1];
    const std::filesystem::path outPath = argv[2];

    std::ifstream fin(inPath);
    if (!fin) 
    { 
        std::cerr << "Cannot open input file\n"; 
        return 1; 
    }

    std::ofstream fout(outPath, std::ios::binary);

    if (!fout) 
    {
        std::cerr << "Cannot open output file\n";
        return 1;
    }

    std::string line;
    std::size_t lineNo = 0;

    try
    {
        while (std::getline(fin, line))
        {
            ++lineNo;
            if (line.empty()) continue;

            Lexer lx{ line };
            Parser px{ lx.tokenize() };
            Encoder enc{ px.parse() };
            Decoder dec{ enc.encode() };
            GCodeCommand cmd{};
            dec.decode(cmd);

            auto t = px.parse();
            std::cout << t.letter << " " << t.code << " ";
            for (auto p : t.params)
            {
                std::cout << p.first << " " << p.second << " ";
            }

            std::cout << "Hex:";
            std::vector<std::byte> byteCode = enc.encode();
            for (std::byte b : byteCode)
            {
                std::cout << ' ' << std::hex << std::uppercase << std::setw(2)
                    << std::setfill('0') << std::to_integer<int>(b);
            }
            std::cout << "  (size = " << std::dec << byteCode.size() << ")";

			std::cout << " (Decoded: " << cmd.letter << cmd.code << " ";
			for (const auto& p : cmd.params)
			{
				std::cout << p.first << p.second << " ";
			}

            std::cout << ")" << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error at line " << lineNo << ": " << ex.what() << '\n';
        return 1;

    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

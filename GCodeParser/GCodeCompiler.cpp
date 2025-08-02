#include <iostream>
#include <fstream>
#include <filesystem>
#include <charconv>
#include <iomanip>

#include "..\GCodeLib\inc\Lexer.h"
#include "..\GCodeLib\inc\Parser.h"
#include "..\GCodeLib\inc\Encoder.h"

struct ParseError
{
	std::size_t lineNo;
	std::string message;
    std::string line;
};

void printGCodeLine(std::size_t lineNo, const GCodeCommand& ir, const std::vector<std::byte>& byteCode, const GCodeCommand& cmd) 
{
    std::cout << "N" << lineNo << " " << ir.letter << ir.code << " ";
    for (auto p : ir.params) {
        std::cout << p.first << p.second << " ";
    }
    std::cout << "Hex:";
    for (std::byte b : byteCode) {
        std::cout << ' ' << std::hex << std::uppercase << std::setw(2)
            << std::setfill('0') << std::to_integer<int>(b);
    }
    std::cout << " (size: " << std::dec << byteCode.size() << ")";
    std::cout << " (Decoded: " << cmd.letter << cmd.code << " ";
    for (const auto& p : cmd.params) {
        std::cout << p.first << p.second << " ";
    }
    std::cout << ") " << ir.comment.value_or("") << '\n';
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: gcode_compiler <input.gcode> [output.bin]\n";
    }
    
    const std::filesystem::path inPath = argv[1];
    const std::filesystem::path outPath = argv[2];

    std::ifstream fin(inPath);
    if (!fin) { 
        std::cerr << "Cannot open input file\n"; 
        return 1; 
    }

    std::ofstream fout(outPath, std::ios::binary);

    if (!fout) {
        std::cerr << "Cannot open output file\n";
        return 1;
    }

    std::string line;
    std::size_t lineNo = 0;
    int errorCount = 0;
	std::vector<ParseError> errors;
    bool break_on_error = false;

    while (std::getline(fin, line))
    {
        try
        {
            ++lineNo;

            if (line.empty()) 
                continue;

            Lexer lx{ line };

            std::vector<Token> tokens = lx.tokenize();

            Parser px{ tokens };

            GCodeCommand ir = px.parse();

            Encoder enc{ ir };

            std::vector<std::byte> byteCode = enc.encode();

            Decoder dec{ byteCode };
            
            GCodeCommand cmd = dec.decode();

#ifdef _DEBUG
			printGCodeLine(lineNo, ir, byteCode, cmd);
#endif

            if (ir.letter != '\0') {
                for (auto b : byteCode) {
                    fout.write(reinterpret_cast<const char*>(&b), sizeof(std::byte));
                }
                fout.flush();
            }
        }
        catch (const std::exception& ex) {
            errors.push_back({ lineNo, ex.what(), line });
            errorCount++;

			if (break_on_error) {
				std::cerr << "Breaking on error.\n";
				break;
			}

            continue;
        }
    }

	if (errorCount > 0) {
		std::cerr << "\n" << "Completed With Errors: " << errorCount << '\n';
		for (const auto& err : errors) {
			std::cerr << "Error: Line " << err.lineNo << ": " << err.message << " (" << err.line << ")" << '\n';
		}
	} else {
		std::cout << "\n" << "Compilation successful, no errors found.\n";
	}

    return 0;
}

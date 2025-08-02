#pragma once

#include "Compiler.h"

class Encoder
{
public:
	explicit Encoder(GCodeCommand cmd) : m_cmd(cmd) {}

	std::vector<std::byte> encode();
private:
	GCodeCommand m_cmd;
};

class Decoder
{
public:
	Decoder(std::vector<std::byte> data)
		: m_data(data), m_pos(0) {}

	GCodeCommand decode();
private:


	std::vector<std::byte> m_data;
	std::size_t m_pos;
};
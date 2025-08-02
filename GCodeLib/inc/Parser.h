#pragma once

#include "Compiler.h"

class Parser
{
public:
	explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) { }

	GCodeCommand parse();

private:
	bool optional(TokenKind k, std::size_t idx);
	void expect(TokenKind k, std::size_t idx, const char* msg);

	std::vector<Token> m_tokens;

};
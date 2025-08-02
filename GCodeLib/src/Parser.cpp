#include <vector>
#include <optional>
#include <unordered_map>
#include <stdexcept>

#include "..\inc\Parser.h"

GCodeCommand Parser::parse()
{
	GCodeCommand cmd{};
	std::size_t i = 0;

	if (optional(TokenKind::Program, i))
	{
		i++;
		if (optional(TokenKind::Star, i))
		{
			i++;
			expect(TokenKind::Checksum, i, "Missing Checksum Value");
			i++;
		}
		expect(TokenKind::EOL, i, "Expected Program EOL");
		cmd.is_program = true;
		return cmd;
	}

	if (optional(TokenKind::LineNumber, i))
	{
		i++;

		expect(TokenKind::Number, i, "Expected Line Number");
		cmd.lineNo = m_tokens[i].number;
		i++;
	}

	expect(TokenKind::Command, i, "Expected Command Letter");

	cmd.letter = m_tokens[i].letter;
	i++;

	expect(TokenKind::Number, i, "Expceted command number");

	cmd.code = static_cast<int>(m_tokens[i].number);
	i++;

	for (; i < m_tokens.size(); i++)
	{
		if (optional(TokenKind::EOL, i)) break;

		if (optional(TokenKind::Comment, i))
		{
			cmd.comment = std::string(m_tokens[i].slice);
			continue;
		}

		if (optional(TokenKind::Star, i))
		{
			//cmd.letter = '*';
			i++;
			expect(TokenKind::Checksum, i, "Expected Checksum (Given)");
			i++;
			break;
		}

		expect(TokenKind::Param, i, "Expected Paramter Letter");

		char pLetter = m_tokens[i].letter;
		i++;

		expect(TokenKind::Number, i, "Expected Parameter Value.");

		double val = m_tokens[i].number;
		cmd.params[pLetter] = val;
	}

	return cmd;
}

bool Parser::optional(TokenKind k, std::size_t idx)
{
	if (idx >= m_tokens.size() || m_tokens[idx].kind != k)
	{
		return false;
	}

	return true;
}

void Parser::expect(TokenKind k, std::size_t idx, const char* msg)
{
	if (idx >= m_tokens.size() || m_tokens[idx].kind != k)
	{
		throw std::runtime_error(msg);
	}

}
#include <array>
#include <stdexcept>

#include "..\inc\Encoder.h"

template <typename T, std::size_t N>
struct ByteEncoder
{
	union
	{
		T value;
		std::byte bytes[N];
	};
};

template <typename T, std::size_t N>
constexpr ByteEncoder<T, N> encodeData(T data)
{
	ByteEncoder<T, N> encoder;
	encoder.value = data;
	return encoder;
}

template <typename T, std::size_t N>
constexpr ByteEncoder<T, N> decodeData(std::vector<std::byte>& m_data, std::size_t& m_pos)
{
	ByteEncoder<T, N> encoder{};
	for (int i = m_pos; i < m_pos + N; ++i)
	{
		if (i >= m_data.size())
			throw std::runtime_error("Not enough data to decode");

		encoder.bytes[i - m_pos] = m_data[i];
	}
	m_pos += N;

	return encoder;
}

std::vector<std::byte> Encoder::encode()
{
	std::vector<std::byte> buffer;

	if (m_cmd.is_program.has_value() && *m_cmd.is_program)
	{
		buffer.reserve(1);
		buffer.push_back(encodeData<char, sizeof(char)>('\0').bytes[0]);
	}
	else
	{
		buffer.reserve(1 + 4 + m_cmd.params.size() + (m_cmd.params.size() * 4) + 1);

		buffer.push_back(encodeData<char, sizeof(char)>(m_cmd.letter).bytes[0]);
		auto encodedInt = encodeData<int, sizeof(int)>(m_cmd.code);
		buffer.push_back(encodedInt.bytes[0]);
		buffer.push_back(encodedInt.bytes[1]);
		buffer.push_back(encodedInt.bytes[2]);
		buffer.push_back(encodedInt.bytes[3]);

		for (auto p : m_cmd.params)
		{
			buffer.push_back(encodeData<char, sizeof(char)>(p.first).bytes[0]);
			auto encodedFloat = encodeData<float, sizeof(float)>(p.second);
			buffer.push_back(encodedFloat.bytes[0]);
			buffer.push_back(encodedFloat.bytes[1]);
			buffer.push_back(encodedFloat.bytes[2]);
			buffer.push_back(encodedFloat.bytes[3]);
		}

		buffer.push_back(encodeData<char, 1>('\0').bytes[0]);
	}

	return buffer;
}

GCodeCommand Decoder::decode()
{
	GCodeCommand cmd{};
	auto letter = decodeData<char, 1>(m_data, m_pos).value;

	if (letter == '\0')
	{
		cmd.is_program = true;
	}
	else
	{
		cmd.letter = letter;
		cmd.code = decodeData<int, sizeof(int)>(m_data, m_pos).value;

		while (m_pos < m_data.size())
		{
			auto nextLetter = decodeData<char, sizeof(char)>(m_data, m_pos).value;

			if (nextLetter == '\0')
				break;

			float paramValue = decodeData<float, sizeof(float)>(m_data, m_pos).value;
			cmd.params[nextLetter] = paramValue;

		}
	}

	return cmd;
}
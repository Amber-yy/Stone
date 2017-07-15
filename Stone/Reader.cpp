#include "Reader.h"

#include <vector>
#include <fstream>

struct Reader::readerData
{
	std::vector<std::string> allCode;
	int currentLine;
};

Reader::Reader()
{
	data = std::make_unique<readerData>();
	data->currentLine = 0;
}

void Reader::load(std::ifstream & file)
{
	std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	load(str);
}

void Reader::load(const std::string & buffer)
{
	std::vector<char> code(buffer.length()+1);
	memcpy(code.data(),buffer.c_str(),code.size());

	data->allCode.clear();

	char *firstIt = code.data();
	char *secondIt = firstIt;
	char *end = firstIt + code.size();

	while (secondIt != end)
	{
		if (*secondIt == '\n')
		{
			*secondIt = 0;
			if (*firstIt != 0 && *firstIt != '\n')
			{
				data->allCode.push_back(std::string(firstIt));
			}
			firstIt = secondIt+1;
		}

		++secondIt;
	}

	if (*firstIt != 0 && *firstIt != '\n')
	{
		data->allCode.push_back(std::string(firstIt));
	}

}

int Reader::getLineNumber()
{
	return data->currentLine+1;
}

std::string Reader::readLine()
{
	return data->allCode[data->currentLine++];
}

bool Reader::hasMore()
{
	return data->currentLine!=data->allCode.size();
}

Reader::~Reader()
{
}

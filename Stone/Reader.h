#pragma once

#include <string>
#include <memory>

class Reader
{
public:
	Reader();
	void load(std::ifstream &file);
	void load(const std::string &buffer);
	int getLineNumber();
	bool hasMore();
	std::string readLine();
	~Reader();
protected:
	struct readerData;
	std::unique_ptr<readerData> data;
};


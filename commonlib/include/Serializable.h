#pragma once

#include <string>

class Serializable
{
public:
	virtual ~Serializable() = default;
	virtual std::string serialize() = 0;
	virtual void deserialize(std::string) = 0;
};
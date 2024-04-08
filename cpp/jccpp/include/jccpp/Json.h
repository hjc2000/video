#pragma once
#include<jccpp/ToString.h>
#include<nlohmann/json.hpp>
#include<string>

using Json = nlohmann::json;
using JsonTypeError = Json::type_error;

class IJsonSerializable :public ICanToString
{
public:
	~IJsonSerializable() {}

public:
	std::string ToString() override
	{
		return ToJson().dump(4);
	}

	virtual Json ToJson() = 0;
};
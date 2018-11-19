#ifndef JSON_GENERATOR_H
#define JSON_GENERATOR_H

#include "Json_value.h"

class Json_generator
{
public:
	static std::string generate(Json_value& v);
private:
	static void stringify(Json_value& v, std::string& j);
	static void stringify_string(std::string& j, std::string s);
};
#endif

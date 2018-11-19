#include "Json_generator.h"

#include <sstream>

std::string Json_generator::generate(Json_value & v)
{
	std::string j;
	stringify(v, j);

	return j;
}

void Json_generator::stringify(Json_value & v, std::string & j)
{
	std::vector<Json_value> a;
	std::unordered_map<std::string, Json_value> m;

	switch (v.get_data_type())
	{
	case JSON_NULL:
		j += "null";
		break;
	case JSON_FALSE:
		j += "false";
		break;
	case JSON_TRUE:
		j += "true";
		break;
	case JSON_NUMBER:
		j += std::_Floating_to_string<double>("%.17g", v.get_data_number());
		break;
	case JSON_STRING:
		stringify_string(j, v.get_data_string());
		break;
	case JSON_ARRAY:
		j.push_back('[');
		a = v.get_data_array();
		for (decltype(a.end()) i = a.begin(); i != a.end(); i++)
		{
			if (i != a.begin())
				j.push_back(',');
			stringify(*i, j);
		}
		j.push_back(']');
		break;
	case JSON_OBJECT:
		j.push_back('{');
		m = v.get_data_object();
		for (decltype(m.end()) i = m.begin(); i != m.end(); i++)
		{
			if (i != m.begin())
				j.push_back(',');
			stringify_string(j, (*i).first);
			j.push_back(':');
			stringify((*i).second, j);
		}
		j.push_back('}');
		break;
	default:
		std::exit(EXIT_FAILURE);
		break;
	}
}

void Json_generator::stringify_string(std::string & j, std::string s)
{
	j.push_back('\"');
	for (unsigned char c : s)
	{
		switch (c)
		{
		case '\"': j += "\\\""; break;
		case '\\': j += "\\\\"; break;
		case '\b': j += "\\b"; break;
		case '\f': j += "\\f"; break;
		case '\n': j += "\\n"; break;
		case '\r': j += "\\r"; break;
		case '\t': j += "\\t"; break;
		default:
			if (c < 0x20)
			{
				char buffer[7];
				sprintf_s(buffer, "\\u%04X", c);
				j += buffer;
			}
			else
				j.push_back(c);
		}
	}
	j.push_back('\"');
}

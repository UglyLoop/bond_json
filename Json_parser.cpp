#include "Json_parser.h"

#if 0
#include <cerrno>
#include <cstdlib>
#endif
#include <cstring>

Json_parser::Json_parser(std::string j) : json(j)
{
	end_ite = json.end();
}

Json_parser::~Json_parser()
{
}

error_code Json_parser::parse(Json_value& v)
{
	error_code ret;
	p = json.begin();
	parse_whitespace(p, end_ite);
	ret = parse_data(v);
	if (!ret)    // ret == PARSE_OK
	{
		parse_whitespace(p, end_ite);
		if (p != end_ite)
		{
			v.set_data_type(JSON_NULL);
			ret = PARSE_ROOT_NOT_SINGULAR;
		}
	}

	return ret;
}

error_code Json_parser::parse_data(Json_value& v)
{
	if (p == end_ite)
		return PARSE_EXPECT_VALUE;
	switch (*p)
	{
	case 't':  return parse_literal(v, JSON_TRUE);
	case 'f':  return parse_literal(v, JSON_FALSE);
	case 'n':  return parse_literal(v, JSON_NULL);
	default :  return parse_number(v);
	case '"':  return parse_string(v);
	case '[':  return parse_array(v);
	case '{':  return parse_object(v);
	case '\0': return PARSE_EXPECT_VALUE;
	}
}

error_code Json_parser::parse_literal(Json_value & v, json_data_type t)
{
	std::string literal;
	switch (t)
	{
	case JSON_TRUE : literal = "true"; break;
	case JSON_FALSE: literal = "false"; break;
	case JSON_NULL : literal = "null"; break;
	}

	for (const auto &c : literal)
		if (p != end_ite && *p == c)
			p++;
		else
			return PARSE_INVALID_VALUE;
	v.set_data_type(t);
	
	return PARSE_OK;
}

#define ISDIGIT_0TO9(c)     ((c) >= '0' && (c) <= '9')
#define ISDIGIT_1TO9(c)     ((c) >= '1' && (c) <= '9')
error_code Json_parser::parse_number(Json_value & v)
{
	decltype(json.end()) t = p;
	/* parse '-' */
	if (*p == '-') p++;
	/* parse integer */
	if (p != end_ite && *p == '0') p++;
	else
	{
		if (!ISDIGIT_1TO9(*p)) return PARSE_INVALID_VALUE;
		do { p++; } while (p != end_ite && ISDIGIT_0TO9(*p));
		//for (p++; p != end_ite && ISDIGIT_0TO9(*p); p++);
	}
	/* parse floating point */
	if (p != end_ite && *p == '.')
	{
		p++;
		if (p != end_ite && !ISDIGIT_0TO9(*p)) return PARSE_INVALID_VALUE;
		do { p++; } while (p != end_ite && ISDIGIT_0TO9(*p));
		//for (p++; p != end_ite && ISDIGIT_0TO9(*p); p++);
	}
	if (p != end_ite && (*p == 'e' || *p == 'E'))
	{
		p++;
		if (p != end_ite && (*p == '+' || *p == '-')) p++;
		if (p != end_ite && !ISDIGIT_0TO9(*p)) return PARSE_INVALID_VALUE;
		do { p++; } while (p != end_ite && ISDIGIT_0TO9(*p));
		//for (p++; p != end_ite && ISDIGIT_0TO9(*p); p++);
	}

	const unsigned size = p - t;
	char *temp = new char[size + 1];
	//for (unsigned s = 0; s < size; s++)
	//	*(temp + s) = *(t + s);
	std::memcpy(temp, &*t, size);
	temp[size] = '\0';
		
	//errno = 0;
	//double n = std::strtod(temp, nullptr);
	double n = std::stod(temp);
	delete temp;
	//if (errno == ERANGE && (n == HUGE_VAL || n == -HUGE_VAL))
	//	return PARSE_NUMBER_TOO_BIG;
	v.set_data_type(JSON_NUMBER);
	v.set_data(&n);
	
	return PARSE_OK;
}

error_code Json_parser::parse_string(Json_value & v)
{
	error_code ret;
	std::string str;

	ret = parse_string_raw(p, end_ite, str);
	if (!ret)    // ret == PARSE_OK
	{
		v.set_data_type(JSON_STRING);
		v.set_data(&str);
	}

	return ret;
}

error_code Json_parser::parse_array(Json_value & v)
{
	error_code ret;
	std::vector<Json_value> e;

	if (*p == '[') p++;
	else return PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
	parse_whitespace(p, end_ite);
	if (p != end_ite && *p == ']')
	{
		p++;
		v.set_data_type(JSON_ARRAY);
		v.set_data(&e);
		return PARSE_OK;
	}
	while (true)
	{
		Json_value element;
		ret = parse_data(element);
		if (ret)    // ret != PARSE_OK
			break;
		e.push_back(element);
		parse_whitespace(p,end_ite);
		if (p != end_ite && *p == ',')
		{
			p++;
			parse_whitespace(p,end_ite);
		}
		else if (p != end_ite && *p == ']')
		{
			p++;
			v.set_data_type(JSON_ARRAY);
			v.set_data(&e);
			return PARSE_OK;
		}
		else
		{
			ret = PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
			break;
		}
	}

	return ret;
}

error_code Json_parser::parse_object(Json_value & v)
{
	error_code ret;
	std::unordered_map<std::string, Json_value> m;

	if (*p == '{') p++;
	else return PARSE_MISS_COMMA_OR_CURLY_BRACKET;
	parse_whitespace(p, end_ite);
	if (p != end_ite && *p == '}')
	{
		p++;
		v.set_data_type(JSON_OBJECT);
		v.set_data(&m);
		return PARSE_OK;
	}
	while (true)
	{
		/* parse key */
		if (p != end_ite && *p != '\"')
		{
			ret = PARSE_MISS_KEY;
			break;
		}
		std::string key;
		ret = parse_string_raw(p, end_ite, key);
		if (ret)    // ret != PARSE_OK
			break;
		////////////
		/* parse ws_colon_ws */
		parse_whitespace(p, end_ite);
		if (p != end_ite && *p != ':')
		{
			ret = PARSE_MISS_COLON;
			break;
		}
		parse_whitespace(++p, end_ite);
		/* parse value */
		Json_value member;
		ret = parse_data(member);
		if (ret)    // ret != PARSE_OK
			break;
		m.insert(std::make_pair(key, member));
		/* parse ws [comma | right-curly-brace] ws */
		parse_whitespace(p, end_ite);
		if (p != end_ite && *p == ',')
		{
			p++;
			parse_whitespace(p, end_ite);
		}
		else if (p != end_ite && *p == '}')
		{
			p++;
			v.set_data_type(JSON_OBJECT);
			v.set_data(&m);
			return PARSE_OK;
		}
		else
		{
			ret = PARSE_MISS_COMMA_OR_CURLY_BRACKET;
			break;
		}
	}

	return ret;
}

void Json_parser::parse_whitespace(decltype(json.end())& p, decltype(json.end())& end)
{
	while (p != end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
	{
		p++;
	}
}

bool Json_parser::hex4_translate(decltype(json.end())& p, unsigned & u)
{
	u = 0;
	for (unsigned i = 0; i < 4; i++)
	{
		char ch = *p++;
		u <<= 4;
		if (ch >= '0' && ch <= '9')
			u |= ch - '0';
		else if (ch >= 'A' && ch <= 'F')
			u |= ch - ('A' - 10);
		else if (ch >= 'a' && ch <= 'f')
			u |= ch - ('a' - 10);
		else
			return false;
	}

	return true;
}

bool Json_parser::utf8_encode(std::string & stack, unsigned u)
{
	if (u <= 0x7F)
		stack.push_back(u & 0xFF);
	else if (u <= 0x7FF)
	{
		stack.push_back(0xC0 | ((u >> 6) & 0xFF));
		stack.push_back(0x80 | ( u       & 0x3F));
	}
	else if (u <= 0xFFFF)
	{
		stack.push_back(0xE0 | ((u >> 12) & 0xFF));
		stack.push_back(0x80 | ((u >> 6)  & 0x3F));
		stack.push_back(0x80 | ( u        & 0x3F));
	}
	else if (u <= 0x10FFFF)
	{
		stack.push_back(0xF0 | ((u >> 18) & 0xFF));
		stack.push_back(0x80 | ((u >> 12) & 0x3F));
		stack.push_back(0x80 | ((u >> 6)  & 0x3F));
		stack.push_back(0x80 | ( u        & 0x3F));
	}
	else
		return false;

	return true;
}

error_code Json_parser::parse_string_raw(decltype(json.end())& p, decltype(json.end())& end, std::string & str)
{
	std::string &stack = str;
	decltype(json.end()) t = p;
	if (*p == '\"') p++;
	else return PARSE_INVALID_STRING_ESCAPE;

	while (true)
	{
		if(p == end)
			return PARSE_MISS_QUOTATION_MARK;
		char c = *p++;
		switch (c)
		{
		case '\"':
			return PARSE_OK;
		case '\\':
			switch (*p++)
			{
			case '\"': stack.push_back('\"'); break;
			case '\\': stack.push_back('\\'); break;
			case '/' : stack.push_back('/') ; break;
			case 'b' : stack.push_back('\b'); break;
			case 'f' : stack.push_back('\f'); break;
			case 'n' : stack.push_back('\n'); break;
			case 'r' : stack.push_back('\r'); break;
			case 't' : stack.push_back('\t'); break;
			case 'u' :
				unsigned u;
				if (!hex4_translate(p, u))
					return PARSE_INVALID_UNICODE_HEX;
				if (u >= 0xD800 && u <= 0xDBFF)
				{
					if (*p++ != '\\')
						return PARSE_INVALID_UNICODE_SURROGATE;
					if (*p++ != 'u')
						return PARSE_INVALID_UNICODE_SURROGATE;
					unsigned low;
					if (!hex4_translate(p, low))
						return PARSE_INVALID_UNICODE_HEX;
					if (low < 0xDC00 || low > 0xDFFF)
						return PARSE_INVALID_UNICODE_SURROGATE;
					u = (((u - 0xD800) << 10) | (low - 0xDC00)) + 0x10000;
					if (u > 0x10FFFF)
						return PARSE_INVALID_UNICODE_HEX;
				}
				utf8_encode(stack, u);
				break;
			default:
				return PARSE_INVALID_STRING_ESCAPE;
			}
			break;

		case '\0':
			return PARSE_MISS_QUOTATION_MARK;
		default:
			if ((unsigned char)c < 0x20)
				return PARSE_INVALID_STRING_CHAR;
			stack.push_back(c);
		}
	}

	return PARSE_OK;
}

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "Json_value.h"

enum error_code {
	PARSE_OK = 0,
	PARSE_EXPECT_VALUE,
	PARSE_INVALID_VALUE,
	PARSE_ROOT_NOT_SINGULAR,
	PARSE_NUMBER_TOO_BIG,
	PARSE_MISS_QUOTATION_MARK,
	PARSE_INVALID_STRING_ESCAPE,
	PARSE_INVALID_STRING_CHAR,
	PARSE_INVALID_UNICODE_HEX,
	PARSE_INVALID_UNICODE_SURROGATE,
	PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
	PARSE_MISS_KEY,
	PARSE_MISS_COLON,
	PARSE_MISS_COMMA_OR_CURLY_BRACKET
};

class Json_parser
{
private:
	const std::string json;
	decltype(json.end()) p, end_ite;
public:
	Json_parser(std::string j);
	~Json_parser();

	error_code parse(Json_value& v);
private:
	error_code parse_data(Json_value& v);
	error_code parse_literal(Json_value& v, json_data_type t);
	error_code parse_number(Json_value& v);
	error_code parse_string(Json_value& v);
	error_code parse_array(Json_value& v);
	error_code parse_object(Json_value& v);
public:
	static void parse_whitespace(decltype(json.end())& p, decltype(json.end())& end);
	static bool hex4_translate(decltype(json.end()) &p, unsigned &u);
	static bool utf8_encode(std::string &stack, unsigned u);
	static error_code parse_string_raw(decltype(json.end()) &p, decltype(json.end())& end, std::string& str);
};
#endif

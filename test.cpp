#include "Json_value.h"
#include "Json_parser.h"
#include "Json_generator.h"

#include <iostream>

int main()
{
	Json_parser parser(
		" { "
        "\"n\" : null , "
        "\"f\" : false , "
        "\"t\" : true , "
        "\"i\" : 123 , "
        "\"s\" : \"abc\", "
        "\"a\" : [ 1, 2, 3 ],"
        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
		" } "
	);

	Json_value v;
	if (parser.parse(v))
		std::cout << "PARSE_FAILURE" << std::endl;
	else
	{
		// expectant value is 6
		std::cout << v.get_data_type() << std::endl;
		auto m = v.get_data_object();
		/* TODO */
	}

	std::string json =  Json_generator::generate(v);

	std::cout << json << std::endl;

	return 0;
}
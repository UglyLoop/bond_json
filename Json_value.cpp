#include "Json_value.h"

#include <cstring>

Json_value::Json_value()
{
	type = JSON_NULL;
	size = 0;
	data = nullptr;
}

Json_value::Json_value(const Json_value & src)
{
	type = src.type;
	size = src.size;
	switch (src.type)
	{
	case JSON_NUMBER:
		data = new double;
		*(double*)data = *(double*)src.data;
		break;
	case JSON_STRING:
		data = new std::string(*(std::string*)src.data);
		break;
	case JSON_ARRAY:
		data = new std::vector<Json_value>(*(std::vector<Json_value>*)src.data);
		break;
	case JSON_OBJECT:
		data = new std::unordered_map<std::string, Json_value>(*(std::unordered_map<std::string, Json_value>*)src.data);
		break;
	default:
		data = nullptr;
		break;
	}
}

Json_value::~Json_value()
{
	if (data) delete data;
}

void Json_value::set_data_type(json_data_type t)
{
	type = t;
}

json_data_type Json_value::get_data_type()
{
	return type;
}

void Json_value::set_boolean(bool b)
{
	if (data) delete data;
	data = nullptr;
	type = b ? JSON_TRUE : JSON_FALSE;
}

void Json_value::set_data(void * d, unsigned size)
{
	switch (type)
	{
	case JSON_NUMBER:
		data = new double;
		*(double*)data = *(double*)d;
		this->size = sizeof(double);
		break;
	case JSON_STRING:
		data = new std::string(*(std::string*)d);
		this->size = size;
		break;
	case JSON_ARRAY:
		data = new std::vector<Json_value>(*(std::vector<Json_value>*)d);
		this->size = sizeof(std::vector<Json_value>);
		break;
	case JSON_OBJECT:
		data = new std::unordered_map<std::string, Json_value>(*(std::unordered_map<std::string, Json_value>*)d);
		this->size = sizeof(std::unordered_map<std::string, Json_value>);
		break;
	}
}

unsigned Json_value::get_data_size()
{
	return size;
}

double Json_value::get_data_number()
{
	if (type == JSON_NUMBER && data)
		return *(double*)data;
	else
		std::exit(EXIT_FAILURE);
}

std::string Json_value::get_data_string()
{
	if(type == JSON_STRING && data)
		return *(std::string*)data;
	else
		std::exit(EXIT_FAILURE);
}

std::vector<Json_value> Json_value::get_data_array()
{
	if (type == JSON_ARRAY && data)
		return *(std::vector<Json_value>*)data;
	else
		std::exit(EXIT_FAILURE);
}

std::unordered_map<std::string, Json_value> Json_value::get_data_object()
{
	if (type == JSON_OBJECT && data)
		return *(std::unordered_map<std::string, Json_value>*)data;
	else
		std::exit(EXIT_FAILURE);
}

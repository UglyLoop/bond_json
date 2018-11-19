#ifndef JSON_VALUE_H
#define JSON_VALUE_H

#include <string>
#include <vector>
#include <unordered_map>

enum json_data_type
{
	JSON_NULL, JSON_FALSE, JSON_TRUE, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT
};

class Json_value
{
private:
	json_data_type type;
	unsigned size;
	void *data;
public:
	Json_value();
	Json_value(const Json_value &src);
	~Json_value();

	void set_data_type(json_data_type t);
	json_data_type get_data_type();
	void set_boolean(bool b);
	void set_data(void *d, unsigned size = 0);
	unsigned get_data_size();
	double get_data_number();
	std::string get_data_string();
	std::vector<Json_value> get_data_array();
	std::unordered_map<std::string, Json_value> get_data_object();
};
#endif

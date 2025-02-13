//
// Created by Peer De bakker on 7/31/22.
//

#ifndef JSON_PARSER_JSONNODE_HPP
#define JSON_PARSER_JSONNODE_HPP

#include <vector>
#include <map>
#include <string>
#include <variant>
#include <iostream>

enum class e_type {
	FLOAT,
	INTEGER,
	BOOLEAN,
	STRING,
	LIST,
	OBJECT,
	NULL_TYPE
};

class JsonNode;
using JSONList = std::vector<JsonNode*>;
using JSONObject = std::map<std::string, JsonNode*>;

class JsonNode {
//	std::variant<JSONList, JSONObject, std::string, float, int, bool>	values;
	union Values {
		JSONList	*list;
		JSONObject	*object;
		std::string	*str;
		float		f;
		bool		b;
	} values;
	e_type type;

public:
	JsonNode();
	JsonNode(e_type t);
	~JsonNode();
	JSONObject	returnObject();
	JSONList	returnList();
	std::string	returnString();
	float		returnFloat();
	int			returnInt();
	bool		returnBool();

	void	setObject(JSONObject *object);
	void	setList(JSONList *list);
	void	setString(std::string *s);
	void	setFloat(float f);
	void	setInteger(int i);
	void	setBoolean(bool b);
	void	setNull();

	std::string	toString(int indentLevel) const;
};

std::ostream&	operator<<(std::ostream& o, const JsonNode& x);


#endif //JSON_PARSER_JSONNODE_HPP

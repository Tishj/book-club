//
// Created by Peer De bakker on 7/31/22.
//

#include "../include/JsonNode.hpp"

JsonNode::JsonNode() {

}

JsonNode::JsonNode(e_type t) : type(t) {

}

JsonNode::~JsonNode() {
	if (this->type == e_type::LIST) {
		for (auto it : *this->values.list) {
			delete it;
		}
	}
	else if (this->type == e_type::OBJECT) {
		for (auto it : *this->values.object) {
			delete it.second;
		}
	}
	else if (this->type == e_type::STRING) {
		delete this->values.object;
	}
}

JSONObject JsonNode::returnObject() {
	if (type != e_type::OBJECT)
		throw std::logic_error("returnObject called on non-object");
	return (*values.object);
}

JSONList JsonNode::returnList() {
	if (type != e_type::LIST)
		throw std::logic_error("returnList called on non-list");
	return (*values.list);
}

std::string JsonNode::returnString() {
	if (type != e_type::STRING)
		throw std::logic_error("returnString called on non-string");
	return (*values.str);
}

float JsonNode::returnFloat() {
	if (type != e_type::FLOAT)
		throw std::logic_error("returnFloat called on non-number");
	return (values.f);
}

int JsonNode::returnInt() {
	if (type != e_type::INTEGER)
		throw std::logic_error("returnFloat called on non-number");
	return (static_cast<int>(values.f));
}

//int JsonNode::returnInt() {
//	if (type != e_type::INTEGER)
//		throw std::logic_error("returnInt called on non-int");
//	return (values.integer);
//}

bool JsonNode::returnBool() {
	if (type != e_type::BOOLEAN)
		throw std::logic_error("returnBool called on non-boolean");
	return (values.b);
}

void JsonNode::setObject(JSONObject* object) {
	this->type = e_type::OBJECT;
	this->values.object = object;
}

void JsonNode::setList(JSONList* list) {
	this->type = e_type::LIST;
	this->values.list = list;
}

void JsonNode::setString(std::string* s) {
	this->type = e_type::STRING;
	this->values.str = s;
}

void JsonNode::setFloat(float f) {
	this->type = e_type::FLOAT;
	this->values.f = f;
}

void JsonNode::setInteger(int i) {
	this->type = e_type::INTEGER;
	this->values.f = i;
}

void JsonNode::setBoolean(bool b) {
	this->type = e_type::BOOLEAN;
	this->values.b = b;
}

void	JsonNode::setNull() {
	this->type = e_type::NULL_TYPE;
}

std::string JsonNode::toString(int indentLevel) const {
	std::string spaceString = std::string(indentLevel, ' ');
	std::string outputString;
	switch (type) {
		case e_type::STRING: {
			outputString += spaceString + '"' + *values.str + '"';
			break;
		}
		case e_type::FLOAT: {
			outputString += spaceString + std::to_string(values.f);
			break;
		}
		case e_type::INTEGER: {
			outputString += spaceString + std::to_string((int)values.f);
			break;
		}
		case e_type::BOOLEAN: {
			outputString += spaceString + (values.b ? "true" : "false");
			break;
		}
		case e_type::NULL_TYPE: {
			outputString += spaceString + "null";
			break;
		}
		case e_type::LIST: {
			std::cout << "[";
			size_t index = 0;
			for (auto node: (*values.list)) {
				outputString += node->toString(indentLevel + 1);
				if (index < (*values.list).size() - 1) {
					outputString += spaceString + ", ";
				}
				index++;
			}
			outputString += spaceString + "]\n";
			break;
		}
		case e_type::OBJECT: {
			outputString += "{\n";
			for (JSONObject::iterator i = (*values.object).begin();
				 i != (*values.object).end(); i++) {
				outputString += spaceString + i->first + ": ";
				outputString += i->second->toString(indentLevel + 1);
				JSONObject::iterator next = i;
				next++;
				if ((next) != (*values.object).end()) {
					outputString += spaceString + ",";
				}
				outputString += spaceString + "\n";
			}
			outputString += "}\n";
		}
	}
	return (outputString);
}

std::ostream&	operator<<(std::ostream& o, const JsonNode& x) {
	o << x.toString(0);
	return (o);
}

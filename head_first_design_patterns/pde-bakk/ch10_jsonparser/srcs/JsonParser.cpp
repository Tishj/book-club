//
// Created by Peer De bakker on 7/31/22.
//

#include "JsonParser.hpp"

#include <utility>
#include <iostream>
#include <cctype>


e_type	token_to_type(e_token token) {
	switch (token) {
		case (e_token::CURLY_OPEN):
			return (e_type::OBJECT);
		case (e_token::STRING):
			return (e_type::STRING);
		case (e_token::INTEGER):
			return (e_type::INTEGER);
		case (e_token::ARRAY_OPEN):
			return (e_type::LIST);
		case (e_token::BOOLEAN):
			return (e_type::BOOLEAN);
		case (e_token::NULL_TYPE):
			return (e_type::NULL_TYPE);
		default:
			throw std::runtime_error("WHAT TYPE IS THIS? " + tokenToString(token));
	}
}

JsonParser::JsonParser(const std::string& filename) : _file(filename), _root() {
	this->_root = nullptr;
}

JsonParser::~JsonParser() {

}

JsonNode* JsonParser::parse() {
//	char c;

	if (_file.eof()) {
		std::cerr << "EOF reached\n";
		return (nullptr);
	}

	e_token token = this->parse_token();
	e_type type = token_to_type(token);
	fprintf(stderr, "token = %s\n", tokenToString(token).c_str());
	JsonNode*	node = nullptr;
	switch (type) {
		case (e_type::FLOAT):
		case (e_type::INTEGER):
			node = parse_number();
			break ;
		case (e_type::BOOLEAN):
			node = parse_boolean();
			break ;
		case (e_type::STRING):
			node = parse_string();
			break ;
		case (e_type::LIST):
			node = parse_list();
			break ;
		case (e_type::OBJECT):
			node = parse_object();
			break ;
		case (e_type::NULL_TYPE):
			node = parse_nulltype();
			break ;
	}
	assert(node != nullptr);
	if (this->_root == nullptr)
		this->_root = node;
	return (node);
}

static e_token parse_int(char c) {
	//TODO: check if float
	if ((c >= '0' && c <= '9') || c == '+' || c == '-')
		return (e_token::INTEGER);
	std::string error("invalid token: ");
	error.push_back(c);
	throw std::runtime_error(error);
}

e_token JsonParser::parse_token() {
	char c = get_next_char();

	switch (c) {
		case '{':
			return (e_token::CURLY_OPEN);
		case '}':
			return (e_token::CURLY_CLOSE);
		case ':':
			return (e_token::COLON);
		case '"':
			return (e_token::STRING);
		case '[':
			return (e_token::ARRAY_OPEN);
		case ']':
			return (e_token::ARRAY_CLOSE);
		case ',':
			return (e_token::COMMA);
		case 't':
		case 'f':
			this->_file.unget();
			return (e_token::BOOLEAN);
		case 'n':
			this->_file.unget();
			return (e_token::NULL_TYPE);
		default:
			this->_file.unget();
			return (parse_int(c));
	}
}

char JsonParser::get_next_char() {
	char c;
	while (_file.get(c)) {
		if (!isspace(c)) {
			break ;
		}
	}
	return (c);
}

JsonNode* JsonParser::parse_object() {
	JsonNode*	node = new JsonNode();
	JSONObject*	object = new JSONObject();
	bool completed = false;

	if (_file.peek() == '{') {
		std::cerr << "I thought I wouldn't see this opening bracket, but I guess here it is\n";
	}
	while (!completed && !_file.eof()) {
		char c = this->get_next_nonspace();
		if (c == '}') {
			fprintf(stderr, "found the end of OBJECT\n");
			completed = true;
		}
		else if (c == ',') {
			fprintf(stderr, " found a comma in my object\n");
		} else {
			assert(c == '"');
			std::string key = get_next_string();
			std::cerr << "key = " << key << "\n";
			char colon = _file.get();
			assert(colon == ':');
			JsonNode*	value = parse();
			assert(value);
			std::cerr << "value = " << value->toString(0) << "\n";
			(*object)[key] = value;
			std::cerr << "object now has " << object->size() << " elems\n";
		}
	}
	node->setObject(object);
	return (node);
}

JsonNode* JsonParser::parse_list() {
	JsonNode*	node = new JsonNode();
	JSONList*	list = new JSONList();
	bool completed = false;

	fprintf(stderr, "parsing LIST\n");
	if (_file.peek() == '[') {
		std::cerr << "I thought I wouldn't see this opening bracket, but I guess here it is\n";
	}
	while (!completed && !_file.eof()) {
		char c = this->get_next_nonspace();
		if (c == ']') {
			fprintf(stderr, "found the end of LIST\n");
			completed = true;
		}
		else if (c == ',') {
			std::cerr << "found a comma in my list\n";
		}
		else {
			this->_file.unget();
			JsonNode*	new_elem = this->parse();
			assert(new_elem);
//			std::cerr << "LIST_element = " << new_elem->toString(2) << '\n';
			list->push_back(new_elem);
//			std::cerr << "list now has " << list->size() << " elems\n";
//			std::cerr << "next elem is " << _file.peek() << "\n";
		}
	}
	node->setList(list);
	return (node);
}

JsonNode* JsonParser::parse_string() {
	const auto next = get_next_string();
	std::string* s = new std::string(next);
	JsonNode* node = new JsonNode();
	node->setString(s);
	return (node);
}

JsonNode* JsonParser::parse_number() {
	const auto next = get_next_item();
	const float value = std::stof(next);
	JsonNode* node = new JsonNode();
	if (next.find('.') == std::string::npos)
		node->setInteger(static_cast<int>(value));
	else
		node->setFloat(value);
	return (node);
}

JsonNode* JsonParser::parse_boolean() {
	const auto next = get_next_item();
	bool value;
	if (next == "true")
		value = true;
	else if (next == "false")
		value = false;
	else
		throw std::runtime_error("Invalid boolean: " + next);
	JsonNode* node = new JsonNode();
	node->setBoolean(value);
	return (node);
}

JsonNode* JsonParser::parse_nulltype() {
	const auto next = get_next_item();
	if (next != "null")
		throw std::runtime_error("Invalid nulltype: " + next);
	JsonNode* node = new JsonNode();
	node->setNull();
	return (node);
}

std::string JsonParser::get_next_item() {
	std::string str;

	str.reserve(1290); // scientifically proven to be optimal

	skipws(this->_file);
	while (!_file.eof()) {
		char newchar = _file.get();
		if (isspace(newchar) || newchar == ',' || newchar == '}' || newchar == ']') {
			this->_file.unget();
			break ;
		}
		str += newchar;
	}
	return (str);
}

std::string JsonParser::get_next_string() {
	std::string str;

	str.reserve(1290); // scientifically proven to be optimal

	skipws(this->_file);
	while (!_file.eof()) {
		char newchar = _file.get();
		if (newchar == '"' && (str.empty() || str.back() != '\\'))
			break ;
		str += newchar;
	}
	return (str);
}

char JsonParser::get_next_nonspace() {
	char c = _file.get();
	std::cerr << "got: " << c;
	while (isspace(c) && !this->_file.eof()) {
		c = _file.get();
		std::cerr << c;
	}
	fprintf(stderr, "|last c = %c\n", c);
	return (c);
}

std::string	tokenToString(const e_token& token) {
	switch (token) {
		case e_token::CURLY_OPEN:
			return ("e_token::CURLY_OPEN");
		case e_token::CURLY_CLOSE:
			return ("e_token::CURLY_CLOSE");
		case e_token::COLON:
			return ("e_token::COLON");
		case e_token::STRING:
			return ("e_token::STRING");
		case e_token::INTEGER:
			return ("e_token::INTEGER");
		case e_token::ARRAY_OPEN:
			return ("e_token::ARRAY_OPEN");
		case e_token::ARRAY_CLOSE:
			return ("e_token::ARRAY_CLOSE");
		case e_token::COMMA:
			return ("e_token::COMMA");
		case e_token::BOOLEAN:
			return ("e_token::BOOLEAN");
		case e_token::NULL_TYPE:
			return ("e_token::NULL_TYPE");
	}
}

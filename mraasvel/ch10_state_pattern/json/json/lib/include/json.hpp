#pragma once

#include "option.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace json {

enum class Type {
	Number,
	Boolean,
	String,
	Array,
	Object,
	Null
};

std::ostream& operator<<(std::ostream& out, const Type& rhs);

/*
std::variant is probably a better solution obviously
*/
class Json {
	private:
		union JsonData {
			JsonData(int n);
			JsonData(bool b);
			JsonData(const std::string& s);
			JsonData(std::vector<std::unique_ptr<Json>>&& array);
			JsonData(std::unordered_map<std::string, std::unique_ptr<Json>>&& object);
			~JsonData();

			JsonData& operator=(JsonData&&);
			JsonData& operator=(const JsonData&) = delete;
			JsonData(JsonData&&) = delete;
			JsonData(const JsonData&) = delete;

			int n;
			bool b;
			std::string str;
			std::vector<std::unique_ptr<Json>> array;
			std::unordered_map<std::string, std::unique_ptr<Json>> object;
		};

	public:
		using PointerType = std::unique_ptr<Json>;
		using ArrayType = std::vector<PointerType>;
		using ObjectType = std::unordered_map<std::string, PointerType>;

	public:
		Json();
		Json(int n);
		Json(bool b);
		Json(const std::string& s);
		Json(ArrayType&& array);
		Json(ObjectType&& object);
		~Json();

		friend bool operator==(const Json& a, const Json& b);
		friend bool operator!=(const Json& a, const Json& b);

		Json(Json&&);
		Json& operator=(Json&&) = delete;
		Json& operator=(const Json&) = delete;
		Json(const Json&) = delete;

		json::Type get_type() const;
		int get_num() const;
		bool get_bool() const;
		const std::string& get_string() const;
		const ArrayType& get_array() const;
		const ObjectType& get_object() const;

		void print() const;

	private:
		void print_depth(int depth) const;
		void print_array(int depth) const;
		void print_object(int depth) const;
		void destroy_int();
		void destroy_bool();
		void destroy_string();
		void destroy_array();
		void destroy_object();

	private:
		Option<JsonData> data;
		Type type;
};

}

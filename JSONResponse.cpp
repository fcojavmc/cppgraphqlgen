// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "JSONResponse.h"

namespace facebook {
namespace graphql {
namespace rapidjson {

Value convertResponse(Document::AllocatorType& allocator, response::Value&& response)
{
	switch (response.type())
	{
		case response::Value::Type::Map:
		{
			Value result(Type::kObjectType);
			auto members = response.release<response::Value::MapType>();

			for (auto& entry : members)
			{
				result.AddMember(
					Value(entry.first.c_str(), static_cast<SizeType>(entry.first.size()), allocator),
					convertResponse(allocator, std::move(entry.second)), allocator);
			}

			return result;
		}

		case response::Value::Type::List:
		{
			Value result(Type::kArrayType);
			auto elements = response.release<response::Value::ListType>();

			result.Reserve(static_cast<SizeType>(elements.size()), allocator);
			for (auto& entry : elements)
			{
				result.PushBack(convertResponse(allocator, std::move(entry)), allocator);
			}

			return result;
		}

		case response::Value::Type::String:
		case response::Value::Type::EnumValue:
		{
			Value result(Type::kStringType);
			auto value = response.release<response::Value::StringType>();

			result.SetString(value.c_str(), static_cast<SizeType>(value.size()), allocator);

			return result;
		}

		case response::Value::Type::Null:
		{
			Value result(Type::kNullType);

			return result;
		}

		case response::Value::Type::Boolean:
		{
			Value result(response.get<response::Value::BooleanType>()
				? Type::kTrueType
				: Type::kFalseType);

			return result;
		}

		case response::Value::Type::Int:
		{
			Value result(Type::kNumberType);

			result.SetInt(response.get<response::Value::IntType>());

			return result;
		}

		case response::Value::Type::Float:
		{
			Value result(Type::kNumberType);

			result.SetDouble(response.get<response::Value::FloatType>());

			return result;
		}

		case response::Value::Type::Scalar:
		{
			return convertResponse(allocator, response.release<response::Value::ScalarType>());
		}

		default:
		{
			return Value(Type::kNullType);
		}
	}
}

Document convertResponse(response::Value&& response)
{
	Document document;
	auto result = convertResponse(document.GetAllocator(), std::move(response));

	static_cast<Value&>(document).Swap(result);

	return document;
}


response::Value convertResponse(const Value& value)
{
	switch (value.GetType())
	{
		case Type::kNullType:
			return response::Value();

		case Type::kFalseType:
			return response::Value(false);

		case Type::kTrueType:
			return response::Value(true);

		case Type::kObjectType:
		{
			response::Value response(response::Value::Type::Map);

			response.reserve(static_cast<size_t>(value.MemberCount()));
			for (const auto& member : value.GetObject())
			{
				response.emplace_back(member.name.GetString(),
					convertResponse(member.value));
			}

			return response;
		}

		case Type::kArrayType:
		{
			response::Value response(response::Value::Type::List);

			response.reserve(static_cast<size_t>(value.Size()));
			for (const auto& element : value.GetArray())
			{
				response.emplace_back(convertResponse(element));
			}

			return response;
		}

		case Type::kStringType:
			return response::Value(std::string(value.GetString()));

		case Type::kNumberType:
		{
			response::Value response(value.IsInt()
				? response::Value::Type::Int
				: response::Value::Type::Float);

			if (value.IsInt())
			{
				response.set<response::Value::IntType>(value.GetInt());
			}
			else
			{
				response.set<response::Value::FloatType>(value.GetDouble());
			}

			return response;
		}

		default:
			return response::Value();
	}
}

response::Value convertResponse(const Document& document)
{
	return convertResponse(static_cast<const Value&>(document));
}

} /* namespace rapidjson */
} /* namespace graphql */
} /* namespace facebook */

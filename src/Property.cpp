// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Types.h"

#include "BinaryValueObject.h"
#include "StreamValueObject.h"
#include "BoolValueObject.h"
#include "DateTimeValueObject.h"
#include "GuidValueObject.h"
#include "IntIdObject.h"
#include "IntValueObject.h"
#include "NamedIdObject.h"
#include "PropIdObject.h"
#include "PropValueObject.h"
#include "StringValueObject.h"

namespace graphql::mapi {

Property::Property(const id_variant& id, value_variant&& value)
	: m_id { std::visit(
		[](const auto& id) -> std::shared_ptr<object::PropId> {
			using T = std::decay_t<decltype(id)>;

			if constexpr (std::is_same_v<T, ULONG>)
			{
				return std::make_shared<object::PropId>(
					std::make_shared<object::IntId>(std::make_shared<IntId>(id)));
			}
			else if constexpr (std::is_same_v<T, MAPINAMEID>)
			{
				return std::make_shared<object::PropId>(
					std::make_shared<object::NamedId>(std::make_shared<NamedId>(id)));
			}
			else
			{
				throw new std::invalid_argument("unsupported variant type");
			}
		},
		id) }
	, m_value { std::visit(
		[](auto&& value) -> std::shared_ptr<object::PropValue> {
			using T = std::decay_t<decltype(value)>;

			if constexpr (std::is_same_v<T, mapi_ptr<SPropValue>>)
			{
				switch (PROP_TYPE(value->ulPropTag))
				{
					case PT_I2:
						return std::make_shared<object::PropValue>(std::make_shared<object::IntValue>(
							std::make_shared<IntValue>(static_cast<int>(value->Value.i))));

					case PT_LONG:
						return std::make_shared<object::PropValue>(std::make_shared<object::IntValue>(
							std::make_shared<IntValue>(static_cast<int>(value->Value.l))));

					case PT_I8:
						return std::make_shared<object::PropValue>(std::make_shared<object::IntValue>(
							std::make_shared<IntValue>(static_cast<int>(value->Value.li.QuadPart))));

					case PT_BOOLEAN:
						return std::make_shared<object::PropValue>(std::make_shared<object::BoolValue>(
							std::make_shared<BoolValue>(!!value->Value.b)));

					case PT_STRING8:
						return std::make_shared<object::PropValue>(std::make_shared<object::StringValue>(
							std::make_shared<StringValue>(std::string { value->Value.lpszA })));

					case PT_UNICODE:
						return std::make_shared<object::PropValue>(std::make_shared<object::StringValue>(
							std::make_shared<StringValue>(value->Value.lpszW)));

					case PT_CLSID:
						return std::make_shared<object::PropValue>(std::make_shared<object::GuidValue>(
							std::make_shared<GuidValue>(*value->Value.lpguid)));

					case PT_SYSTIME:
						return std::make_shared<object::PropValue>(std::make_shared<object::DateTimeValue>(
							std::make_shared<DateTimeValue>(value->Value.ft)));

					case PT_BINARY:
						return std::make_shared<object::PropValue>(std::make_shared<object::BinaryValue>(
							std::make_shared<BinaryValue>(value->Value.bin)));

					default:
						return nullptr;
				}
			}
			else if constexpr (std::is_same_v<T, DataStream>)
			{
				return std::make_shared<object::PropValue>(std::make_shared<object::StreamValue>(
					std::make_shared<StreamValue>(std::move(value))));
			}
			else
			{
				throw new std::invalid_argument("unsupported variant type for property value");
			}
		}, std::move(value)) }
{
}

std::shared_ptr<object::PropId> Property::getId() const
{
	return m_id;
}

std::shared_ptr<object::PropValue> Property::getValue() const
{
	return m_value;
}

} // namespace graphql::mapi
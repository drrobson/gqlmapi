// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef STRINGVALUEOBJECT_H
#define STRINGVALUEOBJECT_H

#include "MAPISchema.h"

namespace graphql::mapi::object {
namespace implements {

template <class I>
concept StringValueIs = std::is_same_v<I, PropValue>;

} // namespace implements

namespace methods::StringValueHas {

template <class TImpl>
concept getValueWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<std::string> { impl.getValue(std::move(params)) } };
};

template <class TImpl>
concept getValue = requires (TImpl impl)
{
	{ service::AwaitableScalar<std::string> { impl.getValue() } };
};

template <class TImpl>
concept beginSelectionSet = requires (TImpl impl, const service::SelectionSetParams params)
{
	{ impl.beginSelectionSet(params) };
};

template <class TImpl>
concept endSelectionSet = requires (TImpl impl, const service::SelectionSetParams params)
{
	{ impl.endSelectionSet(params) };
};

} // namespace methods::StringValueHas

class [[nodiscard]] StringValue final
	: public service::Object
{
private:
	[[nodiscard]] service::AwaitableResolver resolveValue(service::ResolverParams&& params) const;

	[[nodiscard]] service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct [[nodiscard]] Concept
	{
		virtual ~Concept() = default;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;

		[[nodiscard]] virtual service::AwaitableScalar<std::string> getValue(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct [[nodiscard]] Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		[[nodiscard]] service::AwaitableScalar<std::string> getValue(service::FieldParams&& params) const final
		{
			if constexpr (methods::StringValueHas::getValueWithParams<T>)
			{
				return { _pimpl->getValue(std::move(params)) };
			}
			else if constexpr (methods::StringValueHas::getValue<T>)
			{
				return { _pimpl->getValue() };
			}
			else
			{
				throw std::runtime_error(R"ex(StringValue::getValue is not implemented)ex");
			}
		}

		void beginSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::StringValueHas::beginSelectionSet<T>)
			{
				_pimpl->beginSelectionSet(params);
			}
		}

		void endSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::StringValueHas::endSelectionSet<T>)
			{
				_pimpl->endSelectionSet(params);
			}
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	StringValue(std::unique_ptr<const Concept>&& pimpl) noexcept;

	// Unions which include this type
	friend PropValue;

	template <class I>
	[[nodiscard]] static constexpr bool implements() noexcept
	{
		return implements::StringValueIs<I>;
	}

	[[nodiscard]] service::TypeNames getTypeNames() const noexcept;
	[[nodiscard]] service::ResolverMap getResolvers() const noexcept;

	void beginSelectionSet(const service::SelectionSetParams& params) const final;
	void endSelectionSet(const service::SelectionSetParams& params) const final;

	const std::unique_ptr<const Concept> _pimpl;

public:
	template <class T>
	StringValue(std::shared_ptr<T> pimpl) noexcept
		: StringValue { std::unique_ptr<const Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
	}

	[[nodiscard]] static constexpr std::string_view getObjectType() noexcept
	{
		return { R"gql(StringValue)gql" };
	}
};

} // namespace graphql::mapi::object

#endif // STRINGVALUEOBJECT_H

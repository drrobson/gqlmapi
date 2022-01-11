// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef FOLDERUPDATEDOBJECT_H
#define FOLDERUPDATEDOBJECT_H

#include "MAPISchema.h"

namespace graphql::mapi::object {
namespace implements {

template <class I>
concept FolderUpdatedIs = std::is_same_v<I, FolderChange>;

} // namespace implements

namespace methods::FolderUpdatedHas {

template <class TImpl>
concept getIndexWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<int> { impl.getIndex(std::move(params)) } };
};

template <class TImpl>
concept getIndex = requires (TImpl impl)
{
	{ service::AwaitableScalar<int> { impl.getIndex() } };
};

template <class TImpl>
concept getUpdatedWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableObject<std::shared_ptr<Folder>> { impl.getUpdated(std::move(params)) } };
};

template <class TImpl>
concept getUpdated = requires (TImpl impl)
{
	{ service::AwaitableObject<std::shared_ptr<Folder>> { impl.getUpdated() } };
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

} // namespace methods::FolderUpdatedHas

class FolderUpdated
	: public service::Object
{
private:
	service::AwaitableResolver resolveIndex(service::ResolverParams&& params) const;
	service::AwaitableResolver resolveUpdated(service::ResolverParams&& params) const;

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct Concept
	{
		virtual ~Concept() = default;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;

		virtual service::AwaitableScalar<int> getIndex(service::FieldParams&& params) const = 0;
		virtual service::AwaitableObject<std::shared_ptr<Folder>> getUpdated(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::AwaitableScalar<int> getIndex(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderUpdatedHas::getIndexWithParams<T>)
			{
				return { _pimpl->getIndex(std::move(params)) };
			}
			else if constexpr (methods::FolderUpdatedHas::getIndex<T>)
			{
				return { _pimpl->getIndex() };
			}
			else
			{
				throw std::runtime_error(R"ex(FolderUpdated::getIndex is not implemented)ex");
			}
		}

		service::AwaitableObject<std::shared_ptr<Folder>> getUpdated(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderUpdatedHas::getUpdatedWithParams<T>)
			{
				return { _pimpl->getUpdated(std::move(params)) };
			}
			else if constexpr (methods::FolderUpdatedHas::getUpdated<T>)
			{
				return { _pimpl->getUpdated() };
			}
			else
			{
				throw std::runtime_error(R"ex(FolderUpdated::getUpdated is not implemented)ex");
			}
		}

		void beginSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::FolderUpdatedHas::beginSelectionSet<T>)
			{
				_pimpl->beginSelectionSet(params);
			}
		}

		void endSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::FolderUpdatedHas::endSelectionSet<T>)
			{
				_pimpl->endSelectionSet(params);
			}
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	FolderUpdated(std::unique_ptr<Concept>&& pimpl) noexcept;

	// Unions which include this type
	friend FolderChange;

	template <class I>
	static constexpr bool implements() noexcept
	{
		return implements::FolderUpdatedIs<I>;
	}

	service::TypeNames getTypeNames() const noexcept;
	service::ResolverMap getResolvers() const noexcept;

	void beginSelectionSet(const service::SelectionSetParams& params) const final;
	void endSelectionSet(const service::SelectionSetParams& params) const final;

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	FolderUpdated(std::shared_ptr<T> pimpl) noexcept
		: FolderUpdated { std::unique_ptr<Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
	}
};

} // namespace graphql::mapi::object

#endif // FOLDERUPDATEDOBJECT_H

// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef FOLDEROBJECT_H
#define FOLDEROBJECT_H

#include "MAPISchema.h"

namespace graphql::mapi::object {
namespace methods::FolderHas {

template <class TImpl>
concept getIdWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<response::IdType> { impl.getId(std::move(params)) } };
};

template <class TImpl>
concept getId = requires (TImpl impl)
{
	{ service::AwaitableScalar<response::IdType> { impl.getId() } };
};

template <class TImpl>
concept getParentFolderWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableObject<std::shared_ptr<Folder>> { impl.getParentFolder(std::move(params)) } };
};

template <class TImpl>
concept getParentFolder = requires (TImpl impl)
{
	{ service::AwaitableObject<std::shared_ptr<Folder>> { impl.getParentFolder() } };
};

template <class TImpl>
concept getStoreWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableObject<std::shared_ptr<Store>> { impl.getStore(std::move(params)) } };
};

template <class TImpl>
concept getStore = requires (TImpl impl)
{
	{ service::AwaitableObject<std::shared_ptr<Store>> { impl.getStore() } };
};

template <class TImpl>
concept getNameWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<std::string> { impl.getName(std::move(params)) } };
};

template <class TImpl>
concept getName = requires (TImpl impl)
{
	{ service::AwaitableScalar<std::string> { impl.getName() } };
};

template <class TImpl>
concept getContainerClassWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<std::optional<std::string>> { impl.getContainerClass(std::move(params)) } };
};

template <class TImpl>
concept getContainerClass = requires (TImpl impl)
{
	{ service::AwaitableScalar<std::optional<std::string>> { impl.getContainerClass() } };
};

template <class TImpl>
concept getCountWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<int> { impl.getCount(std::move(params)) } };
};

template <class TImpl>
concept getCount = requires (TImpl impl)
{
	{ service::AwaitableScalar<int> { impl.getCount() } };
};

template <class TImpl>
concept getUnreadWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<int> { impl.getUnread(std::move(params)) } };
};

template <class TImpl>
concept getUnread = requires (TImpl impl)
{
	{ service::AwaitableScalar<int> { impl.getUnread() } };
};

template <class TImpl>
concept getSpecialFolderWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<std::optional<SpecialFolder>> { impl.getSpecialFolder(std::move(params)) } };
};

template <class TImpl>
concept getSpecialFolder = requires (TImpl impl)
{
	{ service::AwaitableScalar<std::optional<SpecialFolder>> { impl.getSpecialFolder() } };
};

template <class TImpl>
concept getColumnsWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableObject<std::vector<std::shared_ptr<Property>>> { impl.getColumns(std::move(params)) } };
};

template <class TImpl>
concept getColumns = requires (TImpl impl)
{
	{ service::AwaitableObject<std::vector<std::shared_ptr<Property>>> { impl.getColumns() } };
};

template <class TImpl>
concept getSubFoldersWithParams = requires (TImpl impl, service::FieldParams params, std::optional<std::vector<response::IdType>> idsArg)
{
	{ service::AwaitableObject<std::vector<std::shared_ptr<Folder>>> { impl.getSubFolders(std::move(params), std::move(idsArg)) } };
};

template <class TImpl>
concept getSubFolders = requires (TImpl impl, std::optional<std::vector<response::IdType>> idsArg)
{
	{ service::AwaitableObject<std::vector<std::shared_ptr<Folder>>> { impl.getSubFolders(std::move(idsArg)) } };
};

template <class TImpl>
concept getConversationsWithParams = requires (TImpl impl, service::FieldParams params, std::optional<std::vector<response::IdType>> idsArg)
{
	{ service::AwaitableObject<std::vector<std::shared_ptr<Conversation>>> { impl.getConversations(std::move(params), std::move(idsArg)) } };
};

template <class TImpl>
concept getConversations = requires (TImpl impl, std::optional<std::vector<response::IdType>> idsArg)
{
	{ service::AwaitableObject<std::vector<std::shared_ptr<Conversation>>> { impl.getConversations(std::move(idsArg)) } };
};

template <class TImpl>
concept getItemsWithParams = requires (TImpl impl, service::FieldParams params, std::optional<std::vector<response::IdType>> idsArg)
{
	{ service::AwaitableObject<std::vector<std::shared_ptr<Item>>> { impl.getItems(std::move(params), std::move(idsArg)) } };
};

template <class TImpl>
concept getItems = requires (TImpl impl, std::optional<std::vector<response::IdType>> idsArg)
{
	{ service::AwaitableObject<std::vector<std::shared_ptr<Item>>> { impl.getItems(std::move(idsArg)) } };
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

} // namespace methods::FolderHas

class [[nodiscard]] Folder final
	: public service::Object
{
private:
	[[nodiscard]] service::AwaitableResolver resolveId(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveParentFolder(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveStore(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveName(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveContainerClass(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveCount(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveUnread(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveSpecialFolder(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveColumns(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveSubFolders(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveConversations(service::ResolverParams&& params) const;
	[[nodiscard]] service::AwaitableResolver resolveItems(service::ResolverParams&& params) const;

	[[nodiscard]] service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct [[nodiscard]] Concept
	{
		virtual ~Concept() = default;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;

		[[nodiscard]] virtual service::AwaitableScalar<response::IdType> getId(service::FieldParams&& params) const = 0;
		[[nodiscard]] virtual service::AwaitableObject<std::shared_ptr<Folder>> getParentFolder(service::FieldParams&& params) const = 0;
		[[nodiscard]] virtual service::AwaitableObject<std::shared_ptr<Store>> getStore(service::FieldParams&& params) const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<std::string> getName(service::FieldParams&& params) const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<std::optional<std::string>> getContainerClass(service::FieldParams&& params) const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<int> getCount(service::FieldParams&& params) const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<int> getUnread(service::FieldParams&& params) const = 0;
		[[nodiscard]] virtual service::AwaitableScalar<std::optional<SpecialFolder>> getSpecialFolder(service::FieldParams&& params) const = 0;
		[[nodiscard]] virtual service::AwaitableObject<std::vector<std::shared_ptr<Property>>> getColumns(service::FieldParams&& params) const = 0;
		[[nodiscard]] virtual service::AwaitableObject<std::vector<std::shared_ptr<Folder>>> getSubFolders(service::FieldParams&& params, std::optional<std::vector<response::IdType>>&& idsArg) const = 0;
		[[nodiscard]] virtual service::AwaitableObject<std::vector<std::shared_ptr<Conversation>>> getConversations(service::FieldParams&& params, std::optional<std::vector<response::IdType>>&& idsArg) const = 0;
		[[nodiscard]] virtual service::AwaitableObject<std::vector<std::shared_ptr<Item>>> getItems(service::FieldParams&& params, std::optional<std::vector<response::IdType>>&& idsArg) const = 0;
	};

	template <class T>
	struct [[nodiscard]] Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		[[nodiscard]] service::AwaitableScalar<response::IdType> getId(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderHas::getIdWithParams<T>)
			{
				return { _pimpl->getId(std::move(params)) };
			}
			else if constexpr (methods::FolderHas::getId<T>)
			{
				return { _pimpl->getId() };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getId is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableObject<std::shared_ptr<Folder>> getParentFolder(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderHas::getParentFolderWithParams<T>)
			{
				return { _pimpl->getParentFolder(std::move(params)) };
			}
			else if constexpr (methods::FolderHas::getParentFolder<T>)
			{
				return { _pimpl->getParentFolder() };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getParentFolder is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableObject<std::shared_ptr<Store>> getStore(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderHas::getStoreWithParams<T>)
			{
				return { _pimpl->getStore(std::move(params)) };
			}
			else if constexpr (methods::FolderHas::getStore<T>)
			{
				return { _pimpl->getStore() };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getStore is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableScalar<std::string> getName(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderHas::getNameWithParams<T>)
			{
				return { _pimpl->getName(std::move(params)) };
			}
			else if constexpr (methods::FolderHas::getName<T>)
			{
				return { _pimpl->getName() };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getName is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableScalar<std::optional<std::string>> getContainerClass(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderHas::getContainerClassWithParams<T>)
			{
				return { _pimpl->getContainerClass(std::move(params)) };
			}
			else if constexpr (methods::FolderHas::getContainerClass<T>)
			{
				return { _pimpl->getContainerClass() };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getContainerClass is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableScalar<int> getCount(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderHas::getCountWithParams<T>)
			{
				return { _pimpl->getCount(std::move(params)) };
			}
			else if constexpr (methods::FolderHas::getCount<T>)
			{
				return { _pimpl->getCount() };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getCount is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableScalar<int> getUnread(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderHas::getUnreadWithParams<T>)
			{
				return { _pimpl->getUnread(std::move(params)) };
			}
			else if constexpr (methods::FolderHas::getUnread<T>)
			{
				return { _pimpl->getUnread() };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getUnread is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableScalar<std::optional<SpecialFolder>> getSpecialFolder(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderHas::getSpecialFolderWithParams<T>)
			{
				return { _pimpl->getSpecialFolder(std::move(params)) };
			}
			else if constexpr (methods::FolderHas::getSpecialFolder<T>)
			{
				return { _pimpl->getSpecialFolder() };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getSpecialFolder is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableObject<std::vector<std::shared_ptr<Property>>> getColumns(service::FieldParams&& params) const final
		{
			if constexpr (methods::FolderHas::getColumnsWithParams<T>)
			{
				return { _pimpl->getColumns(std::move(params)) };
			}
			else if constexpr (methods::FolderHas::getColumns<T>)
			{
				return { _pimpl->getColumns() };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getColumns is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableObject<std::vector<std::shared_ptr<Folder>>> getSubFolders(service::FieldParams&& params, std::optional<std::vector<response::IdType>>&& idsArg) const final
		{
			if constexpr (methods::FolderHas::getSubFoldersWithParams<T>)
			{
				return { _pimpl->getSubFolders(std::move(params), std::move(idsArg)) };
			}
			else if constexpr (methods::FolderHas::getSubFolders<T>)
			{
				return { _pimpl->getSubFolders(std::move(idsArg)) };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getSubFolders is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableObject<std::vector<std::shared_ptr<Conversation>>> getConversations(service::FieldParams&& params, std::optional<std::vector<response::IdType>>&& idsArg) const final
		{
			if constexpr (methods::FolderHas::getConversationsWithParams<T>)
			{
				return { _pimpl->getConversations(std::move(params), std::move(idsArg)) };
			}
			else if constexpr (methods::FolderHas::getConversations<T>)
			{
				return { _pimpl->getConversations(std::move(idsArg)) };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getConversations is not implemented)ex");
			}
		}

		[[nodiscard]] service::AwaitableObject<std::vector<std::shared_ptr<Item>>> getItems(service::FieldParams&& params, std::optional<std::vector<response::IdType>>&& idsArg) const final
		{
			if constexpr (methods::FolderHas::getItemsWithParams<T>)
			{
				return { _pimpl->getItems(std::move(params), std::move(idsArg)) };
			}
			else if constexpr (methods::FolderHas::getItems<T>)
			{
				return { _pimpl->getItems(std::move(idsArg)) };
			}
			else
			{
				throw std::runtime_error(R"ex(Folder::getItems is not implemented)ex");
			}
		}

		void beginSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::FolderHas::beginSelectionSet<T>)
			{
				_pimpl->beginSelectionSet(params);
			}
		}

		void endSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::FolderHas::endSelectionSet<T>)
			{
				_pimpl->endSelectionSet(params);
			}
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Folder(std::unique_ptr<const Concept>&& pimpl) noexcept;

	[[nodiscard]] service::TypeNames getTypeNames() const noexcept;
	[[nodiscard]] service::ResolverMap getResolvers() const noexcept;

	void beginSelectionSet(const service::SelectionSetParams& params) const final;
	void endSelectionSet(const service::SelectionSetParams& params) const final;

	const std::unique_ptr<const Concept> _pimpl;

public:
	template <class T>
	Folder(std::shared_ptr<T> pimpl) noexcept
		: Folder { std::unique_ptr<const Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
	}

	[[nodiscard]] static constexpr std::string_view getObjectType() noexcept
	{
		return { R"gql(Folder)gql" };
	}
};

} // namespace graphql::mapi::object

#endif // FOLDEROBJECT_H

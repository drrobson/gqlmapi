// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "SubscriptionObject.h"
#include "ItemChangeObject.h"
#include "FolderChangeObject.h"

#include "graphqlservice/internal/Schema.h"

#include "graphqlservice/introspection/IntrospectionSchema.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std::literals;

namespace graphql::mapi {
namespace object {

Subscription::Subscription(std::unique_ptr<const Concept>&& pimpl) noexcept
	: service::Object{ getTypeNames(), getResolvers() }
	, _pimpl { std::move(pimpl) }
{
}

service::TypeNames Subscription::getTypeNames() const noexcept
{
	return {
		R"gql(Subscription)gql"sv
	};
}

service::ResolverMap Subscription::getResolvers() const noexcept
{
	return {
		{ R"gql(items)gql"sv, [this](service::ResolverParams&& params) { return resolveItems(std::move(params)); } },
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ R"gql(subFolders)gql"sv, [this](service::ResolverParams&& params) { return resolveSubFolders(std::move(params)); } },
		{ R"gql(rootFolders)gql"sv, [this](service::ResolverParams&& params) { return resolveRootFolders(std::move(params)); } }
	};
}

void Subscription::beginSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->beginSelectionSet(params);
}

void Subscription::endSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->endSelectionSet(params);
}

service::AwaitableResolver Subscription::resolveItems(service::ResolverParams&& params) const
{
	auto argFolderId = service::ModifiedArgument<mapi::ObjectId>::require("folderId", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getItems(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argFolderId));
	resolverLock.unlock();

	return service::ModifiedResult<ItemChange>::convert<service::TypeModifier::List>(std::move(result), std::move(params));
}

service::AwaitableResolver Subscription::resolveSubFolders(service::ResolverParams&& params) const
{
	auto argParentFolderId = service::ModifiedArgument<mapi::ObjectId>::require("parentFolderId", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getSubFolders(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argParentFolderId));
	resolverLock.unlock();

	return service::ModifiedResult<FolderChange>::convert<service::TypeModifier::List>(std::move(result), std::move(params));
}

service::AwaitableResolver Subscription::resolveRootFolders(service::ResolverParams&& params) const
{
	auto argStoreId = service::ModifiedArgument<response::IdType>::require("storeId", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getRootFolders(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argStoreId));
	resolverLock.unlock();

	return service::ModifiedResult<FolderChange>::convert<service::TypeModifier::List>(std::move(result), std::move(params));
}

service::AwaitableResolver Subscription::resolve_typename(service::ResolverParams&& params) const
{
	return service::Result<std::string>::convert(std::string{ R"gql(Subscription)gql" }, std::move(params));
}

} // namespace object

void AddSubscriptionDetails(const std::shared_ptr<schema::ObjectType>& typeSubscription, const std::shared_ptr<schema::Schema>& schema)
{
	typeSubscription->AddFields({
		schema::Field::Make(R"gql(items)gql"sv, R"md(Get updates on items in a folder.)md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->WrapType(introspection::TypeKind::LIST, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(ItemChange)gql"sv)))), {
			schema::InputValue::Make(R"gql(folderId)gql"sv, R"md(ID of the folder)md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(ObjectId)gql"sv)), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(subFolders)gql"sv, R"md(Get updates on sub-folders of a folder.)md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->WrapType(introspection::TypeKind::LIST, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(FolderChange)gql"sv)))), {
			schema::InputValue::Make(R"gql(parentFolderId)gql"sv, R"md(ID of the parent folder)md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(ObjectId)gql"sv)), R"gql()gql"sv)
		}),
		schema::Field::Make(R"gql(rootFolders)gql"sv, R"md(Get updates on the root folders of a store.)md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->WrapType(introspection::TypeKind::LIST, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(FolderChange)gql"sv)))), {
			schema::InputValue::Make(R"gql(storeId)gql"sv, R"md(ID of the store)md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(ID)gql"sv)), R"gql()gql"sv)
		})
	});
}

} // namespace graphql::mapi

// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "QueryObject.h"
#include "StoreObject.h"
#include "PropertyObject.h"

#include "graphqlservice/internal/Introspection.h"

#include "graphqlservice/introspection/SchemaObject.h"
#include "graphqlservice/introspection/TypeObject.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std::literals;

namespace graphql::mapi {
namespace object {

Query::Query(std::unique_ptr<const Concept>&& pimpl) noexcept
	: service::Object{ getTypeNames(), getResolvers() }
	, _schema { GetSchema() }
	, _pimpl { std::move(pimpl) }
{
}

service::TypeNames Query::getTypeNames() const noexcept
{
	return {
		R"gql(Query)gql"sv
	};
}

service::ResolverMap Query::getResolvers() const noexcept
{
	return {
		{ R"gql(__type)gql"sv, [this](service::ResolverParams&& params) { return resolve_type(std::move(params)); } },
		{ R"gql(stores)gql"sv, [this](service::ResolverParams&& params) { return resolveStores(std::move(params)); } },
		{ R"gql(__schema)gql"sv, [this](service::ResolverParams&& params) { return resolve_schema(std::move(params)); } },
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ R"gql(msgFileData)gql"sv, [this](service::ResolverParams&& params) { return resolveMsgFileData(std::move(params)); } }
	};
}

void Query::beginSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->beginSelectionSet(params);
}

void Query::endSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->endSelectionSet(params);
}

service::AwaitableResolver Query::resolveStores(service::ResolverParams&& params) const
{
	auto argIds = service::ModifiedArgument<response::IdType>::require<service::TypeModifier::Nullable, service::TypeModifier::List>("ids", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getStores(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argIds));
	resolverLock.unlock();

	return service::ModifiedResult<Store>::convert<service::TypeModifier::List>(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolveMsgFileData(service::ResolverParams&& params) const
{
	auto argFilepath = service::ModifiedArgument<std::string>::require("filepath", params.arguments);
	auto argProps = service::ModifiedArgument<mapi::Column>::require<service::TypeModifier::List>("props", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getMsgFileData(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argFilepath), std::move(argProps));
	resolverLock.unlock();

	return service::ModifiedResult<Property>::convert<service::TypeModifier::List, service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Query::resolve_typename(service::ResolverParams&& params) const
{
	return service::Result<std::string>::convert(std::string{ R"gql(Query)gql" }, std::move(params));
}

service::AwaitableResolver Query::resolve_schema(service::ResolverParams&& params) const
{
	return service::Result<service::Object>::convert(std::static_pointer_cast<service::Object>(std::make_shared<introspection::object::Schema>(std::make_shared<introspection::Schema>(_schema))), std::move(params));
}

service::AwaitableResolver Query::resolve_type(service::ResolverParams&& params) const
{
	auto argName = service::ModifiedArgument<std::string>::require("name", params.arguments);
	const auto& baseType = _schema->LookupType(argName);
	std::shared_ptr<introspection::object::Type> result { baseType ? std::make_shared<introspection::object::Type>(std::make_shared<introspection::Type>(baseType)) : nullptr };

	return service::ModifiedResult<introspection::object::Type>::convert<service::TypeModifier::Nullable>(result, std::move(params));
}

} // namespace object

void AddQueryDetails(const std::shared_ptr<schema::ObjectType>& typeQuery, const std::shared_ptr<schema::Schema>& schema)
{
	typeQuery->AddFields({
		schema::Field::Make(R"gql(stores)gql"sv, R"md(List of stores, which may include stores that are not associated with any account.)md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->WrapType(introspection::TypeKind::LIST, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Store)gql"sv)))), {
			schema::InputValue::Make(R"gql(ids)gql"sv, R"md(Optional list of store IDs, return all stores if `null`)md"sv, schema->WrapType(introspection::TypeKind::LIST, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(ID)gql"sv))), R"gql(null)gql"sv)
		}),
		schema::Field::Make(R"gql(msgFileData)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->WrapType(introspection::TypeKind::LIST, schema->LookupType(R"gql(Property)gql"sv))), {
			schema::InputValue::Make(R"gql(filepath)gql"sv, R"md(Filepath on the local machine of the .msg file to read)md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(String)gql"sv)), R"gql()gql"sv),
			schema::InputValue::Make(R"gql(props)gql"sv, R"md(List of MAPI property IDs to read from the file)md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->WrapType(introspection::TypeKind::LIST, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Column)gql"sv)))), R"gql()gql"sv)
		})
	});
}

} // namespace graphql::mapi

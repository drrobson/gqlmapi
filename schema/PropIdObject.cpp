// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "PropIdObject.h"

#include "graphqlservice/internal/Schema.h"

#include "graphqlservice/introspection/IntrospectionSchema.h"

using namespace std::literals;

namespace graphql::mapi {
namespace object {

PropId::PropId(std::unique_ptr<const Concept>&& pimpl) noexcept
	: service::Object { pimpl->getTypeNames(), pimpl->getResolvers() }
	, _pimpl { std::move(pimpl) }
{
}

void PropId::beginSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->beginSelectionSet(params);
}

void PropId::endSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->endSelectionSet(params);
}

} // namespace object

void AddPropIdDetails(const std::shared_ptr<schema::UnionType>& typePropId, const std::shared_ptr<schema::Schema>& schema)
{
	typePropId->AddPossibleTypes({
		schema->LookupType(R"gql(IntId)gql"sv),
		schema->LookupType(R"gql(NamedId)gql"sv)
	});
}

} // namespace graphql::mapi

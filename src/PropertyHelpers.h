#include "Types.h"

namespace graphql::mapi::prop {

std::vector<std::pair<ULONG, LPMAPINAMEID>> LookupPropIdInputs(
	IMAPIProp* pObject, NameIdToPropId& nameIdMap, std::vector<PropIdInput>&& namedProps);

std::vector<std::pair<ULONG, LPMAPINAMEID>> LookupPropIds(
	IMAPIProp* pObject, NameIdToPropId& nameIdMap, const std::vector<ULONG>& propIds);

std::vector<std::shared_ptr<object::Property>> GetProperties(
	IMAPIProp* pObject, NameIdToPropId& nameIdMap, std::optional<std::vector<Column>>&& idsArg);

} // namespace graphql::mapi
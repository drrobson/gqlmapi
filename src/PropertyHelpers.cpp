#include "PropertyHelpers.h"

#include "Guid.h"
#include "PropertyObject.h"

namespace graphql::mapi::prop {

std::vector<std::pair<ULONG, LPMAPINAMEID>> LookupPropIdInputs(
	IMAPIProp* pObject, NameIdToPropId& nameIdMap, std::vector<PropIdInput>&& namedProps)
{
	std::vector<std::pair<ULONG, LPMAPINAMEID>> result(namedProps.size());
	std::vector<std::pair<mapi_ptr<MAPINAMEID>, size_t>> resolve;

	resolve.reserve(namedProps.size());
	for (size_t i = 0; i < namedProps.size(); ++i)
	{
		const auto& id = namedProps[i];

		if (id.id)
		{
			// Regular PROP_ID, just add it to the result list.
			const ULONG propId = static_cast<ULONG>(*id.id);

			CFRt(!id.named);
			result[i] = std::make_pair(PROP_TAG(PT_UNSPECIFIED, propId), nullptr);
			continue;
		}

		if (!id.named)
		{
			constexpr bool Empty_PropIdInput = false;
			CFRt(Empty_PropIdInput);
		}

		// It's a named prop, need to break it down further.
		mapi_ptr<MAPINAMEID> namedId;

		CORt(::MAPIAllocateBuffer(sizeof(*namedId) + sizeof(*namedId->lpguid),
			reinterpret_cast<void**>(&out_ptr { namedId })));

		const auto& propset = id.named->propset.get<std::string>();

		namedId->lpguid = reinterpret_cast<LPGUID>(namedId.get() + 1);
		*namedId->lpguid = convert::guid::from_string(propset);

		if (id.named->id)
		{
			CFRt(!id.named->name);
			namedId->ulKind = MNID_ID;
			namedId->Kind.lID = *id.named->id;
		}
		else if (id.named->name)
		{
			auto name = convert::utf8::to_utf16(*id.named->name);

			namedId->ulKind = MNID_STRING;
			CORt(::MAPIAllocateMore(static_cast<ULONG>((name.size() + 1) * sizeof(wchar_t)),
				namedId.get(),
				reinterpret_cast<void**>(&namedId->Kind.lpwstrName)));
			CFRt(namedId->Kind.lpwstrName != nullptr);
			std::copy(name.cbegin(), name.cend(), namedId->Kind.lpwstrName);
			namedId->Kind.lpwstrName[name.size()] = L'\0';
		}
		else
		{
			constexpr bool Missing_NamedId = false;
			CFRt(Missing_NamedId);
		}

		const auto itr = nameIdMap.find(namedId);

		if (itr != nameIdMap.cend())
		{
			// Already cached, just add it directly to the results.
			result[i] = std::make_pair(itr->second, itr->first.get());
		}
		else
		{
			// Not cached, add it to the list to resolve.
			result[i] = std::make_pair(PR_NULL, nullptr);
			resolve.push_back(std::make_pair(std::move(namedId), i));
		}
	}

	if (!resolve.empty())
	{
		std::vector<LPMAPINAMEID> pmnids;
		mapi_ptr<SPropTagArray> namedPropIds;

		pmnids.resize(resolve.size());
		std::transform(resolve.cbegin(),
			resolve.cend(),
			pmnids.begin(),
			[](const auto& entry) noexcept {
				return entry.first.get();
			});

		CORt(pObject->GetIDsFromNames(static_cast<ULONG>(pmnids.size()),
			pmnids.data(),
			0,
			&out_ptr { namedPropIds }));
		CFRt(nullptr != namedPropIds);
		CFRt(static_cast<size_t>(namedPropIds->cValues) == resolve.size());
		for (size_t i = 0; i < resolve.size(); ++i)
		{
			const ULONG namedPropId = namedPropIds->aulPropTag[i];
			const size_t offset = resolve[i].second;

			if (PROP_TYPE(namedPropId) == PT_ERROR)
			{
				result[offset] = std::make_pair(PR_NULL, nullptr);
				continue;
			}

			const ULONG propId = PROP_TAG(PT_UNSPECIFIED, PROP_ID(namedPropId));
			auto itr =
				nameIdMap.insert(std::make_pair(std::move(resolve[i].first), propId)).first;

			result[offset] = std::make_pair(propId, itr->first.get());
		}
	}

	return result;
}

std::vector<std::pair<ULONG, LPMAPINAMEID>> LookupPropIds(
	IMAPIProp* pObject, NameIdToPropId& nameIdMap, const std::vector<ULONG>& propIds)
{
	std::vector<std::pair<ULONG, LPMAPINAMEID>> result(propIds.size());
	std::vector<std::pair<ULONG, size_t>> resolve;

	resolve.reserve(propIds.size());
	for (size_t i = 0; i < propIds.size(); ++i)
	{
		const ULONG propId = PROP_ID(propIds[i]);

		if (propId < 0x8000)
		{
			// Regular PROP_ID, just add it to the result list.
			result[i] = std::make_pair(PROP_TAG(PT_UNSPECIFIED, propId), nullptr);
			continue;
		}

		const auto itr = std::find_if(nameIdMap.cbegin(),
			nameIdMap.cend(),
			[propId](const auto& entry) noexcept {
				return PROP_ID(entry.second) == propId;
			});

		if (itr != nameIdMap.cend())
		{
			// Already cached, just add it directly to the results.
			result[i] = std::make_pair(itr->second, itr->first.get());
		}
		else
		{
			// Not cached, add it to the list to resolve.
			result[i] = std::make_pair(PR_NULL, nullptr);
			resolve.push_back(std::make_pair(propId, i));
		}
	}

	if (!resolve.empty())
	{
		mapi_ptr<SPropTagArray> propIds;

		CORt(::MAPIAllocateBuffer(CbNewSPropTagArray(static_cast<ULONG>(resolve.size())),
			reinterpret_cast<void**>(&out_ptr { propIds })));
		CFRt(propIds != nullptr);

		propIds->cValues = static_cast<ULONG>(resolve.size());
		std::transform(resolve.begin(),
			resolve.end(),
			propIds->aulPropTag,
			[](const auto& entry) noexcept -> ULONG {
				return PROP_TAG(PT_UNSPECIFIED, entry.first);
			});

		LPSPropTagArray pPropIds = propIds.get();
		ULONG cPropNames = 0;
		mapi_ptr<LPMAPINAMEID> propNames;

		CORt(pObject->GetNamesFromIDs(&pPropIds, nullptr, 0, &cPropNames, &out_ptr { propNames }));
		CFRt(static_cast<size_t>(cPropNames) == resolve.size());
		CFRt(propNames != nullptr);

		for (size_t i = 0; i < resolve.size(); ++i)
		{
			const LPMAPINAMEID name = propNames.get()[i];
			const ULONG propId = PROP_TAG(PT_UNSPECIFIED, resolve[i].first);
			const size_t offset = resolve[i].second;
			mapi_ptr<MAPINAMEID> namedId;

			CORt(::MAPIAllocateBuffer(sizeof(*namedId) + sizeof(*namedId->lpguid),
				reinterpret_cast<void**>(&out_ptr { namedId })));

			namedId->lpguid = reinterpret_cast<LPGUID>(namedId.get() + 1);
			memmove(namedId->lpguid, name->lpguid, sizeof(namedId->lpguid));
			namedId->ulKind = name->ulKind;

			if (name->ulKind == MNID_STRING)
			{
				std::wstring_view source { name->Kind.lpwstrName };

				namedId->ulKind = MNID_STRING;
				CORt(::MAPIAllocateMore(static_cast<ULONG>((source.size() + 1) * sizeof(wchar_t)),
					namedId.get(),
					reinterpret_cast<void**>(&namedId->Kind.lpwstrName)));
				CFRt(namedId->Kind.lpwstrName != nullptr);
				std::copy(source.cbegin(), source.cend(), namedId->Kind.lpwstrName);
				namedId->Kind.lpwstrName[source.size()] = L'\0';
			}
			else
			{
				CFRt(name->ulKind == MNID_ID);
				namedId->Kind.lID = name->Kind.lID;
			}

			auto itr = nameIdMap.insert(std::make_pair(std::move(namedId), propId)).first;

			result[offset] = std::make_pair(itr->second, itr->first.get());
		}
	}

	return result;
}

std::vector<std::shared_ptr<object::Property>> GetProperties(
	IMAPIProp* pObject, NameIdToPropId& nameIdMap, std::optional<std::vector<Column>>&& idsArg)
{
	constexpr std::array c_propTypes {
				PT_LONG,
				PT_BOOLEAN,
				PT_UNICODE,
				PT_CLSID,
				PT_SYSTIME,
				PT_BINARY,
			};

	std::map<ULONG, Property::id_variant> idMap;
	ULONG cValues = 0;
	mapi_ptr<SPropValue> props;
	LPSPropValue propBegin = nullptr;
	LPSPropValue propEnd = nullptr;

	if (idsArg && !idsArg->empty())
	{
		// Only get selected properties.
		std::vector<PropIdInput> inputs(idsArg->size());

		std::transform(idsArg->cbegin(),
			idsArg->cend(),
			inputs.begin(),
			[](const Column& column) noexcept {
				return column.property;
			});

		auto resolved = LookupPropIdInputs(pObject, nameIdMap, std::move(inputs));

		CFRt(resolved.size() == idsArg->size());

		mapi_ptr<SPropTagArray> propIds;

		CORt(::MAPIAllocateBuffer(CbNewSPropTagArray(static_cast<ULONG>(resolved.size())),
			reinterpret_cast<void**>(&out_ptr { propIds })));
		CFRt(propIds != nullptr);
		propIds->cValues = static_cast<ULONG>(resolved.size());

		for (size_t i = 0; i < resolved.size(); ++i)
		{
			CFRt(static_cast<size_t>(idsArg->at(i).type) < c_propTypes.size());

			const auto& entry = resolved[i];
			const auto propType = c_propTypes[static_cast<size_t>(idsArg->at(i).type)];
			const ULONG propId = PROP_ID(entry.first);
			const LPMAPINAMEID name = entry.second;

			propIds->aulPropTag[i] = PROP_TAG(propType, propId);

			if (name == nullptr)
			{
				idMap[propId] = propId;
			}
			else
			{
				idMap[propId] = *name;
			}
		}

		// Get the props
		LPSPropValue values = nullptr;

		CORt(pObject->GetProps(propIds.get(), MAPI_UNICODE, &cValues, &values));
		CFRt(values != nullptr);
		CFRt(cValues > 0);
		props.reset(values);
		propBegin = props.get();
		propEnd = propBegin + static_cast<size_t>(cValues);
	}
	else
	{
		// Get all of the properties.
		LPSPropValue values = nullptr;

		CORt(pObject->GetProps(nullptr, MAPI_UNICODE, &cValues, &values));
		CFRt(values != nullptr);
		CFRt(cValues > 0);
		props.reset(values);
		propBegin = props.get();
		propEnd = propBegin + static_cast<size_t>(cValues);

		std::vector<ULONG> propIds(static_cast<size_t>(cValues));

		std::transform(propBegin, propEnd, propIds.begin(), [](const SPropValue& value) noexcept {
			return value.ulPropTag;
		});

		auto resolved = LookupPropIds(pObject, nameIdMap, propIds);

		for (const auto& entry : resolved)
		{
			const ULONG propId = PROP_ID(entry.first);
			const LPMAPINAMEID name = entry.second;

			if (name == nullptr)
			{
				idMap[propId] = propId;
			}
			else
			{
				idMap[propId] = *name;
			}
		}
	}

	// Double check that we mapped all of the property IDs.
	CFRt(idMap.size() == static_cast<size_t>(cValues));

	// Split the values allocation into a separate buffer for each property and build the result vector.
	std::vector<std::shared_ptr<object::Property>> result(cValues);

	for (size_t i = 0; i < cValues; ++i)
	{
		auto& prop = props.get()[i];
		const ULONG propId = PROP_ID(prop.ulPropTag);
	
		if (idsArg && !idsArg->empty() && PROP_TYPE(prop.ulPropTag) == PT_ERROR && prop.Value.err == MAPI_E_NOT_ENOUGH_MEMORY)
		{
			// We failed to get a property because it was too large, and we know the type the consumer wanted
			// because they provided a list of properties. Try again with OpenProperty for their requested type.
			CComPtr<IStream> stream;
			CORt(pObject->OpenProperty(PROP_TAG(c_propTypes[static_cast<size_t>(idsArg->at(i).type)], propId),
				&IID_IStream,
				STGM_READ,
				0,
				reinterpret_cast<LPUNKNOWN*>(&stream)));
			
			StreamEncoding encoding = StreamEncoding::unknown;
			switch(idsArg->at(i).type)
			{
				case PropType::BINARY:
					encoding = StreamEncoding::binary;
					break;
				case PropType::STRING:
					encoding = StreamEncoding::utf16; // MAPI's PT_UNICODE is UTF-16
					break;
			}

			result[i] = std::make_shared<object::Property>(
				std::make_shared<Property>(std::move(idMap[propId]), DataStream { std::move(stream), encoding }));
		}
		else
		{
			mapi_ptr<SPropValue> dupe;

			CORt(ScDupPropset(1, &prop, ::MAPIAllocateBuffer, &out_ptr { dupe }));
			CFRt(dupe != nullptr);

			result[i] = std::make_shared<object::Property>(
				std::make_shared<Property>(std::move(idMap[propId]), std::move(dupe)));
		}
	}

	return result;
}

} // namespace graphql::mapi::prop
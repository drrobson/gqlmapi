// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Types.h"


#include "Guid.h"
#include "PropertyHelpers.h"
#include "StoreObject.h"

#include <Imessage.h>

namespace graphql::mapi {

Query::Query(const std::shared_ptr<Session>& session)
	: m_session { session }
{
}

Query::~Query()
{
	// Release all of the MAPI objects we opened and free any MAPI memory allocations.
	if (m_storeSink)
	{
		m_storeSink->Unadvise();
	}

	m_stores.reset();
	m_session.reset();
}

const std::vector<std::shared_ptr<Store>>& Query::stores()
{
	LoadStores({});
	return *m_stores;
}

std::shared_ptr<Store> Query::lookup(const response::IdType& id)
{
	LoadStores({});

	auto itr = m_ids->find(id);

	if (itr == m_ids->cend())
	{
		return nullptr;
	}

	return m_stores->at(itr->second);
}

void Query::ClearCaches()
{
	if (m_stores)
	{
		for (const auto& entry : *m_stores)
		{
			entry->ClearCaches();
		}
	}
}

void Query::LoadStores(service::Directives&& fieldDirectives)
{
	if (m_storeDirectives != fieldDirectives)
	{
		// Reset the stores and reload if the directives change
		m_stores.reset();
		m_storeDirectives = std::move(fieldDirectives);
	}

	if (m_stores)
	{
		return;
	}

	m_ids = std::make_unique<std::map<response::IdType, size_t>>();
	m_stores = std::make_unique<std::vector<std::shared_ptr<Store>>>();

	// Enumerate the message stores table and fill in the Store object collection.
	constexpr auto c_storeProps = Store::GetStoreColumns();
	mapi_ptr<SPropTagArray> storeProps;

	CORt(::MAPIAllocateBuffer(CbNewSPropTagArray(static_cast<ULONG>(c_storeProps.size())),
		reinterpret_cast<void**>(&out_ptr { storeProps })));
	CFRt(storeProps != nullptr);
	storeProps->cValues = static_cast<ULONG>(c_storeProps.size());
	std::copy(c_storeProps.begin(), c_storeProps.end(), storeProps->aulPropTag);

	constexpr std::array c_storeSorts = Store::GetStoreSorts();
	mapi_ptr<SSortOrderSet> storeSorts;

	CORt(::MAPIAllocateBuffer(CbNewSSortOrderSet(static_cast<ULONG>(c_storeSorts.size())),
		reinterpret_cast<void**>(&out_ptr { storeSorts })));
	CFRt(storeSorts != nullptr);
	storeSorts->cSorts = static_cast<ULONG>(c_storeSorts.size());
	storeSorts->cCategories = 0;
	storeSorts->cExpanded = 0;
	std::copy(c_storeSorts.begin(), c_storeSorts.end(), storeSorts->aSort);

	const TableDirectives directives { {}, m_storeDirectives };
	CComPtr<IMAPITable> sptable;

	CORt(m_session->session()->GetMsgStoresTable(0, &sptable));

	const rowset_ptr sprows =
		directives.read(sptable, std::move(storeProps), std::move(storeSorts));

	m_stores->reserve(static_cast<size_t>(sprows->cRows));
	for (ULONG i = 0; i != sprows->cRows; i++)
	{
		auto& row = sprows->aRow[i];
		const size_t columnCount = static_cast<size_t>(row.cValues);
		mapi_ptr<SPropValue> columns { row.lpProps };

		row.lpProps = nullptr;

		auto store = std::make_shared<Store>(m_session->session(), columnCount, std::move(columns));

		m_ids->insert(std::make_pair(store->id(), m_stores->size()));
		m_stores->push_back(std::move(store));
	}

	if (!m_storeSink)
	{
		auto spThis = shared_from_this();
		CComPtr<AdviseSinkProxy<IMAPITable>> sinkProxy;
		ULONG_PTR connectionId = 0;

		sinkProxy.Attach(new AdviseSinkProxy<IMAPITable>(
			[wpQuery = std::weak_ptr { spThis }](size_t, LPNOTIFICATION) {
				auto spQuery = wpQuery.lock();

				if (spQuery)
				{
					spQuery->m_stores.reset();
				}
			}));

		CORt(sptable->Advise(fnevTableModified, sinkProxy, &connectionId));
		sinkProxy->OnAdvise(sptable, connectionId);

		m_storeSink = sinkProxy;
	}
}

void Query::endSelectionSet(const service::SelectionSetParams&)
{
	ClearCaches();
}

std::vector<std::shared_ptr<object::Store>> Query::getStores(
	service::FieldParams&& params, std::optional<std::vector<response::IdType>>&& idsArg)
{
	std::vector<std::shared_ptr<object::Store>> result {};

	LoadStores(std::move(params.fieldDirectives));

	if (idsArg)
	{
        // Error out if caller requested a non-existent store
        for (const auto& id : idsArg.value())
        {
            if (!lookup(id))
                throw std::exception(std::format("Store not found with id=\"{}\"", id.c_str()).c_str());
        }

		// Lookup the stores with the specified IDs
		result.resize(idsArg->size());
		std::transform(idsArg->cbegin(),
			idsArg->cend(),
			result.begin(),
			[this](const response::IdType& id) noexcept {
				return std::make_shared<object::Store>(lookup(id));
			});
	}
	else
	{
		// Just clone the entire vector of stores.
		result.resize(m_stores->size());
		std::transform(m_stores->cbegin(),
			m_stores->cend(),
			result.begin(),
			[this](const std::shared_ptr<Store>& store) noexcept {
				return std::make_shared<object::Store>(store);
			});
	}

	return result;
}

std::vector<std::shared_ptr<object::Property>> Query::getMsgFileData(
		service::FieldParams&& params, std::string&& filepathArg, std::vector<Column>&& propsArg)
{
	auto filepath = convert::utf8::to_utf16(filepathArg);
	NameIdToPropId nameIdMap;

	CComPtr<IMalloc> malloc;
	CORt(CoGetMalloc(1, &malloc));

	CComPtr<IStorage> storage;
	CORt(StgOpenStorage(filepath.c_str(),
		nullptr,
		STGM_READ | STGM_SHARE_DENY_WRITE,
		nullptr,
		0,
		&storage));

	mapi_ptr<IMessage> message;
	CORt(OpenIMsgOnIStg(nullptr, //session.get(),
		MAPIAllocateBuffer,
		MAPIAllocateMore,
		MAPIFreeBuffer,
		malloc,
		nullptr /*support*/,
		storage,
		nullptr /*release callback*/,
		0 /*caller data*/,
		MAPI_UNICODE /*flags*/,
		&out_ptr {message}));

	return prop::GetProperties(message.get(), nameIdMap, propsArg);
}

} // namespace graphql::mapi
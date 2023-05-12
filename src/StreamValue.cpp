// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Unicode.h"
#include "Types.h"

#include <graphqlservice/internal/Base64.h>

namespace graphql::mapi {

std::string ReadBlobFromStream(DataStream& value)
{
    IStream* stream = value.first;

    STATSTG stat {};
    CORt(stream->Stat(&stat, STATFLAG_NONAME));

    if (stat.cbSize.HighPart != 0)
    {
        throw new std::runtime_error("stream is too large to convert to a string");
    }

    switch (value.second)
    {
        case StreamEncoding::utf8:
        {
            std::string blob(stat.cbSize.LowPart, 0);
            ULONG cbRead = 0;
            CORt(stream->Read(blob.data(), blob.size(), &cbRead));
            CFRt(cbRead == stat.cbSize.LowPart);
            
            return blob;
        }
        case StreamEncoding::utf16:
        {
            std::wstring blob(stat.cbSize.LowPart / sizeof(wchar_t), 0);
            ULONG cbRead = 0;
            CORt(stream->Read(blob.data(), blob.size() * sizeof(wchar_t), &cbRead));
            CFRt(cbRead == stat.cbSize.LowPart);

            return convert::utf8::to_utf8(reinterpret_cast<const wchar_t*>(blob.data()));
        }
        default:
        {
            response::IdType::ByteData binary(stat.cbSize.LowPart);

            ULONG cbRead = 0;
            CORt(stream->Read(binary.data(), binary.size(), &cbRead));
            CFRt(cbRead == stat.cbSize.LowPart);

            return internal::Base64::toBase64(binary);
        }
    }
}

StreamValue::StreamValue(DataStream&& dataStream)
    : m_dataStream { std::move(dataStream) }
    , m_data { ReadBlobFromStream(m_dataStream) }
{
}

const std::string& StreamValue::getData() const
{
    return m_data;
}

} // namespace graphql::mapi
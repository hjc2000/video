//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsAbstractDescriptor.h"
#include "tsAbstractTable.h"
#include "tsDescriptor.h"
#include "tsMemory.h"
#include "tsNames.h"
#include "tsPSIRepository.h"


//----------------------------------------------------------------------------
// Constructors for Descriptor
// Note that the max size of a descriptor is 257 bytes: 2 (header) + 255
//----------------------------------------------------------------------------

ts::Descriptor::Descriptor(const void *addr, size_t size) :
	_data(size >= 2 && size < 258 && (reinterpret_cast<const uint8_t *>(addr))[1] == size - 2 ? new ByteBlock(addr, size) : nullptr)
{
}

ts::Descriptor::Descriptor(const ByteBlock &bb) :
	_data(bb.size() >= 2 && bb.size() < 258 && bb[1] == bb.size() - 2 ? new ByteBlock(bb) : nullptr)
{
}

ts::Descriptor::Descriptor(DID tag, const void *data, size_t size) :
	_data(size < 256 ? new ByteBlock(size + 2) : nullptr)
{
	if (!_data.isNull())
	{
		(*_data)[0] = tag;
		(*_data)[1] = uint8_t(size);
		std::memcpy(_data->data() + 2, data, size);
	}
}

ts::Descriptor::Descriptor(DID tag, const ByteBlock &data) :
	_data(data.size() < 256 ? new ByteBlock(2) : nullptr)
{
	if (!_data.isNull())
	{
		(*_data)[0] = tag;
		(*_data)[1] = uint8_t(data.size());
		_data->append(data);
	}
}

ts::Descriptor::Descriptor(const ByteBlockPtr &bbp, ShareMode mode)
{
	if (!bbp.isNull() && bbp->size() >= 2 && bbp->size() < 258 && (*bbp)[1] == bbp->size() - 2)
	{
		switch (mode)
		{
		case ShareMode::SHARE:
			_data = bbp;
			break;
		case ShareMode::COPY:
			_data = new ByteBlock(*bbp);
			break;
		default:
			// should not get there
			assert(false);
		}
	}
}

ts::Descriptor::Descriptor(const Descriptor &desc, ShareMode mode)
{
	switch (mode)
	{
	case ShareMode::SHARE:
		_data = desc._data;
		break;
	case ShareMode::COPY:
		_data = new ByteBlock(*desc._data);
		break;
	default:
		// should not get there
		assert(false);
	}
}

ts::Descriptor::Descriptor(Descriptor &&desc) noexcept :
	_data(std::move(desc._data))
{
}


//----------------------------------------------------------------------------
// Assignment operators.
//----------------------------------------------------------------------------

ts::Descriptor &ts::Descriptor::operator=(const Descriptor &desc)
{
	if (&desc != this)
	{
		_data = desc._data;
	}
	return *this;
}

ts::Descriptor &ts::Descriptor::operator=(Descriptor &&desc) noexcept
{
	if (&desc != this)
	{
		_data = std::move(desc._data);
	}
	return *this;
}

ts::Descriptor &ts::Descriptor::copy(const Descriptor &desc)
{
	if (&desc != this)
	{
		_data = new ByteBlock(*desc._data);
	}
	return *this;
}


//----------------------------------------------------------------------------
// Get the extended descriptor id.
//----------------------------------------------------------------------------

ts::EDID ts::Descriptor::edid(PDS pds, const AbstractTable *table) const
{
	return edid(pds, table == nullptr ? TID(TID_NULL) : table->tableId());
}

ts::EDID ts::Descriptor::edid(PDS pds, TID tid) const
{
	if (!isValid())
	{
		return EDID();  // invalid value.
	}
	const DID did = tag();
	if (tid != TID_NULL && names::HasTableSpecificName(did, tid))
	{
		// Table-specific descriptor.
		return EDID::TableSpecific(did, tid);
	}
	else if (did >= 0x80)
	{
		// Private descriptor.
		return EDID::Private(did, pds);
	}
	else if (did == DID_DVB_EXTENSION && payloadSize() > 0)
	{
		// DVB extension descriptor.
		return EDID::ExtensionDVB(payload()[0]);
	}
	else if (did == DID_MPEG_EXTENSION && payloadSize() > 0)
	{
		// MPEG extension descriptor.
		return EDID::ExtensionMPEG(payload()[0]);
	}
	else
	{
		// Standard descriptor.
		return EDID::Standard(did);
	}
}


//----------------------------------------------------------------------------
// Replace the payload of the descriptor. The tag is unchanged,
// the size is adjusted.
//----------------------------------------------------------------------------

void ts::Descriptor::replacePayload(const void *addr, size_t size)
{
	if (size > 255)
	{
		// Payload size too long, invalidate descriptor
		_data.clear();
	}
	else if (!_data.isNull())
	{
		assert(_data->size() >= 2);
		// Erase previous payload
		_data->erase(2, _data->size() - 2);
		// Add new payload
		_data->append(addr, size);
		// Adjust descriptor size
		(*_data)[1] = uint8_t(_data->size() - 2);
	}
}


//----------------------------------------------------------------------------
// Resize (truncate or extend) the payload of the descriptor.
// The tag is unchanged, the size is adjusted.
// If the payload is extended, new bytes are zeroes.
//----------------------------------------------------------------------------

void ts::Descriptor::resizePayload(size_t new_size)
{
	if (new_size > 255)
	{
		// Payload size too long, invalidate descriptor
		_data.clear();
	}
	else if (!_data.isNull())
	{
		assert(_data->size() >= 2);
		size_t old_size = _data->size() - 2;
		_data->resize(new_size + 2);
		// If payload extended, zero additional bytes
		if (new_size > old_size)
		{
			Zero(_data->data() + 2 + old_size, new_size - old_size);
		}
		// Adjust descriptor size
		(*_data)[1] = uint8_t(_data->size() - 2);
	}
}


//----------------------------------------------------------------------------
// Comparison
//----------------------------------------------------------------------------

bool ts::Descriptor::operator== (const Descriptor &desc) const
{
	return _data == desc._data ||
		(_data.isNull() && desc._data.isNull()) ||
		(!_data.isNull() && !desc._data.isNull() && *_data == *desc._data);
}


//----------------------------------------------------------------------------
// Deserialize the descriptor.
//----------------------------------------------------------------------------

ts::AbstractDescriptorPtr ts::Descriptor::deserialize(DuckContext &duck, PDS pds, const AbstractTable *table) const
{
	return deserialize(duck, pds, table == nullptr ? TID(TID_NULL) : table->tableId());
}

ts::AbstractDescriptorPtr ts::Descriptor::deserialize(DuckContext &duck, PDS pds, TID tid) const
{
	// Do we know how to deserialize this descriptor?
	PSIRepository::DescriptorFactory fac = PSIRepository::Instance().getDescriptorFactory(edid(pds), tid);
	if (fac != nullptr)
	{
		// We know how to deserialize it.
		AbstractDescriptorPtr dp(fac());
		if (!dp.isNull())
		{
			// Deserialize from binary to object.
			dp->deserialize(duck, *this);
			if (dp->isValid())
			{
				// Successfully deserialized.
				return dp;
			}
		}
	}
	return AbstractDescriptorPtr(); // null pointer
}

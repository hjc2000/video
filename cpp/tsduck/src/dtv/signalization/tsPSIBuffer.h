//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  A specialized subclass of ts::Buffer for PSI serialization.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsBuffer.h"
#include "tsCharset.h"
#include "tsDescriptorList.h"
#include "tsDuckContext.h"
#include "tsSection.h"
#include "tsTS.h"

namespace ts
{
	//!
	//! A specialized subclass of ts::Buffer for PSI serialization.
	//! @ingroup mpeg
	//!
	class TSDUCKDLL PSIBuffer : public Buffer
	{
		TS_NOBUILD_NOCOPY(PSIBuffer);
	public:
		//!
		//! Default constructor.
		//!
		//! The read and write index are at the beginning of the buffer.
		//! So, initially, there is nothing to read and the entire buffer to write.
		//!
		//! @param [in,out] duck Reference to TSDuck execution context.
		//! @param [in] size Initial internal size in bytes of the buffer.
		//!
		explicit PSIBuffer(DuckContext &duck, size_t size = DEFAULT_SIZE);

		//!
		//! Constructor using an external memory area which must remain valid as long as the PSIBuffer object is used and not reset.
		//!
		//! When @a read_only is true, the read index is at the beginning of the buffer and
		//! the write index is at the end of the buffer. When @a read_only is false,
		//! the read and write index are both at the beginning of the buffer.
		//!
		//! @param [in,out] duck Reference to TSDuck execution context.
		//! @param [in] data Address of data area to use as memory buffer.
		//! @param [in] size Size in bytes of the data area.
		//! @param [in] read_only The buffer is read-only.
		//!
		PSIBuffer(DuckContext &duck, void *data, size_t size, bool read_only = false);

		//!
		//! Constructor using a read-only external memory area which must remain valid as long as the PSIBuffer object is used and not reset.
		//!
		//! The read index is at the beginning of the buffer and the write index is at the end of the buffer.
		//!
		//! @param [in,out] duck Reference to TSDuck execution context.
		//! @param [in] data Address of data area to use as memory buffer.
		//! @param [in] size Size in bytes of the data area.
		//!
		PSIBuffer(DuckContext &duck, const void *data, size_t size);

		//!
		//! Constructor over the payload of a read-only section which must remain unmodified as long as the PSIBuffer object is used and not reset.
		//!
		//! The read index is at the beginning of the section payload and the write index is at the end of the section payload.
		//!
		//! @param [in,out] duck Reference to TSDuck execution context.
		//! @param [in] section Section the payload of which is analysed by this PSIBuffer.
		//!
		PSIBuffer(DuckContext &duck, const Section &section);

		//!
		//! Get a reference to the associated TSDuck execution context.
		//! @return A reference to the associated TSDuck execution context.
		//!
		DuckContext &duck() const { return _duck; }

		//!
		//! Deserialize a 13-bit PID value.
		//! If the current read pointer is byte-aligned, 3 bits are skipped first.
		//! If the current read bit pointer is 3, the PID value is directly read.
		//! For all other read pointers, a read error is generated.
		//! @return The decoded 13-bit PID value.
		//!
		PID getPID();

		//!
		//! Serialize a 13-bit PID value.
		//! If the current write pointer is byte-aligned, 3 '1' bits are written first.
		//! If the current write bit pointer is 3, the PID value is directly written.
		//! For all other write pointers, a write error is generated.
		//! @param [in] pid 13-bit PID value to write.
		//! @return True on success, false if there is not enough space to write (and set write error flag).
		//!
		bool putPID(PID pid);

		//!
		//! Serialize a 3-byte language or country code and advance the write pointer.
		//! @param [in] str String to serialize. Generate a buffer error if not 3 characters long.
		//! @param [in] allow_empty If true, an empty string is allowed and serialized as zeroes.
		//! @return True on success, false if there is not enough space to write (and set write error flag).
		//!
		bool putLanguageCode(const UString &str, bool allow_empty = false);

		//!
		//! Read the next 24 bits as a 3-character language or country code and advance the read pointer.
		//! Set the read error flag if there are not enough bits to read or if the current read pointer
		//! is not at a byte boundary. Non-ASCII characters are ignored.
		//! @param [out] str Returned decoded string.
		//! @return True on success, false on error (truncated, unsupported format, etc.)
		//!
		bool getLanguageCode(UString &str);

		//!
		//! Read the next 24 bits as a 3-character language or country code and advance the read pointer.
		//! Set the read error flag if there are not enough bits to read or if the current read pointer
		//! is not at a byte boundary. Non-ASCII characters are ignored.
		//! @return The language or country code string.
		//!
		UString getLanguageCode();

		//!
		//! Put a string using the preferred output character set.
		//! Generate a write error when the buffer is full before writing the complete string.
		//! @param [in] str The UTF-16 string to encode.
		//! @param [in] start Starting offset to convert in this UTF-16 string.
		//! @param [in] count Maximum number of characters to convert.
		//! @param [in] charset An optional specific character set to use instead of the default one.
		//! @return True on success, false if there is not enough space to write (and set write error flag).
		//!
		bool putString(const UString &str, size_t start = 0, size_t count = NPOS, const Charset *charset = nullptr)
		{
			return putStringCommon(str, start, count, &Charset::encode, false, 0, charset) != 0;
		}

		//!
		//! Put a partial string using the preferred output character set.
		//! Stop either when this string is serialized or when the buffer is full, whichever comes first.
		//! Do not generate a write error when the buffer is full.
		//! @param [in] str The UTF-16 string to encode.
		//! @param [in] start Starting offset to convert in this UTF-16 string.
		//! @param [in] count Maximum number of characters to convert.
		//! @param [in] charset An optional specific character set to use instead of the default one.
		//! @return The number of serialized characters (which is usually not the same as the number of written bytes).
		//!
		size_t putPartialString(const UString &str, size_t start = 0, size_t count = NPOS, const Charset *charset = nullptr)
		{
			return putStringCommon(str, start, count, &Charset::encode, true, 0, charset);
		}

		//!
		//! Put a string (preceded by its one-byte length) using the preferred output character set.
		//! Generate a write error when the buffer is full before writing the complete string.
		//! @param [in] str The UTF-16 string to encode.
		//! @param [in] start Starting offset to convert in this UTF-16 string.
		//! @param [in] count Maximum number of characters to convert.
		//! @param [in] charset An optional specific character set to use instead of the default one.
		//! @return True on success, false if there is not enough space to write (and set write error flag).
		//!
		bool putStringWithByteLength(const UString &str, size_t start = 0, size_t count = NPOS, const Charset *charset = nullptr)
		{
			return putStringCommon(str, start, count, &Charset::encodeWithByteLength, false, 1, charset) != 0;
		}

		//!
		//! Put a partial string (preceded by its one-byte length) using the preferred output character set.
		//! Stop either when this string is serialized or when the buffer is full, whichever comes first.
		//! Do not generate a write error when the buffer is full.
		//! @param [in] str The UTF-16 string to encode.
		//! @param [in] start Starting offset to convert in this UTF-16 string.
		//! @param [in] count Maximum number of characters to convert.
		//! @param [in] charset An optional specific character set to use instead of the default one.
		//! @return The number of serialized characters (which is usually not the same as the number of written bytes).
		//!
		size_t putPartialStringWithByteLength(const UString &str, size_t start = 0, size_t count = NPOS, const Charset *charset = nullptr)
		{
			return putStringCommon(str, start, count, &Charset::encodeWithByteLength, true, 1, charset);
		}

		//!
		//! Get a string using the default input character set.
		//! @param [out] str Returned decoded string.
		//! @param [in] size Size in bytes of the encoded string. If specified as @a NPOS (the default), read up to
		//! the end of the buffer. If different from @a NPOS, the exact number of bytes must be available or a read
		//! error is generated.
		//! @param [in] charset An optional specific character set to use instead of the default one.
		//! @return True on success, false on error (truncated, unsupported format, etc.)
		//!
		bool getString(UString &str, size_t size = NPOS, const Charset *charset = nullptr);

		//!
		//! Get a string using the default input character set.
		//! @param [in] size Size in bytes of the encoded string. If specified as @a NPOS (the default), read up to
		//! the end of the buffer. If different from @a NPOS, the exact number of bytes must be available or a read
		//! error is generated.
		//! @param [in] charset An optional specific character set to use instead of the default one.
		//! @return The decoded string.
		//!
		UString getString(size_t size = NPOS, const Charset *charset = nullptr);

		//!
		//! Get a string (preceded by its one-byte length) using the default input character set.
		//! The specified number of bytes must be available or a read error is generated.
		//! @param [out] str Returned decoded string.
		//! @param [in] charset An optional specific character set to use instead of the default one.
		//! @return True on success, false on error (truncated, unsupported format, etc.)
		//!
		bool getStringWithByteLength(UString &str, const Charset *charset = nullptr);

		//!
		//! Get a string (preceded by its one-byte length) using the default input character set.
		//! The specified number of bytes must be available or a read error is generated.
		//! @param [in] charset An optional specific character set to use instead of the default one.
		//! @return The decoded string.
		//!
		UString getStringWithByteLength(const Charset *charset = nullptr);

		//!
		//! Put (serialize) a duration in minutes as 4 BCD digits (HHMM), 2 bytes.
		//! Generate a write error when the buffer is not large enough.
		//! @param [in] duration A number of minutes.
		//! @return True on success, false if there is not enough space to write (and set write error flag).
		//!
		bool putMinutesBCD(SubSecond duration);

		//!
		//! Put (serialize) a duration in seconds as 6 BCD digits (HHMMSS), 3 bytes.
		//! Generate a write error when the buffer is not large enough.
		//! @param [in] duration A number of seconds.
		//! @return True on success, false if there is not enough space to write (and set write error flag).
		//!
		bool putSecondsBCD(Second duration);

		//!
		//! Get (deserialize) a duration in minutes as 4 BCD digits (HHMM), 2 bytes.
		//! Generate a read error when there is not enough bytes.
		//! @return The duration in minutes.
		//!
		SubSecond getMinutesBCD();

		//!
		//! Get (deserialize) a duration in seconds as 6 BCD digits (HHMMSS), 3 bytes.
		//! Generate a read error when there is not enough bytes.
		//! @return The duration in minutes.
		//!
		Second getSecondsBCD();

		//!
		//! Get (deserialize) an integer value in "vluimsbf5" format.
		//!
		//! This weird format is defined in ISO/IEC 23005-6 as follow:
		//!
		//! "vluimsbf5: Variable length unsigned integer most significant bit first representation consisting of two parts.
		//! The first part defines the number n of 4-bit fields used for the value representation, encoded by a sequence of
		//! n-1 "1" bits, followed by a "0" bit signaling its end. The second part contains the value of the integer encoded
		//! using the number of bit fields specified in the first part."
		//!
		//! Very similar to the following (for connoisseurs only):
		//!
		//! "And Saint Attila raised the hand grenade up on high, saying, 'O Lord, bless this thy hand grenade, that with it
		//! thou mayst blow thine enemies to tiny bits, in thy mercy.' And the Lord did grin. And the people did feast upon
		//! the lambs, and sloths, and carp, and anchovies, and orangutans, and breakfast cereals, and fruit bats, and large
		//! chulapas. And the Lord spake, saying, 'First shalt thou take out the Holy Pin. Then shalt thou count to three,
		//! no more, no less. Three shall be the number thou shalt count, and the number of the counting shall be three.
		//! Four shalt thou not count, neither count thou two, excepting that thou then proceed to three. Five is right out.
		//! Once the number three, being the third number, be reached, then lobbest thou thy Holy Hand Grenade of Antioch
		//! towards thy foe, who, being naughty in My sight, shall snuff it.'"
		//!
		//! Generate a read error when there is not enough bytes.
		//! @return The decoded integer value as a 64-bit integer.
		//!
		uint64_t getVluimsbf5();

		//!
		//! Put (serialize) an integer value in "vluimsbf5" format.
		//! Generate a write error when the buffer is not large enough.
		//!
		//! @param [in] value An integer to serialize.
		//! @return True on success, false if there is not enough space to write (and set write error flag).
		//! @see getVluimsbf5()
		//!
		bool putVluimsbf5(uint64_t value);

		//!
		//! Put (serialize) a complete descriptor list.
		//!
		//! Generate a write error when the buffer is not large enough to write all descriptors or when the
		//! write pointer is not byte-aligned.
		//!
		//! @param [in] descs The descriptor list to serialize into the buffer.
		//! @param [in] start Starting index in the descriptor list.
		//! @param [in] count Maximum number of descriptors to serialize.
		//! @return True on success, false if there is not enough space to write (and set write error flag).
		//!
		bool putDescriptorList(const DescriptorList &descs, size_t start = 0, size_t count = NPOS);

		//!
		//! Put (serialize) as many descriptors as possible from a descriptor list.
		//!
		//! Generate a write error when the write pointer is not byte-aligned.
		//!
		//! @param [in] descs The descriptor list to serialize into the buffer.
		//! @param [in] start Starting index in the descriptor list.
		//! @param [in] count Maximum number of descriptors to serialize.
		//! @return The index of the first descriptor that could not be serialized (or descs.size() if all
		//! descriptors were serialized). In the first case, the returned index can be used as @a start
		//! parameter to serialized the rest of the list (in another section for instance).
		//!
		size_t putPartialDescriptorList(const DescriptorList &descs, size_t start = 0, size_t count = NPOS);

		//!
		//! Put (serialize) a complete descriptor list with a 2-byte length field before the descriptor list.
		//!
		//! Generate a write error when the buffer is not large enough to write all descriptors or when the
		//! write pointer is not byte-aligned.
		//!
		//! The 2-byte length field uses only its N least significant bits (@a length_bits) for the length
		//! of the descriptor list. If the current write pointer is byte-aligned, 16-N '1' bits are written
		//! first. If the current write bit pointer is 16-N, the length is directly written after that bit.
		//! For all other write pointers, a write error is generated.
		//!
		//! @param [in] descs The descriptor list to serialize into the buffer.
		//! @param [in] start Starting index in the descriptor list.
		//! @param [in] count Maximum number of descriptors to serialize.
		//! @param [in] length_bits Number of meaningful bits in the length field.
		//! @return True on success, false if there is not enough space to write (and set write error flag).
		//!
		bool putDescriptorListWithLength(const DescriptorList &descs, size_t start = 0, size_t count = NPOS, size_t length_bits = 12);

		//!
		//! Put (serialize) as many descriptors as possible from a descriptor list with a 2-byte length field before the descriptor list.
		//!
		//! The 2-byte length field uses only its N least significant bits (@a length_bits) for the length
		//! of the descriptor list. If the current write pointer is byte-aligned, 16-N '1' bits are written
		//! first. If the current write bit pointer is 16-N, the length is directly written after that bit.
		//! For all other write pointers, a write error is generated.
		//!
		//! @param [in] descs The descriptor list to serialize into the buffer.
		//! @param [in] start Starting index in the descriptor list.
		//! @param [in] count Maximum number of descriptors to serialize.
		//! @param [in] length_bits Number of meaningful bits in the length field.
		//! @return The index of the first descriptor that could not be serialized (or descs.size() if all
		//! descriptors were serialized). In the first case, the returned index can be used as @a start
		//! parameter to serialized the rest of the list (in another section for instance).
		//!
		size_t putPartialDescriptorListWithLength(const DescriptorList &descs, size_t start = 0, size_t count = NPOS, size_t length_bits = 12);

		//!
		//! Get (deserialize) a descriptor list.
		//! @param [in,out] descs The descriptor list into which the deserialized descriptors are appended.
		//! @param [in] length Number of bytes to read. If NPOS is specified (the default), read the rest of the buffer.
		//! @return True on success, false on error (truncated, misaligned, etc.)
		//!
		bool getDescriptorList(DescriptorList &descs, size_t length = NPOS);

		//!
		//! Get (deserialize) a descriptor list with a 2-byte length field before the descriptor list.
		//!
		//! The 2-byte length field uses only its N least significant bits (@a length_bits) for the length
		//! of the descriptor list. If the current read pointer is byte-aligned, 16-N bits are skipped first.
		//! If the current read bit pointer is 16-N, the length is directly read after that bit.
		//! For all other read pointers, a read error is generated.
		//!
		//! @param [in,out] descs The descriptor list into which the deserialized descriptors are appended.
		//! @param [in] length_bits Number of meaningful bits in the length field.
		//! @return True on success, false on error (truncated, misaligned, etc.)
		//!
		bool getDescriptorListWithLength(DescriptorList &descs, size_t length_bits = 12);

		//!
		//! Get a 2-byte integer length field, typically a length before a descriptor list.
		//!
		//! The 2-byte integer field uses only its N least significant bits (@a length_bits). If the current
		//! read pointer is byte-aligned, 16-N bits are skipped first. If the current read bit pointer is 16-N,
		//! the length is directly read after that bit. For all other read pointers, a read error is generated.
		//!
		//! @param [in] length_bits Number of meaningful bits in the integer field.
		//! @return The integer value. Return zero on error. The returned value cannot be larger than the remaining
		//! number of bytes to read.
		//!
		size_t getUnalignedLength(size_t length_bits);

	private:
		DuckContext &_duck;

		// Common profile of Charset encoding methods.
		typedef size_t(Charset:: *EncodeMethod)(uint8_t *&, size_t &, const UString &, size_t, size_t) const;

		// Common code the various putString functions.
		// When partial == true, return the number of encoded characters.
		// When partial == false, return 1 on success, 0 on error.
		size_t putStringCommon(const UString &str, size_t start, size_t count, EncodeMethod em, bool partial, size_t min_req_size, const Charset *);
	};
}

//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Abstract base class for MPEG PSI/SI tables and descriptors.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsAbstractDefinedByStandards.h"
#include "tsNamesFile.h"

namespace ts
{

	class DuckContext;
	class ByteBlock;
	class TablesDisplay;

	//!
	//! Abstract base class for MPEG PSI/SI tables and descriptors.
	//! @ingroup mpeg
	//!
	//! Some methods are declared as "virtual final". Since these methods are not
	//! inherited, this seems useless. This is in fact a compilation check. These
	//! methods were formerly designed to be overridden by subclasses but the
	//! implementation has changed. They are now defined in this class only and
	//! call a new pure virtual method. The "final" attribute is here to detect
	//! old subclasses which do not yet use the new scheme.
	//!
	class TSDUCKDLL AbstractSignalization : public AbstractDefinedByStandards
	{
	public:
		//!
		//! Check if this object is valid.
		//! @return True if this object is valid.
		//!
		bool isValid() const { return _is_valid; }

		//!
		//! Invalidate this object.
		//! This object must be rebuilt.
		//!
		void invalidate() { _is_valid = false; }

		//!
		//! This method clears the content of the table or descriptor.
		//! Upon return, the object is valid and in the same empty state as after a default constructor.
		//!
		virtual void clear();

		//!
		//! Get the XMl node name representing this table or descriptor.
		//! @return The XML node name.
		//!
		UString xmlName() const;

		// Implementation of AbstractDefinedByStandards
		virtual Standards definingStandards() const override;

		//!
		//! Virtual destructor
		//!
		virtual ~AbstractSignalization() override;

		//!
		//! XML tag name for generic descriptors.
		//!
		static const UChar *const XML_GENERIC_DESCRIPTOR;
		//!
		//! XML tag name for generic short sections.
		//!
		static const UChar *const XML_GENERIC_SHORT_TABLE;
		//!
		//! XML tag name for generic tables with long sections.
		//!
		static const UChar *const XML_GENERIC_LONG_TABLE;

		//!
		//! Get a name from a specified section in the DVB names file.
		//! @tparam INT An integer type.
		//! @param [in] xml_name Table or descriptor name, as used in XML structures.
		//! @param [in] section Name of section to search. Not case-sensitive. The actual section in
		//! the names file is prefixed by the XML name, followed by a dot.
		//! @param [in] value Value to get the name for.
		//! @param [in] flags Presentation flags.
		//! @param [in] bits Nominal size in bits of the data, optional.
		//! @param [in] alternate Display this integer value if flags ALTERNATE is set.
		//! @return The corresponding name.
		//!
		template <typename INT, typename std::enable_if<std::is_integral<INT>::value, int>::type = 0>
		static UString DataName(const UChar *xml_name, const UChar *section, INT value, NamesFlags flags = NamesFlags::NAME, size_t bits = 0, INT alternate = 0)
		{
			return NamesFile::Instance(NamesFile::Predefined::DTV)->
				nameFromSection(UString::Format(u"%s.%s", { xml_name, section }), NamesFile::Value(value), flags, bits, NamesFile::Value(alternate));
		}

		//!
		//! Get a name from a specified section in the DVB names file for that signalization structure.
		//! @tparam INT An integer type.
		//! @param [in] section Name of section to search. Not case-sensitive. The actual section in
		//! the names file is prefixed by the XML name of the structure, followed by a dot.
		//! @param [in] value Value to get the name for.
		//! @param [in] flags Presentation flags.
		//! @param [in] bits Nominal size in bits of the data, optional.
		//! @param [in] alternate Display this integer value if flags ALTERNATE is set.
		//! @return The corresponding name.
		//!
		template <typename INT, typename std::enable_if<std::is_integral<INT>::value, int>::type = 0>
		UString dataName(const UChar *section, INT value, NamesFlags flags = NamesFlags::NAME, size_t bits = 0, INT alternate = 0)
		{
			return DataName<INT>(_xml_name, section, value, flags, bits, alternate);
		}

	protected:
		//!
		//! Protected constructor for subclasses.
		//! @param [in] xml_name Table or descriptor name, as used in XML structures.
		//! @param [in] standards A bit mask of standards which define this structure.
		//! @param [in] xml_legacy_name Table or descriptor legacy XML name. Ignored if null pointer.
		//!
		AbstractSignalization(const UChar *xml_name, Standards standards, const UChar *xml_legacy_name = nullptr);

		//!
		//! Copy constructor.
		//! Use default implementation, just tell the compiler we understand
		//! the consequences of copying a pointer member.
		//! @param [in] other The other instance to copy.
		//!
		AbstractSignalization(const AbstractSignalization &other) = default;

		//!
		//! Move constructor.
		//! Use default implementation, just tell the compiler we understand
		//! the consequences of copying a pointer member.
		//! @param [in] other The other instance to copy.
		//!
		AbstractSignalization(AbstractSignalization &&other) = default;

		//!
		//! Assignment operator.
		//! @param [in] other The other instance to copy.
		//! @return A reference to this object.
		//!
		AbstractSignalization &operator=(const AbstractSignalization &other);

		//!
		//! Assignment move operator.
		//! @param [in] other The other instance to copy.
		//! @return A reference to this object.
		//!
		AbstractSignalization &operator=(AbstractSignalization &&other);

		//!
		//! Helper method to clear the content of the table or descriptor.
		//!
		//! It is called by clear(). In clearContent(), the subclass shall simply
		//! revert the value of all fields to their original values in the default
		//! constructor.
		//!
		virtual void clearContent() = 0;

	private:
		bool               _is_valid = true;  // This object is valid.
		const UChar *const _xml_name;         // XML table or descriptor name.
		const UChar *const _xml_legacy_name;  // Optional XML table or descriptor legacy name. Ignored if null pointer.
		const Standards    _standards;        // Defining standards (usually only one).

		// Unreachable constructors and operators.
		AbstractSignalization() = delete;
	};
}

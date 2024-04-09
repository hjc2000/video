//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/#license
//
//----------------------------------------------------------------------------

#include "tsNames.h"
#include "tsTSScrambling.h"

//----------------------------------------------------------------------------
// Constructors.
//----------------------------------------------------------------------------

ts::TSScrambling::TSScrambling(Report &report, uint8_t scrambling) :
	_report(report),
	_scrambling_type(scrambling),
	_next_cw(_cw_list.end()),
	_dvbcsa(),    // required on old gcc 10 and below (gcc bug)
	_dvbcissa(),  // required on old gcc 10 and below (gcc bug)
	_idsa(),      // required on old gcc 10 and below (gcc bug)
	_aescbc(),    // required on old gcc 10 and below (gcc bug)
	_aesctr()     // required on old gcc 10 and below (gcc bug)

{
	setScramblingType(scrambling);
}

ts::TSScrambling::TSScrambling(const TSScrambling &other) :
	BlockCipherAlertInterface(other),  // required on old gcc 8.5 and below (gcc bug)
	_report(other._report),
	_scrambling_type(other._scrambling_type),
	_explicit_type(other._explicit_type),
	_cw_list(other._cw_list),
	_next_cw(_cw_list.end()),
	_dvbcsa(),    // required on old gcc 10 and below (gcc bug)
	_dvbcissa(),  // required on old gcc 10 and below (gcc bug)
	_idsa(),      // required on old gcc 10 and below (gcc bug)
	_aescbc(),    // required on old gcc 10 and below (gcc bug)
	_aesctr()     // required on old gcc 10 and below (gcc bug)
{
	setScramblingType(_scrambling_type);
	_dvbcsa[0].setEntropyMode(other._dvbcsa[0].entropyMode());
	_dvbcsa[1].setEntropyMode(other._dvbcsa[1].entropyMode());
}

ts::TSScrambling::TSScrambling(TSScrambling &&other) :
	_report(other._report),
	_scrambling_type(other._scrambling_type),
	_explicit_type(other._explicit_type),
	_cw_list(other._cw_list),
	_next_cw(_cw_list.end()),
	_dvbcsa(),    // required on old gcc 10 and below (gcc bug)
	_dvbcissa(),  // required on old gcc 10 and below (gcc bug)
	_idsa(),      // required on old gcc 10 and below (gcc bug)
	_aescbc(),    // required on old gcc 10 and below (gcc bug)
	_aesctr()     // required on old gcc 10 and below (gcc bug)
{
	setScramblingType(_scrambling_type);
	_dvbcsa[0].setEntropyMode(other._dvbcsa[0].entropyMode());
	_dvbcsa[1].setEntropyMode(other._dvbcsa[1].entropyMode());
}


//----------------------------------------------------------------------------
// Force the usage of a specific algorithm.
//----------------------------------------------------------------------------

bool ts::TSScrambling::setScramblingType(uint8_t scrambling, bool overrideExplicit)
{
	if (overrideExplicit || !_explicit_type)
	{

		// Select the right pair of scramblers.
		switch (scrambling)
		{
		case SCRAMBLING_DVB_CSA1:
		case SCRAMBLING_DVB_CSA2:
			_scrambler[0] = &_dvbcsa[0];
			_scrambler[1] = &_dvbcsa[1];
			break;
		case SCRAMBLING_DVB_CISSA1:
			_scrambler[0] = &_dvbcissa[0];
			_scrambler[1] = &_dvbcissa[1];
			break;
		case SCRAMBLING_ATIS_IIF_IDSA:
			_scrambler[0] = &_idsa[0];
			_scrambler[1] = &_idsa[1];
			break;
		case SCRAMBLING_DUCK_AES_CBC:
			_scrambler[0] = &_aescbc[0];
			_scrambler[1] = &_aescbc[1];
			break;
		case SCRAMBLING_DUCK_AES_CTR:
			_scrambler[0] = &_aesctr[0];
			_scrambler[1] = &_aesctr[1];
			break;
		default:
			// Fallback to DVB-CSA2 if no scrambler was previously defined.
			if (_scrambler[0] == nullptr || _scrambler[1] == nullptr)
			{
				_scrambling_type = SCRAMBLING_DVB_CSA2;
				_scrambler[0] = &_dvbcsa[0];
				_scrambler[1] = &_dvbcsa[1];
			}
			return false;
		}

		// Set scrambling type.
		if (_scrambling_type != scrambling)
		{
			_report.debug(u"switching scrambling type from %s to %s", { NameFromDTV(u"ScramblingMode", _scrambling_type), NameFromDTV(u"ScramblingMode", scrambling) });
			_scrambling_type = scrambling;
		}
	}

	// Make sure the current scramblers notify alerts to this object.
	_scrambler[0]->setAlertHandler(this);
	_scrambler[1]->setAlertHandler(this);
	_scrambler[0]->setCipherId(0);
	_scrambler[1]->setCipherId(1);
	return true;
}

void ts::TSScrambling::setEntropyMode(DVBCSA2::EntropyMode mode)
{
	_dvbcsa[0].setEntropyMode(mode);
	_dvbcsa[1].setEntropyMode(mode);
}

ts::DVBCSA2::EntropyMode ts::TSScrambling::entropyMode() const
{
	return (_scrambling_type == SCRAMBLING_DVB_CSA1 || _scrambling_type == SCRAMBLING_DVB_CSA2) ?
		_dvbcsa[0].entropyMode() : DVBCSA2::FULL_CW;
}

//----------------------------------------------------------------------------
// Start the scrambling session.
//----------------------------------------------------------------------------

bool ts::TSScrambling::start()
{
	bool success = true;

	// Point next CW to end of list. Will loop to first one.
	_next_cw = _cw_list.end();

	// Create the output file for control words.
	if (!_out_cw_name.empty())
	{
		_out_cw_file.open(_out_cw_name.toUTF8().c_str(), std::ios::out);
		success = !_out_cw_file.fail();
		if (!success)
		{
			_report.error(u"error creating %s", { _out_cw_name });
		}
	}

	return success;
}


//----------------------------------------------------------------------------
// Stop the scrambling session.
//----------------------------------------------------------------------------

bool ts::TSScrambling::stop()
{
	// Close the output file for control words, if one was created.
	if (_out_cw_file.is_open())
	{
		_out_cw_file.close();
	}
	return true;
}


//----------------------------------------------------------------------------
// Implementation of BlockCipherAlertInterface.
//----------------------------------------------------------------------------

bool ts::TSScrambling::handleBlockCipherAlert(BlockCipher &cipher, AlertReason reason)
{
	switch (reason)
	{
	case FIRST_ENCRYPTION:
	case FIRST_DECRYPTION:
		{
			// First usage of a new CW. Report it on debug and add it in --output-cw-file when necessary.
			ByteBlock key;
			cipher.getKey(key);
			if (!key.empty())
			{
				const UString key_string(UString::Dump(key, UString::SINGLE_LINE));
				_report.debug(u"starting using CW %s (%s)", { key_string, cipher.cipherId() == 0 ? u"even" : u"odd" });
				if (_out_cw_file.is_open())
				{
					_out_cw_file << key_string << std::endl;
				}
			}
			return true;
		}
	case ENCRYPTION_EXCEEDED:
	case DECRYPTION_EXCEEDED:
	default:
		{
			// Not interested in other alerts.
			return true;
		}
	}
}


//----------------------------------------------------------------------------
// Rewind the list of fixed control words. Will use first CW.
//----------------------------------------------------------------------------

void ts::TSScrambling::rewindFixedCW()
{
	_next_cw = _cw_list.end();
	_encrypt_scv = SC_CLEAR;
	_decrypt_scv = SC_CLEAR;
}


//----------------------------------------------------------------------------
// Set the next fixed control word as scrambling key.
//----------------------------------------------------------------------------

bool ts::TSScrambling::setNextFixedCW(int parity)
{
	// Error if no fixed control word were provided on the command line.
	if (_cw_list.empty())
	{
		_report.error(u"no fixed CW from command line");
		return false;
	}

	// Point to next CW.
	if (_next_cw != _cw_list.end())
	{
		++_next_cw;
	}
	if (_next_cw == _cw_list.end())
	{
		_next_cw = _cw_list.begin();
	}
	assert(_next_cw != _cw_list.end());

	// Set the key in the descrambler.
	return setCW(*_next_cw, parity);
}


//----------------------------------------------------------------------------
// Set the control word for encrypt and decrypt.
//----------------------------------------------------------------------------

bool ts::TSScrambling::setCW(const ByteBlock &cw, int parity)
{
	CipherChaining *algo = _scrambler[parity & 1];
	assert(algo != nullptr);

	if (algo->setKey(cw.data(), cw.size()))
	{
		_report.debug(u"using scrambling key: " + UString::Dump(cw, UString::SINGLE_LINE));
		return true;
	}
	else
	{
		_report.error(u"error setting %d-byte key to %s", { cw.size(), algo->name() });
		return false;
	}
}


//----------------------------------------------------------------------------
// Set the parity of all subsequent encryptions.
//----------------------------------------------------------------------------

bool ts::TSScrambling::setEncryptParity(int parity)
{
	// Remember parity.
	const uint8_t previous_scv = _encrypt_scv;
	_encrypt_scv = SC_EVEN_KEY | (parity & 1);

	// In case of fixed control words, use next key when the parity changes.
	return !hasFixedCW() || _encrypt_scv == previous_scv || setNextFixedCW(_encrypt_scv);
}


//----------------------------------------------------------------------------
// Encrypt a TS packet with the current parity and corresponding CW.
//----------------------------------------------------------------------------

bool ts::TSScrambling::encrypt(TSPacket &pkt)
{
	// Filter out encrypted packets.
	if (pkt.isScrambled())
	{
		_report.error(u"try to scramble an already scrambled packet");
		return false;
	}

	// Silently pass packets without payload.
	if (!pkt.hasPayload())
	{
		return true;
	}

	// If no current parity is set, start with even by default.
	if (_encrypt_scv == SC_CLEAR && !setEncryptParity(SC_EVEN_KEY))
	{
		return false;
	}

	// Select scrambling algo.
	assert(_encrypt_scv == SC_EVEN_KEY || _encrypt_scv == SC_ODD_KEY);
	CipherChaining *algo = _scrambler[_encrypt_scv & 1];
	assert(algo != nullptr);

	// Check if the residue shall be included in the scrambling.
	size_t psize = pkt.getPayloadSize();
	if (!algo->residueAllowed())
	{
		// Remove the residue from the payload.
		assert(algo->blockSize() != 0);
		psize -= psize % algo->blockSize();
	}

	// Encrypt the packet.
	const bool ok = psize == 0 || algo->encryptInPlace(pkt.getPayload(), psize);
	if (ok)
	{
		pkt.setScrambling(_encrypt_scv);
	}
	else
	{
		_report.error(u"packet encryption error using %s", { algo->name() });
	}
	return ok;
}


//----------------------------------------------------------------------------
// Decrypt a TS packet with the CW corresponding to the parity in the packet.
//----------------------------------------------------------------------------

bool ts::TSScrambling::decrypt(TSPacket &pkt)
{
	// Clear or invalid packets are silently accepted.
	const uint8_t scv = pkt.getScrambling();
	if (scv != SC_EVEN_KEY && scv != SC_ODD_KEY)
	{
		return true;
	}

	// Update current parity.
	const uint8_t previous_scv = _decrypt_scv;
	_decrypt_scv = scv;

	// In case of fixed control word, use next key when the scrambling control changes.
	if (hasFixedCW() && previous_scv != _decrypt_scv && !setNextFixedCW(_decrypt_scv))
	{
		return false;
	}

	// Select descrambling algo.
	CipherChaining *algo = _scrambler[_decrypt_scv & 1];
	assert(algo != nullptr);

	// Check if the residue shall be included in the scrambling.
	size_t psize = pkt.getPayloadSize();
	if (!algo->residueAllowed())
	{
		// Remove the residue from the payload.
		assert(algo->blockSize() != 0);
		psize -= psize % algo->blockSize();
	}

	// Decrypt the packet.
	const bool ok = psize == 0 || algo->decryptInPlace(pkt.getPayload(), psize);
	if (ok)
	{
		pkt.setScrambling(SC_CLEAR);
	}
	else
	{
		_report.error(u"packet decryption error using %s", { algo->name() });
	}
	return ok;
}

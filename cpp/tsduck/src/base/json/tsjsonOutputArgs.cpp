//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2023, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/#license
//
//----------------------------------------------------------------------------

#include "tsTextFormatter.h"
#include "tsjsonOutputArgs.h"
#include "tsjsonRunningDocument.h"
#include "tsjsonValue.h"


//----------------------------------------------------------------------------
// Constructors and destructors.
//----------------------------------------------------------------------------

ts::json::OutputArgs::~OutputArgs()
{
	if (_tcp_sock.isOpen())
	{
		_tcp_sock.closeWriter(NULLREP);
		_tcp_sock.disconnect(NULLREP);
		_tcp_sock.close(NULLREP);
	}
}

//----------------------------------------------------------------------------
// Open/close the UDP socket.
//----------------------------------------------------------------------------

bool ts::json::OutputArgs::udpOpen(Report &rep)
{
	return true;
}

bool ts::json::OutputArgs::udpClose(Report &rep)
{
	return true;
}


//----------------------------------------------------------------------------
// Connect/disconnect the TCP session.
//----------------------------------------------------------------------------

bool ts::json::OutputArgs::tcpConnect(Report &rep)
{
	if (_tcp_sock.isOpen())
	{
		return true;
	}
	else if (!_tcp_sock.open(rep))
	{
		return false;
	}
	else if ((_sock_buffer_size == 0 || _tcp_sock.setSendBufferSize(_sock_buffer_size, rep)) &&
		_tcp_sock.bind(IPv4SocketAddress::AnySocketAddress, rep) &&
		_tcp_sock.connect(_tcp_destination, rep))
	{
		return true;
	}
	else
	{
		_tcp_sock.close(rep);
		return false;
	}
}

bool ts::json::OutputArgs::tcpDisconnect(bool force, Report &rep)
{
	bool ok = true;
	if (_tcp_sock.isOpen() && (force || !_json_tcp_keep))
	{
		ok = _tcp_sock.closeWriter(rep) && _tcp_sock.disconnect(rep);
		ok = _tcp_sock.close(rep) && ok;
	}
	return ok;
}


//----------------------------------------------------------------------------
// Issue a JSON report, except --json file.
//----------------------------------------------------------------------------

bool ts::json::OutputArgs::reportOthers(const json::Value &root, Report &rep)
{
	bool udp_ok = true;
	bool tcp_ok = true;

	if (_json_line || _json_tcp || _json_udp)
	{

		// Generate one JSON line.
		const UString line(root.oneLiner(rep));

		// When sent over the network, use a UTF-8 string.
		std::string line8;
		if (_json_tcp || _json_udp)
		{
			line.toUTF8(line8);
		}

		// Report in logger.
		if (_json_line)
		{
			rep.info(_line_prefix + line);
		}

		// Report through TCP. Connect to TCP server the first time (--json-tcp-keep) or every time.
		if (_json_tcp)
		{
			tcp_ok = tcpConnect(rep);
			if (tcp_ok)
			{
				tcp_ok = _tcp_sock.sendLine(line8, rep);
				// In case of send error, retry opening the socket once.
				// This is useful when the session is kept open and the server disconnected since last time.
				if (!tcp_ok)
				{
					tcpDisconnect(true, rep);
					tcp_ok = tcpConnect(rep) && _tcp_sock.sendLine(line8, rep);
				}
				// Disconnect on error or when the connection shall not be kept open.
				tcpDisconnect(!tcp_ok, rep);
			}
		}
	}

	return udp_ok && tcp_ok;
}


//----------------------------------------------------------------------------
// Issue a JSON report according to options.
//----------------------------------------------------------------------------

bool ts::json::OutputArgs::report(const json::Value &root, std::ostream &stm, Report &rep)
{
	// Process file output.
	if (_json_opt)
	{
		TextFormatter text(rep);
		text.setStream(stm);
		root.print(text);
		text << ts::endl;
		text.close();
	}

	// Other output forms.
	return reportOthers(root, rep);
}

bool ts::json::OutputArgs::report(const json::Value &root, json::RunningDocument &doc, Report &rep)
{
	// Process file output.
	if (_json_opt)
	{
		doc.add(root);
	}

	// Other output forms.
	return reportOthers(root, rep);
}

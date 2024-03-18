#pragma once
#include<Json.h>
#include<string>
#include<tsPAT.h>
#include<tsPMT.h>
#include<tsSDT.h>
#include<tsTSPacket.h>

std::string ToString(ts::TSPacket const &packet);
std::string ToString(ts::PAT const &pat);
std::string ToString(ts::PMT const &pmt);
std::string ToString(ts::SDT const &sdt, ts::DuckContext &duck);
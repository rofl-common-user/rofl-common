/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmeterstatsarray.cc
 *
 *  Created on: 27.05.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofmeterstatsarray.h"

using namespace rofl::openflow;



size_t
cofmeterstatsarray::length() const
{
	size_t len = 0;
	for (std::map<unsigned int, cofmeter_stats_reply>::const_iterator
			it = array.begin(); it != array.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
cofmeterstatsarray::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<unsigned int, cofmeter_stats_reply>::iterator
				it = array.begin(); it != array.end(); ++it) {
			it->second.pack(buf, it->second.length());
			buf += it->second.length();
		}

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



void
cofmeterstatsarray::unpack(uint8_t *buf, size_t buflen)
{
	array.clear();

	if ((0 == buf) || (0 == buflen))
		return;

	unsigned int offset = 0;
	unsigned int index = 0;

	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {

		while ((buflen - offset) >= sizeof(struct rofl::openflow13::ofp_meter_stats)) {

			struct rofl::openflow13::ofp_meter_stats* mstats =
					(struct rofl::openflow13::ofp_meter_stats*)(buf + offset);

			uint16_t mstats_len = be16toh(mstats->len);

			if (mstats_len < sizeof(struct rofl::openflow13::ofp_meter_stats)) {
				throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);
			}

			add_meter_stats(index++).unpack(buf, mstats_len);

			offset += mstats_len;
		}
	} break;
	default:
		throw eBadRequestBadVersion("eBadRequestBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}






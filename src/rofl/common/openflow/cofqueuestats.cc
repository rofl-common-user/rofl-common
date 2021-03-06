/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofqueuestats.h"

#ifndef htobe16
#include "../endian_conversion.h"
#endif


using namespace rofl::openflow;


size_t
cofqueue_stats_request::length() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_queue_stats_request));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_queue_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_queue_stats_request));
	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
	return 0;
}


void
cofqueue_stats_request::pack(
		uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_queue_stats_request))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow10::ofp_queue_stats_request *stats = (struct rofl::openflow10::ofp_queue_stats_request*)buf;

		stats->port_no		= htobe16((uint16_t)(port_no & 0x0000ffff));
		stats->queue_id		= htobe32(queue_id);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_queue_stats_request))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow12::ofp_queue_stats_request *stats = (struct rofl::openflow12::ofp_queue_stats_request*)buf;

		stats->port_no		= htobe32(port_no);
		stats->queue_id		= htobe32(queue_id);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_queue_stats_request))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_queue_stats_request *stats = (struct rofl::openflow13::ofp_queue_stats_request*)buf;

		stats->port_no		= htobe32(port_no);
		stats->queue_id		= htobe32(queue_id);

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



void
cofqueue_stats_request::unpack(
		uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_queue_stats_request))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow10::ofp_queue_stats_request *stats = (struct rofl::openflow10::ofp_queue_stats_request*)buf;

		port_no 		= (uint32_t)be16toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_queue_stats_request))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow12::ofp_queue_stats_request *stats = (struct rofl::openflow12::ofp_queue_stats_request*)buf;

		port_no 		= be32toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_queue_stats_request))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_queue_stats_request *stats = (struct rofl::openflow13::ofp_queue_stats_request*)buf;

		port_no 		= be32toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}





size_t
cofqueue_stats_reply::length() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_queue_stats));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_queue_stats));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_queue_stats));
	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
	return 0;
}



void
cofqueue_stats_reply::pack(
		uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_queue_stats))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow10::ofp_queue_stats *stats = (struct rofl::openflow10::ofp_queue_stats*)buf;

		stats->port_no		= htobe16((uint16_t)(port_no & 0x0000ffff));
		stats->queue_id		= htobe32(queue_id);
		stats->tx_bytes		= htobe64(tx_bytes);
		stats->tx_packets	= htobe64(tx_packets);
		stats->tx_errors	= htobe64(tx_errors);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_queue_stats))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow12::ofp_queue_stats *stats = (struct rofl::openflow12::ofp_queue_stats*)buf;

		stats->port_no		= htobe32(port_no);
		stats->queue_id		= htobe32(queue_id);
		stats->tx_bytes		= htobe64(tx_bytes);
		stats->tx_packets	= htobe64(tx_packets);
		stats->tx_errors	= htobe64(tx_errors);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_queue_stats))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_queue_stats *stats = (struct rofl::openflow13::ofp_queue_stats*)buf;

		stats->port_no		= htobe32(port_no);
		stats->queue_id		= htobe32(queue_id);
		stats->tx_bytes		= htobe64(tx_bytes);
		stats->tx_packets	= htobe64(tx_packets);
		stats->tx_errors	= htobe64(tx_errors);
		stats->duration_sec	= htobe32(duration_sec);
		stats->duration_nsec= htobe32(duration_nsec);

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



void
cofqueue_stats_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_queue_stats))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow10::ofp_queue_stats *stats = (struct rofl::openflow10::ofp_queue_stats*)buf;

		port_no 		= (uint32_t)be16toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);
		tx_bytes		= be64toh(stats->tx_bytes);
		tx_packets		= be64toh(stats->tx_packets);
		tx_errors		= be64toh(stats->tx_errors);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_queue_stats))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow12::ofp_queue_stats *stats = (struct rofl::openflow12::ofp_queue_stats*)buf;

		port_no 		= be32toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);
		tx_bytes		= be64toh(stats->tx_bytes);
		tx_packets		= be64toh(stats->tx_packets);
		tx_errors		= be64toh(stats->tx_errors);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_queue_stats))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_queue_stats *stats = (struct rofl::openflow13::ofp_queue_stats*)buf;

		port_no 		= be32toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);
		tx_bytes		= be64toh(stats->tx_bytes);
		tx_packets		= be64toh(stats->tx_packets);
		tx_errors		= be64toh(stats->tx_errors);
		duration_sec	= be32toh(stats->duration_sec);
		duration_nsec	= be32toh(stats->duration_nsec);

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}





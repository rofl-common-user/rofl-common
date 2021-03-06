/*
 * cofrole.cc
 *
 *  Created on: 18.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofrole.h"

using namespace rofl::openflow;


/*static*/int64_t
cofrole::distance(int64_t rcvd_gen_id, int64_t cached_gen_id)
{
	return (rcvd_gen_id - cached_gen_id);
}



cofrole::cofrole(
		uint8_t ofp_version) :
				ofp_version(ofp_version),
				role(rofl::openflow13::OFPCR_ROLE_EQUAL),
				generation_id((int64_t)-1)
{}



cofrole::~cofrole()
{}



cofrole::cofrole(
		cofrole const& role)
{
	*this = role;
}



cofrole&
cofrole::operator= (
		cofrole const& r)
{
	if (this == &r)
		return *this;

	ofp_version		= r.ofp_version;
	role			= r.role;
	generation_id 	= r.generation_id;

	return *this;
}



void
cofrole::clear()
{
	role = 0;
	generation_id = 0;
}



size_t
cofrole::length() const
{
	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return sizeof(struct role_t);
	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



void
cofrole::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct role_t* r = (struct role_t*)buf;

		r->role 			= htobe32(role);
		r->generation_id 	= htobe64(generation_id);

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



void
cofrole::unpack(uint8_t *buf, size_t buflen)
{
	if (buflen < length())
		throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct role_t* r = (struct role_t*)buf;

		role 			= be32toh(r->role);
		generation_id 	= be64toh(r->generation_id);

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



bool
cofrole::is_valid_role() const
{
	switch (get_role()) {
	// same for OF12 and OF13
	case rofl::openflow13::OFPCR_ROLE_EQUAL:
	case rofl::openflow13::OFPCR_ROLE_MASTER:
	case rofl::openflow13::OFPCR_ROLE_SLAVE:
	case rofl::openflow13::OFPCR_ROLE_NOCHANGE: {
		return true;
	} break;
	default:
		return false;
	}
}




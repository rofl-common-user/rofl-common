/*
 * csockaddr.cc
 *
 *  Created on: 10.06.2014
 *      Author: andreas
 */

#include "rofl/common/csockaddr.h"

using namespace rofl;


csockaddr::csockaddr(
		int family, const std::string& addr, uint16_t portno)
{
	switch (family) {
	case AF_INET: {
		ca_mem = (uint8_t*)cmemory::resize(salen = sizeof(struct sockaddr_in));
		ca_saddr->sa_family = AF_INET;
		caddress_in4 saddr(addr);
		ca_s4addr->sin_addr.s_addr = saddr.get_addr_nbo();
		ca_s4addr->sin_port = htobe16(portno);
	} break;
	case AF_INET6: {
		ca_mem = cmemory::resize(salen = sizeof(struct sockaddr_in6));
		ca_saddr->sa_family = AF_INET6;
		caddress_in6 saddr(addr);
		memcpy(ca_s6addr->sin6_addr.s6_addr, saddr.somem(), 16);
		ca_s6addr->sin6_port = htobe16(portno);
	} break;
	default:
		throw eInvalid("csockaddr::csockaddr() unsupported family");
	}
}



csockaddr::csockaddr(
		const caddress_in4& addr, uint16_t portno)
{
	ca_mem = cmemory::resize(salen = sizeof(struct sockaddr_in));
	ca_saddr->sa_family = AF_INET;
	caddress_in4 saddr(addr);
	ca_s4addr->sin_addr.s_addr = saddr.get_addr_nbo();
	ca_s4addr->sin_port = htobe16(portno);
}



csockaddr::csockaddr(
		const caddress_in6& addr, uint16_t portno)
{
	ca_mem = cmemory::resize(salen = sizeof(struct sockaddr_in6));
	ca_saddr->sa_family = AF_INET6;
	caddress_in6 saddr(addr);
	memcpy(ca_s6addr->sin6_addr.s6_addr, saddr.somem(), 16);
	ca_s6addr->sin6_port = htobe16(portno);
}



int
csockaddr::get_family() const
{
	return ca_saddr->sa_family;
}



size_t
csockaddr::length() const
{
	switch (ca_saddr->sa_family) {
	case AF_INET: {
		return sizeof(struct sockaddr_in);
	} break;
	case AF_INET6: {
		return sizeof(struct sockaddr_in6);
	} break;
	default:
		throw eInvalid("csockaddr::length() unsupported family");
	}
}



void
csockaddr::pack(
		uint8_t* sa, size_t salen)
{
	if ((0 == sa) || (0 == salen))
		return;

	if (salen < csockaddr::length())
		throw eInvalid("csockaddr::pack() buflen too short");

	cmemory::pack((uint8_t*)sa, salen);
}



void
csockaddr::unpack(
		uint8_t* sa, size_t salen)
{
	if ((0 == sa) || (0 == salen))
		return;

	if (salen < csockaddr::length())
		throw eInvalid("csockaddr::unpack() salen too short");

	cmemory::unpack((uint8_t*)sa, salen);
	ca_mem = cmemory::somem();
}




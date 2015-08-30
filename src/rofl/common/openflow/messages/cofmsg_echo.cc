#include "rofl/common/openflow/messages/cofmsg_echo.h"

using namespace rofl::openflow;

size_t
cofmsg_echo_request::length() const
{
	return (cofmsg::length() + body.memlen());
}



void
cofmsg_echo_request::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_echo_request::pack() buf too short");

	body.pack(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}



void
cofmsg_echo_request::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	body.resize(0);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_echo_request::length())
		throw eBadRequestBadLen("cofmsg_echo_request::unpack() buf too short");

	switch (get_version()) {
	default: {
		if (get_type() != rofl::openflow13::OFPT_ECHO_REQUEST)
			throw eMsgInval("cofmsg_echo_request::unpack() invalid message type");

		if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
			body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
					buflen - sizeof(struct rofl::openflow::ofp_header));
		}
	};
	}

	if (get_length() < cofmsg_echo_request::length())
		throw eBadRequestBadLen("cofmsg_echo_request::unpack() buf too short");
}



size_t
cofmsg_echo_reply::length() const
{
	return (cofmsg::length() + body.memlen());
}



void
cofmsg_echo_reply::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_echo_reply::pack() buf too short");

	body.pack(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}



void
cofmsg_echo_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	body.resize(0);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_echo_reply::length())
		throw eBadRequestBadLen("cofmsg_echo_reply::unpack() buf too short");

	switch (get_version()) {
	default: {
		if (get_type() != rofl::openflow13::OFPT_ECHO_REPLY)
			throw eMsgInval("cofmsg_echo_reply::unpack() invalid message type");

		if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
			body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
					buflen - sizeof(struct rofl::openflow::ofp_header));
		}
	};
	}

	if (get_length() < cofmsg_echo_reply::length())
		throw eBadRequestBadLen("cofmsg_echo_reply::unpack() buf too short");
}



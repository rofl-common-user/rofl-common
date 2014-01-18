/*
 * crofdptImpl.h
 *
 *  Created on: 25.09.2013
 *      Author: andreas
 */

#ifndef CROFDPTIMPL_H
#define CROFDPTIMPL_H 1

#include <map>
#include <set>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include <stdio.h>
#include <strings.h>
#ifdef __cplusplus
}
#endif

#include "rofl/common/ciosrv.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/protocols/fetherframe.h"
#include "rofl/common/logging.h"
#include "rofl/common/crofdpt.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/crofchan.h"
#include "rofl/common/ctransactions.h"
#include "rofl/common/openflow/cofpacketqueue.h"

namespace rofl
{

/**
 * \class	cofdptImpl
 *
 * This class stores state for an attached data path element
 * including its ports (@see cofport). It is tightly bound to
 * class crofbase (@see crofbase). Created upon reception of an
 * OpenFlow HELLO message from the data path element, cofdpath
 * acquires all state by sending FEATURES-request, GET-CONFIG-request,
 * and TABLE-STATS-request. It also provides a number of convenience
 * methods for controlling the datapath, e.g. clearing all flowtable
 * or grouptable entries.
 *
 */
class crofdptImpl :
	public crofdpt,
	public rofl::openflow::crofchan_env,
	public rofl::openflow::ctransactions_env,
	public ciosrv
{

private: // data structures

		enum crofdptImpl_timer_t {
		};

		enum crofdptImpl_state_t {
			STATE_INIT 							= 0,
			STATE_DISCONNECTED					= 1,
			STATE_CONNECTED						= 2,
			STATE_FEATURES_RCVD 				= 3,
			STATE_GET_CONFIG_RCVD				= 4,
			STATE_ESTABLISHED					= 5,
		};

		enum crofdptImpl_event_t {
			EVENT_NONE							= 0,
			EVENT_DISCONNECTED					= 1,
			EVENT_CONNECTED						= 2,
			EVENT_FEATURES_REPLY_RCVD			= 3,
			EVENT_FEATURES_REQUEST_EXPIRED		= 4,
			EVENT_GET_CONFIG_REPLY_RCVD			= 5,
			EVENT_GET_CONFIG_REQUEST_EXPIRED	= 6,
			EVENT_TABLE_STATS_REPLY_RCVD		= 7,
			EVENT_TABLE_STATS_REQUEST_EXPIRED	= 8,
			EVENT_TABLE_FEATURES_STATS_REPLY_RCVD		= 9,
			EVENT_TABLE_FEATURES_STATS_REQUEST_EXPIRED	= 10,
		};


		rofl::openflow::crofchan				rofchan;		// OFP control channel

		std::bitset<32>     		            flags;

		uint64_t 								dpid;			// datapath id
		std::string	 							s_dpid;			// datapath id as std::string
		cmacaddr 								hwaddr;			// datapath mac address
		uint32_t 								n_buffers; 		// number of buffer lines
		uint8_t 								n_tables;		// number of tables
		uint32_t 								capabilities;	// capabilities flags

		std::map<uint8_t, coftable_stats_reply> tables;	// map of tables: table_id:coftable_stats_reply
		cofports								ports;			// list of ports
		std::bitset<32> 						dptflags;		// 'fragmentation' flags
		uint16_t								config;
		uint16_t 								miss_send_len; 	// length of bytes sent to controller

		cfsptable 								fsptable;		// flowspace registration table

		crofbase 								*rofbase;		// layer-(n) entity
		rofl::openflow::ctransactions			transactions;	// pending OFP transactions

		unsigned int							state;
		std::deque<enum crofdptImpl_event_t> 	events;

public:

	/**
	 * @brief 	Default constructor for generating an empty cofdpt instance
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	crofdptImpl(
			crofbase *rofbase,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap);

	/**
	 * @brief 	Constructor for accepted incoming connection on socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param newsd socket descriptor of new established control connection socket
	 */
	crofdptImpl(
			crofbase *rofbase,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
			int newsd);

	/**
	 * @brief 	Constructor for creating a new cofdpt instance and actively connecting to a data path element.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param ra peer address of control connection
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 */
	crofdptImpl(
			crofbase *rofbase,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
			int reconnect_start_timeout,
			caddress const& ra,
			int domain,
			int type,
			int protocol);



	/**
	 * @brief	Destructor.
	 *
	 * Deallocates all previously allocated resources for storing data model
	 * exposed by the data path element.
	 */
	virtual
	~crofdptImpl();


public:

	/**
	 * @brief	Returns OpenFlow version negotiated for control connection.
	 */
	virtual uint8_t
	get_version() const { return rofchan.get_version(); };

	/**
	 * @brief	Returns a reference to rofchan's cofhello_elem_versionbitmap instance
	 */
	virtual rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap() { return rofchan.get_versionbitmap(); };

	/**
	 * @brief	Returns a reference to the associated crofchan instance
	 */
	virtual rofl::openflow::crofchan&
	get_channel() { return rofchan; };

	/**
	 * @brief	Returns true, when the OFP control channel is up
	 */
	virtual bool
	is_established() const { return (rofchan.is_established()); };

	/**
	 *
	 */
	virtual caddress
	get_peer_addr() { return rofchan.get_conn(0).get_rofsocket().get_socket().raddr; };

public:

	/* overloaded from crofchan_env */

	virtual void
	handle_connected(rofl::openflow::crofchan *chan, uint8_t aux_id);

	virtual void
	handle_closed(rofl::openflow::crofchan *chan, uint8_t aux_id);

	virtual void
	recv_message(rofl::openflow::crofchan *chan, uint8_t aux_id, cofmsg *msg);

	virtual uint32_t
	get_async_xid(rofl::openflow::crofchan *chan);

	virtual uint32_t
	get_sync_xid(rofl::openflow::crofchan *chan, uint8_t msg_type = 0, uint16_t msg_sub_type = 0);

	virtual void
	release_sync_xid(rofl::openflow::crofchan *chan, uint32_t xid);

public:

	/* overloaded from ctransactions_env */

	virtual void
	ta_expired(rofl::openflow::ctransactions& tas, rofl::openflow::ctransaction& ta);

public:

	/**
	 * @name	Data path property related methods
	 *
	 * These methods provide an interface for querying various properties of the attached data path element.
	 */

	/**@{*/

	/**
	 * @brief	Returns the data path element's data path ID.
	 *
	 * @return dpid
	 */
	virtual uint64_t
	get_dpid() const { return dpid; };


	/**
	 * @brief	Returns the data path element's ID string.
	 *
	 * @return s_dpid
	 */
	virtual std::string
	get_dpid_s() const { return s_dpid; };


	/**
	 * @brief	Returns the data path element's hardware address.
	 *
	 * @return hwaddr
	 */
	virtual cmacaddr
	get_hwaddr() const { return hwaddr; };


	/**
	 * @brief	Returns the data path element's number of buffers for storing data packets.
	 *
	 * @return n_buffers
	 */
	virtual uint32_t
	get_n_buffers() const { return n_buffers; };


	/**
	 * @brief	Returns the data path element's number of tables in the OpenFlow pipeline.
	 *
	 * @return n_tables
	 */
	virtual uint8_t
	get_n_tables() const { return n_tables; };


	/**
	 * @brief	Returns the data path element's capabilities.
	 *
	 * @return capabilities
	 */
	virtual uint32_t
	get_capabilities() const { return capabilities; };


	/**
	 * @brief	Returns the data path element's configuration.
	 *
	 * @return config
	 */
	virtual uint16_t
	get_config() const { return config; };


	/**
	 * @brief	Returns the data path element's current miss_send_len parameter.
	 *
	 * @return miss_send_len
	 */
	virtual uint16_t
	get_miss_send_len() const { return miss_send_len; };


	/**
	 * @brief	Returns reference to the data path element's flowspace table.
	 *
	 * @return fsptable
	 */
	virtual cfsptable&
	get_fsptable() { return fsptable; };


	/**
	 * @brief	Returns reference to the data path element's cofport list.
	 *
	 * @return ports
	 */
	virtual cofports&
	get_ports() { return ports; };


	/**
	 * @brief	Returns reference to the data path element's coftable_stats_reply list.
	 *
	 * @return tables
	 */
	virtual std::map<uint8_t, coftable_stats_reply>&
	get_tables() { return tables; };


	/**@}*/


public:


	/**
	 * @name Flowspace management methods
	 *
	 * ROFL contains a set of extensions that allows a controller to express parts
	 * of the overall namespace he is willing to control. The flowspace registration
	 * contains an OpenFlow match structure. Currently, a registration is hard state,
	 * i.e. it will be removed only when explicitly requested by the controller or
	 * the control connection between controller and data path is lost.
	 *
	 * Please note: this is going to change in a future revision by a soft state approach!
	 */

	/**@{*/

	/**
	 * @brief 	Makes a new flowspace registration at the data path element.
	 *
	 * This method registers a flowspace on the attached datapath element.
	 * Calling this method multiple times results in several flowspace
	 * registrations.
	 *
	 * @param ofmatch the flowspace definition to be registered
	 */
	virtual void
	fsp_open(
			cofmatch const& ofmatch);


	/**
	 * @brief 	Removes a flowspace registration from the attached data path element.
	 *
	 * This method deregisters a flowspace on the attached datapath element.
	 * The default argument is an empty (= all wildcard ofmatch) and removes
	 * all active flowspace registrations from the datapath element.
	 *
	 * @param ofmatch the flowspace definition to be removed
	 */
	virtual void
	fsp_close(
			cofmatch const& ofmatch = cofmatch());

	/**@}*/


public:

	/**
	 * @name	FlowMod management methods
	 *
	 * These methods provide a simple to use interface for managing FlowMod
	 * entries.
	 *
	 * Please note: these methods are subject to change in future revisions.
	 */

	/**@{*/

	/**
	 * @brief	Removes all flowtable entries from the attached datapath element.
	 *
	 * Sends a FlowMod-Delete message to the attached datapath element for removing
	 * all flowtable entries.
	 */
	virtual void
	flow_mod_reset();


	/**
	 * @brief	Removes all grouptable entries from the attached datapath element.
	 *
	 * Sends a GroupMod-Delete message to the attached datapath element for removing
	 * all grouptable entries.
	 */
	virtual void
	group_mod_reset();

	/**@}*/

private:

	/*
	 * overloaded from ciosrv
	 */

	/**
	 * @name 	handle_timeout
	 * @brief	handler for timeout events
	 *
	 * This virtual method is overloaded from (@see ciosrv) and
	 * is called upon expiration of a timer.
	 *
	 * @param[in] opaque The integer value specifying the type of the expired timer.
	 */
	void
	handle_timeout(
		int opaque);

private:

	/**
	 *
	 */
	void
	run_engine(
			enum crofdptImpl_event_t state = EVENT_NONE);

	/**
	 *
	 */
	void
	event_disconnected();

	/**
	 *
	 */
	void
	event_connected();

	/**
	 *
	 */
	void
	event_features_reply_rcvd();

	/**
	 *
	 */
	void
	event_features_request_expired();

	/**
	 *
	 */
	void
	event_get_config_reply_rcvd();

	/**
	 *
	 */
	void
	event_get_config_request_expired();

	/**
	 *
	 */
	void
	event_table_stats_reply_rcvd();

	/**
	 *
	 */
	void
	event_table_stats_request_expired();

	/**
	 *
	 */
	void
	event_table_features_stats_reply_rcvd();

	/**
	 *
	 */
	void
	event_table_features_stats_request_expired();



private:


	/** handle incoming vendor message (ROFL extensions)
	 */
	void
	experimenter_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);



	/**
	 * @name	features_reply_rcvd
	 * @brief	Called by cfwdekem when a FEATURES-reply was received.
	 *
	 * Cancels the internal timer waiting for FEATURES-reply.
	 * Stores parameters received in internal variables including ports.
	 * Starts timer for sending a GET-CONFIG-request.
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	features_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	/**
	 * @name	get_config_reply_rcvd
	 * @brief	Called by cfwdekem when a GET-CONFIG-reply was received.
	 *
	 * Cancels the internal timer waiting for GET-CONFIG-reply.
	 * Stores parameters received in internal variables.
	 * Starts timer for sending a TABLE-STATS-request.
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	get_config_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	/**
	 * @name	stats_reply_rcvd
	 * @brief	Called by cfwdelem when a STATS-reply was received.
	 *
	 * Cancels the internal timer waiting for STATS-reply.
	 * Stores parameters received in internal variables.
	 * Calls method fwdelem->handle_dpath_open().
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	multipart_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);



	void
	desc_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	table_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	port_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	flow_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	aggregate_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	queue_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	group_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	group_desc_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	group_features_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	meter_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	meter_config_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	meter_features_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	table_features_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	port_desc_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	void
	experimenter_stats_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);


	/**
	 * @name	barrier_reply_rcvd
	 * @brief	Called by cfwdekem when a BARRIER-reply was received.
	 *
	 * Cancels the internal timer waiting for STATS-reply.
	 * Calls method fwdelem->handle_barrier_reply().
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	barrier_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	/**
	 * @name	port_mod_sent
	 * @brief	Called by crofbase when a PORT-MOD-message was sent.
	 *
	 * Applies PortMod message to local cofport instance.
	 *
	 * @param[in] pack The OpenFlow message sent.
	 */
	void
	port_mod_sent(
			cofmsg *pack);

	/** handle PACKET-IN message
	 */
	void
	packet_in_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	/** handle FlowRemoved message
	 */
	void
	flow_removed_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	/** handle PORT-STATUS message
	 */
	void
	port_status_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	/** handle ROLE-REPLY messages
	 *
	 */
	void
	role_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	/**
	 *
	 */
	void
	queue_get_config_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);

	/**
	 * @name	get_async_config_reply_rcvd
	 * @brief	Called by crofbase when a GET-ASYNC-CONFIG-reply was received.
	 *
	 * Cancels the internal timer waiting for GET-ASYNC-CONFIG-reply.
	 * Stores parameters received in internal variables.
	 * Starts timer for sending a TABLE-STATS-request.
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	get_async_config_reply_rcvd(
			cofmsg *msg, uint8_t aux_id = 0);


private:


	/**
	 *
	 */
	void
	parse_message(
			cmemory *mem);


public:


	/**
	 * @name	Methods for sending OpenFlow messages
	 *
	 * These methods may be called by a derived class for sending
	 * a specific OF message.
	 */

	/**@{*/

	/**
	 * @brief	Sends a FEATURES.request to a data path element.
	 *
	 * @return transaction ID assigned to this request
	 */
	virtual uint32_t
	send_features_request();

	/**
	 * @brief	Sends a GET-CONFIG.request to a data path element.
	 *
	 * @return transaction ID assigned to this request
	 */
	virtual uint32_t
	send_get_config_request();

	/**
	 * @brief	Sends a TABLE-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this TABLE-STATS.request
	 */
	virtual uint32_t
	send_table_features_stats_request(
			uint16_t stats_flags);

	/**
	 * @brief	Sends a STATS.request to a data path element.
	 *
	 * @param stats_type one of the OFPMP_* constants
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param body body of STATS.request
	 * @param bodylen length of STATS.request body
	 * @return transaction ID for this STATS.request
	 */
	virtual uint32_t
	send_stats_request(
			uint16_t stats_type,
			uint16_t stats_flags,
			uint8_t *body = NULL,
			size_t bodylen = 0);

	/**
	 * @brief	Sends a DESC-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this DESC-STATS.request
	 */
	virtual uint32_t
	send_desc_stats_request(
			uint16_t stats_flags);

	/**
	 * @brief	Sends a FLOW-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param flow_stats_request body of a FLOW-STATS.request
	 * @return transaction ID for this FLOW-STATS.request
	 */
	virtual uint32_t
	send_flow_stats_request(
			uint16_t stats_flags,
			cofflow_stats_request const& flow_stats_request);

	/**
	 * @brief	Sends a AGGREGATE-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param aggr_stats_request body of an AGGREGATE-STATS.request
	 * @return transaction ID for this AGGREGATE-STATS.request
	 */
	virtual uint32_t
	send_aggr_stats_request(
			uint16_t flags,
			cofaggr_stats_request const& aggr_stats_request);


	/**
	 * @brief	Sends a TABLE-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this TABLE-STATS.request
	 */
	virtual uint32_t
	send_table_stats_request(
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a FLOW-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param port_stats_request body of a PORT-STATS.request
	 * @return transaction ID for this FLOW-STATS.request
	 */
	virtual uint32_t
	send_port_stats_request(
			uint16_t stats_flags,
			cofport_stats_request const& port_stats_request);

	/**
	 * @brief	Sends a QUEUE-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param queue_stats_request body of a QUEUE-STATS.request
	 * @return transaction ID for this QUEUE-STATS.request
	 */
	virtual uint32_t
	send_queue_stats_request(
			uint16_t stats_flags,
			cofqueue_stats_request const& queue_stats_request);

	/**
	 * @brief	Sends a GROUP-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param queue_stats_request body of a GROUP-STATS.request
	 * @return transaction ID for this GROUP-STATS.request
	 */
	virtual uint32_t
	send_group_stats_request(
			uint16_t stats_flags,
			cofgroup_stats_request const& group_stats_request);

	/**
	 * @brief	Sends a GROUP-DESC-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this AGGREGATE-STATS.request
	 */
	virtual uint32_t
	send_group_desc_stats_request(
			uint16_t flags = 0);

	/**
	 * @brief	Sends a GROUP-FEATURES-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this AGGREGATE-STATS.request
	 */
	virtual uint32_t
	send_group_features_stats_request(
			uint16_t flags);

	/**
	 * @brief	Sends an EXPERIMENTER-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param exp_id experimenter ID
	 * @param exp_type user defined type
	 * @param body user defined body
	 * @return transaction ID for this EXPERIMENTER-STATS.request
	 */
	virtual uint32_t
	send_experimenter_stats_request(
			uint16_t stats_flags,
			uint32_t exp_id,
			uint32_t exp_type,
			cmemory const& body);

	/**
	 * @brief	Sends a PACKET-OUT.message to a data path element.
	 *
	 * @param buffer_id buffer ID assigned by datapath (-1 if none) in host byte order
	 * @param in_port packet’s in-port (OFPP_NONE if none) in host byte order
	 * @param aclist OpenFlow ActionList
	 * @param data data packet to be sent out (optional)
	 * @param datalen length of data packet (optional)
	 * @result transaction ID assigned to this request
	 */
	virtual uint32_t
	send_packet_out_message(
			uint32_t buffer_id,
			uint32_t in_port,
			cofactions& aclist,
			uint8_t *data = NULL,
			size_t datalen = 0);

	/**
	 * @brief	Sends a BARRIER.request to a data path element.
	 *
	 * @result transaction ID assigned to this request
	 */
	virtual uint32_t
	send_barrier_request();

	/**
	 * @brief	Sends a ROLE.request to a data path element.
	 *
	 * @param role role as defined by OpenFlow
	 * @param generation_id gen_id as defined by OpenFlow
	 */
	virtual uint32_t
	send_role_request(
			uint32_t role,
			uint64_t generation_id);

	/**
	 * @brief 	Sends a FLOW-MOD.message to a data path element.
	 *
	 * @param flowentry FlowMod entry
	 */
	virtual uint32_t
	send_flow_mod_message(
			cflowentry const& flowentry);

	/**
	 * @brief 	Sends a GROUP-MOD.message to a data path element.
	 *
	 * @param groupentry GroupMod entry
	 */
	virtual uint32_t
	send_group_mod_message(
			cgroupentry const& groupentry);

	/**
	 * @brief	Sends a TABLE-MOD.message to a data path element.
	 *
	 * @param table_id ID of table to be reconfigured
	 * @param config new configuration for table
	 */
	virtual uint32_t
	send_table_mod_message(
			uint8_t table_id,
			uint32_t config);

	/**
	 * @brief	Sends a PORT-MOD.message to a data path element.
	 *
	 * @param port_no number of port to be updated
	 * @param hwaddr MAC address assigned to port
	 * @param config OpenFlow config parameter
	 * * @param mask OpenFlow mask parameter
	 * * @param advertise OpenFlow advertise parameter
	 */
	virtual uint32_t
	send_port_mod_message(
			uint32_t port_no,
			cmacaddr const& hwaddr,
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);

	/**
	 * @brief	Sends a SET-CONFIG.message to a data path element.
	 *
	 * @param flags field of OpenFlow's OFPC_* flags
	 * @param miss_send_len OpenFlow' miss_send_len parameter
	 */
	virtual uint32_t
	send_set_config_message(
		uint16_t flags,
		uint16_t miss_send_len);

	/**
	 * @brief	Sends a QUEUE-GET-CONFIG.request to a data path element.
	 *
	 * @param port port to be queried. Should refer to a valid physical port (i.e. < OFPP_MAX)
	 * @result transaction ID assigned to this request
	 */
	virtual uint32_t
	send_queue_get_config_request(
		uint32_t port);

	/**
	 * @brief	Sends a GET-ASYNC-CONFIG.request to a data path element.
	 *
	 * @return transaction ID assigned to this request
	 */
	virtual uint32_t
	send_get_async_config_request();

	/**
	 * @brief	Sends a SET-ASYNC-CONFIG.message to a data path element.
	 *
	 * @param packet_in_mask0 packet_in_mask[0]
	 * @param packet_in_mask1 packet_in_mask[1]
	 * @param port_status_mask0 port_status_mask[0]
	 * @param port_status_mask1 port_status_mask[1]
	 * @param flow_removed_mask0 flow_removed_mask[0]
	 * @param flow_removed_mask1 flow_removed_mask[1]
	 */
	virtual uint32_t
	send_set_async_config_message(
		uint32_t packet_in_mask0,
		uint32_t packet_in_mask1,
		uint32_t port_status_mask0,
		uint32_t port_status_mask1,
		uint32_t flow_removed_mask0,
		uint32_t flow_removed_mask1);

	/**
	 * @brief	Sends an ERROR.message to a data path element.
	 *
	 * These messages are used for failed HELLO negotiations and
	 * experimental extensions.
	 *
	 * @param xid transaction ID of reply/notification that generated this error message
	 * @param type one of OpenFlow's OFPET_* flags
	 * @param code one of OpenFlow's error codes
	 * @param data first (at least 64) bytes of failed reply/notification
	 * @param datalen length of failed reply/notification appended to error message
	 */
	virtual uint32_t
	send_error_message(
		uint32_t xid,
		uint16_t type,
		uint16_t code,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief 	Sends an EXPERIMENTER.message to a data path element.
	 *
	 * @param experimenter_id exp_id as assigned by ONF
	 * @param exp_type exp_type as defined by the ONF member
	 * @param body pointer to opaque experimenter message body (optional)
	 * @param bodylen length of body (optional)
	 * @result transaction ID assigned to this request
	 */
	virtual uint32_t
	send_experimenter_message(
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body = NULL,
			size_t bodylen = 0);

	/**@}*/

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofdptImpl const& dpt) {
		os << indent(0) << "<cofdptImpl ";
		os << "dpid:0x" << std::hex << (unsigned long long)(dpt.dpid) << std::dec << " ";

		os << ">" << std::endl;
		indent i(2);
		os << dpt.rofchan;
		return os;
	};
};

}; // end of namespace

#endif /* COFDPTIMPL_H_ */
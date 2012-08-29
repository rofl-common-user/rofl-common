/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cethport.h"


/*static*/ std::set<cethport*> cethport::cethport_list;

cethport::cethport(std::string devname, int port_no) :
	clinuxport(devname, std::string("phy"), port_no),
	baddr(AF_PACKET, ETH_P_ALL, devname, 0, 0, NULL, 0)
{
	WRITELOG(CPORT, ROFL_DBG, "cethport::cethport dev(%s)", devname.c_str());
	cpopen(baddr, PF_PACKET, SOCK_RAW, htons(ETH_P_ALL), 10, devname);
	cethport::cethport_list.insert(this);

	enable_interface();
	get_port_no();
	get_config();
	get_hw_addr();

	WRITELOG(CPORT, ROFL_DBG, "cethport::cethport dev(%s) port-no(%d)", devname.c_str(), port_no);

	WRITELOG(CPORT, ROFL_DBG, cport::c_str());
}


cethport::~cethport()
{
	WRITELOG(CPORT, ROFL_DBG, "cethport::~cethport dev(%s)", devname.c_str());
	try {
		port_owner()->port_detach(this);
	} catch (ePortNotFound& e) { }
	cethport::cethport_list.erase(this);
}


void
cethport::handle_read(int fd)
{
	try {
		cmemory *mem = new cmemory(1526);

		int rc = read(sd, (void*)mem->somem(), mem->memlen());

		// error occured (or non-blocking)
		if (rc < 0)
		{
			switch (errno) {
			case EAGAIN:
				break;
			default:
				// after this, you should return and do nothing with this object any more
				delete mem;
				return;
			}

			// socket was closed
			throw eSocketReadFailed();
		}
		else if (rc == 0)
		{
			delete mem; return;
		}
		else
		{
			cpacket *pack = new cpacket(mem, port_no); // calls classify() internally

			if (pack->ether().get_dl_src() == get_hw_addr())
			{
#if 0
				fprintf(stderr, "cethport: rcvd packet from ourselves, stop here %s => %s <= %s\n",
						devname.c_str(), ether.get_dl_src().c_str(), ether.c_str());
#endif
				delete pack;
				return;
			}

			WRITELOG(CPORT, ROFL_DBG, "cethport(%p)::handle_revent() [%s] rc=%d "
							 "pack[%p] %s",
							 this,
							 devname.c_str(),
							 rc,
							 pack,
							 pack->c_str());

			port_owner()->store(this, pack);
			port_owner()->enqueue(this);

			//port_owner()->enqueue(port_no, pack);
		}

	} catch (eSocketReadFailed& e) {
		WRITELOG(CPORT, ROFL_DBG, "cethport(%s)::handle_revent() socket read failed", cport::devname.c_str());


	} catch (ePortNotFound& e) {
		WRITELOG(CPORT, ROFL_DBG, "cethport(%s)::handle_revent() port not found", cport::devname.c_str());

		throw;
	}
}


void
cethport::handle_out_queue()
{
	WRITELOG(CPORT, ROFL_DBG, "cethport(%p)::handle_out_queue() dev(%s) pout_queue.size()=%d",
			this, devname.c_str(), pout_queue.size());

	int i = 0;

	Lock lock(&queuelock);
	while (not pout_queue.empty() && (++i < OUT_QUEUE_MAX_TX_PER_ROUND)) // limit #  of packets this method
	{							// sends per round to 128
		cpacket *pack = pout_queue.front();

		WRITELOG(CPORT, ROFL_DBG, "cethport(%p)::handle_out_queue() %s",
				this, pack->c_str());

		// mem will be deleted by csocket base class
		cmemory *mem = new cmemory(pack->length());

		pack->pack(mem->somem(), mem->memlen());

		csocket::send_packet(mem);

		pout_queue.pop_front();
	}
}




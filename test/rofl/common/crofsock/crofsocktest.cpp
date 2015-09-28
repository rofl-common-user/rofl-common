/*
 * crofsocktest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "crofsocktest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( crofsocktest );

void
crofsocktest::setUp()
{}



void
crofsocktest::tearDown()
{}



void
crofsocktest::test()
{
	try {
		for (unsigned int i = 0; i < 2; i++) {
			test_mode = TEST_MODE_TCP;
			keep_running = true;
			msg_counter = 0;

			slisten = new rofl::crofsock(this);
			sclient = new rofl::crofsock(this);

			rofl::csockaddr baddr(rofl::caddress_in4("127.0.0.1"), 33998);

			sclient->set_raddr(baddr).tcp_connect(true);

			sleep(5);

			slisten->set_baddr(baddr).listen();

			while (keep_running) {
				struct timespec ts;
				ts.tv_sec = 0;
				ts.tv_nsec = 1000;
				pselect(0, NULL, NULL, NULL, &ts, NULL);
			}

			slisten->close();
			sclient->close();
			sserver->close();

			sleep(5);

			delete slisten;
			delete sclient;
			delete sserver;
		}

	} catch (rofl::eSysCall& e) {
		std::cerr << "crofsocktest::test() exception, what: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		std::cerr << "crofsocktest::test() exception, what: " << e.what() << std::endl;
	}
}




void
crofsocktest::test_tls()
{
	try {
		test_mode = TEST_MODE_TLS;
		keep_running = true;
		msg_counter = 0;

		slisten = new rofl::crofsock(this);
		sclient = new rofl::crofsock(this);

		rofl::csockaddr baddr(rofl::caddress_in4("127.0.0.1"), 33999);

		sclient->set_raddr(baddr).
					set_tls_cafile("../../../../../tools/xca/ca.rofl-core.crt.pem").
					set_tls_certfile("../../../../../tools/xca/client.crt.pem").
					set_tls_keyfile("../../../../../tools/xca/client.key.pem").
						tls_connect(true);

		sleep(5);

		slisten->set_baddr(baddr).listen();

		while (keep_running) {
			struct timespec ts;
			ts.tv_sec = 0;
			ts.tv_nsec = 1000;
			pselect(0, NULL, NULL, NULL, &ts, NULL);
		}

		delete slisten;
		delete sclient;
		delete sserver;

	} catch (rofl::eSysCall& e) {
		std::cerr << "crofsocktest::test() exception, what: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		std::cerr << "crofsocktest::test() exception, what: " << e.what() << std::endl;
	}
}




void
crofsocktest::handle_listen(
		rofl::crofsock& socket, int sd)
{
	std::cerr << "handle listen" << std::endl;

	sserver = new rofl::crofsock(this);

	switch (test_mode) {
	case TEST_MODE_TCP: {
		sserver->tcp_accept(sd);

	} break;
	case TEST_MODE_TLS: {
		sserver->set_tls_cafile("../../../../../tools/xca/ca.rofl-core.crt.pem").
					set_tls_certfile("../../../../../tools/xca/server.crt.pem").
					set_tls_keyfile("../../../../../tools/xca/server.key.pem").
						tls_accept(sd);
	} break;
	default: {

	};
	}
}



void
crofsocktest::handle_tcp_connect_refused(
		rofl::crofsock& socket)
{
	std::cerr << "handle tcp connect refused" << std::endl;
}



void
crofsocktest::handle_tcp_connect_failed(
		rofl::crofsock& socket)
{
	std::cerr << "handle tcp connect failed" << std::endl;
}



void
crofsocktest::handle_tcp_connected(
		rofl::crofsock& socket)
{
	std::cerr << "handle connected" << std::endl;

	switch (test_mode) {
	case TEST_MODE_TCP: {

		rofl::openflow::cofmsg_hello* hello =
				new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

		sclient->send_message(hello);

	} break;
	case TEST_MODE_TLS: {

	} break;
	default: {

	};
	}
}



void
crofsocktest::handle_tcp_accept_refused(
		rofl::crofsock& socket)
{
	std::cerr << "handle tcp accept refused" << std::endl;
}



void
crofsocktest::handle_tcp_accept_failed(
		rofl::crofsock& socket)
{
	std::cerr << "handle tcp accept failed" << std::endl;
}



void
crofsocktest::handle_tcp_accepted(
		rofl::crofsock& socket)
{
	std::cerr << "handle tcp accepted" << std::endl;

	switch (test_mode) {
	case TEST_MODE_TCP: {

		rofl::openflow::cofmsg_features_request* features =
				new cofmsg_features_request(rofl::openflow13::OFP_VERSION, 0xb1b2b3b4);

		sserver->send_message(features);

	} break;
	case TEST_MODE_TLS: {

	} break;
	default: {

	};
	}
}



void
crofsocktest::handle_closed(
		rofl::crofsock& socket)
{
	std::cerr << "handle closed" << std::endl;
}



void
crofsocktest::handle_send(
		rofl::crofsock& socket)
{
	std::cerr << "handle send" << std::endl;
}



void
crofsocktest::congestion_indication(
		rofl::crofsock& socket)
{
	std::cerr << "congestion indication" << std::endl;
}



void
crofsocktest::handle_recv(
		rofl::crofsock& socket, rofl::openflow::cofmsg *msg)
{
	if (&socket == sserver) {
		std::cerr << "sserver => handle recv " << std::endl << *msg;
		delete msg;

		if (server_msg_counter < 10) {
			rofl::openflow::cofmsg_features_request* features =
					new cofmsg_features_request(rofl::openflow13::OFP_VERSION, 0xb1b2b3b4);

			sserver->send_message(features);

			server_msg_counter++;
		}

		if ((server_msg_counter >= 10) && (client_msg_counter >= 10)) {
			std::cerr << "AAAAAAAAAAAAAA [sserver]" << std::endl;
			keep_running = false;
		}

	} else
	if (&socket == sclient) {
		std::cerr << "sclient => handle recv " << std::endl << *msg;
		delete msg;

		if (client_msg_counter < 10) {
			rofl::openflow::cofmsg_hello* hello =
					new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

			sclient->send_message(hello);

			client_msg_counter++;
		}

		if ((server_msg_counter >= 10) && (client_msg_counter >= 10)) {
			std::cerr << "AAAAAAAAAAAAAA [sclient]" << std::endl;
			keep_running = false;
		}

	}

}



void
crofsocktest::handle_tls_connect_failed(
		rofl::crofsock& socket)
{
	std::cerr << "handle tls connect failed" << std::endl;
}



void
crofsocktest::handle_tls_connected(
		rofl::crofsock& socket)
{
	std::cerr << "handle tls connected" << std::endl;

	rofl::openflow::cofmsg_hello* hello =
			new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

	sclient->send_message(hello);
}



void
crofsocktest::handle_tls_accept_failed(
		rofl::crofsock& socket)
{
	std::cerr << "handle tls accept failed" << std::endl;
}



void
crofsocktest::handle_tls_accepted(
		rofl::crofsock& socket)
{
	std::cerr << "handle tls accepted" << std::endl;

	rofl::openflow::cofmsg_features_request* features =
			new cofmsg_features_request(rofl::openflow13::OFP_VERSION, 0xb1b2b3b4);

	sserver->send_message(features);
}



/*
 * Copyright 2011 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gr_blob_to_socket.h>
#include <gr_io_signature.h>
#include <boost/asio.hpp>

namespace asio = boost::asio;

/***********************************************************************
 * UDP Implementation
 **********************************************************************/
class gr_blob_to_udp_impl : public gr_blob_to_socket{
public:
    gr_blob_to_udp_impl(const std::string &addr, const std::string &port):
        gr_sync_block(
            "blob_to_udp",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(0, 0, 0)
        )
    {
        asio::ip::udp::resolver resolver(_io_service);
        asio::ip::udp::resolver::query query(asio::ip::udp::v4(), addr, port);
        asio::ip::udp::endpoint endpoint = *resolver.resolve(query);

        _socket = boost::shared_ptr<asio::ip::udp::socket>(new asio::ip::udp::socket(_io_service));
        _socket->open(asio::ip::udp::v4());
        _socket->connect(endpoint);
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        //loop for blobs until this thread is interrupted
        while (true){
            gr_tag_t msg = this->pop_msg_queue();
            if (!pmt::pmt_is_blob(msg.value)) continue;
            if (pmt::pmt_blob_length(msg.value) == 0) break; //empty blob, we are done here
            _socket->send(asio::buffer(
                pmt::pmt_blob_ro_data(msg.value),
                pmt::pmt_blob_length(msg.value)
            ));
        }

        //when handle msgs finished, work is marked done
        return -1;
    }

private:
    asio::io_service _io_service;
    boost::shared_ptr<asio::ip::udp::socket> _socket;
};

/***********************************************************************
 * TCP Implementation
 **********************************************************************/
class gr_blob_to_tcp_impl : public gr_blob_to_socket{
public:
    gr_blob_to_tcp_impl(const std::string &addr, const std::string &port):
        gr_sync_block(
            "blob_to_tcp",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(0, 0, 0)
        ),
        _connected(false)
    {
        asio::ip::tcp::resolver resolver(_io_service);
        asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), addr, port);
        _endpoint = *resolver.resolve(query);

        _socket = boost::shared_ptr<asio::ip::tcp::socket>(new asio::ip::tcp::socket(_io_service));
    }

    bool start(void){
        //ensure connected before starting
        if (!_connected){
            _socket->connect(_endpoint);
            _connected = true;
        }
        return true;
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        //loop for blobs until this thread is interrupted
        while (true){
            gr_tag_t msg = this->pop_msg_queue();
            if (!pmt::pmt_is_blob(msg.value)) continue;
            if (pmt::pmt_blob_length(msg.value) == 0) break; //empty blob, we are done here
            _socket->send(asio::buffer(
                pmt::pmt_blob_ro_data(msg.value),
                pmt::pmt_blob_length(msg.value)
            ));
        }

        //when handle msgs finished, work is marked done
        return -1;
    }

private:
    asio::io_service _io_service;
    asio::ip::tcp::endpoint _endpoint;
    boost::shared_ptr<asio::ip::tcp::socket> _socket;
    bool _connected;
};

/***********************************************************************
 * Factory function
 **********************************************************************/
boost::shared_ptr<gr_blob_to_socket> gr_make_blob_to_socket(
    const std::string &proto, const std::string &addr, const std::string &port
){
    if (proto == "UDP") return boost::shared_ptr<gr_blob_to_socket>(new gr_blob_to_udp_impl(addr, port));
    if (proto == "TCP") return boost::shared_ptr<gr_blob_to_socket>(new gr_blob_to_tcp_impl(addr, port));
    throw std::invalid_argument("unknown protocol for socket to blob: " + proto);
}
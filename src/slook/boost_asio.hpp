#pragma once

#include "slook.hpp"

#include <boost/asio.hpp>
#include <string>
#include <vector>

namespace slook {
struct AsioServer {
    template<typename T, std::size_t>
    using Vec = std::vector<T>;

    template<std::size_t S>
    using Str = std::string;

public:
    using Lookup_t = slook::Lookup<
      Vec,
      Str,
      std::function<void(std::span<std::byte const>)>,
      std::function<void(slook::Service<Str, Vec> const&)>>;

    AsioServer(boost::asio::io_context& ioc_, std::uint16_t port, std::string_view multicastAddress)
      : ioc{ioc_}
      , socket{ioc}
      , lookup{
          [this](std::vector<std::byte> const& data) { send(data); },
        } {
        auto const ep         = boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port);
        auto const mc_address = boost::asio::ip::make_address(multicastAddress);
        socket.open(ep.protocol());
        socket.set_option(boost::asio::socket_base::reuse_address{true});
        socket.set_option(boost::asio::ip::multicast::join_group{mc_address});
        socket.bind(ep);
        sendEndpoint = boost::asio::ip::udp::endpoint(mc_address, port);
        recv();
    }

    Lookup_t& getLookup() { return lookup; }

private:
    boost::asio::ip::udp::endpoint      sendEndpoint;
    std::vector<std::byte>              recvData;
    bool                                sending{false};
    std::vector<std::vector<std::byte>> openSendData;

    boost::asio::io_service&     ioc;
    boost::asio::ip::udp::socket socket;
    Lookup_t                     lookup;

    void startSend(std::span<std::byte const> data) {
        sending = true;
        socket.async_send_to(boost::asio::buffer(data), sendEndpoint, [this](auto ec, auto) {
            handle_send(ec);
        });
    }

    void handle_receive_from(boost::system::error_code error, std::size_t bytesRecvd) {
        if(!error) {
            lookup.messageCallback(std::span<std::byte const>{recvData.data(), bytesRecvd});
        }
        recv();
    }

    void handle_send(boost::system::error_code error) {
        if(error) {
            //TODO
        }
        sending = false;
        if(!openSendData.empty()) {
            startSend(openSendData.front());
            openSendData.erase(openSendData.begin());
        }
    }

    void recv() {
        recvData.resize(1024);
        socket.async_receive(boost::asio::buffer(recvData, 1024), [this](auto ec, auto s) {
            handle_receive_from(ec, s);
        });
    }

    void send(std::span<std::byte const> data) {
        if(!sending) {
            startSend(data);
        } else {
            openSendData.push_back(data);
        }
    }
};
}   // namespace slook

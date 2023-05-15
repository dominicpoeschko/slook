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
      std::function<void(std::optional<slook::IPAddress> const&, std::span<std::byte const>)>,
      std::function<void(slook::Service<Str, Vec> const&)>>;

    AsioServer(boost::asio::io_context& ioc_, std::uint16_t port, std::string_view multicastAddress)
      : ioc{ioc_}
      , socket{ioc}
      , lookup{
          [this](std::optional<slook::IPAddress> const& address, std::span<std::byte const> data) {
              send(address, data);
          },
        } {
        auto const ep         = boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port);
        auto const mc_address = boost::asio::ip::make_address(multicastAddress);
        socket.open(ep.protocol());
        socket.set_option(boost::asio::socket_base::reuse_address{true});
        socket.set_option(boost::asio::ip::multicast::join_group{mc_address});
        socket.bind(ep);
        multicastSendEndpoint = boost::asio::ip::udp::endpoint(mc_address, port);
        recv();
    }

    Lookup_t& getLookup() { return lookup; }

private:
    boost::asio::ip::udp::endpoint multicastSendEndpoint;
    std::vector<std::byte>         recvData;
    bool                           sending{false};
    std::vector<std::pair<boost::asio::ip::udp::endpoint, std::vector<std::byte>>> openSendData;

    boost::asio::ip::udp::endpoint lastRecvEndpoint;

    boost::asio::io_service&     ioc;
    boost::asio::ip::udp::socket socket;
    Lookup_t                     lookup;

    void startSend() {
        sending = true;
        socket.async_send_to(
          boost::asio::buffer(openSendData.front().second),
          openSendData.front().first,
          [this](auto ec, auto) {
              openSendData.erase(openSendData.begin());
              handle_send(ec);
          });
    }

    void handle_receive_from(boost::system::error_code error, std::size_t bytesRecvd) {
        if(!error) {
            slook::IPAddress address;
            if(lastRecvEndpoint.address().is_v4()) {
                auto add   = slook::IPv4Address{};
                auto bytes = lastRecvEndpoint.address().to_v4().to_bytes();
                std::transform(bytes.begin(), bytes.end(), add.begin(), [](auto x) {
                    return std::byte(x);
                });
                address = add;
            } else {
                auto add   = slook::IPv6Address{};
                auto bytes = lastRecvEndpoint.address().to_v6().to_bytes();
                std::transform(bytes.begin(), bytes.end(), add.begin(), [](auto x) {
                    return std::byte(x);
                });
                address = add;
            }
            lookup.messageCallback(
              address,
              std::span<std::byte const>{recvData.data(), bytesRecvd});
        }
        recv();
    }

    void handle_send(boost::system::error_code error) {
        if(error) {
            //TODO
        }
        sending = false;
        if(!openSendData.empty()) {
            startSend();
        }
    }

    void recv() {
        recvData.resize(1024);
        socket.async_receive_from(
          boost::asio::buffer(recvData, 1024),
          lastRecvEndpoint,
          [this](auto ec, auto s) { handle_receive_from(ec, s); });
    }

    void send(std::optional<slook::IPAddress> const& address, std::span<std::byte const> data) {
        boost::asio::ip::udp::endpoint ep;

        if(!address) {
            ep = multicastSendEndpoint;
        } else {
            if(std::holds_alternative<slook::IPv4Address>(*address)) {
                slook::IPv4Address const& slook_address = std::get<slook::IPv4Address>(*address);
                boost::asio::ip::address_v4::bytes_type bytes;
                std::transform(
                  slook_address.begin(),
                  slook_address.end(),
                  bytes.begin(),
                  [](auto x) {
                      return static_cast<boost::asio::ip::address_v4::bytes_type::value_type>(x);
                  });
                boost::asio::ip::address_v4 boost_address{bytes};
                ep.address(boost_address);
            } else {
                slook::IPv6Address const& slook_address = std::get<slook::IPv6Address>(*address);
                boost::asio::ip::address_v6::bytes_type bytes;
                std::transform(
                  slook_address.begin(),
                  slook_address.end(),
                  bytes.begin(),
                  [](auto x) {
                      return static_cast<boost::asio::ip::address_v6::bytes_type::value_type>(x);
                  });
                boost::asio::ip::address_v6 boost_address{bytes};
                ep.address(boost_address);
            }

            ep.port(multicastSendEndpoint.port());
        }

        std::vector<std::byte> d{};
        d.resize(data.size());
        std::copy(data.begin(), data.end(), d.begin());
        openSendData.emplace_back(ep, d);
        if(!sending) {
            startSend();
        }
    }
};
}   // namespace slook

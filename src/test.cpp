#include <fmt/format.h>
#include <fmt/ranges.h>
//
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <slook/slook.hpp>
#include <string_view>
using boost::asio::ip::udp;
#include <aglio/format.hpp>
#include <aglio/packager.hpp>

class server {
    slook::Lookup<std::vector, std::string> lookup;

public:
    server(boost::asio::io_service& io_service, short port)
      : io_service_(io_service)
      , socket_(io_service) {
        static constexpr auto MulticastAddress = std::string_view{"239.255.13.37"};

        auto const ep = udp::endpoint(udp::v4(), port);
        socket_.open(ep.protocol());
        socket_.set_option(boost::asio::socket_base::reuse_address{true});
        socket_.set_option(
          boost::asio::ip::multicast::join_group{boost::asio::ip::make_address(MulticastAddress)});
        socket_.set_option(boost::asio::ip::multicast::enable_loopback{true});
        socket_.bind(ep);

        std::cout << "test\n";
        recv();
    }

    void handle_receive_from(boost::system::error_code const& error, size_t bytes_recvd) {
        lookup.message_callback(std::span<std::byte const>{data_.data(), bytes_recvd});
        recv();
    }

    void recv() {
        data_.resize(1024);
        socket_.async_receive_from(
          boost::asio::buffer(data_, 1024),
          sender_endpoint_,
          boost::bind(
            &server::handle_receive_from,
            this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

private:
    boost::asio::io_service& io_service_;
    udp::socket              socket_;
    udp::endpoint            sender_endpoint_;
    std::vector<std::byte>   data_;
};

int main() {
    static constexpr std::uint16_t UDPPort = 7331;
    using namespace boost::asio;
    io_context ioc;

    server s{ioc, UDPPort};

    ioc.run();
}

#include <fmt/format.h>
#include <fmt/ranges.h>
//
#include <boost/asio.hpp>
#include <iostream>
#include <slook/slook.hpp>
#include <string_view>
using boost::asio::ip::udp;
#include <aglio/format.hpp>
#include <aglio/packager.hpp>
#include <functional>

class server {
    using Clock = std::chrono::steady_clock;

public:
    void send(std::vector<std::byte> const& data) {}

    void startTimer(std::chrono::milliseconds time) {}

    server(boost::asio::io_service& io_service, short port)
      : io_service_{io_service}
      , lookup{
          [this](std::vector<std::byte> const& data) { send(data); },
          [this](std::chrono::milliseconds time) { startTimer(time); },
        }
      , socket_{io_service}
      , timer{io_service} {
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
          [this](auto ec, auto s) { handle_receive_from(ec, s); });
    }

private:
    boost::asio::io_service& io_service_;
    slook::Lookup<
      std::vector,
      std::string,
      std::function<void(std::vector<std::byte> const&)>,
      std::function<void(std::chrono::milliseconds)>>
                                             lookup;
    udp::socket                              socket_;
    udp::endpoint                            sender_endpoint_;
    std::vector<std::byte>                   data_;
    boost::asio::basic_waitable_timer<Clock> timer;
};

int main() {
    static constexpr std::uint16_t UDPPort = 7331;
    using namespace boost::asio;
    io_context ioc;

    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](boost::system::error_code const&, int) { ioc.stop(); });

    server s{ioc, UDPPort};

    ioc.run();
}

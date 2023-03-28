#include <fmt/format.h>
#include <fmt/ranges.h>
//
#include <aglio/format.hpp>
#include <aglio/packager.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <slook/slook.hpp>
#include <string_view>

struct LookupServer {
private:
    using lookup_t = slook::Lookup<
      std::vector,
      std::string,
      std::function<void(std::vector<std::byte> const&)>,
      std::function<void(slook::Service<std::string> const&)>>;

public:
    LookupServer(
      boost::asio::io_context& ioc_,
      std::uint16_t            port,
      std::string_view         multicastAddress)
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

    lookup_t& getLookup() { return lookup; }

private:
    boost::asio::ip::udp::endpoint      sendEndpoint;
    std::vector<std::byte>              recvData;
    bool                                sending{false};
    std::vector<std::vector<std::byte>> openSendData;

    boost::asio::io_service&     ioc;
    boost::asio::ip::udp::socket socket;
    lookup_t                     lookup;

    void startSend(std::vector<std::byte> const& data) {
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
    void send(std::vector<std::byte> const& data) {
        if(!sending) {
            startSend(data);
        } else {
            openSendData.push_back(data);
        }
    }
};

int main(int argc, char** argv) {
    boost::asio::io_context ioc;

    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](boost::system::error_code const&, int) { ioc.stop(); });

    LookupServer server{ioc, 7331, "239.255.13.37"};

    slook::Service<std::string> s;
    s.name     = argv[1];
    s.protocol = slook::Protocol::TCP;
    s.port     = 12345;
    s.address  = slook::IPv4Address{std::byte{127}, std::byte{0}, std::byte{0}, std::byte{1}};
    server.getLookup().addService(s);
    server.getLookup().findServices(argv[2], [](auto x) { fmt::print("found {}\n", x); });

    ioc.run();
}

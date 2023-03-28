#include <aglio/type_descriptor.hpp>
#include <array>
#include <cstdint>
#include <variant>

namespace slook {
using IPv4Address = std::array<std::byte, 4>;
using IPv6Address = std::array<std::byte, 16>;
using IPAddress   = std::variant<IPv4Address, IPv6Address>;

enum Protocol { TCP, UDP };

template<typename String>
struct Service {
    String        name;
    std::uint16_t port;
    Protocol      protocol;
};

template<typename String>
struct ServiceResponce {
    String          nodeName;
    IPAddress       address;
    Service<String> service;
};

template<typename String>
struct NameResponce {
    String    nodeName;
    IPAddress address;
};

template<typename String>
struct Shutup {
    std::uint32_t num;
    String        serviceName;
    std::uint16_t time;
};
}   // namespace slook

#include <TypeDescriptor_types.hpp>
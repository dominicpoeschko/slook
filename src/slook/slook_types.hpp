#pragma once

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
    String         name;
    IPAddress      address;
    std::uint16_t  port;
    Protocol       protocol;
    auto operator<=>(Service const&) const = default;
};

}   // namespace slook

#include "TypeDescriptor_slook_types.hpp"

#pragma once

#include <aglio/type_descriptor.hpp>
#include <array>
#include <cstdint>
#include <variant>

namespace slook {

static constexpr std::size_t MaxNameSize = 128;
static constexpr std::size_t MaxAddreses = 16;

using IPv4Address = std::array<std::byte, 4>;
using IPv6Address = std::array<std::byte, 16>;
using IPAddress   = std::variant<IPv4Address, IPv6Address>;

enum Protocol { TCP, UDP };

template<template<std::size_t> typename String, template<typename, std::size_t> typename Vector>
struct Service {
    String<MaxNameSize>            name;
    Vector<IPAddress, MaxAddreses> addresses;
    std::uint16_t                  port;
    Protocol                       protocol;
    auto                           operator<=>(Service const&) const = default;
};

}   // namespace slook

#include "TypeDescriptor_slook_types.hpp"

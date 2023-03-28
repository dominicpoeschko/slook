#include "commands.hpp"

#include <aglio/packager.hpp>
#include <cstddef>
#include <span>
#include <fmt/format.h>
#include <aglio/format.hpp>

namespace slook {
template<template<typename> typename Vector, typename String>
struct Lookup {
    using packager = aglio::Packager<aglio::IPConfig>;

    auto message_callback(std::span<std::byte const> data) {

        auto const p = packager::unpack<slook::CommandSet<std::vector, std::string>>(data);
        fmt::print("{} {}\n",data.size(), p);
    }
};
}   // namespace slook


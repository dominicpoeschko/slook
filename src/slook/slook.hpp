#include "commands.hpp"

#include <aglio/format.hpp>
#include <aglio/packager.hpp>
#include <chrono>
#include <cstddef>
#include <fmt/format.h>
#include <span>

namespace slook {
template<template<typename> typename Vector, typename String, typename SendFunction, typename TimerFunction>
struct Lookup {
    SendFunction       send_f;
    TimerFunction timer_register_f;

    template<typename SendCb, typename TimerRegisterCb>
    Lookup(SendCb&& send_f_, TimerRegisterCb&& timer_register_f_)
      : send_f{std::forward<SendCb>(send_f_)}
      , timer_register_f{std::forward<TimerRegisterCb>(timer_register_f_)} {}

    using packager = aglio::Packager<aglio::IPConfig>;

    void message_callback(std::span<std::byte const> data) {
        auto const p = packager::unpack<slook::CommandSet<Vector, String>>(data);
        fmt::print("{} {}\n", data.size(), p);
    }

    void timer_callback() {}
};
}   // namespace slook


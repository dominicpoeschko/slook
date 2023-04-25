#pragma once
#include "slook_types.hpp"

#include <aglio/type_descriptor.hpp>
#include <optional>
#include <variant>

namespace slook {
namespace ServiceLookup {
    template<template<std::size_t> typename String>
    struct Request {
        String<MaxNameSize> serviceName;
    };
    template<template<std::size_t> typename String, template<typename, std::size_t> typename Vector>
    struct Response {
        Service<String, Vector> service;
    };
}   // namespace ServiceLookup

template<template<std::size_t> typename String, template<typename, std::size_t> typename Vector>
using CommandSet
  = std::variant<ServiceLookup::Request<String>, ServiceLookup::Response<String, Vector>>;

}   // namespace slook

#include "TypeDescriptor_slook_commands.hpp"

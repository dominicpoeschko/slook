#pragma once
#include "slook_types.hpp"

#include <aglio/type_descriptor.hpp>
#include <optional>
#include <variant>

namespace slook {
namespace ServiceLookup {
    template<typename String>
    struct Request {
        String serviceName;
    };
    template<typename String>
    struct Response {
        Service<String> service;
    };
}   // namespace ServiceLookup

template<typename String>
using CommandSet = std::variant<ServiceLookup::Request<String>, ServiceLookup::Response<String>>;

}   // namespace slook

#include "TypeDescriptor_slook_commands.hpp"

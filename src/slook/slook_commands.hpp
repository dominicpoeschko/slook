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
    template<typename String, template<typename> typename Vector>
    struct Response {
        Service<String, Vector> service;
    };
}   // namespace ServiceLookup

template<typename String, template<typename> typename Vector>
using CommandSet
  = std::variant<ServiceLookup::Request<String>, ServiceLookup::Response<String, Vector>>;

}   // namespace slook

#include "TypeDescriptor_slook_commands.hpp"

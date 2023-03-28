#include "types.hpp"

#include <aglio/type_descriptor.hpp>
#include <optional>
#include <variant>

namespace slook {
namespace multicast {
    struct Ping {};

    namespace ServiceLookup {
        template<typename String>
        struct Request {
            std::uint32_t num;
            String        serviceName;
            std::uint8_t  answeringPropabilaty;
        };
        template<typename String>
        struct Response {
            std::uint32_t    num;
            String           nodeName;
            slook::IPAddress address;
            Service<String>  service;
        };
    }   // namespace ServiceLookup

    namespace NameLookup {
        template<template<typename> typename Vector, typename String>
        struct Request {
            Vector<String> nodeNames;
        };
        template<template<typename> typename Vector, typename String>
        struct Response {
            Vector<slook::NameResponce<String>> nodes;
        };
    }   // namespace NameLookup

    template<typename String>
    struct ShutupRequest : slook::Shutup<String> {
        std::vector<String> nodeNames;
    };
}   // namespace multicast

namespace unicast {
    template<typename String>
    struct ShutupRequest : slook::Shutup<String> {};

    namespace ServiceLookup {
        template<typename String>
        struct Request {
            String serviceName;
        };
        template<typename String>
        struct Response {
            String                                nodeName;
            slook::IPAddress                      address;
            String                                serviceName;
            std::optional<slook::Service<String>> service;
        };
    }   // namespace ServiceLookup
}   // namespace unicast

template<template<typename> typename Vector, typename String>
using CommandSet = std::variant<
  multicast::Ping,
  multicast::ServiceLookup::Request<String>,
  multicast::ServiceLookup::Response<String>,
  multicast::NameLookup::Request<Vector, String>,
  multicast::NameLookup::Response<Vector, String>,
  multicast::ShutupRequest<String>,
  unicast::ShutupRequest<String>,
  unicast::ServiceLookup::Request<String>,
  unicast::ServiceLookup::Response<String>>;

}   // namespace slook

#include <TypeDescriptor_commands.hpp>

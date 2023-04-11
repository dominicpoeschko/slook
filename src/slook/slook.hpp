#pragma once
#include "slook_commands.hpp"

#include <aglio/format.hpp>
#include <aglio/packager.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <fmt/format.h>
#include <span>
#include <utility>

namespace slook {
template<
  template<typename>
  typename Vector,
  typename String,
  typename SendFunction,
  typename ServiceCallback>
struct Lookup {
    template<typename SendFunction_>
    Lookup(SendFunction_&& sendFunction_)
      : sendFunction{std::forward<SendFunction_>(sendFunction_)} {}

    void messageCallback(std::span<std::byte const> data) {
        auto const p = packager::unpack<slook::CommandSet<String>>(data);

        if(p) {
            std::visit([this](auto const& v) { handle(v); }, *p);
        }
    }

    void addService(slook::Service<String> const& new_service) {
        if(
          services.end()
          == std::find_if(services.begin(), services.end(), [&](auto const& service) {
                 return service.name == new_service.name;
             }))
        {
            services.push_back(new_service);

            slook::ServiceLookup::Response<String> res;
            res.service = new_service;
            send(res);
        }
    }

    template<typename Cb>
    void findServices(String const& name, Cb&& cb) {
        if(
          serviceCallbacks.end()
          == std::find_if(serviceCallbacks.begin(), serviceCallbacks.end(), [&](auto const& scb) {
                 return scb.first == name;
             }))
        {
            serviceCallbacks.emplace_back(name, ServiceCallback{std::forward<Cb>(cb)});
        }
        ServiceLookup::Request<String> req;
        req.serviceName = name;
        send(req);
    }

private:
    using packager = aglio::Packager<aglio::IPConfig>;
    SendFunction            sendFunction;
    Vector<Service<String>> services;

    Vector<std::pair<String, ServiceCallback>> serviceCallbacks;
    template<typename T>
    void send(T const& v) {
        Vector<std::byte> buffer;
        packager::pack(buffer, slook::CommandSet<String>{v});
        sendFunction(buffer);
    }

    static bool serviceNameMatches(std::string_view query, std::string_view name) {
        if(query == name) {
            return true;
        }

        if(query.ends_with("#")) {
            if(name.starts_with(query.substr(0, query.size() - 1))) {
                return true;
            }
        }
        return false;
    }

    void handle(slook::ServiceLookup::Request<String> const& v) {
        for(auto const& s : services) {
            if(serviceNameMatches(v.serviceName, s.name)) {
                slook::ServiceLookup::Response<String> res;
                res.service = s;
                send(res);
            }
        }
    }

    void handle(slook::ServiceLookup::Response<String> const& v) {
        auto const it
          = std::find_if(serviceCallbacks.begin(), serviceCallbacks.end(), [&](auto const& scb) {
                return serviceNameMatches(scb.first, v.service.name);
            });
        if(it != serviceCallbacks.end()) {
            it->second(v.service);
        }
    }
};
}   // namespace slook


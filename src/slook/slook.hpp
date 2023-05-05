#pragma once
#include "slook_commands.hpp"

#include <aglio/packager.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <span>
#include <utility>

namespace slook {
template<
  template<typename, std::size_t>
  typename Vector,
  template<std::size_t>
  typename String,
  typename SendFunction,
  typename ServiceCallback,
  std::size_t MaxServices  = 2,
  std::size_t MaxCallbacks = 2>
struct Lookup {
    using Request    = slook::ServiceLookup::Request<String>;
    using Service    = slook::Service<String, Vector>;
    using Response   = slook::ServiceLookup::Response<String, Vector>;
    using CommandSet = slook::CommandSet<String, Vector>;

    template<typename SendFunction_>
    Lookup(SendFunction_&& sendFunction_)
      : sendFunction{std::forward<SendFunction_>(sendFunction_)} {}

    void messageCallback(slook::IPAddress const& address, std::span<std::byte const> data) {
        auto const p = packager::unpack<CommandSet>(data);

        if(p) {
            std::visit([&](auto const& v) { handle(address, v); }, *p);
        }
    }

    void addService(Service const& new_service) {
        auto const it  = std::find_if(services.begin(), services.end(), [&](auto const& service) {
            return std::string_view{service.name} == std::string_view{new_service.name};
        });
        bool       add = false;
        if(it == services.end()) {
            add = true;
        } else if(*it != new_service) {
            add = true;
            services.erase(it);
        }

        if(add) {
            services.push_back(new_service);

            Response res;
            res.service = new_service;
            send(std::nullopt, res);
        }
    }

    template<typename Cb>
    void findServices(std::string_view name, Cb&& cb) {
        if(
          serviceCallbacks.end()
          == std::find_if(serviceCallbacks.begin(), serviceCallbacks.end(), [&](auto const& scb) {
                 return scb.first == name;
             }))
        {
            serviceCallbacks.emplace_back(name, ServiceCallback{std::forward<Cb>(cb)});
        }
        Request req;
        req.serviceName = name;
        send(std::nullopt, req);
    }

private:
    using packager = aglio::Packager<aglio::IPConfig>;
    SendFunction                 sendFunction;
    Vector<Service, MaxServices> services;

    Vector<std::pair<String<slook::MaxNameSize>, ServiceCallback>, MaxCallbacks> serviceCallbacks;

    template<typename T>
    void send(std::optional<slook::IPAddress> const& address, T const& v) {
        Vector<std::byte, 1024> buffer;
        packager::pack(buffer, CommandSet{v});
        sendFunction(address, buffer);
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

    void handle(slook::IPAddress const& address, Request const& v) {
        for(auto const& s : services) {
            if(serviceNameMatches(v.serviceName, s.name)) {
                Response res;
                res.service = s;
                send(address, res);
            }
        }
    }

    void handle(slook::IPAddress const&, Response const& v) {
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


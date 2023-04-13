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
    using Request    = slook::ServiceLookup::Request<String>;
    using Service    = slook::Service<String, Vector>;
    using Response   = slook::ServiceLookup::Response<String, Vector>;
    using CommandSet = slook::CommandSet<String, Vector>;

    template<typename SendFunction_>
    Lookup(SendFunction_&& sendFunction_)
      : sendFunction{std::forward<SendFunction_>(sendFunction_)} {}

    void messageCallback(std::span<std::byte const> data) {
        auto const p = packager::unpack<CommandSet>(data);

        if(p) {
            std::visit([this](auto const& v) { handle(v); }, *p);
        }
    }

    void addService(Service const& new_service) {
        auto const it  = std::find_if(services.begin(), services.end(), [&](auto const& service) {
            return service.name == new_service.name;
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
        Request req;
        req.serviceName = name;
        send(req);
    }

private:
    using packager = aglio::Packager<aglio::IPConfig>;
    SendFunction    sendFunction;
    Vector<Service> services;

    Vector<std::pair<String, ServiceCallback>> serviceCallbacks;
    template<typename T>
    void send(T const& v) {
        Vector<std::byte> buffer;
        packager::pack(buffer, CommandSet{v});
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

    void handle(Request const& v) {
        for(auto const& s : services) {
            if(serviceNameMatches(v.serviceName, s.name)) {
                Response res;
                res.service = s;
                send(res);
            }
        }
    }

    void handle(Response const& v) {
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


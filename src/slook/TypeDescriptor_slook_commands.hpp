namespace aglio {
template<template<std::size_t> class String>
struct TypeDescriptorGen<slook::ServiceLookup::Request<String>>
  : MemberList<
      MemberDescriptor<&slook::ServiceLookup::Request<String>::serviceName, "serviceName">> {
    static constexpr std::string_view Name{"Request"};
    static constexpr std::string_view QualifiedName{"slook::ServiceLookup::Request"};
};
}   // namespace aglio
namespace aglio {
template<template<std::size_t> class String, template<typename, std::size_t> class Vector>
struct TypeDescriptorGen<slook::ServiceLookup::Response<String, Vector>>
  : MemberList<
      MemberDescriptor<&slook::ServiceLookup::Response<String, Vector>::service, "service">> {
    static constexpr std::string_view Name{"Response"};
    static constexpr std::string_view QualifiedName{"slook::ServiceLookup::Response"};
};
}   // namespace aglio

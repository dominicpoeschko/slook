namespace aglio {
template<typename String>
struct TypeDescriptorGen<slook::ServiceLookup::Request<String>>
  : MemberList<
      MemberDescriptor<&slook::ServiceLookup::Request<String>::serviceName, "serviceName">> {
    static constexpr std::string_view Name{"Request"};
    static constexpr std::string_view QualifiedName{"slook::ServiceLookup::Request"};
};
}   // namespace aglio
namespace aglio {
template<typename String>
struct TypeDescriptorGen<slook::ServiceLookup::Response<String>>
  : MemberList<MemberDescriptor<&slook::ServiceLookup::Response<String>::service, "service">> {
    static constexpr std::string_view Name{"Response"};
    static constexpr std::string_view QualifiedName{"slook::ServiceLookup::Response"};
};
}   // namespace aglio

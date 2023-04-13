namespace aglio {
template<typename String, template<typename> class Vector>
struct TypeDescriptorGen<slook::Service<String, Vector>>
  : MemberList<
      MemberDescriptor<&slook::Service<String, Vector>::name, "name">,
      MemberDescriptor<&slook::Service<String, Vector>::addresses, "addresses">,
      MemberDescriptor<&slook::Service<String, Vector>::port, "port">,
      MemberDescriptor<&slook::Service<String, Vector>::protocol, "protocol">> {
    static constexpr std::string_view Name{"Service"};
    static constexpr std::string_view QualifiedName{"slook::Service"};
};
}   // namespace aglio

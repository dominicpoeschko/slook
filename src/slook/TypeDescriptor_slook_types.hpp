namespace aglio {
template<typename String>
struct TypeDescriptorGen<slook::Service<String>>
  : MemberList<
      MemberDescriptor<&slook::Service<String>::name, "name">,
      MemberDescriptor<&slook::Service<String>::address, "address">,
      MemberDescriptor<&slook::Service<String>::port, "port">,
      MemberDescriptor<&slook::Service<String>::protocol, "protocol">> {
    static constexpr std::string_view Name{"Service"};
    static constexpr std::string_view QualifiedName{"slook::Service"};
};
}   // namespace aglio

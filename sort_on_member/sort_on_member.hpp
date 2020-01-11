#ifndef SORT_ON_MEMBER_HPP
#define SORT_ON_MEMBER_HPP

#include <functional>

template<class Struct, class Member_t, class Cmp = std::less<Member_t>>
struct Comparator {
  private:
    const Member_t Struct::*member;
    const Cmp cmp{};

  public:
    Comparator(Member_t Struct::*member_, const Cmp &cmp_ = {}) : member(member_), cmp(cmp_) {}

    bool operator()(const Struct &s1, const Struct &s2) const {
        return cmp(s1.*member, s2.*member);
    }

    // We can also compare directly to the type of the member
    // It is templated to enable transparent comparisons (i.e. string_view to string
    // without conversions)

    template<class T>
    bool operator()(const Struct &s, const T &mem) const {
        return cmp(s.*member, mem);
    }

    template<class T>
    bool operator()(const T &mem, const Struct &s) const {
        return cmp(mem, s.*member);
    }

    using is_transparent = void;
};

template<class Struct, class Member_t>
Comparator(Member_t Struct::*)->Comparator<Struct, Member_t>;

template<class Struct, class Member_t, class Cmp>
Comparator(Member_t Struct::*, const Cmp &)->Comparator<Struct, Member_t, Cmp>;

#endif   // SORT_ON_MEMBER_HPP

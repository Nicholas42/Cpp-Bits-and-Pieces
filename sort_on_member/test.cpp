#include <vector>
#include <set>
#include <string>
#include <string_view>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "sort_on_member.hpp"

using namespace std::literals;

class Private
{
    int secret;

  public:
    Private(int s) : secret(s) {}

    // Exposing the pointer to private is not great but a possibility (at least the
    // pointer is private in Comparator). I would feel better to friend the Comparator,
    // but partial specializations are not allowed in friend declarations, so I would not
    // be able to template on the Compare class.
    template<class Compare>
    static constexpr Comparator<Private, int, Compare> comparator(Compare c = Compare{})
    {
        return Comparator<Private, int, Compare>{&Private::secret, c};
    }
};

struct Student
{
    std::string name;
    int age;
};

int main()
{
    Comparator cint{&Student::age};
    Comparator cstring{&Student::name, std::greater<>{}};
    auto cpriv = Private::comparator<std::less<>>();

    assert(cpriv(Private{1}, Private{2}));

    std::vector<Student> vec = {{"Paul", 43}, {"John", 42}};

    std::sort(begin(vec), end(vec), cint);
    assert(vec.front().age < vec.back().age);
    for (auto &s: vec)
    {
        std::cout << s.age << "\n";
    }

    std::sort(begin(vec), end(vec), cstring);
    assert(vec.front().name > vec.back().name);
    for (auto &s: vec)
    {
        std::cout << s.name << "\n";
    }

    std::set set{cbegin(vec), cend(vec), cstring};
    auto i = set.find("Paul"sv);
    assert(i != set.end() && i->age == 43);

    return 0;
}

#ifndef ZIP_HPP
#define ZIP_HPP

#include <tuple>
#include <type_traits>
#include <limits>
#include <cstddef>   // std::size_t

namespace zip_iter {
namespace {
// We will stop when any iterator equals its corresponding end, hence the |
template<class... Iters, std::size_t N = sizeof...(Iters)>
constexpr auto comp(const std::tuple<Iters...> &left, const std::tuple<Iters...> &right,
                    std::integral_constant<std::size_t, N> = {}) -> bool {
    if constexpr (N == 0) {
        return false;
    } else {
        return std::get<N - 1>(left) == std::get<N - 1>(right) ||
               comp(left, right, std::integral_constant<std::size_t, N - 1>{});
    }
}

// Poor man's concept...
template<class Container>
using is_container_t =
    std::void_t<decltype((std::begin(std::declval<Container>()), std::end(std::declval<Container>())))>;
}   // namespace

template<class... Iters>
struct zip {
    constexpr zip(std::pair<Iters, Iters>... iters) : m_begin{iters.first...}, m_end{iters.second...} {}

    template<class... containers, class = std::void_t<is_container_t<containers>...>>
    constexpr zip(containers &... cs) : zip(std::make_pair(std::begin(cs), std::end(cs))...) {}

    struct zip_iterator {
        constexpr zip_iterator(Iters... its) : m_iters(its...) {}

        constexpr auto operator++() -> zip_iterator & {
            std::apply([](auto &... it) { std::make_tuple(++it...); }, m_iters);
            return *this;
        }

        constexpr auto operator++(int) -> zip_iterator & {
            return std::apply([](auto &... it) { std::make_tuple(it++...); }, m_iters);
        }

        constexpr auto operator*() -> std::tuple<decltype(*std::declval<Iters>()) &...> {
            return std::apply([](auto &... it) { return std::tie(*it...); }, m_iters);
        }

        // If we are sure, that all iterators have the same "length", we can speed this up by just
        // returning std::get<0>(m_iters) == std::get<0>(end.m_iters)
        friend constexpr auto operator==(const zip_iterator &it1, const zip_iterator &it2) -> bool {
            return comp<Iters...>(it1.m_iters, it2.m_iters);
        }

        friend constexpr auto operator!=(const zip_iterator &it1, const zip_iterator &it2) -> bool {
            return !(it1 == it2);
        }

      private:
        std::tuple<Iters...> m_iters;
    };

    constexpr auto begin() const -> zip_iterator {
        return m_begin;
    }

    constexpr auto end() const -> zip_iterator {
        return m_end;
    }

  private:
    zip_iterator m_begin;
    const zip_iterator m_end;
};

template<class... containers, class = std::void_t<is_container_t<containers>...>>
zip(containers &... cs)->zip<decltype(cs.begin())...>;

// Class that helps with having the index in a ranged-for-loop
template<class IntegerT = std::size_t, class Limits = std::numeric_limits<IntegerT>>
struct iota {
    constexpr iota() = default;
    constexpr iota(IntegerT i) : start{i} {}

    struct iota_iter {
        constexpr iota_iter(IntegerT i) : cur_index{i} {}

        constexpr auto operator*() const -> const IntegerT & {
            return cur_index;
        }

        constexpr auto operator*() -> IntegerT & {
            return cur_index;
        }

        constexpr auto operator++() -> iota_iter & {
            ++cur_index;
            return *this;
        }

        constexpr auto operator++(int) -> iota_iter {
            return iota_iter{cur_index++};
        }

        friend constexpr auto operator==(const iota_iter &it1, const iota_iter &it2) -> bool {
            return it1.cur_index == it2.cur_index;
        }

        friend constexpr auto operator!=(const iota_iter &it1, const iota_iter &it2) -> bool {
            return !(it1 == it2);
        }

      private:
        IntegerT cur_index;
    };

    constexpr auto begin() const -> iota_iter {
        return iota_iter{start};
    }

    constexpr auto end() const -> iota_iter {
        return iota_iter{Limits::max()};
    }

  private:
    IntegerT start{};
};

template<class Container, class IntegerT = std::size_t>
constexpr auto with_index(Container &c, IntegerT start = {})
    -> zip<typename iota<IntegerT>::iota_iter, decltype(std::begin(c))> {
    iota tmp{start};   // Does have to outlive the function
    return zip{tmp, c};
}

}   // namespace zip_iter

#endif   // ZIP_HPP

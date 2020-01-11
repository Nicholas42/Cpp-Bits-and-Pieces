#ifndef OPTIONAL_INSERT_HPP
#define OPTIONAL_INSERT_HPP

#include <iterator>
#include <optional>

template<class Container>
class optional_insert_iterator {
  public:
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using container_type = Container;

  private:
    using iterator_t = typename container_type::iterator;
    using underlying_val_t = typename container_type::value_type;

  public:
    constexpr optional_insert_iterator(Container &c, iterator_t it) :
            container(addressof(c)), iter(it) {}

    constexpr optional_insert_iterator &operator=(const std::optional<underlying_val_t> &opt) {
        if (opt) {
            iter = container->insert(iter, *opt);
            ++iter;
        }
        return *this;
    }

    constexpr optional_insert_iterator &operator=(std::optional<underlying_val_t> &&opt) {
        if (opt) {
            iter = container->insert(iter, *std::move(opt));
            ++iter;
        }
        return *this;
    }

    constexpr optional_insert_iterator &operator*() {
        return *this;
    }

    constexpr optional_insert_iterator &operator++() {
        return *this;
    }

    constexpr optional_insert_iterator &operator++(int) {
        return *this;
    }

  private:
    Container *container;
    iterator_t iter;
};

template<class Container>
optional_insert_iterator(Container &, typename Container::iterator)
    ->optional_insert_iterator<Container>;

template<class Container>
optional_insert_iterator<Container> optional_inserter(Container &c, typename Container::iterator it) {
    return optional_insert_iterator(c, it);
}

#endif   // OPTIONAL_INSERT_HPP

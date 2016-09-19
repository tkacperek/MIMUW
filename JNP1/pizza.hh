/*
 *  JNP: zadanie 4
 *  Autorzy: ad359226, tk334578
 *
 * */
#ifndef PIZZA_HH
#define PIZZA_HH

#include <array>
#include <type_traits>

// Szablon struktury reprezentującej pizzerię.
template <typename... Kinds>
struct Pizzeria {
  private:
    static constexpr size_t N = sizeof...(Kinds);

    // Szablon struktury reprezentującej pizzę.
    // Argumenty to lista liczb poszczególnych rodzajów kawałków,
    // zgodna z kolejnością podaną przy tworzeniu pizzerii.
    template <size_t... counts>
    struct Pizza;

    template <typename Pizza1, typename Pizza2>
    friend struct best_mix;

    // Szablon Struktury, która pomaga best_mix'owi w mieszaniu.
    template <typename Pizza1, typename Pizza2>
    struct Mixer;

  public:
    template <typename Kind>
    struct make_pizza {
    
        // Sprawdzenie, czy podany rodzaj pizzy dokładnie raz wystąpił
        // na liście parametrów przy tworzeniu pizzerii.
        static_assert(((std::is_same<Kind, Kinds>() ? 1 : 0) + ...) == 1,
                "Either we don't serve this kind of pizza "
                "or the pizzeria has been opened with duplicate kinds!");
        using type = Pizza<(std::is_same<Kind, Kinds>() ? 8 : 0)...>;
    };
};

template <typename... Kinds>
template <size_t... counts>
struct Pizzeria<Kinds...>::Pizza {
    template <typename Type>
    static constexpr size_t count() {
        return ((std::is_same<Type, Kinds>() ? counts: 0) + ...);
    }

    static constexpr std::array<size_t, N> as_array() {
        return {{counts...}};
    }

    using sliced_type = Pizza<(2 * counts)...>;

  private:
    using MyPizzeria = Pizzeria<Kinds...>;

    template <typename Pizza1, typename Pizza2>
    friend struct best_mix;
};

template <typename... Kinds>
template <typename Pizza1, typename Pizza2>
struct Pizzeria<Kinds...>::Mixer {
    // Szablon pomocniczy dla HasYumm.
    template <typename T> using VoidTemplate = void;

    // Szablon do testowania, czy typ ma yumminess.
    template <typename T, typename = void>
    struct HasYumm : std::false_type {};

    template <typename T>
    struct HasYumm<T, VoidTemplate<decltype(T::yumminess(0))>> : std::true_type
    {};

    // Oblicza, ile kawałków danego typu będzie w mieszanej pizzy.
    template <typename T>
    static constexpr size_t countMix() {
        static_assert(HasYumm<T>::value,
                "You cannot measure yumminess of this kind!");
        static_assert(std::is_arithmetic<decltype(T::yumminess(0))>::value,
                "Yumminess is not arithmetic!");
        static_assert((T::yumminess(0) == decltype(T::yumminess(0))(0)),
                "No pizza has not taste!");

        size_t bestCount = 0,
               countLimit = Pizza1:: template count<T>()
                   + Pizza2:: template count<T>();
        decltype(T::yumminess(0)) currentYum = 0, bestYum = 0;

        for (size_t count = 0; count <= countLimit; ++count) {
            currentYum = T::yumminess(count);
            if (currentYum > bestYum)
                bestYum = currentYum, bestCount = count;
        }
        return bestCount;
    }
    using type = Pizza<(countMix<Kinds>())...>;
};


template <typename Pizza1, typename Pizza2>
struct best_mix {
    static_assert(std::is_same<
            typename Pizza1::MyPizzeria, typename Pizza2::MyPizzeria >(),
                  "You can't combine pizzas from different pizzerias!");
    using type =
        typename Pizza1::MyPizzeria:: template Mixer<Pizza1, Pizza2>::type;
};

#endif /* PIZZA_HH */

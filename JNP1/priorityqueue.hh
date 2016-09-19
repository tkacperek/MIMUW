//----------------------------------------------------------------
// JNP: Zadanie 5
// Autorzy: Tomasz Knopik, Tomasz Kacperek
//----------------------------------------------------------------

#ifndef PRIORITY_QUEUE_HH
#define PRIORITY_QUEUE_HH

#include <utility>
#include <memory>
#include <set>
#include <exception>
#include <cstdint>
#include <algorithm>

// wyjątek: pusta kolejka
class PriorityQueueEmptyException : public std::exception
{
  public:
    virtual const char* what() const throw()
    {
        return "PriorityQueueEmptyException";
    }
};

// wyjątek: nie znaleziono pary
class PriorityQueueNotFoundException : public std::exception
{
  public:
    virtual const char* what() const throw()
    {
        return "PriorityQueueNotFoundException";
    }
};


// deklaracje poprzedzające

template<typename K, typename V>
class PriorityQueue;
/* Założenia dla typu K / V
    -ma copy-constructor
    -nie rzuca wyjątkami w destrukorze
    -ma operator< z silną gwarancją
 */

template <typename K, typename V>
bool operator==(const PriorityQueue<K, V>& a, const PriorityQueue<K, V>& b);

template <typename K, typename V>
bool operator< (const PriorityQueue<K, V>& a, const PriorityQueue<K, V>& b);

// definicja klasy kolejki
template<typename K, typename V>
class PriorityQueue
{
    // definicje pomocnicze
    using pair_KV = std::pair<K, V>;
    using shared_ptr_KV = std::shared_ptr<pair_KV>;

    // uniwersalny komparator
    // parametr First określa,
    // który element pary jest porównywany jako pierwszy
    template <int8_t First>
    struct shared_ptr_KV_cmp
    {
        // możliwe wyjątki w: K/V :: operator<()
        bool operator() (const shared_ptr_KV& lhs, const shared_ptr_KV& rhs)
        const
        {
            if (std::get<First>(*lhs) < std::get<First>(*rhs))
                return true;
            else if (std::get<First>(*rhs) < std::get<First>(*lhs))
                return false;
            else
                return
                    std::get<(First + 1) % 2>(*lhs)
                    < std::get<(First + 1) % 2>(*rhs);
        }
    };

    // zbiór wskaźników posortowany po kluczach
    std::multiset<shared_ptr_KV, shared_ptr_KV_cmp<0> > key_set;
    // zbiór wskaźników posortowany po wartościach
    std::multiset<shared_ptr_KV, shared_ptr_KV_cmp<1> > value_set;

    // wyrzutnia wyjątków pustej kolejki
    void throw_if_empty() const
    {
        if (empty())
            throw PriorityQueueEmptyException();
    }

  public:

    // publiczne definicje typów
    using size_type = typename decltype(key_set)::size_type;
    using key_type = K;
    using value_type = V;

    // konstruktor bezparametrowy tworzący pustą kolejkę [O(1)]
    // gwarancja: silna (po multiset)
    PriorityQueue() {}

    // konstruktor kopiujący [O(queue.size())]
    // gwarancja: silna (po multiset, shared_ptr ma no-throw)
    PriorityQueue(const PriorityQueue<K, V>& queue) :
    key_set(queue.key_set), value_set(queue.value_set) {}

    // konstruktor przenoszący [O(1)]
    // gwarancja: silna (po multiset)
    PriorityQueue(PriorityQueue<K, V>&& queue) :
    key_set(std::move(queue.key_set)), value_set(std::move(queue.value_set)) {}

    // operator przypisania [O(queue.size()) lub O(1)]
    // gwarancja: silna (konstruktor PriorityQueue, swap jest no-throw)
    PriorityQueue<K, V>& operator=(PriorityQueue<K, V> queue);

    // zaprzyjaźnione komparatory

    // [O(size())]
    // gwarancja: silna (bo const, ale może rzucić coś z K/V :: operator==())
    friend bool
    operator== <>(const PriorityQueue<K, V>& a, const PriorityQueue<K, V>& b);

    // [O(size())]
    // gwarancja: silna (bo const, ale może rzucić coś z K/V :: operator<())
    friend bool
    operator< <>(const PriorityQueue<K, V>& a, const PriorityQueue<K, V>& b);

    // metoda zwracająca true wtw, gdy kolejka jest pusta [O(1)]
    // gwarancja: no-throw (po multiset)
    bool empty() const
    { return key_set.empty(); }

    // metoda zwracająca liczbę par w kolejce [O(1)]
    // gwarancja: no-throw (po multiset)
    size_type size() const
    { return key_set.size(); }

    // metoda wstawiająca do kolejki [O(log size())]
    // gwarancja: silna (odwraca częściowe zmiany)
    void insert(const K& key, const V& value);

    // metoda zwracająca najmniejszą wartość w kolejce [O(1)]
    // gwarancja: silna (const)
    const V& minValue() const
    {
        throw_if_empty();
        return std::get<1>(**value_set.begin());
    }

    // metoda zwracająca największą wartość w kolejce [O(1)]
    // gwarancja: silna (const)
    const V& maxValue() const
    {
        throw_if_empty();
        return std::get<1> (**value_set.rbegin());
    }

    // metoda zwracająca klucz o przypisanej najmniejszej wartości [O(1)]
    // gwarancja: silna (const)
    const K& minKey() const
    {
        throw_if_empty();
        return std::get<0> (**value_set.begin());
    }

    // metoda zwracająca klucz o przypisanej największej wartości [O(1)]
    // gwarancja: silna (const)
    const K& maxKey() const
    {
        throw_if_empty();
        return std::get<0> (**value_set.rbegin());
    }

    // metoda usuwająca parę o najmniejszej wartości [O(log size())]
    // gwarancja: silna (multiset::erase(iterator) jest no-throw)
    void deleteMin();

    // metoda usuwająca parę o największej wartości [O(log size())]
    // gwarancja: silna (multiset::erase(iterator) jest no-throw)
    void deleteMax();

    // metoda zmieniająca wartość przypisaną kluczowi na nową [O(log size())]
    // gwarancja: silna (silny insert, no-throw erase)
    void changeValue(const K& key, const V& value);

    // metoda scalająca zawartość kolejki z podaną kolejką queue
    // [O(size() + queue.size() * log (queue.size() + size()))]
    // gwarancja: silna (wszystkie istotne zmiany są no-throw)
    void merge(PriorityQueue<K, V>& queue);

    // metoda zamieniającą zawartość kolejki z podaną kolejką [O(1)]
    // gwarancja: no-throw (po multiset)
    void swap(PriorityQueue<K, V>& queue)
    {
        if (this != &queue)
        {
            queue.key_set.swap(key_set);
            queue.value_set.swap(value_set);
        }
    }
};

template <typename X>
bool crafty_cmp(const X& lhs, const X& rhs)
{
    return !(lhs < rhs || rhs < lhs);
}

template <typename K, typename V>
PriorityQueue<K, V>& PriorityQueue<K, V>::operator=(PriorityQueue<K, V> queue)
{
    queue.swap(*this);
    return *this;
}

// operacje find, insert [O(log size())]
// operacja erase [O(1)]
template <typename K, typename V>
void PriorityQueue<K, V>::insert(const K& key, const V& value)
{
    auto new_pair_ptr = std::make_shared<pair_KV>(key, value);
    auto found_ptr = key_set.find(new_pair_ptr);

    // czy taki element jest w kolejce
    if (found_ptr != key_set.end())
    {
        auto inserted_key = key_set.insert(*found_ptr);
        try
        {
            value_set.insert(*found_ptr);
        } catch (...)
        {
            // odwracanie zmian
            key_set.erase(inserted_key);
            throw;
        }
    } else
    {
        auto inserted_key = key_set.insert(new_pair_ptr);
        try
        {
            value_set.insert(new_pair_ptr);
        } catch (...)
        {
            // odwracanie zmian
            key_set.erase(inserted_key);
            throw;
        }
    }
}

// operacja find [O(log size())]
// operacja erase [O(1)]
template <typename K, typename V>
void PriorityQueue<K, V>::deleteMin()
{
    if (key_set.empty())
        return;

    auto value_iter = value_set.begin();
    auto key_iter =
        key_set.find(std::make_shared<pair_KV>(minKey(), minValue()));

    value_set.erase(value_iter);
    key_set.erase(key_iter);
}

// operacja find [O(log size())]
// operacja erase [O(1)]
template <typename K, typename V>
void PriorityQueue<K, V>::deleteMax()
{
    if (key_set.empty())
        return;

    auto value_iter = --value_set.rbegin().base();
    auto key_iter =
        key_set.find(std::make_shared<pair_KV>(maxKey(), maxValue()));

    value_set.erase(value_iter);
    key_set.erase(key_iter);
}

// operacje find, insert, lower_bound [O(log size())]
// operacja erase [O(1)]
template <typename K, typename V>
void PriorityQueue<K, V>::changeValue(const K& key, const V& value)
{
    auto key_iter =
        key_set.lower_bound(std::make_shared<pair_KV>(key, minValue()));

    // czy istnieje element o zadanym kluczu
    if (key_iter != key_set.end())
    {
        if (crafty_cmp(std::get<0> (**key_iter), key))
        {
            auto value_iter = value_set.find(
                std::make_shared<pair_KV>(key, std::get<1> (**key_iter))
            );

            insert(key, value);

            key_set.erase(key_iter);
            value_set.erase(value_iter);
        } else
            throw PriorityQueueNotFoundException();
    } else
        throw PriorityQueueNotFoundException();
}


template <typename K, typename V>
void PriorityQueue<K, V>::merge(PriorityQueue<K, V>& queue)
{
    if (this != &queue)
    {
        // [O(size()]
        PriorityQueue tmp (*this);
        // + [queue.size()]
        for (auto it : queue.key_set)
            // * [log (queue.size() + size())]
            tmp.insert(std::get<0> (*it), std::get<1> (*it));

        // ( + [O(queue.size())] )
        // gwarancja no-throw
        queue.key_set.clear();
        queue.value_set.clear();

        // ( + [O(1)] )
        swap(tmp);
    }
}

// globalny swap
// gwarancja: no-throw (lokalny swap)
template <typename K, typename V>
void swap(PriorityQueue<K, V>& a, PriorityQueue<K, V>& b)
{
    a.swap(b);
}

// operatory porównania
// gwarancja: silna (po operator==(), operator<())

template <typename K, typename V>
bool operator==(const PriorityQueue<K, V>& a, const PriorityQueue<K, V>& b)
{
    if (&a == &b)
        return true;
    else if (a.size() != b.size())
        return false;
    else
        return std::equal(a.key_set.begin(), a.key_set.end(),
                          b.key_set.begin(),
                          [](const std::shared_ptr<std::pair<K, V> >& a,
                             const std::shared_ptr<std::pair<K, V> >& b)
                          { return crafty_cmp(std::get<0>(*a), std::get<0>(*b))
                          && crafty_cmp(std::get<1>(*a), std::get<1>(*b)); } );
}

template <typename K, typename V>
bool operator!=(const PriorityQueue<K, V>& a, const PriorityQueue<K, V>& b)
{
    return !(a == b);
}

template <typename K, typename V>
bool operator<(const PriorityQueue<K, V>& a, const PriorityQueue<K, V>& b)
{
    if (&a == &b)
        return false;
    else
        return std::lexicographical_compare(
                                a.key_set.begin(), a.key_set.end(),
                                b.key_set.begin(), b.key_set.end(),
                                [](const std::shared_ptr<std::pair<K, V> >& a,
                                const std::shared_ptr<std::pair<K, V> >& b)
                                { return *a < *b; });
}

template <typename K, typename V>
bool operator>(const PriorityQueue<K, V>& a, const PriorityQueue<K, V>& b) {
    return !(a < b || a == b);
}

template <typename K, typename V>
bool operator<=(const PriorityQueue<K, V>& a, const PriorityQueue<K, V>& b) {
    return !(a > b);
}

template <typename K, typename V>
bool operator>=(const PriorityQueue<K, V>& a, const PriorityQueue<K, V>& b) {
    return !(a < b);
}


#endif /* PRIORITY_QUEUE_HH */

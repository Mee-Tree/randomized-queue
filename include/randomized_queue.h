#pragma once

#include <random>

template <class T>
class randomized_queue {
    template <bool is_const>
    class random_iterator;

    using iterator = random_iterator<false>;
    using const_iterator = random_iterator<true>;

public:
    randomized_queue() = default;
    randomized_queue(randomized_queue &&) = default;

    auto begin()         { return iterator(m_data.begin(), size()); };
    auto end()           { return iterator(m_data.begin(), size(), true); };
    auto begin()   const { return const_iterator(m_data.begin(), size()); };
    auto end()     const { return const_iterator(m_data.begin(), size(), true); };
    auto cbegin()  const { return begin(); };
    auto cend()    const { return end(); };

    bool empty() const;
    std::size_t size() const;

    void enqueue(const T & item);
    void enqueue(T && item);
    T const & sample() const;
    T dequeue();

private:
    std::uniform_int_distribution<std::size_t> get_distribution();
    std::size_t get_random_index() const;

    inline static std::mt19937_64 generator = std::mt19937_64{std::random_device{}()};
    mutable std::uniform_int_distribution<std::size_t> distribution;
    std::vector<T> m_data;
};

#include "randomized_queue.tpp"
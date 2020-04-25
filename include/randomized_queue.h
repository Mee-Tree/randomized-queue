#pragma once

#include <vector>
#include <utility>
#include <random>
#include <algorithm>
#include <chrono>

template <class T>
class randomized_queue {
    template <bool is_const>
    class random_iterator {
        friend class randomized_queue;

        using begin_type = std::conditional_t<is_const,
                typename std::vector<T>::const_iterator,
                typename std::vector<T>::iterator>;

        random_iterator(begin_type begin, std::size_t size, bool is_end = false)
                : m_begin(begin), m_current(is_end ? size : 0) {
            m_permutation.resize(size);
            generate();
        }

        void generate() {
            std::iota(m_permutation.begin(), m_permutation.end(), 0);
            std::shuffle(m_permutation.begin(), m_permutation.end(), get_random_generator());
            m_permutation.push_back(m_permutation.size());
        }

    public:
        using value_type = std::conditional_t<is_const, std::add_const_t<T>, T>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using pointer = value_type *;
        using reference = value_type &;

        template <bool v>
        random_iterator(const random_iterator<v> & other)
                : m_begin(other.m_begin)
                , m_current(other.m_current)
                , m_permutation(other.m_permutation) {}

        reference operator * () const {
            return *(m_begin + m_permutation[m_current]);
        }

        pointer operator -> () const {
            return (m_begin + m_permutation[m_current]).operator -> ();
        }

        random_iterator & operator ++ () {
            ++m_current;
            return *this;
        }

        random_iterator operator ++ (int) {
            auto tmp(*this);
            operator ++ ();
            return tmp;
        }

        bool operator == (const random_iterator & other) const {
            return (m_begin + m_permutation[m_current]) == (other.m_begin + other.m_permutation[other.m_current]);
        }
        bool operator != (const random_iterator & other) const {
            return !(operator == (other));
        }

        ~random_iterator() = default;

    private:
        begin_type m_begin;
        std::size_t m_current;
        std::vector<std::size_t> m_permutation;
    };

    using iterator = random_iterator<false>;
    using const_iterator = random_iterator<true>;

public:
    randomized_queue() = default;
    randomized_queue(randomized_queue && other)
            : m_data(other.m_data) {}

    auto begin()         { return iterator(m_data.begin(), size()); };
    auto end()           { return iterator(m_data.begin(), size(), true); };
    auto begin()   const { return const_iterator(m_data.begin(), size()); };
    auto end()     const { return const_iterator(m_data.begin(), size(), true); };
    auto cbegin()  const { return begin(); };
    auto cend()    const { return end(); };

    bool empty() const {
        return m_data.empty();
    }

    std::size_t size() const {
        return m_data.size();
    }

    void enqueue(const T & item) {
        m_data.push_back(item);
        distribution = get_distribution();
    }

    void enqueue(T && item) {
        m_data.push_back(std::move(item));
        distribution = get_distribution();
    }

    T const & sample() const {
        std::size_t ind = (std::mt19937_64(
                static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()))()) % size();
        return m_data[ind];
    }

    T dequeue() {
        std::size_t ind = get_random_index();
        std::swap(m_data[ind], m_data.back());
        auto tmp = std::move(m_data.back());
        m_data.pop_back();
        distribution = get_distribution();
        return tmp;
    };

    ~randomized_queue() = default;

private:
    std::uniform_int_distribution<std::size_t> get_distribution() {
        return std::uniform_int_distribution<std::size_t>(0, size() - 1);
    }

    std::size_t get_random_index() {
        return distribution(generator);
    }

    static std::mt19937_64 get_random_generator() {
        std::random_device random_device;
        return std::mt19937_64(random_device());
    }

    std::mt19937_64 generator = get_random_generator();
    std::uniform_int_distribution<std::size_t> distribution;

    std::vector<T> m_data;
};

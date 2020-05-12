#pragma once

/* -------RandomizedQueue------- */

template <class T>
randomized_queue<T>::randomized_queue(randomized_queue<T> && other)
        : m_data(std::move(other.m_data)) {}

template <class T>
bool randomized_queue<T>::empty() const {
    return m_data.empty();
}

template <class T>
std::size_t randomized_queue<T>::size() const {
    return m_data.size();
}

template <class T>
void randomized_queue<T>::enqueue(const T & item) {
    m_data.push_back(item);
    distribution = get_distribution();
}

template <class T>
void randomized_queue<T>::enqueue(T && item) {
    m_data.emplace_back(std::move(item));
    distribution = get_distribution();
}

template <class T>
T const & randomized_queue<T>::sample() const {
    std::size_t ind = const_cast<randomized_queue *>(this)->get_random_index();
    return m_data[ind];
}

template <class T>
T randomized_queue<T>::dequeue() {
    std::size_t ind = get_random_index();
    std::swap(m_data[ind], m_data.back());
    auto tmp = std::move(m_data.back());
    m_data.pop_back();
    distribution = get_distribution();
    return tmp;
}

template <class T>
std::uniform_int_distribution<std::size_t> randomized_queue<T>::get_distribution() {
    return std::uniform_int_distribution<std::size_t>(0, size() - 1);
}

template <class T>
std::size_t randomized_queue<T>::get_random_index() {
    return distribution(generator);
}

/* -------RandomIterator------- */

template <class T>
template <bool is_const>
class randomized_queue<T>::random_iterator {
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
        return &(m_begin + m_permutation[m_current]);
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

private:
    begin_type m_begin;
    std::size_t m_current;
    std::vector<std::size_t> m_permutation;
};
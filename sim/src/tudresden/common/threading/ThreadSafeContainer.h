#ifndef THREADSAFECONTAINER_H
#define THREADSAFECONTAINER_H

#include <list>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

///
/// Thread-Safe wrapper for a std::unordered_set.
///
template <class T> class ThreadSafeUnorderedSet {
  public:
    ThreadSafeUnorderedSet() {}
    ~ThreadSafeUnorderedSet() = default;

    void FromExistingSet(std::unordered_set<T> input) {
        std::lock_guard<std::mutex> l(mtx);
        data.clear();
        data = input;
    }

    ///
    /// \brief Inserts an element into the set.
    /// \param element
    ///
    void InsertElement(T element) {
        std::lock_guard<std::mutex> l(mtx);
        data.insert(element);
    }

    ///
    /// \brief Checks whether a certain element is present in the list
    /// \param element
    /// \return
    ///
    bool ContainsElement(T element) {
        std::lock_guard<std::mutex> l(mtx);
        return data.find(element) != data.end();
    }

    ///
    /// \brief Retrieves the internally stored std::unordered_set. The returned container is no longer thread safe.
    /// \return
    ///
    std::unordered_set<T> RetrieveData() {
        std::lock_guard<std::mutex> l(mtx);
        return data;
    }

  private:
    std::unordered_set<T> data;
    mutable std::mutex mtx;
};

///
/// Thread-Safe wrapper for a std::unordered_map.
///
template <class T, class C> class ThreadSafeUnorderedMap {
  public:
    ThreadSafeUnorderedMap() {}
    ~ThreadSafeUnorderedMap() = default;

    ///
    /// \brief Inserts a key value pair into the map.
    /// \param element
    ///
    void InsertElement(T key, C value) {
        std::lock_guard<std::mutex> l(mtx);
        data.insert(std::make_pair(key, value));
    }

    ///
    /// \brief Checks whether a certain key is present in the map
    /// \param element
    /// \return
    ///
    bool ContainsKey(T element) {
        std::lock_guard<std::mutex> l(mtx);
        return data.find(element) != data.end();
    }

    ///
    /// \brief Retrieves the internally stored std::unordered_map. The returned container is no longer thread safe.
    /// \return
    ///
    std::unordered_map<T, C> RetrieveData() {
        std::lock_guard<std::mutex> l(mtx);
        return data;
    }

    ///
    /// \brief Clears the internally stored map
    ///
    void Clear() {
        std::lock_guard<std::mutex> l(mtx);
        data.clear();
    }

  private:
    std::unordered_map<T, C> data;
    mutable std::mutex mtx;
};

///
/// Thread-Safe wrapper for a std::list.
///
template <class T> class ThreadSafeList {
  public:
    ThreadSafeList() {}
    ~ThreadSafeList() = default;

    ///
    /// \brief Inserts an element into the list.
    /// \param element
    ///
    void InsertElement(T element) {
        std::lock_guard<std::mutex> l(mtx);
        data.push_back(element);
    }

    ///
    /// \brief Checks whether a certain element is present in the list
    /// \param element
    /// \return
    ///
    bool ContainsKey(T element) {
        std::lock_guard<std::mutex> l(mtx);
        return data.find(element) != data.end();
    }

    ///
    /// \brief Retrieves the internally stored std::list. The returned container is no longer thread safe.
    /// \return
    ///
    std::list<T> RetrieveData() {
        std::lock_guard<std::mutex> l(mtx);
        return data;
    }

    ///
    /// \brief Clears the internally stored list
    ///
    void Clear() {
        std::lock_guard<std::mutex> l(mtx);
        data.clear();
    }

  private:
    std::list<T> data;
    mutable std::mutex mtx;
};

///
/// Thread-Safe wrapper for a std::vector.
///
template <class T> class ThreadSafeVector {
  public:
    ThreadSafeVector() {}
    ~ThreadSafeVector() = default;

    ///
    /// \brief Inserts an element into the vector.
    /// \param element
    ///
    void InsertElement(T element) {
        std::lock_guard<std::mutex> l(mtx);
        data.push_back(element);
    }

    ///
    /// \brief Checks whether a certain key is present in the vector
    /// \param element
    /// \return
    ///
    bool ContainsKey(T element) {
        std::lock_guard<std::mutex> l(mtx);
        return data.find(element) != data.end();
    }

    ///
    /// \brief Retrieves the internally stored std::vector. The returned container is no longer thread safe.
    /// \return
    ///
    std::vector<T> RetrieveData() {
        std::lock_guard<std::mutex> l(mtx);
        return data;
    }

    ///
    /// \brief Clears the internally stored vector
    ///
    void Clear() {
        std::lock_guard<std::mutex> l(mtx);
        data.clear();
    }

  private:
    std::vector<T> data;
    mutable std::mutex mtx;
};

#endif // THREADSAFECONTAINER_H

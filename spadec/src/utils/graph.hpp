#pragma once

#include <cstddef>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <concepts>
#include <utility>

template<class T, class Equal = std::equal_to<T>>
    requires std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T> && std::is_destructible_v<T>
class BasicEdge {
    using EdgeType = T;

    EdgeType m_origin;
    EdgeType m_destination;

  public:
    BasicEdge() = default;
    BasicEdge(const BasicEdge<EdgeType, Equal> &) = default;
    BasicEdge &operator=(const BasicEdge<EdgeType, Equal> &) = default;
    ~BasicEdge() = default;

    template<class U, class V>
    BasicEdge(const U &origin, const V &destination) : m_origin(origin), m_destination(destination) {}

    std::pair<EdgeType, EdgeType> endpoints() const {
        return {m_origin, m_destination};
    }

    std::pair<EdgeType &, EdgeType &> endpoints() {
        return {m_origin, m_destination};
    }

    const EdgeType &opposite(const EdgeType &v) const {
        return Equal{}(m_origin, v) ? m_destination : m_origin;
    }

    EdgeType &opposite(const EdgeType &v) {
        return Equal{}(m_origin, v) ? m_destination : m_origin;
    }

    const EdgeType &origin() const {
        return m_origin;
    }

    const EdgeType &destination() const {
        return m_destination;
    }
};

template<class EdgeType, class VertexType>
concept IsEdge = requires(const EdgeType &edge) {
    { edge.origin() } -> std::same_as<const VertexType &>;
    { edge.destination() } -> std::same_as<const VertexType &>;
} && requires(const EdgeType edge, const VertexType vertex) {
    { edge.endpoints() } -> std::same_as<std::pair<VertexType, VertexType>>;
    { edge.opposite(vertex) } -> std::same_as<const VertexType &>;
} && requires(EdgeType edge, VertexType vertex) {
    { edge.endpoints() } -> std::same_as<std::pair<VertexType &, VertexType &>>;
    { edge.opposite(vertex) } -> std::same_as<VertexType &>;
};

template<class T, class Equal = std::equal_to<T>, class EdgeType = BasicEdge<T, Equal>, class Hasher = std::hash<T>>
    requires std::is_default_constructible_v<T> && std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T> && std::is_destructible_v<T> &&
             IsEdge<EdgeType, T>
class DirectedGraph {
    class EdgeIterator {
      public:
        // Iterator tags
        using iterator = EdgeIterator;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const EdgeType;
        using pointer = value_type *;
        using reference = value_type &;

        EdgeIterator() = default;
        EdgeIterator(const EdgeIterator &other) = default;
        EdgeIterator &operator=(const EdgeIterator &other) = default;

        EdgeIterator(const typename std::unordered_map<T, EdgeType, Hasher, Equal>::const_iterator &begin) : it(begin) {}

        iterator &operator++() {
            ++it;
            return *this;
        }

        iterator &operator++(int) {
            it++;
            return *this;
        }

        bool operator==(const iterator &other) {
            return it == other.it;
        }

        bool operator!=(const iterator &other) {
            return it != other.it;
        }

        reference operator*() {
            return it->second;
        }

        pointer operator->() {
            return &it->second;
        }

      private:
        typename std::unordered_map<T, EdgeType, Hasher, Equal>::const_iterator it;
    };

    class VertexIterator {
      public:
        // Iterator tags
        using iterator = VertexIterator;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const T;
        using pointer = value_type *;
        using reference = value_type &;

        VertexIterator() = default;
        VertexIterator(const VertexIterator &other) = default;
        VertexIterator &operator=(const VertexIterator &other) = default;

        VertexIterator(const typename std::unordered_map<T, std::unordered_map<T, EdgeType, Hasher, Equal>, Hasher, Equal>::const_iterator &begin)
            : it(begin) {}

        iterator &operator++() {
            ++it;
            return *this;
        }

        iterator &operator++(int) {
            it++;
            return *this;
        }

        bool operator==(const iterator &other) {
            return it == other.it;
        }

        bool operator!=(const iterator &other) {
            return it != other.it;
        }

        reference operator*() {
            return it->first;
        }

        pointer operator->() {
            return &it->first;
        }

      private:
        typename std::unordered_map<T, std::unordered_map<T, EdgeType, Hasher, Equal>, Hasher, Equal>::const_iterator it;
    };

    template<class IType>
    class Iterable {
        IType m_begin, m_end;

      public:
        Iterable() = delete;
        Iterable(const Iterable &other) = default;
        Iterable &operator=(const Iterable &other) = default;
        ~Iterable() = default;

        Iterable(IType begin, IType end) : m_begin(begin), m_end(end) {}

        IType begin() {
            return m_begin;
        }

        IType end() {
            return m_end;
        }

        size_t size() const {
            return std::distance(m_begin, m_end);
        }
    };

  public:
    DirectedGraph() = default;
    DirectedGraph(const DirectedGraph &) = default;
    DirectedGraph &operator=(const DirectedGraph &) = default;
    ~DirectedGraph() = default;

    Iterable<EdgeIterator> edges(const T &vertex, bool out = true) const {
        return out ? Iterable<EdgeIterator>(EdgeIterator(outgoing.at(vertex).begin()), EdgeIterator(outgoing.at(vertex).end()))
                   : Iterable<EdgeIterator>(EdgeIterator(incoming.at(vertex).begin()), EdgeIterator(incoming.at(vertex).end()));
    }

    Iterable<VertexIterator> vertices() const {
        return Iterable<VertexIterator>(VertexIterator(outgoing.begin()), VertexIterator(outgoing.end()));
    }

    bool is_empty() const {
        return incoming.empty();    // or outgoing.empty() both are equivalent
    }

    constexpr bool is_directed() const {
        return true;
    }

    size_t vertex_size() const {
        return outgoing.size();
    }

    size_t edge_size() const {
        size_t result = 0;
        for (auto it = outgoing.begin(); it != outgoing.end(); it++) result += it->second.size();
        return result;
    }

    bool contains(const T &vertex) const {
        return outgoing.contains(vertex);
    }

    bool contains(const T &vertex_from, const T &vertex_to) const {
        if (outgoing.contains(vertex_from))
            if (const auto &m = outgoing[vertex_from]; m.contains(vertex_to))
                return true;
        return false;
    }

    std::optional<EdgeType> get_edge(const T &vertex_from, const T &vertex_to) const {
        if (outgoing.contains(vertex_from))
            if (auto &m = outgoing[vertex_from]; m.contains(vertex_to))
                return m[vertex_to];
        return std::nullopt;
    }

    std::optional<EdgeType &> get_edge(const T &vertex_from, const T &vertex_to) {
        if (outgoing.contains(vertex_from))
            if (auto &m = outgoing[vertex_from]; m.contains(vertex_to))
                return m[vertex_to];
        return std::nullopt;
    }

    size_t degree(const T &vertex, bool out = true) const {
        auto adjacent_map = out ? outgoing : incoming;
        if (adjacent_map.contains(vertex))
            return adjacent_map[vertex].size();
        return 0;
    }

    void insert_vertex(const T &vertex) {
        outgoing[vertex] = {};
        incoming[vertex] = {};
    }

    template<class... Args>
    void emplace_vertex(Args... args) {
        T vertex{std::forward<Args>(args)...};
        outgoing[vertex] = {};
        incoming[vertex] = {};
    }

    template<typename... Args>
    EdgeType insert_edge(const T &vertex_from, const T &vertex_to, Args... args) {
        EdgeType edge(vertex_from, vertex_to, std::forward<Args>(args)...);
        outgoing[vertex_from][vertex_to] = edge;
        incoming[vertex_to][vertex_from] = edge;
        return edge;
    }

    void remove_vertex(const T &vertex) {
        if (!contains(vertex))
            return;
        auto out_adj = outgoing[vertex];
        for (auto it = out_adj.begin(); it != out_adj.end(); it++) {
            incoming[it->first].erase(vertex);
        }
        outgoing.erase(vertex);
    }

    void remove_edge(const T &vertex_from, const T &vertex_to) {
        if (outgoing.contains(vertex_from))
            if (auto &m = outgoing[vertex_from]; m.contains(vertex_to)) {
                m.erase(vertex_to);
                incoming[vertex_to].erase(vertex_from);
            }
    }

    void remove_edge(const EdgeType &edge) {
        if (outgoing.contains(edge.origin()))
            if (auto &m = outgoing[edge.origin()]; m.contains(edge.destination())) {
                m.erase(edge.destination());
                incoming[edge.destination()].erase(edge.origin());
            }
    }

  private:
    std::unordered_map<T, std::unordered_map<T, EdgeType, Hasher, Equal>, Hasher, Equal> outgoing, incoming;
};

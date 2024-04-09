#include "wfc/id.h"

#include <cstdint>

#include <catch2/catch_test_macros.hpp>

using namespace wfc;

TEST_CASE("Zero set", "[id_set]") {
    static constexpr auto max_id = std::size_t(0);
    auto id_set = IdSet(max_id);
    CHECK(id_set.size() == max_id);
    CHECK(id_set.capacity() == max_id + 1);
}

TEST_CASE("Single insert", "[id_set]") {
    static constexpr auto max_id = std::size_t(1);
    auto id_set = IdSet(max_id);
    CHECK(id_set.size() == 0);
    CHECK(id_set.capacity() == max_id + 1);

    id_set.insert(0);
    CHECK(id_set.size() == 1);
    CHECK(id_set.capacity() == max_id + 1);
    CHECK(id_set.contains(0));
}

TEST_CASE("Single remove", "[id_set]") {
    static constexpr auto max_id = std::size_t(1);
    auto id_set = IdSet(max_id);
    CHECK(id_set.size() == 0);
    CHECK(id_set.capacity() == max_id + 1);

    id_set.insert(0);
    CHECK(id_set.size() == 1);
    CHECK(id_set.capacity() == max_id + 1);
    CHECK(id_set.contains(0));

    id_set.remove(0);
    CHECK(id_set.size() == 0);
    CHECK(id_set.capacity() == max_id + 1);
    CHECK(!id_set.contains(0));
}

TEST_CASE("Strided fill and remove", "[id_set]") {
    static constexpr auto max_id = std::size_t(10);
    static constexpr auto stride = std::size_t(2);

    auto id_set = IdSet(max_id);
    CHECK(id_set.size() == 0);
    CHECK(id_set.capacity() == max_id + 1);

    for (std::size_t i = 0; i < max_id; i += stride) {
        CHECK(!id_set.contains(i));
        id_set.insert(i);
        CHECK(id_set.size() == (i / stride) + 1);
        CHECK(id_set.contains(i));
    }

    for (std::size_t i = 0; i < max_id; i += stride) {
        CHECK(id_set.contains(i));
        id_set.remove(i);
        CHECK(!id_set.contains(i));
        CHECK(id_set.size() == (max_id / stride) - ((i / stride) + 1));
    }

    CHECK(id_set.capacity() == max_id + 1);
}

TEST_CASE("Insert already inserted", "[id_set]") {
    static constexpr auto max_id = std::size_t(10);

    auto id_set = IdSet(max_id);
    CHECK(id_set.size() == 0);
    CHECK(id_set.capacity() == max_id + 1);

    for (std::size_t i = 0; i < max_id; i++) {
        CHECK(!id_set.contains(i));
        id_set.insert(i);
        CHECK(id_set.size() == (i + 1));
        CHECK(id_set.contains(i));
        id_set.insert(i);
        CHECK(id_set.size() == (i + 1));
        CHECK(id_set.contains(i));
    }

    CHECK(id_set.capacity() == max_id + 1);
}

TEST_CASE("Remove already removed", "[id_set]") {
    static constexpr auto max_id = std::size_t(10);

    auto id_set = IdSet(max_id);
    CHECK(id_set.size() == 0);
    CHECK(id_set.capacity() == max_id + 1);

    for (std::size_t i = 0; i < max_id; i++) {
        CHECK(!id_set.contains(i));
        id_set.insert(i);
        CHECK(id_set.size() == (i + 1));
        CHECK(id_set.contains(i));
    }

    for (std::size_t i = 0; i < max_id; i++) {
        CHECK(id_set.contains(i));
        id_set.remove(i);
        CHECK(!id_set.contains(i));
        CHECK(id_set.size() == (max_id - (i + 1)));
        id_set.remove(i);
        CHECK(!id_set.contains(i));
        CHECK(id_set.size() == (max_id - (i + 1)));
    }

    CHECK(id_set.capacity() == max_id + 1);
}

TEST_CASE("Fill and remove", "[id_set]") {
    static constexpr auto max_id = std::size_t(10);

    auto id_set = IdSet(max_id);
    CHECK(id_set.size() == 0);
    CHECK(id_set.capacity() == max_id + 1);

    for (std::size_t i = 0; i < max_id; i++) {
        CHECK(!id_set.contains(i));
        id_set.insert(i);
        CHECK(id_set.size() == (i + 1));
        CHECK(id_set.contains(i));
    }

    for (std::size_t i = 0; i < max_id; i++) {
        CHECK(id_set.contains(i));
        id_set.remove(i);
        CHECK(!id_set.contains(i));
        CHECK(id_set.size() == (max_id - (i + 1)));
    }

    CHECK(id_set.capacity() == max_id + 1);
}

TEST_CASE("Fill and clear", "[id_set]")  {
    static constexpr auto max_id = std::size_t(10);

    auto id_set = IdSet(max_id);
    CHECK(id_set.size() == 0);

    for (std::size_t i = 0; i < max_id; i++) {
        CHECK(!id_set.contains(i));
        id_set.insert(i);
        CHECK(id_set.size() == (i + 1));
        CHECK(id_set.contains(i));
    }

    id_set.clear();
    CHECK(id_set.size() == 0);

    for (std::size_t i = 0; i < max_id; i++) {
        CHECK(!id_set.contains(i));
    }
}

TEST_CASE("Fill, clear and refill", "[id_set]")  {
    static constexpr auto max_id = std::size_t(10);

    auto id_set = IdSet(max_id);
    CHECK(id_set.size() == 0);

    for (std::size_t i = 0; i < max_id; i++) {
        CHECK(!id_set.contains(i));
        id_set.insert(i);
        CHECK(id_set.size() == (i + 1));
        CHECK(id_set.contains(i));
    }

    id_set.clear();
    CHECK(id_set.size() == 0);

    for (std::size_t i = 0; i < max_id; i++) {
        CHECK(!id_set.contains(i));
    }

    for (std::size_t i = 0; i < max_id; i++) {
        CHECK(!id_set.contains(i));
        id_set.insert(i);
        CHECK(id_set.size() == (i + 1));
        CHECK(id_set.contains(i));
    }
}

#include "wfc/data/bit_set.h"

#include <cstdint>

#include <catch2/catch_test_macros.hpp>

using namespace wfc::data;

TEST_CASE("Zero set", "[bit_set]") {
    static constexpr auto count = std::size_t(0);
    auto bit_set = BitSet(count, false);
    CHECK(bit_set.size() == 0);
    CHECK(bit_set.capacity() == 0);
}

TEST_CASE("Single set", "[bit_set]") {
    static constexpr auto count = std::size_t(1);
    auto bit_set = BitSet(count, false);
    CHECK(bit_set.size() == 0);

    bit_set.set(0);
    CHECK(bit_set.size() == 1);
    CHECK(bit_set.test(0));
}

TEST_CASE("Single unset", "[bit_set]") {
    static constexpr auto count = std::size_t(1);
    auto bit_set = BitSet(count, false);
    CHECK(bit_set.size() == 0);

    bit_set.set(0);
    CHECK(bit_set.size() == 1);
    CHECK(bit_set.test(0));

    bit_set.unset(0);
    CHECK(bit_set.size() == 0);
    CHECK(!bit_set.test(0));
}

TEST_CASE("Strided fill and unset", "[bit_set]") {
    static constexpr auto count = std::size_t(1000);
    static constexpr auto stride = std::size_t(2);

    auto bit_set = BitSet(count, false);
    CHECK(bit_set.size() == 0);

    for (std::size_t i = 0; i < count; i += stride) {
        CHECK(!bit_set.test(i));
        bit_set.set(i);
        CHECK(bit_set.size() == (i / stride) + 1);
        CHECK(bit_set.test(i));
    }

    for (std::size_t i = 0; i < count; i += stride) {
        CHECK(bit_set.test(i));
        bit_set.unset(i);
        CHECK(!bit_set.test(i));
        CHECK(bit_set.size() == (count / stride) - ((i / stride) + 1));
    }
}

TEST_CASE("Set already set", "[bit_set]") {
    static constexpr auto count = std::size_t(1000);

    auto bit_set = BitSet(count, false);
    CHECK(bit_set.size() == 0);

    for (std::size_t i = 0; i < count; i++) {
        CHECK(!bit_set.test(i));
        bit_set.set(i);
        CHECK(bit_set.size() == (i + 1));
        CHECK(bit_set.test(i));
        bit_set.set(i);
        CHECK(bit_set.size() == (i + 1));
        CHECK(bit_set.test(i));
    }
}

TEST_CASE("Unset already unset", "[bit_set]") {
    static constexpr auto count = std::size_t(1000);

    auto bit_set = BitSet(count, false);
    CHECK(bit_set.size() == 0);

    for (std::size_t i = 0; i < count; i++) {
        CHECK(!bit_set.test(i));
        bit_set.set(i);
        CHECK(bit_set.size() == (i + 1));
        CHECK(bit_set.test(i));
    }

    for (std::size_t i = 0; i < count; i++) {
        CHECK(bit_set.test(i));
        bit_set.unset(i);
        CHECK(!bit_set.test(i));
        CHECK(bit_set.size() == (count - (i + 1)));
        bit_set.unset(i);
        CHECK(!bit_set.test(i));
        CHECK(bit_set.size() == (count - (i + 1)));
    }

}

TEST_CASE("Fill and unset", "[bit_set]") {
    static constexpr auto count = std::size_t(1000);

    auto bit_set = BitSet(count, false);
    CHECK(bit_set.size() == 0);

    for (std::size_t i = 0; i < count; i++) {
        CHECK(!bit_set.test(i));
        bit_set.set(i);
        CHECK(bit_set.size() == (i + 1));
        CHECK(bit_set.test(i));
    }

    for (std::size_t i = 0; i < count; i++) {
        CHECK(bit_set.test(i));
        bit_set.unset(i);
        CHECK(!bit_set.test(i));
        CHECK(bit_set.size() == (count - (i + 1)));
    }
}

TEST_CASE("Fill and clear", "[bit_set]")  {
    static constexpr auto count = std::size_t(1000);

    auto bit_set = BitSet(count, false);
    CHECK(bit_set.size() == 0);

    for (std::size_t i = 0; i < count; i++) {
        CHECK(!bit_set.test(i));
        bit_set.set(i);
        CHECK(bit_set.size() == (i + 1));
        CHECK(bit_set.test(i));
    }

    bit_set.clear();
    CHECK(bit_set.size() == 0);

    for (std::size_t i = 0; i < count; i++) {
        CHECK(!bit_set.test(i));
    }
}

TEST_CASE("Fill, clear and refill", "[bit_set]")  {
    static constexpr auto count = std::size_t(1000);

    auto bit_set = BitSet(count, false);
    CHECK(bit_set.size() == 0);

    for (std::size_t i = 0; i < count; i++) {
        CHECK(!bit_set.test(i));
        bit_set.set(i);
        CHECK(bit_set.size() == (i + 1));
        CHECK(bit_set.test(i));
    }

    bit_set.clear();
    CHECK(bit_set.size() == 0);

    for (std::size_t i = 0; i < count; i++) {
        CHECK(!bit_set.test(i));
    }

    for (std::size_t i = 0; i < count; i++) {
        CHECK(!bit_set.test(i));
        bit_set.set(i);
        CHECK(bit_set.size() == (i + 1));
        CHECK(bit_set.test(i));
    }
}

TEST_CASE("Intersect", "[bit_set]")  {
    static constexpr auto count = std::size_t(1000);

    auto bit_set1 = BitSet(count, false);
    auto bit_set2 = BitSet(count, false);

    for (std::size_t i = 0; i < count; i++) {
        bit_set1.set(i);
        bit_set2.set(i / 3);
    }

    CHECK(bit_set1.size() == count);
    CHECK(bit_set2.size() == (count / 3) + 1);

    bit_set1.intersect_with(bit_set2);

    CHECK(bit_set1.size() == (count / 3) + 1);
    CHECK(bit_set2.size() == (count / 3) + 1);

    for (std::size_t i = 0; i < count; i++) {
        CHECK(bit_set1.test(i / 3));
    }
}

TEST_CASE("Union", "[bit_set]")  {
    static constexpr auto count = std::size_t(1000);

    auto bit_set1 = BitSet(count, false);
    auto bit_set2 = BitSet(count, false);

    for (std::size_t i = 0; i < count; i++) {
        bit_set1.set(i / 2);
        bit_set2.set(i);
    }

    CHECK(bit_set1.size() == (count / 2));

    bit_set1.union_with(bit_set2);

    CHECK(bit_set1.size() == count);

    for (std::size_t i = 0; i < count; i++) {
        CHECK(bit_set1.test(i));
    }

    bit_set2.unset(0);
    bit_set1.union_with(bit_set2);

    CHECK(bit_set1.size() == count);

    CHECK(bit_set1.test(0));
    for (std::size_t i = 1; i < count; i++) {
        CHECK(bit_set1.test(i));
    }

    bit_set2.unset(1);
    bit_set1.union_with(bit_set2);

    CHECK(bit_set1.size() == count);

    CHECK(bit_set1.test(0));
    CHECK(bit_set1.test(1));
    for (std::size_t i = 2; i < count; i++) {
        CHECK(bit_set1.test(i));
    }
}

TEST_CASE("For each loop", "[bit_set]")  {
    static constexpr auto count = std::size_t(1000);

    auto bit_set = BitSet(count, false);

    for (std::size_t i = 0; i < count; i++) {
        if (i % 2 == 0) {
            bit_set.set(i);
        } else {
            bit_set.unset(i);
        }
    }

    CHECK(bit_set.size() == (count / 2));

    for (const auto index : bit_set) {
        CHECK(bit_set.test(index));
        CHECK(index % 2 == 0);
    }
}

TEST_CASE("Construct with true and for each", "[bit_set]")  {
    static constexpr auto count = std::size_t(1000);

    auto bit_set = BitSet(count, true);

    CHECK(bit_set.size() == count);

    for (std::size_t i = 0; i < count; i++) {
        CHECK(bit_set.test(i));
    }

    auto total = static_cast<std::size_t>(0);

    for (const auto index : bit_set) {
        CHECK(bit_set.test(index));
        total++;
    }

    CHECK(total == bit_set.size());
}

/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for M5Utility
*/
#include <gtest/gtest.h>
#include <M5Utility.hpp>
#include <M5Unified.hpp>
#include <list>

namespace {

using namespace m5::container;

void cb_basic_test()
{
    SCOPED_TRACE("Basic");

    FixedCircularBuffer<int, 4> rbuf;

    EXPECT_TRUE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.capacity(), 4U);
    EXPECT_EQ(rbuf.size(), 0U);
    EXPECT_FALSE(rbuf.front());
    EXPECT_FALSE(rbuf.back());
    EXPECT_FALSE(rbuf.at(0));
    EXPECT_FALSE(rbuf.at(1));
    EXPECT_FALSE(rbuf.at(2));
    EXPECT_FALSE(rbuf.at(3));

    // H
    // |   |   |   | 1 |
    //             T
    rbuf.push_front(1);
    EXPECT_FALSE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 1U);
    EXPECT_EQ(rbuf[0], 1);
    EXPECT_EQ(rbuf.front(), 1);
    EXPECT_EQ(rbuf.back(), 1);
    // M5_LOGW("1> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    // H
    // |   |   |   |   |
    // T
    auto r = rbuf.front();
    EXPECT_TRUE(r);
    int v = *r;
    rbuf.pop_front();
    EXPECT_EQ(v, 1);
    EXPECT_TRUE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 0U);
    EXPECT_FALSE(rbuf.front());
    EXPECT_FALSE(rbuf.back());
    // M5_LOGW("2> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    //     H
    // | 2 |   |   |   |
    // T
    rbuf.push_back(2);
    EXPECT_FALSE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 1U);
    EXPECT_EQ(rbuf[0], 2);
    EXPECT_EQ(rbuf.front(), 2);
    EXPECT_EQ(rbuf.back(), 2);
    // M5_LOGW("3> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    // H
    // |   |   |   |   |
    // T
    r = rbuf.back();
    EXPECT_TRUE(r);
    v = *r;
    rbuf.pop_back();
    EXPECT_EQ(v, 2);
    EXPECT_TRUE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 0U);
    EXPECT_FALSE(rbuf.front());
    EXPECT_FALSE(rbuf.back());
    // M5_LOGW("3> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    //         H
    // | 4 | 5 | 6 | 3 |
    //         T
    rbuf.push_front(3);  // to front
    EXPECT_FALSE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 1U);
    EXPECT_EQ(rbuf.front(), 3);
    EXPECT_EQ(rbuf.back(), 3);

    rbuf.push_back(4);  // to back
    EXPECT_FALSE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 2U);
    EXPECT_EQ(rbuf.front(), 3);
    EXPECT_EQ(rbuf.back(), 4);

    rbuf.push_back(5);  // to back
    EXPECT_FALSE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 3U);
    EXPECT_EQ(rbuf.front(), 3);
    EXPECT_EQ(rbuf.back(), 5);

    rbuf.push_front(6);  // to front
    EXPECT_FALSE(rbuf.empty());
    EXPECT_TRUE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 4U);
    EXPECT_EQ(rbuf[0], 6);
    EXPECT_EQ(rbuf[1], 3);
    EXPECT_EQ(rbuf[2], 4);
    EXPECT_EQ(rbuf[3], 5);
    EXPECT_EQ(rbuf.front(), 6);
    EXPECT_EQ(rbuf.back(), 5);
    // M5_LOGW("4> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    //     H
    // | 4 | 7 | 6 | 3 |
    //     T
    rbuf.push_front(7);
    EXPECT_FALSE(rbuf.empty());
    EXPECT_TRUE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 4U);
    EXPECT_EQ(rbuf[0], 7);
    EXPECT_EQ(rbuf[1], 6);
    EXPECT_EQ(rbuf[2], 3);
    EXPECT_EQ(rbuf[3], 4);
    EXPECT_EQ(rbuf.front(), 7);
    EXPECT_EQ(rbuf.back(), 4);
    // M5_LOGW("5> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    //         H
    // | 4 | 8 | 6 | 3 |
    //         T
    rbuf.push_back(8);
    EXPECT_FALSE(rbuf.empty());
    EXPECT_TRUE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 4U);
    EXPECT_EQ(rbuf[0], 6);
    EXPECT_EQ(rbuf[1], 3);
    EXPECT_EQ(rbuf[2], 4);
    EXPECT_EQ(rbuf[3], 8);
    EXPECT_EQ(rbuf.front(), 6);
    EXPECT_EQ(rbuf.back(), 8);
    // M5_LOGW("5> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    //         H
    // |   |   |   |   |
    //         T
    rbuf.pop_back();
    EXPECT_EQ(rbuf.back(), 4);
    rbuf.pop_back();
    EXPECT_EQ(rbuf.back(), 3);
    rbuf.pop_back();
    EXPECT_EQ(rbuf.back(), 6);
    rbuf.pop_back();
    EXPECT_TRUE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 0U);
    EXPECT_FALSE(rbuf.front());
    EXPECT_FALSE(rbuf.back());
    // M5_LOGW("6> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    //             H
    // | 11| 12| 13| 10 |
    //             T
    rbuf.push_back(9);
    rbuf.push_back(10);
    rbuf.push_back(11);
    rbuf.push_back(12);
    rbuf.push_back(13);
    EXPECT_FALSE(rbuf.empty());
    EXPECT_TRUE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 4U);
    EXPECT_EQ(rbuf[0], 10);
    EXPECT_EQ(rbuf[1], 11);
    EXPECT_EQ(rbuf[2], 12);
    EXPECT_EQ(rbuf[3], 13);
    EXPECT_EQ(rbuf.front(), 10);
    EXPECT_EQ(rbuf.back(), 13);
    // M5_LOGW("7> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    //             H
    // |   |   | 13|   |
    //         T
    rbuf.pop_front();
    EXPECT_EQ(rbuf.front(), 11);
    EXPECT_EQ(rbuf.back(), 13);
    rbuf.pop_front();
    EXPECT_EQ(rbuf.front(), 12);
    EXPECT_EQ(rbuf.back(), 13);
    rbuf.pop_front();
    EXPECT_FALSE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 1U);
    EXPECT_EQ(rbuf.front(), 13);
    EXPECT_EQ(rbuf.back(), 13);
    // M5_LOGW("8> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    //         H
    // |   |   |   |   |
    //         T
    rbuf.clear();
    EXPECT_TRUE(rbuf.empty());
    EXPECT_FALSE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 0U);
    EXPECT_FALSE(rbuf.front());
    EXPECT_FALSE(rbuf.back());
    // M5_LOGW("9> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);

    // H
    // |111|111|111|111|
    // T
    rbuf.fill(111);
    EXPECT_TRUE(rbuf.full());
    EXPECT_EQ(rbuf.size(), 4U);
    EXPECT_EQ(rbuf[0], 111);
    EXPECT_EQ(rbuf[1], 111);
    EXPECT_EQ(rbuf[2], 111);
    EXPECT_EQ(rbuf[3], 111);
    EXPECT_EQ(rbuf.front(), 111);
    EXPECT_EQ(rbuf.back(), 111);
    // M5_LOGW("A> %zu/%zu/%u", rbuf._head, rbuf._tail, rbuf._full);
}

void cb_constructor_test()
{
    SCOPED_TRACE("Constructor");

    std::vector<int> table(100);
    for (int i = 0; i < 100; ++i) {
        table[i] = i;
    }

    {
        FixedCircularBuffer<int, 8> rbuf(2U, 52);
        EXPECT_FALSE(rbuf.empty());
        EXPECT_FALSE(rbuf.full());
        EXPECT_EQ(rbuf.capacity(), 8U);
        EXPECT_EQ(rbuf.size(), 2U);
        EXPECT_EQ(rbuf[0], 52);
        EXPECT_EQ(rbuf[1], 52);
    }
    {
        FixedCircularBuffer<int, 8> rbuf(100U, 52);
        EXPECT_FALSE(rbuf.empty());
        EXPECT_TRUE(rbuf.full());
        EXPECT_EQ(rbuf.capacity(), 8U);
        EXPECT_EQ(rbuf.size(), 8U);
        EXPECT_EQ(rbuf.front(), 52);
        EXPECT_EQ(rbuf.back(), 52);
    }

    {
        FixedCircularBuffer<int, 10> rbuf2({9, 8, 7, 6, 5});
        EXPECT_FALSE(rbuf2.empty());
        EXPECT_FALSE(rbuf2.full());
        EXPECT_EQ(rbuf2.capacity(), 10U);
        EXPECT_EQ(rbuf2.size(), 5U);
        EXPECT_EQ(rbuf2[0], 9);
        EXPECT_EQ(rbuf2[1], 8);
        EXPECT_EQ(rbuf2[2], 7);
        EXPECT_EQ(rbuf2[3], 6);
        EXPECT_EQ(rbuf2[4], 5);
    }
    {
        FixedCircularBuffer<int, 8> rbuf2(table.begin(), table.end());
        EXPECT_FALSE(rbuf2.empty());
        EXPECT_TRUE(rbuf2.full());
        EXPECT_EQ(rbuf2.capacity(), 8U);
        EXPECT_EQ(rbuf2.size(), 8U);
        EXPECT_EQ(rbuf2.front(), 100 - 8);
        EXPECT_EQ(rbuf2.back(), 99);
    }

    FixedCircularBuffer<float, 3> rbuf3 = {1.1f, 2.2f, 3.3f};
    EXPECT_FALSE(rbuf3.empty());
    EXPECT_TRUE(rbuf3.full());
    EXPECT_EQ(rbuf3.capacity(), 3U);
    EXPECT_EQ(rbuf3.size(), 3U);
    EXPECT_FLOAT_EQ(rbuf3[0], 1.1f);
    EXPECT_FLOAT_EQ(rbuf3[1], 2.2f);
    EXPECT_FLOAT_EQ(rbuf3[2], 3.3f);
}

void cb_read()
{
    SCOPED_TRACE("Read");

    FixedCircularBuffer<int, 128> rb;
    int buf[128] = {};
    size_t rcnt  = 0;

    // empty
    rcnt = rb.read(buf, 64);
    EXPECT_EQ(rcnt, 0U);

    // fill
    for (size_t i = 0; i < rb.capacity(); ++i) {
        rb.push_back(i);
    }
    EXPECT_EQ(rb.size(), 128U);

    // read half
    rcnt = rb.read(buf, 64);
    EXPECT_EQ(rcnt, 64U);
    for (int i = 0; i < 64; ++i) {
        EXPECT_EQ(buf[i], i) << i;
    }

    EXPECT_EQ(rb.size(), 128U);
    EXPECT_EQ(rb.front(), 0);
    EXPECT_EQ(rb.back(), 127);

    // pop half
    for (int i = 0; i < 64; ++i) {
        rb.pop_front();
    }

    // read
    rcnt = rb.read(buf, 64);
    EXPECT_EQ(rcnt, 64U);
    for (int i = 0; i < 64; ++i) {
        EXPECT_EQ(buf[i], i + 64) << (i + 64);
    }

    EXPECT_EQ(rb.size(), 64U);
    EXPECT_EQ(rb.front(), 64);
    EXPECT_EQ(rb.back(), 127);

    // pop half
    for (int i = 0; i < 32; ++i) {
        rb.pop_front();
    }

    // read
    rcnt = rb.read(buf, 64);
    EXPECT_EQ(rcnt, 32U);
    for (int i = 0; i < 32; ++i) {
        EXPECT_EQ(buf[i], i + 96) << (i + 96);
    }

    EXPECT_EQ(rb.size(), 32U);
    EXPECT_EQ(rb.front(), 96);
    EXPECT_EQ(rb.back(), 127);

    // push
    for (int i = 0; i < 100; ++i) {
        rb.push_back(i + 128);
    }

    // read
    rcnt = rb.read(buf, 64);
    EXPECT_EQ(rcnt, 64U);
    for (int i = 0; i < 64; ++i) {
        EXPECT_EQ(buf[i], i + 100) << (i + 100);
    }

    EXPECT_EQ(rb.size(), 128U);
    EXPECT_EQ(*rb.front(), 227 - 127);
    EXPECT_EQ(*rb.back(), 227);
}

void cb_iterator_test()
{
    SCOPED_TRACE("Iterators");

    FixedCircularBuffer<int, 4> rb = {0, 1, 2};
    FixedCircularBuffer<int, 6> rb2;

    // empty
    EXPECT_EQ(rb2.begin(), rb2.end());
    EXPECT_EQ(rb2.rbegin(), rb2.rend());
    EXPECT_EQ(rb2.cbegin(), rb2.cend());
    EXPECT_EQ(rb2.crbegin(), rb2.crend());

    //
    int c = 0;
    for (auto it = rb.begin(); it != rb.end(); ++it) {
        EXPECT_EQ(*it, c++);
    }
    c = 2;
    for (auto it = rb.rbegin(); it != rb.rend(); ++it) {
        EXPECT_EQ(*it, c--);
    }
    c = 0;
    for (auto it = rb.cbegin(); it != rb.cend(); ++it) {
        EXPECT_EQ(*it, c++);
    }
    c = 2;
    for (auto it = rb.crbegin(); it != rb.crend(); ++it) {
        EXPECT_EQ(*it, c--);
    }

    //
    rb.clear();
    rb.push_back(9);
    rb.push_back(8);
    rb.push_back(7);
    rb.push_back(6);
    EXPECT_TRUE(rb.full());

    c = 9;
    for (auto&& e : rb) {
        EXPECT_EQ(e, c--);
    }
    c = 6;
    for (auto it = rb.crbegin(); it != rb.crend(); ++it) {
        EXPECT_EQ(*it, c++);
    }

    // make the rounds
    rb.push_back(5);
    rb.push_back(4);
    c = 7;
    for (auto&& e : rb) {
        EXPECT_EQ(e, c--);
    }
    c = 4;
    for (auto it = rb.crbegin(); it != rb.crend(); ++it) {
        EXPECT_EQ(*it, c++);
    }

    c = 7;
    std::for_each(std::begin(rb), std::end(rb), [&c](const int& e) { EXPECT_EQ(e, c--); });
#if __cplusplus >= 201402L
    // std::rbegin, rend require C++14 or later
    c = 4;
    std::for_each(std::rbegin(rb), std::rend(rb), [&c](const int& e) { EXPECT_EQ(e, c++); });
#endif

    c = 4;
    for (auto it = rb.cend(); it != rb.cbegin(); /**/) {
        --it;
        EXPECT_EQ(*it, c++);
    }
    c = 7;
    for (auto it = rb.rend(); it != rb.rbegin(); /**/) {
        --it;
        EXPECT_EQ(*it, c--);
    }

    {
        auto it = rb.begin();
        EXPECT_EQ(*it++, 7);
        EXPECT_EQ(*it--, 6);
        EXPECT_EQ(*++it, 6);
        EXPECT_EQ(*--it, 7);

        auto itr = rb.rbegin();
        EXPECT_EQ(*itr++, 4);
        EXPECT_EQ(*itr--, 5);
        EXPECT_EQ(*++itr, 5);
        EXPECT_EQ(*--itr, 4);
    }
}

void cb_copy_move_test()
{
    SCOPED_TRACE("Copy/Move");

    // Copy constructor
    {
        FixedCircularBuffer<int, 4> original = {1, 2, 3};
        FixedCircularBuffer<int, 4> copied(original);

        EXPECT_EQ(copied.size(), 3U);
        EXPECT_EQ(copied[0], 1);
        EXPECT_EQ(copied[1], 2);
        EXPECT_EQ(copied[2], 3);

        // Verify original unchanged
        EXPECT_EQ(original.size(), 3U);
        EXPECT_EQ(original[0], 1);
    }

    // Move constructor
    {
        FixedCircularBuffer<int, 4> original = {4, 5, 6};
        FixedCircularBuffer<int, 4> moved(std::move(original));

        EXPECT_EQ(moved.size(), 3U);
        EXPECT_EQ(moved[0], 4);
        EXPECT_EQ(moved[1], 5);
        EXPECT_EQ(moved[2], 6);
    }

    // Copy assignment
    {
        FixedCircularBuffer<int, 4> original = {7, 8};
        FixedCircularBuffer<int, 4> assigned;
        assigned = original;

        EXPECT_EQ(assigned.size(), 2U);
        EXPECT_EQ(assigned[0], 7);
        EXPECT_EQ(assigned[1], 8);
    }

    // Move assignment
    {
        FixedCircularBuffer<int, 4> original = {9, 10};
        FixedCircularBuffer<int, 4> assigned;
        assigned = std::move(original);

        EXPECT_EQ(assigned.size(), 2U);
        EXPECT_EQ(assigned[0], 9);
        EXPECT_EQ(assigned[1], 10);
    }
}

void cb_swap_test()
{
    SCOPED_TRACE("Swap");

    // Member swap
    {
        FixedCircularBuffer<int, 4> a = {1, 2, 3};
        FixedCircularBuffer<int, 4> b = {4, 5};

        a.swap(b);

        EXPECT_EQ(a.size(), 2U);
        EXPECT_EQ(a[0], 4);
        EXPECT_EQ(a[1], 5);

        EXPECT_EQ(b.size(), 3U);
        EXPECT_EQ(b[0], 1);
        EXPECT_EQ(b[1], 2);
        EXPECT_EQ(b[2], 3);
    }

    // std::swap specialization
    {
        FixedCircularBuffer<int, 4> a = {10, 20};
        FixedCircularBuffer<int, 4> b = {30, 40, 50};

        std::swap(a, b);

        EXPECT_EQ(a.size(), 3U);
        EXPECT_EQ(a[0], 30);
        EXPECT_EQ(a[1], 40);
        EXPECT_EQ(a[2], 50);

        EXPECT_EQ(b.size(), 2U);
        EXPECT_EQ(b[0], 10);
        EXPECT_EQ(b[1], 20);
    }

    // Swap with empty
    {
        FixedCircularBuffer<int, 4> a = {1, 2, 3, 4};
        FixedCircularBuffer<int, 4> b;

        a.swap(b);

        EXPECT_TRUE(a.empty());
        EXPECT_TRUE(b.full());
        EXPECT_EQ(b.size(), 4U);
    }

    // Self swap
    {
        FixedCircularBuffer<int, 4> a = {1, 2, 3};
        a.swap(a);

        EXPECT_EQ(a.size(), 3U);
        EXPECT_EQ(a[0], 1);
        EXPECT_EQ(a[1], 2);
        EXPECT_EQ(a[2], 3);
    }

    // ADL swap (using std::swap should find m5::container::swap via ADL)
    {
        CircularBuffer<int> a(4);
        CircularBuffer<int> b(6);

        a.push_back(1);
        b.push_back(10);
        b.push_back(20);

        using std::swap;
        swap(a, b);

        EXPECT_EQ(a.capacity(), 6U);
        EXPECT_EQ(a.size(), 2U);
        EXPECT_EQ(a[0], 10);
        EXPECT_EQ(a[1], 20);

        EXPECT_EQ(b.capacity(), 4U);
        EXPECT_EQ(b.size(), 1U);
        EXPECT_EQ(b[0], 1);
    }
}

void cb_assign_test()
{
    SCOPED_TRACE("Assign");

    // assign with initializer_list
    {
        FixedCircularBuffer<int, 4> rb = {1, 2, 3, 4};
        rb.assign({10, 20});

        EXPECT_EQ(rb.size(), 2U);
        EXPECT_EQ(rb[0], 10);
        EXPECT_EQ(rb[1], 20);
    }

    // assign with count and value (use size_t to avoid template overload)
    {
        FixedCircularBuffer<int, 4> rb;
        rb.assign(size_t(3), 42);

        EXPECT_EQ(rb.size(), 3U);
        EXPECT_EQ(rb[0], 42);
        EXPECT_EQ(rb[1], 42);
        EXPECT_EQ(rb[2], 42);
    }

    // assign with iterators
    {
        std::vector<int> src = {100, 200, 300};
        FixedCircularBuffer<int, 4> rb;
        rb.assign(src.begin(), src.end());

        EXPECT_EQ(rb.size(), 3U);
        EXPECT_EQ(rb[0], 100);
        EXPECT_EQ(rb[1], 200);
        EXPECT_EQ(rb[2], 300);
    }

    // assign with non-random-access iterators (std::list = BidirectionalIterator)
    {
        std::list<int> src = {10, 20, 30, 40, 50};
        FixedCircularBuffer<int, 3> rb;
        rb.assign(src.begin(), src.end());

        // capacity=3, input=5 => last 3 elements
        EXPECT_EQ(rb.size(), 3U);
        EXPECT_EQ(rb[0], 30);
        EXPECT_EQ(rb[1], 40);
        EXPECT_EQ(rb[2], 50);
    }

    // assign with list that fits
    {
        std::list<int> src = {1, 2};
        FixedCircularBuffer<int, 4> rb;
        rb.assign(src.begin(), src.end());

        EXPECT_EQ(rb.size(), 2U);
        EXPECT_EQ(rb[0], 1);
        EXPECT_EQ(rb[1], 2);
    }
}

void cb_dynamic_buffer_test()
{
    SCOPED_TRACE("DynamicBuffer");

    // CircularBuffer (dynamic size)
    {
        CircularBuffer<int> rb(4);

        EXPECT_TRUE(rb.empty());
        EXPECT_EQ(rb.capacity(), 4U);

        rb.push_back(1);
        rb.push_back(2);
        rb.push_back(3);

        EXPECT_EQ(rb.size(), 3U);
        EXPECT_EQ(rb[0], 1);
        EXPECT_EQ(rb[1], 2);
        EXPECT_EQ(rb[2], 3);
    }

    // CircularBuffer with assign (full fill)
    {
        CircularBuffer<int> rb(4);
        rb.assign(size_t(4), 99);

        EXPECT_TRUE(rb.full());
        EXPECT_EQ(rb.size(), 4U);
        EXPECT_EQ(rb[0], 99);
        EXPECT_EQ(rb[1], 99);
        EXPECT_EQ(rb[2], 99);
        EXPECT_EQ(rb[3], 99);
    }

    // CircularBuffer with assign (partial fill)
    {
        CircularBuffer<int> rb(4);
        rb.assign(size_t(3), 99);

        EXPECT_EQ(rb.size(), 3U);
        EXPECT_EQ(rb[0], 99);
        EXPECT_EQ(rb[1], 99);
        EXPECT_EQ(rb[2], 99);
    }

    // CircularBuffer with iterator constructor
    {
        std::vector<int> src = {1, 2, 3, 4, 5};
        CircularBuffer<int> rb(3, src.begin(), src.end());

        EXPECT_TRUE(rb.full());
        EXPECT_EQ(rb.size(), 3U);
        EXPECT_EQ(rb[0], 3);  // Last 3 elements
        EXPECT_EQ(rb[1], 4);
        EXPECT_EQ(rb[2], 5);
    }

    // CircularBuffer with initializer_list constructor
    {
        CircularBuffer<int> rb(4, {10, 20, 30});

        EXPECT_EQ(rb.size(), 3U);
        EXPECT_EQ(rb[0], 10);
        EXPECT_EQ(rb[1], 20);
        EXPECT_EQ(rb[2], 30);
    }

    // CircularBuffer swap
    {
        CircularBuffer<int> a(4);
        CircularBuffer<int> b(6);

        a.push_back(1);
        a.push_back(2);
        b.push_back(10);

        a.swap(b);

        EXPECT_EQ(a.capacity(), 6U);
        EXPECT_EQ(a.size(), 1U);
        EXPECT_EQ(a[0], 10);

        EXPECT_EQ(b.capacity(), 4U);
        EXPECT_EQ(b.size(), 2U);
        EXPECT_EQ(b[0], 1);
    }
}

}  // namespace

TEST(Utility, CircularBuffer)
{
    cb_basic_test();
    cb_constructor_test();
    cb_read();
    cb_iterator_test();
    cb_copy_move_test();
    cb_swap_test();
    cb_assign_test();
    cb_dynamic_buffer_test();
}

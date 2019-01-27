#include <gtest/gtest.h>
#include "unique_ptr.h"
#include <functional>

template <typename T>
struct MyDeleter
{
    void operator()(T* ptr)
    {
        std::cerr << "my deleter\n";
        delete ptr;
    }
};

TEST(constructors, default_ctor)
{
    unique_ptr<int> x;
}

TEST(constructors, make_un)
{
    unique_ptr<int> x = make_unique<int>(5);
}

TEST(constructors, my_deleter)
{
    int* x = new int(5);
    unique_ptr<int> y = unique_ptr<int>(x, MyDeleter<int>());
}

TEST(constructors, lambda_deleter)
{
    int* x = new int(5);
    unique_ptr<int> y = unique_ptr<int>(x, [](int* x) -> void { std::cerr<<"my lambda\n"; delete x;});
}

TEST(correctness, a)
{
    auto x = make_unique<int>(1);
    ASSERT_EQ(*x, 1);
    (*x) = 5;
    ASSERT_EQ(*x, 5);
}

struct C {

    double val;

    C(int a, int b) : val(a + b) {}
    C(double x) : val(x * 10) {}

};

void test() {

    unique_ptr<int> r(new int(1));
    if (r) {
        assert(*r == 1);
    }
    int *ptr = r.release();
    r.reset(new int(2));

    unique_ptr<int> p(std::move(ptr));
    assert(p.get() == ptr);
    assert(*p.get() == 1);

    unique_ptr<int> q = std::move(r);
    assert(*q == 2);

    auto f = make_unique<C>(1, 1);
    auto g = make_unique<C>(10.);
    f.swap(g);

    assert(f->val == 100.);
    assert(g->val == 2.);

}

TEST(danya, a)
{
    test();
}
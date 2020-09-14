//
//  main.cpp
//  Playground
//
//  Created by Albert Slepak on 12/9/19.
//  Copyright © 2019 none. All rights reserved.
//

#include <iostream>
#include <chrono>
#include <functional>

#include "fast_string.h"
#include <string>

template <typename T> class basic_stopwatch
{
    typedef T clock;
    typename clock::time_point p;
    typename clock::duration   d;
    
public:
    void start() { p  = clock::now();            }
    void stop()  { d += clock::now() - p;        }
    void reset() { d  = clock::duration::zero(); }
    
    template <typename S> unsigned long long int report() const
    {
        return std::chrono::duration_cast<S>(d).count();
    }
    
    unsigned long long int report_ms() const
    {
        return report<std::chrono::milliseconds>();
    }
    
    unsigned long long int report_ns() const
    {
        return report<std::chrono::nanoseconds>();
    }
    
    basic_stopwatch() : p(), d() { }
};

typedef basic_stopwatch<std::chrono::high_resolution_clock> stopwatch;

class TestFramework
{
    const char* m_Name;
    std::function<void()> m_Fn1, m_Fn2;

    void RunTest(stopwatch& sw)
    {
        sw.start();

        for (size_t i = 0; i < 1000000; i++)
            m_Fn1();

        sw.stop();
        std::cout << sw.report_ms() << "ms VS ";
        sw.reset();

        sw.start();

        for (size_t i = 0; i < 1000000; i++)
            m_Fn2();

        sw.stop();
        std::cout << sw.report_ms() << "ms\n";
        sw.reset();
    }

public:
    TestFramework(const char* name) : m_Name(name) {}

    void SetFn1(std::function<void()> fn) { m_Fn1 = fn; }
    void SetFn2(std::function<void()> fn) { m_Fn2 = fn; }

    void Run()
    {
        std::cout << "Running Test: " << m_Name << "\n";
        stopwatch sw;

        for (size_t i = 0; i < 10; i++)
            RunTest(sw);

        std::cout << "\n";
    }
};

void replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return;
    str.replace(start_pos, from.length(), to);
}

void replace(fast_string& str, const fast_string& from, const fast_string& to)
{
    str.replace(from, to);
}

void test1()
{
    TestFramework ShortConstructionTest("Short Construction");
    ShortConstructionTest.SetFn1([]() {
        fast_string str("Hello World!");
    });
    ShortConstructionTest.SetFn2([]() {
        std::string str("Hello World!");
    });
    
    ShortConstructionTest.Run();
    
    TestFramework LongConstructionTest("Long Construction");
    LongConstructionTest.SetFn1([]() {
        fast_string str("Hello World! This is Great!");
    });
    LongConstructionTest.SetFn2([]() {
        std::string str("Hello World! This is Great!");
    });

    LongConstructionTest.Run();
}

void test2()
{
    TestFramework PushPopBackTest("push_back/pop_back");
    PushPopBackTest.SetFn1([]() {
        fast_string str("Hello World!");
        str.push_back('c');
        str.pop_back();
    });
    PushPopBackTest.SetFn2([]() {
        std::string str("Hello World!");
        str.push_back('c');
        str.pop_back();
    });

    PushPopBackTest.Run();
}

void test3()
{
    TestFramework EraseTest("Erasing");
    EraseTest.SetFn1([]() {
        fast_string str("Hello World!");
        str.erase(4, 5);
    });
    EraseTest.SetFn2([]() {
        std::string str("Hello World!");
        str.erase(4, 5);
    });

    EraseTest.Run();
}

void test4()
{
    TestFramework InsertTest("Inserting");
    InsertTest.SetFn1([]() {
        fast_string str("Hello");
        str.insert(str.length(), "World!");
    });
    InsertTest.SetFn2([]() {
        std::string str("Hello");
        str.insert(str.length(), "World!");
    });

    InsertTest.Run();
}

void test5()
{
    TestFramework SubstrTest("Substring");
    SubstrTest.SetFn1([]() {
        fast_string str("Hello World!");
        fast_string& s = str.substr(4, 4);
    });
    SubstrTest.SetFn2([]() {
        std::string str("Hello World!");
        std::string& s = str.substr(4, 4);
    });

    SubstrTest.Run();
}

void test6()
{
    TestFramework AppendTest("Append");
    AppendTest.SetFn1([]() {
        fast_string str("Hello World!");
        str.append("Nice World!");
        });
    AppendTest.SetFn2([]() {
        std::string str("Hello World!");
        str.append("Nice World!");
        });

    AppendTest.Run();
}

void test7()
{
    TestFramework FindTest("Find");
    FindTest.SetFn1([]() {
        fast_string str("Hello World!");
        str.find("World!");
    });
    FindTest.SetFn2([]() {
        std::string str("Hello World!");

#pragma warning(suppress: 4834)
        str.find("World!");
    });

    FindTest.Run();
}

int main(int argc, const char * argv[])
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    
    return 0;
}

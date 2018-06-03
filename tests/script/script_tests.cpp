#include <string>

#include <gtest/gtest.h>

#include "lexer.hpp"
#include "parser.hpp"
#include "compiler.hpp"
#include "virtual_machine.hpp"

using std::string_literals::operator""s;

TEST(script_tests, swap_key_values)
{
    const auto src = R"src(
var a = {
    0 : 'a',
    1 : 'b',
    2 : 'c'
};
var b = { };

foreach(var k in a)
{
    b[a[k]] = k;
}
)src";

    const eng::lexer l{ src };
    eng::parser p{ l.tokens() };
    eng::compiler c{ p.yield() };
    eng::virtual_machine vm{ c.get_chunk() };
    vm.run();

    const auto globals = vm.globals();
    ASSERT_EQ(2u, globals.size());
    ASSERT_EQ(eng::value{ "a"s }, globals.at("a").get<eng::object*>()->at(eng::value{ 0.0f }));
    ASSERT_EQ(eng::value{ "b"s }, globals.at("a").get<eng::object*>()->at(eng::value{ 1.0f }));
    ASSERT_EQ(eng::value{ "c"s }, globals.at("a").get<eng::object*>()->at(eng::value{ 2.0f }));
    ASSERT_EQ(eng::value{ 0.0f }, globals.at("b").get<eng::object*>()->at(eng::value{ "a"s }));
    ASSERT_EQ(eng::value{ 1.0f }, globals.at("b").get<eng::object*>()->at(eng::value{ "b"s }));
    ASSERT_EQ(eng::value{ 2.0f }, globals.at("b").get<eng::object*>()->at(eng::value{ "c"s }));
}

TEST(script_tests, do_while_false)
{
    const auto src = R"src(
var a = 0;
do
{
    if(a == 0)
    {
        a = 2;
        break;
    }
    a = 5;
}while(false)
)src";

    const eng::lexer l{ src };
    eng::parser p{ l.tokens() };
    eng::compiler c{ p.yield() };
    eng::virtual_machine vm{ c.get_chunk() };
    vm.run();

    const auto globals = vm.globals();
    ASSERT_EQ(1u, globals.size());
    ASSERT_EQ(eng::value{ 2.0f }, globals.at("a"));
}

TEST(script_tests, fibonacci)
{
    const auto src = R"src(
function fibonacci(var n)
{
    if(n <= 1)
    {
        return 1;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

var a = fibonacci(15);
)src";

    const eng::lexer l{ src };
    eng::parser p{ l.tokens() };
    eng::compiler c{ p.yield() };
    eng::virtual_machine vm{ c.get_chunk() };
    vm.run();

    const auto globals = vm.globals();
    ASSERT_EQ(2u, globals.size());
    ASSERT_EQ(eng::value{ 987.0f }, globals.at("a"));
}

TEST(script_tests, function_return_function)
{
    const auto src = R"src(
function foo()
{
    function bar()
    {
        return 1;
    }
    return bar;
}

var a = foo()();
)src";

    const eng::lexer l{ src };
    eng::parser p{ l.tokens() };
    eng::compiler c{ p.yield() };
    eng::virtual_machine vm{ c.get_chunk() };
    vm.run();

    const auto globals = vm.globals();
    ASSERT_EQ(2u, globals.size());
    ASSERT_EQ(eng::value{ 1.0f }, globals.at("a"));
}

TEST(script_tests, closure)
{
    const auto src = R"src(
function foo()
{
    var x = 1;
    function bar()
    {
        x = x + 1;
        return x;
    }
    return bar;
}

var funcone = foo();
var a = funcone();
var b = funcone();

var functwo = foo();
var c = functwo();
var d = functwo();

)src";

    const eng::lexer l{ src };
    eng::parser p{ l.tokens() };
    eng::compiler c{ p.yield() };
    eng::virtual_machine vm{ c.get_chunk() };
    vm.run();

    const auto globals = vm.globals();
    ASSERT_EQ(7u, globals.size());
    ASSERT_EQ(eng::value{ 2.0f }, globals.at("a"));
    ASSERT_EQ(eng::value{ 3.0f }, globals.at("b"));
    ASSERT_EQ(eng::value{ 2.0f }, globals.at("c"));
    ASSERT_EQ(eng::value{ 3.0f }, globals.at("d"));
}

TEST(script_tests, complex_access)
{
    const auto src = R"src(
function foo(var x)
{
    return x;
}

var y = { 'func' : foo };
var x = { };

y['func'](x)[0] = 1;


)src";

    const eng::lexer l{ src };
    eng::parser p{ l.tokens() };
    eng::compiler c{ p.yield() };
    eng::virtual_machine vm{ c.get_chunk() };
    vm.run();

    const auto globals = vm.globals();
    ASSERT_EQ(3u, globals.size());
    ASSERT_EQ(eng::value{ 1.0f }, globals.at("x").get<eng::object*>()->at(eng::value{ 0.0f }));
}


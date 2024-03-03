#include "ParserTestUtils.hpp"
#include "doctest/doctest.h"

TEST_CASE("Lexer tests" * doctest::test_suite("Parser tests")) {
  LEX_TEST("number test", R"(
10 + 20
)",
           R"(2:1:number:10
2:4:plus:+
2:6:number:20
3:1:eof:
)")

  LEX_TEST("number test - 2", R"(
10.
10.0
10
"abcd"
)",
           R"(2:1:number:10.
3:1:number:10.0
4:1:number:10
5:1:string:abcd
6:1:eof:
)")

  LEX_TEST("keywords", R"(
and
class
else
false
for
fun
if
nil
or
print
return
super
this
true
var
while
)",
           R"(2:1:and:and
3:1:class:class
4:1:else:else
5:1:false:false
6:1:for:for
7:1:fun:fun
8:1:if:if
9:1:nil:nil
10:1:or:or
11:1:print:print
12:1:return:return
13:1:super:super
14:1:this:this
15:1:true:true
16:1:var:var
17:1:while:while
18:1:eof:
)")
}
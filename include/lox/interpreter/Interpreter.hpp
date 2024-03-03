#ifndef __LOX_INTERPRETER_HPP__
#define __LOX_INTERPRETER_HPP__

#include "lox/interpreter/Value.hpp"
#include "lox/ast/AST.hpp"
#include "utils/TypeUtils.hpp"

namespace lox {

struct Interpreter {


private:
};

struct ExprInterpreter {
  uptr<Value> operator()(const BinaryE &binaryE);
  uptr<Value> operator()(const UnaryE &unaryE);
  uptr<Value> operator()(const GroupingE &groupingE);
  uptr<Value> operator()(const LiteralE &literalE);

private:

};


}



#endif /// __LOX_INTERPRETER_HPP__


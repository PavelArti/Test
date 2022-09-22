#pragma once
#include <PascalLexer.h>

#include <iosfwd>

namespace pascal {

void dump_tokens(PascalLexer& lexer, std::ostream& out);

}  // namespace pascal

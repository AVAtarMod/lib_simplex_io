#ifndef LIB_SIMPLEX_IO_WRITE_FILE_HPP
#define LIB_SIMPLEX_IO_WRITE_FILE_HPP
#include <ostream>

#include "types.hpp"
namespace lib_simplex_io {
   bool writeFunction(std::ostream& in, const FunctionBase& function);
   bool writeConstraint(std::ostream& in, const ConstraintBase& constraint);
   bool writeSimplexTableau(std::ostream& in, const SimplexTableau& tableau);
} // namespace lib_simplex_io

#endif // LIB_SIMPLEX_IO_WRITE_FILE_HPP

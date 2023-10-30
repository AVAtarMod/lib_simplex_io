#ifndef LIB_SIMPLEX_IO_WRITE_FILE_HPP
#define LIB_SIMPLEX_IO_WRITE_FILE_HPP
#include <ostream>

namespace lib_simplex_io {
   bool writeFunction(std::ostream& in);
   bool writeConstraint(std::ostream& in);
   bool writeSimplexTableu(std::ostream& in);
} // namespace lib_simplex_io

#endif // LIB_SIMPLEX_IO_WRITE_FILE_HPP

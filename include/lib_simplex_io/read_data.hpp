#ifndef LIB_SIMPLEX_IO_READ_FILE_HPP
#define LIB_SIMPLEX_IO_READ_FILE_HPP

#include "types.hpp"

namespace lib_simplex_io {
   ParsedFunction readFunction(std::istream& in,
                               const size_t buffer_size = 255);
   ParsedConstraint readConstraint(std::istream& in,
                                   const size_t buffer_size = 255);

} // namespace lib_simplex_io

#endif // LIB_SIMPLEX_IO_READ_FILE_HPP

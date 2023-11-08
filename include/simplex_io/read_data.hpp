#ifndef LIB_SIMPLEX_IO_READ_DATA
#define LIB_SIMPLEX_IO_READ_DATA

#include "types.hpp"

namespace simplex_io {
   ParsedFunction readFunction(std::istream& in,
                               const size_t buffer_size = 255);
   ParsedConstraint readConstraint(std::istream& in,
                                   const size_t buffer_size = 255);

} // namespace simplex_io

#endif // LIB_SIMPLEX_IO_READ_DATA

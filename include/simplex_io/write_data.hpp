#ifndef LIB_SIMPLEX_IO_WRITE_DATA
#define LIB_SIMPLEX_IO_WRITE_DATA
#include <ostream>

#include "types.hpp"
namespace simplex_io {
   std::ostream& operator<<(std::ostream& out,
                            const FunctionBase& function);
   std::ostream& operator<<(std::ostream& out,
                            const ConstraintBase& constraint);
   std::ostream& operator<<(std::ostream& out,
                            const SimplexTableau& tableau);
   bool writeFunction(std::ostream& out,
                      const FunctionBase& function);
   bool writeConstraint(std::ostream& out,
                        const ConstraintBase& constraint);
   bool writeSimplexTableau(std::ostream& out,
                            const SimplexTableau& tableau);
} // namespace simplex_io

#endif // LIB_SIMPLEX_IO_WRITE_DATA

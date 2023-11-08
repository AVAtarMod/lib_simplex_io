#ifndef LIB_SIMPLEX_IO_CONVERSION
#define LIB_SIMPLEX_IO_CONVERSION

#include "types.hpp"

namespace simplex_io {

   struct Problem
   {
      enum Type
      {
         COMMON,
         STANDARD,
         CANONICAL,
         EMPTY
      } type = COMMON;
      FunctionBase function;
      std::vector<ConstraintBase> constraints;
   };
   bool isStandardProblem(const Problem& problem);
   bool isCanonicalProblem(const Problem& problem);
   /**
    * @brief Convert problem to standard
    *
    * @param problem source problem
    * @return Problem standard problem or source if it not convertible
    */
   Problem convertToStandard(const Problem& problem);
   /**
    * @brief Convert problem to canonical
    *
    * @param problem source problem
    * @return Problem canonical problem or source if it not
    * convertible
    */
   Problem convertToCanonical(const Problem& problem);
} // namespace simplex_io

#endif // LIB_SIMPLEX_IO_CONVERSION

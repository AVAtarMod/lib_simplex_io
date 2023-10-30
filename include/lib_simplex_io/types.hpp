#ifndef LIB_SIMPLEX_IO_TYPES
#define LIB_SIMPLEX_IO_TYPES
#include <sstream>
#include <vector>

namespace lib_simplex_io {
   enum MinMaxType
   {
      MIN,
      MAX
   };
   enum ConstraintType
   {
      NONE,
      LESSER_OR_EQUAL,
      GREATER_OR_EQUAL,
      EQUAL,
   };
   struct FunctionBase
   {
      std::vector<std::pair<std::string, float>> function;
      MinMaxType function_type;
   };
   struct ConstraintBase
   {
      typedef std::vector<float> SingleConstraint;
      SingleConstraint constraint;
      ConstraintType constraint_type = NONE;
      /**
       * @brief Expression is non-negative constraint
       * for all variables. These constraints have following format:
       * x_1 >= 0
       * x_2 >= 0
       * ...
       * x_n >= 0
       * n - amount of variables in optimization problem
       */
      bool is_last = false;
   };

   struct ParsedFunction : FunctionBase
   {
      /**
       * @brief Result of parsing operation
       */
      bool success = true;
   };
   struct ParsedConstraint : ConstraintBase
   {
      /**
       * @brief Result of parsing operation
       */
      bool success = true;
   };
} // namespace lib_simplex_io

#endif // LIB_SIMPLEX_IO_TYPES

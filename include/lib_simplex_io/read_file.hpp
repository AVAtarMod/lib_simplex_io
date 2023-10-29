#ifndef LIB_SIMPLEX_IO_READ_FILE_HPP
#define LIB_SIMPLEX_IO_READ_FILE_HPP
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

   struct ParsedFunction
   {
      std::vector<std::pair<std::string, float>> function;
      MinMaxType function_type;
      bool success = true;
   };

   struct ParsedConstraint
   {
      typedef std::vector<float> SingleConstraint;
      SingleConstraint constraint;
      ConstraintType constraint_type = NONE;
      bool success = true;
      bool is_last = false;
   };

   ParsedFunction readFunction(std::istream& in,
                               const size_t buffer_size = 255);
   ParsedConstraint readConstraint(std::istream& in,
                                   const size_t buffer_size = 255);

} // namespace lib_simplex_io

#endif // LIB_SIMPLEX_IO_READ_FILE_HPP

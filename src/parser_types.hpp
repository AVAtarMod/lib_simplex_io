#ifndef LIB_SIMPLEX_IO_SRC_PARSER_TYPES_HPP
#define LIB_SIMPLEX_IO_SRC_PARSER_TYPES_HPP

#include <map>
#include <variant>

#include "types.hpp"

namespace simplex_io {
   constexpr std::string_view min_str = "->min", max_str = "->max";
   constexpr std::string_view equal_str = "=",
                              lesser_or_equal_str = "<=",
                              greater_or_equal = ">=";
   constexpr std::string_view comma = ",";
   constexpr std::string_view label_prefix = "x_";

   struct Coefficient
   {
      double val;
      Coefficient(double value = 0) : val(value) {};
   };
   struct Variable
   {
      std::string_view label;
   };
   enum ParsedSymbolType
   {
      TYPE_NULL,
      TYPE_COEFF = 1,
      TYPE_VARIABLE = (1 << 1),
      TYPE_MIN = (1 << 2),
      TYPE_MAX = (1 << 3),
      TYPE_CONSTRAINT = (1 << 4),
      TYPE_COMMENT = (1 << 5),
      TYPE_COMMA = (1 << 6),
   };
   typedef std::variant<Coefficient, Variable, MinMaxType,
                        ConstraintType, ParsedSymbolType>
     ParsedSymbol;

   constexpr std::string_view to_string(MinMaxType t)
   {
      switch (t) {
         case MIN:
            return min_str;
         case MAX:
            return max_str;
         default:
            break;
      }
      return "";
   }
   constexpr std::string_view to_string(ConstraintType t)
   {
      switch (t) {
         case LESSER_OR_EQUAL:
            return lesser_or_equal_str;
         case GREATER_OR_EQUAL:
            return greater_or_equal;
         case EQUAL:
            return equal_str;
         default:
            break;
      }
      return "";
   }
   inline size_t indexFromLabel(std::string_view label)
   {
      return std::atoi(label.begin() + label_prefix.size());
   }
   inline std::string labelFromIndex(const size_t index)
   {
      return label_prefix.data() + std::to_string(index);
   }
} // namespace simplex_io

#endif // LIB_SIMPLEX_IO_SRC_PARSER_TYPES_HPP

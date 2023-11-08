#include "write_data.hpp"
#include "parser_types.hpp"

#include <algorithm>
#include <iomanip>

namespace simplex_io {
   std::ostream& operator<<(std::ostream& out,
                            const FunctionBase& function)
   {
      writeFunction(out, function);
      return out;
   }
   std::ostream& operator<<(std::ostream& out,
                            const ConstraintBase& constraint)
   {
      writeConstraint(out, constraint);
      return out;
   }
   std::ostream& operator<<(std::ostream& out,
                            const SimplexTableau& tableau)
   {
      writeSimplexTableau(out, tableau);
      return out;
   }
   bool writeFunction(std::ostream& out, const FunctionBase& function)
   {
      const size_t size = function.function.size();
      try {
         for (size_t i = 0; i < size - 1; ++i) {
            out << function.function[i].second
                << function.function[i].first << " ";
         }
         out << to_string(function.function_type) << " "
             << function.function[size - 1].second;
      } catch (const std::exception& e) {
         return false;
      }
      return true;
   }
   bool writeConstraint(std::ostream& out,
                        const ConstraintBase& constraint)
   {
      const size_t size = constraint.constraint.size();
      try {
         if (constraint.is_last) {
            for (size_t i = 0; i < size - 2; ++i) {
               out << constraint.constraint[i] << comma << " ";
            }
            out << constraint.constraint[size - 2] << " "
                << to_string(constraint.constraint_type) << " "
                << constraint.constraint[size - 1];
         }
      } catch (const std::exception& e) {
         return false;
      }
      return true;
   }
   class InvalidTableuFormatException : std::exception
   {};

   std::string functionFromIndex(size_t i)
   {
      return "f" + std::string("'", i + 1);
   }
   std::string constructSpace(size_t length)
   {
      return std::string(length, ' ');
   }
   size_t getStrLength(size_t val)
   {
      return std::to_string(val).size();
   }
   typedef decltype(SimplexTableau::basis_variables_indexes)
     st_vector_t;
   typedef decltype(SimplexTableau::variable_count) st_count_t;
   st_vector_t invertBasisVariables(
     const st_vector_t& basis_variables, st_count_t total_variables)
   {
      size_t size = basis_variables.size();

      if (size >= total_variables)
         return {};
      auto it = basis_variables.cbegin();
      st_vector_t result;
      result.reserve(total_variables);
      for (size_t i = 0; i < total_variables; ++i) {
         st_count_t val =
           (it == basis_variables.cend()) ? total_variables : *it;
         if (i < val) {
            result.push_back(i);
         } else if (i == val) {
            ++it;
            continue;
         } else {
            throw std::runtime_error(
              "ERROR (invertBasisVariables): basis_variables contain invalid indexes");
         }
      }
      result.shrink_to_fit();
      return result;
   }
   typedef decltype(SimplexTableau::table) st_table_t;
   std::string getMaxOfIntegerPartStr(const st_table_t& table)
   {
      const size_t size = table.size();
      if (size == 0)
         return 0;
      using table_el_t = st_table_t::value_type::value_type;
      table_el_t max = 0;
      for (auto&& i : table) {
         if (i.size() == 0)
            return "";
         max = std::max(*std::max_element(i.cbegin(), i.cend()), max);
      }
      return std::to_string(static_cast<long>(max));
   }
   bool writeSimplexTableau(std::ostream& out,
                            const SimplexTableau& tableau)
   {
      const size_t size = tableau.table.size();
      size_t max_line_length = 0, index = 0;
      std::stringstream tmp_line;
      const size_t basis_vars_count = size - tableau.function_count;
      const size_t out_width = out.precision();
      try {
         if (tableau.basis_variables_indexes.size() !=
             basis_vars_count)
            throw InvalidTableuFormatException();
         size_t max_basis_len =
           labelFromIndex(*std::max_element(
                            tableau.basis_variables_indexes.cbegin(),
                            tableau.basis_variables_indexes.cend()))
             .size();
         {
            auto max_number_len =
              getMaxOfIntegerPartStr(tableau.table).size();
            size_t precision = (max_number_len == 1)
                                 ? out_width - max_number_len
                                 : out_width - max_number_len + 1;
            out << std::setprecision(precision);
         }

         out << constructSpace(max_basis_len) << " | ";
         for (auto&& i :
              invertBasisVariables(tableau.basis_variables_indexes,
                                   tableau.variable_count)) {
            out << std::setw(out_width) << labelFromIndex(i) << " | ";
         }
         out << "free val" << '\n';

         int coefs_size = 0, prev_coefs_length = -1;
         for (size_t i = 0; i < basis_vars_count; ++i) {
            coefs_size = tableau.table[i].size();
            if (prev_coefs_length != -1 &&
                prev_coefs_length != coefs_size)
               throw InvalidTableuFormatException();

            out << std::setw(max_basis_len)
                << labelFromIndex(tableau.basis_variables_indexes[i])
                << " | ";
            for (size_t coef_i = 0; coef_i < coefs_size - 1;
                 ++coef_i) {
               out << std::setw(out_width) << tableau.table[i][coef_i]
                   << " | ";
            }
            out << std::setw(out_width)
                << tableau.table[i][coefs_size - 1] << '\n';
            prev_coefs_length = coefs_size;
         }
         for (size_t i = basis_vars_count; i < size; ++i) {
            out << std::setw(max_basis_len)
                << functionFromIndex(i - basis_vars_count) << " | ";
            coefs_size = tableau.table[i].size();
            for (size_t coef_i = 0; coef_i < coefs_size - 1;
                 ++coef_i) {
               out << std::setw(out_width) << tableau.table[i][coef_i]
                   << " | ";
            }
            out << std::setw(out_width)
                << tableau.table[i][coefs_size - 1] << '\n';
         }
      } catch (const InvalidTableuFormatException& ue) {
         out << "[invalid SimplexTableau (rows with different size)]";
         return false;
      } catch (...) {
         return false;
      }
      return true;
   }
} // namespace simplex_io

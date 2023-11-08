#include "conversion.hpp"

using namespace simplex_io;

bool isAllVarsNonnegative(
  const decltype(Problem::constraints)& constraints)
{
   const size_t size = constraints.size();
   if (size < 1)
      return false;
   if (constraints[size - 1].is_last &&
       constraints[size - 1].constraint.size() !=
         constraints[0].constraint.size())
      return false;
   const auto& e = constraints[size - 1].constraint;
   for (size_t i = 0; i < e.size(); ++i) {
      if (e[i] != 1)
         return false;
   }
   return true;
}

bool simplex_io::isStandardProblem(const Problem& problem)
{
   bool is_min = problem.function.function_type == MIN;
   bool is_correct_constr = true;
   const size_t size = problem.constraints.size();
   if (size > 2) {
      for (size_t i = 0; i < size; ++i) {
         const auto& e = problem.constraints[i];
         try {
            if (e.constraint_type != ConstraintType::EQUAL)
               throw -1;
            const size_t constr_size = e.constraint.size();
            if (constr_size < 2)
               throw -1;
            if (e.constraint[constr_size - 1] < 0)
               throw -1;
         } catch (int error) {
            is_correct_constr = false;
            break;
         }
      }
   } else
      is_correct_constr = false;
   return is_min && is_correct_constr &&
          isAllVarsNonnegative(problem.constraints);
}
bool simplex_io::isCanonicalProblem(const Problem& problem)
{
   const size_t size = problem.constraints.size();
   auto& c = problem.constraints;
   if (size < 1 || size != c[0].constraint.size())
      return false;
   const size_t c_size = c[0].constraint.size();
   std::vector<bool> basis_var(c_size);
   for (size_t coef_i = 0; coef_i < c_size; ++coef_i) {
      std::vector<size_t> nonzero_indexes;
      if (problem.function.function[coef_i].second != 0)
         nonzero_indexes.push_back(0);
      for (size_t i = 0; i < size - 1; ++i) {
         if (c[i].constraint[coef_i] != 0)
            nonzero_indexes.push_back(i + 1);
      }
      if (nonzero_indexes.size() == 1 && nonzero_indexes[0] != 0)
         basis_var[nonzero_indexes[0]] = true;
   }
   size_t basis_count = 0;
   for (size_t i = 0; i < c_size; ++i) {
      if (basis_var[i])
         ++basis_count;
   }
   return isStandardProblem(problem) && (basis_count == size - 1);
}
Problem simplex_io::convertToStandard(const Problem& problem)
{
   if (isStandardProblem(problem)) {
      if (problem.type != Problem::STANDARD) {
         Problem result = problem;
         result.type = Problem::STANDARD;
         return result;
      } else
         return problem;
   }

   Problem result = problem;
   if (result.function.function_type == MAX) {
      const size_t size = result.constraints.size();
      if (size < 2)
         throw std::runtime_error(
           "ERROR (simplex_io::convertToStandard): Invalid problem received, constraints count < 2");
      for (size_t i = 0; i < size - 1; ++i) {
         auto& e = result.constraints[i];
         size_t e_size = e.constraint.size();
         if (e.constraint_type == EQUAL &&
             e.constraint[e_size - 1] < 0) {
            for (size_t e_i = 0; e_i < e_size - 1; ++e_i)
               e.constraint[e_i] *= -1;
         } else if (e.constraint_type == LESSER_OR_EQUAL) {
            e.constraint.insert(e.constraint.cend() - 1, 1);
            for (size_t c_i = 0; c_i < size - 1; ++c_i) {
               if (c_i == i)
                  continue;
               result.constraints[c_i].constraint.push_back(0);
            }
            result.constraints[size - 1].constraint.push_back(1);
         } else if (e.constraint_type == GREATER_OR_EQUAL) {
            e.constraint.insert(e.constraint.cend() - 1, -1);
            for (size_t c_i = 0; c_i < size - 1; ++c_i) {
               if (c_i == i)
                  continue;
               result.constraints[c_i].constraint.push_back(0);
            }
            result.constraints[size - 1].constraint.push_back(1);
         }
      }
      result.type = Problem::STANDARD;
   }
   return result;
}
Problem simplex_io::convertToCanonical(const Problem& problem)
{
   if (isCanonicalProblem(problem)) {
      if (problem.type != Problem::CANONICAL) {
         Problem result = problem;
         result.type = Problem::CANONICAL;
         return result;
      } else
         return problem;
   }
   Problem result = convertToStandard(problem);
   if (isCanonicalProblem(result)) {
      result.type = Problem::CANONICAL;
      return result;
   }
   return problem;
}

#ifndef LIB_SIMPLEX_IO_READ_FILE_HPP
#define LIB_SIMPLEX_IO_READ_FILE_HPP
#include <vector>
#include <sstream>

struct ParsedFunction
{
   std::vector<std::pair<std::string, float>> function;
   SimplexMethodSolver::MinMaxType function_type;
   bool success = true;
};

struct ParsedConstraint
{
   typedef std::vector<float> SingleConstraint;
   SingleConstraint constraint;
   SimplexMethodSolver::ConstraintType constraint_type =
       SimplexMethodSolver::NONE;
   bool success = true;
   bool is_last = false;
};

ParsedFunction readFunction(std::istream &in,
                            const size_t buffer_size = 255);
ParsedConstraint readConstraint(std::istream &in,
                                const size_t buffer_size = 255);

#endif // LIB_SIMPLEX_IO_READ_FILE_HPP

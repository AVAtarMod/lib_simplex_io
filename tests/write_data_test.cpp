#include <gtest/gtest.h>

#include "simplex_io/write_data.hpp"

TEST(WriteData, test1){
   using namespace lib_simplex_io;
   SimplexTableau t;
   t.basis_variables_indexes = { 1, 2 };
   t.variable_count = 4;
   t.function_count = 1;
   t.table = { { 1, 2, 3 },
               { 99999999.123456, 0.2, -4 },
               { -1, -2, -4 } };
   // std::cout << std::setprecision(1);
   writeSimplexTableau(std::cout, t);
}

#include <gtest/gtest.h>

#include "simplex_io/read_data.hpp"

#include <sstream>
#include <string_view>

template<size_t N>
bool compare_function(
  const std::array<std::pair<std::string_view, float>, N>&
    constexpr_f,
  const decltype(simplex_io::ParsedFunction::function)& runtime_f)
{
   if (constexpr_f.size() != runtime_f.size())
      return false;
   for (size_t i = 0; i < constexpr_f.size(); ++i) {
      auto& arr_i = constexpr_f[i];
      auto& vec_i = runtime_f[i];
      if (std::string(arr_i.first) != vec_i.first ||
          arr_i.second != vec_i.second)
         return false;
   }
   return true;
}

TEST(ReadData, read_problem1)
{
   using namespace simplex_io;
   constexpr std::string_view problem1 = R"(
   1 -1 ->max
   -1 +2 >=-2
   1 +1 <=8
   0 +1 <=4
   1,1 >=0)";
   constexpr size_t constr_count = 4, last_constr_index = 3;
   std::stringstream s;
   ParsedFunction f;
   std::array<ParsedConstraint, constr_count> constr_arr;
   constexpr std::array<std::pair<std::string_view, float>, 2>
     coefs = { { { "x_1", 1.0 }, { "x_2", -1.0 } } };

   s.str(problem1.data());
   f = readFunction(s);
   for (size_t i = 0; i < constr_count; ++i) {
      constr_arr[i] = readConstraint(s);
   }

   EXPECT_EQ(f.function_type, MinMaxType::MAX);
   EXPECT_TRUE(compare_function(coefs, f.function));
   for (size_t i = 0; i < constr_count; ++i) {
      EXPECT_TRUE(constr_arr[i].success);
      if (i == last_constr_index)
         EXPECT_TRUE(constr_arr[i].is_last);
      else
         EXPECT_FALSE(constr_arr[i].is_last);
   }
}

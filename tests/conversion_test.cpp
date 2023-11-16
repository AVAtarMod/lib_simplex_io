#include <gtest/gtest.h>

#include "simplex_io/conversion.hpp"

using namespace simplex_io;

class Conversion : public testing::Test
{
  public:
   Problem non_std1, non_std2;
   Problem converted_non_std1, converted_non_std2;
   void SetUp()
   {
      {
         FunctionBase f = {
            { { "x_0", -4.0 }, { "x_1", 2.0 }, { "x_2", -1.0 } }, MAX
         };
         std::vector<ConstraintBase> c = {
            { { 8, -3, 4, -16 }, EQUAL },
            { { 1, -4, 0, 5 }, GREATER_OR_EQUAL },
            { { 18, 0, 11, 24 }, LESSER_OR_EQUAL },
            { { 1, 1, 1 }, GREATER_OR_EQUAL, true }
         };
         non_std1 = { Problem::COMMON, f, c };
      }
      {
         FunctionBase f = { { { "x_0", 1 }, { "x_1", -1 } }, MAX };
         std::vector<ConstraintBase> c = {
            { { -1, 2, -2 }, GREATER_OR_EQUAL },
            { { 1, 1, 8 }, LESSER_OR_EQUAL },
            { { 0, 1, 4 }, LESSER_OR_EQUAL },
            { { 1, 1 }, GREATER_OR_EQUAL, true }
         };
         non_std2 = { Problem::COMMON, f, c };
      }
      {
         FunctionBase f = {
            { { "x_0", 4.0 }, { "x_1", -2.0 }, { "x_2", 1.0 } }, MIN
         };
         std::vector<ConstraintBase> c = {
            { { -8, 3, -4, 0, 0, 16 }, EQUAL },
            { { 1, -4, 0, -1, 0, 5 }, EQUAL },
            { { 18, 0, 11, 0, 1, 24 }, EQUAL },
            { { 1, 1, 1, 1, 1 }, GREATER_OR_EQUAL, true }
         };
         converted_non_std1 = { Problem::STANDARD, f, c };
      }
      {
         FunctionBase f = { { { "x_0", -1 }, { "x_1", 1 } }, MIN };
         std::vector<ConstraintBase> c = {
            { { 1, -2, 1, 0, 0, 2 }, EQUAL },
            { { 1, 1, 0, 1, 0, 8 }, EQUAL },
            { { 0, 1, 0, 0, 1, 4 }, EQUAL },
            { { 1, 1, 1, 1, 1 }, GREATER_OR_EQUAL, true }
         };
         converted_non_std2 = { Problem::CANONICAL, f, c };
      }
   }
};

TEST_F(Conversion, isStandard1False)
{
   bool result = isStandardProblem(non_std1);

   ASSERT_FALSE(result);
}

TEST_F(Conversion, isStandard2False)
{
   bool result = isStandardProblem(non_std2);

   ASSERT_FALSE(result);
}

TEST_F(Conversion, toStandard1)
{
   Problem result = convertToStandard(non_std1);

   ASSERT_NE(result, non_std1);
   ASSERT_EQ(result, converted_non_std1);
}

TEST_F(Conversion, toStandard2)
{
   Problem result = convertToStandard(non_std2);
   Problem tmp = converted_non_std2;
   tmp.type = Problem::STANDARD;

   ASSERT_NE(result, non_std2);
   ASSERT_EQ(result, tmp);
}

TEST_F(Conversion, toCanonical1)
{
   Problem result = convertToCanonical(non_std1);

   ASSERT_EQ(result, non_std1);
}

TEST_F(Conversion, toCanonical2)
{
   Problem result = convertToCanonical(non_std2);

   ASSERT_NE(result, non_std2);
   ASSERT_EQ(result, converted_non_std2);
}

TEST_F(Conversion, getBasisVariables1)
{
   std::vector<size_t> expected = { 3, 4 };

   auto basis = getBasisVariablesVector(converted_non_std1);

   ASSERT_EQ(basis, expected);
}

TEST_F(Conversion, getBasisVariables2)
{
   std::vector<size_t> expected = { 2, 3, 4 };

   auto basis = getBasisVariablesVector(converted_non_std2);

   ASSERT_EQ(basis, expected);
}

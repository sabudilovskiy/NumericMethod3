#include <gtest/gtest.h>
#include <math/matrix.hpp>

TEST(MatrixTests, Multiple){
    math::Matrix<> left{
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12}
    };
    math::Matrix<> right{
            {2, 4},
            {2, 5},
            {6, 2},
            {4, 9}
    };
    math::Matrix<> expected{
            {40, 56},
            {96, 136},
            {152, 216}
    };
    math::Matrix<> result = left * right;
    ASSERT_TRUE(result == expected);
}
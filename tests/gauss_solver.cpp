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

TEST(MatrixTests, MultipleCf){
    math::Matrix<> left{
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12}
    };
    double cf = 4;
    math::Matrix<> expected{
            {4, 8, 12, 16},
            {20, 24, 28, 32},
            {36, 40, 44, 48}
    };
    math::Matrix<> result = left * cf;
    ASSERT_TRUE(result == expected);
}

TEST(MatrixTests, Transposition){
    math::Matrix<> left{
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12}
    };
    math::Matrix<> expected{
            {1, 5, 9},
            {2, 6, 10},
            {3, 7, 11},
            {4, 8, 12}
    };
    math::Matrix<> result = left.Transposition();
    ASSERT_TRUE(result == expected);
}
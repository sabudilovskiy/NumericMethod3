#ifndef NUMERIC_METHODS2_MATH_SOLVER
#define NUMERIC_METHODS2_MATH_SOLVER
#include <iostream>
#include <type_traits>
#include "matrix.hpp"
#include <cassert>
#include <cmath>

//m*n X n*p -> m*p
//n*n -> n*1 = n*1
//1*n X n*n -> 1*n
namespace math {
    double Abs(Matrix<>& matrix){
        double sum = 0;
        for (size_t i = 0; i < matrix.nRows(); i++){
            for (size_t j = 0; j < matrix.nColumns(); j++){
                sum += abs(matrix[i][j]);
            }
        }
        return sum;
    }

    auto Normalized(Matrix<>& matrix){
        return matrix/Abs(matrix);
    }

    struct Solver{
        size_t N_;
        Matrix<> A;
        double previous_lambda_;
        double lambda_;
        Matrix<> eigen_vector_;
        Matrix<> previous_eigen_vector_;
        double kEpsEigenVector_;
        double kEpsEigenLambda_;
        size_t kMaxCountIterations_;
        size_t count_iteration;
        Solver(Matrix<> matrix,
               size_t N,
               double lambda,
               Matrix<> eigen_vector,
               double kEpsEigenVector,
               double kEpsEigenLambda,
               size_t kMaxCountIterations_) :
               lambda_(lambda),
               eigen_vector_(std::move(eigen_vector)),
               kEpsEigenLambda_{kEpsEigenLambda},
               kEpsEigenVector_{kEpsEigenVector},
               kMaxCountIterations_(kMaxCountIterations_),
               N_(N_)
               {
                    A = matrix - eigen_vector * eigen_vector_.Transposition() * lambda_;
               }
        void OneStep(){
            previous_eigen_vector_ = std::move(eigen_vector_);
            auto v = Normalized(previous_eigen_vector_);
            eigen_vector_ = A * v;
            previous_lambda_ = lambda_;
            lambda_ = (v.Transposition() * eigen_vector_)(0, 0);
            count_iteration++;
        }
        double CountEpsLambda(){
            return abs(previous_lambda_ - lambda_);
        }
        double CountEpsVector(){
            double max = abs(previous_eigen_vector_[0][0] - eigen_vector_[0][0]);
            for (size_t index = 0; index < N_; index++){
                double temp = abs(previous_eigen_vector_[index][0] - eigen_vector_[index][0]);
                if (temp > max) max = temp;
            }
            return max;
        }
        void Solve(){
            do{
                OneStep();
            }
            while (CountEpsVector() >= kEpsEigenVector_
            && CountEpsLambda() >= kEpsEigenLambda_
            && count_iteration < kMaxCountIterations_);
        }
    };
}
#endif
#ifndef NUMERIC_METHODS2_MATH_SOLVER
#define NUMERIC_METHODS2_MATH_SOLVER
#include <iostream>
#include <type_traits>
#include "matrix.hpp"
#include <cassert>
#include <cmath>
#include <random>

//m*n X n*p -> m*p
//n*n -> n*1 = n*1
//1*n X n*n -> 1*n*
namespace math {
    template<typename Number>
    struct Traits{
        Number kMin;
        Number kMax;
        double kEpsEigenVector;
        double kEpsEigenLambda;
        size_t kMaxCountIterations;
    };

    enum struct RandomSeed{
        Yes, No
    };

    template<typename Number,
            Traits<Number> traits,
            RandomSeed kRandomSeed = RandomSeed::No,
            typename Distribution = std::uniform_int_distribution<Number>>
    class Solver{
        static constexpr auto kMin = traits.kMin;
        static constexpr auto kMax = traits.kMax;
        static constexpr auto kEpsEigenVector = traits.kEpsEigenVector;
        static constexpr auto kEpsEigenLambda = traits.kEpsEigenLambda;
        static constexpr auto kMaxCountIterations = traits.kMaxCountIterations;
        size_t N_;
        Matrix<> A;
        double previous_lambda_;
        double lambda_;
        Matrix<> get_vector_;
        Matrix<> eigen_vector_;
        Matrix<> previous_eigen_vector_;
        size_t count_iteration = 0;
        void OneStep(){
            previous_eigen_vector_ = std::move(eigen_vector_);
            auto v = Normalized(previous_eigen_vector_);
            eigen_vector_ = A * v;
            previous_lambda_ = lambda_;
            auto temp = v.Transposition() * eigen_vector_;
            lambda_ = temp[0][0];
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
    public:
        void Solve(){
            double cur_eps_vector;
            double cur_eps_eigen_lambda;
            do{
                OneStep();
                cur_eps_eigen_lambda = CountEpsLambda();
                cur_eps_vector = CountEpsVector();
            }
            while (cur_eps_vector > kEpsEigenVector
            && cur_eps_eigen_lambda > kEpsEigenLambda
            && count_iteration < kMaxCountIterations);
        }
        //previous_lambda_, lambda_, get_vector_, eigen_vector_, previous_eigen_vector_, count_iteration
        decltype(auto) GetAll(){
            return std::tie(previous_lambda_, lambda_, get_vector_, eigen_vector_, previous_eigen_vector_, count_iteration);
        }
        Solver(size_t N,
               Matrix<> matrix,
               double lambda,
               Matrix<> get_vector) :
                N_(N),
                lambda_(lambda),
                get_vector_(std::move(get_vector)),
                eigen_vector_(N_, 1)
        {
            static_assert(kMin < kMax, "минимум должен быть меньше максимума");
            Distribution distribution_{kMin, kMax};
            std::mt19937 number_generator_{kRandomSeed == RandomSeed::Yes ? std::random_device{}() : 0};
            for (size_t index = 0; index < N_; index++){
                eigen_vector_[index][0] = distribution_(number_generator_);
            }
            A = matrix - get_vector * get_vector.Transposition() * lambda_;
        }
    };
}
#endif
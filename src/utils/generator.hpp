//
// Created by MRV on 19.02.2023.
//

#ifndef NUMERICMETHODS1_GENERATOR_HPP
#define NUMERICMETHODS1_GENERATOR_HPP

#include <random>
#include <array>
#include <functional>
#include "math/matrix.hpp"

namespace utils {
    template<typename Number>
    struct Traits{
        Number kMin;
        Number kMax;
        size_t kSize;
    };

    enum struct RandomSeed{
        Yes, No
    };

    template<typename Number,
            Traits<Number> traits,
            RandomSeed kRandomSeed = RandomSeed::No,
            typename Distribution = std::uniform_real_distribution<Number>>
    class Generator final {
    public:
        static constexpr inline auto kSize = traits.kSize;
        static constexpr inline auto kMin = traits.kMin;
        static constexpr inline auto kMax = traits.kMax;
    protected:
        void GenerateVector() {
            vector_ = math::Matrix<>(kSize, 1);
            for (size_t i = 0; i < kSize; i++){
                vector_[i][0] = GenerateNumber();
            }
            vector_ = math::Normalized(vector_);
        }
        void GenerateHouseHolderMatrix(){
            house_holder_matrix_ = math::MakeIdentityMatrix<>(kSize) - vector_ * vector_.Transposition() * 2;
        }
        void GenerateDiagonalMatrix(){
            diagonal_matrix_ = math::Matrix<>{kSize};
            for (size_t i = 0; i < kSize; i++){
                diagonal_matrix_[i][i] = GenerateNumber();
            }
        }
        void GenerateResultMatrix(){
            result_matrix_ = house_holder_matrix_ * diagonal_matrix_ * house_holder_matrix_.Transposition();
        }
        Number GenerateNumber() {
            Number number = distribution_(number_generator_);
            while (number == Number{}){
                number = distribution_(number_generator_);
            }
            return number;
        }
    public:
        Generator() {
            static_assert(kMin < kMax, "минимум должен быть меньше максимума");
        }
        void GenerateAll(){
            GenerateVector();
            GenerateHouseHolderMatrix();
            GenerateDiagonalMatrix();
            GenerateResultMatrix();
        }
        //vector, Householder matrix,diagonal matrix, result matrix
        decltype(auto) GetAll() const {
            return std::tie(vector_, house_holder_matrix_, diagonal_matrix_, result_matrix_);
        }
    private:
        math::Matrix<> vector_;
        math::Matrix<> house_holder_matrix_;
        math::Matrix<> diagonal_matrix_;
        math::Matrix<> result_matrix_;
        Distribution distribution_{kMin, kMax};
        std::mt19937 number_generator_{kRandomSeed == RandomSeed::Yes ? std::random_device{}() : 0};
    };
}

#endif //NUMERICMETHODS1_GENERATOR_HPP

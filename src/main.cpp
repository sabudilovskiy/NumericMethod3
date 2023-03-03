#include <iostream>
#include "math/solver.hpp"
#include "utils/generator.hpp"
#include <optional>

template <typename Solution>
void PrintGeneratedSolution(Solution&& solution){
    std::cout << "Сгенерированное решение: \n";
    for (auto& x : solution){
        std::cout << x << ' ';
    }
    std::cout << '\n';
}

template<typename Result>
void PrintGenerateResult(Result&& result){
    const auto& [matrix, free_column, solution] = result;
    std::cout << "Сгенерированная матрица: \n";
    for (auto& row: matrix) {
        for (auto elem : row) std::cout << elem << ' ';
        std::cout << '\n';
    }
    std::cout << "Сгенерированный столбец свободных членов: \n";
    for (auto& elem_column : free_column){
        std::cout << elem_column << '\n';
    }
    PrintGeneratedSolution(solution);
}

template <typename Solution>
void PrintSolution(Solution&& solution){
    std::cout << "Решение: \n";
    for (auto& x : solution){
        std::cout << x << ' ';
    }
    std::cout << '\n';
}

//template<typename From>
//math::Matrix ToMatrix(From&& from) {
//    math::Matrix matrix;
//    const auto &[arr, free_column, solution] = from;
//    matrix.n = arr.size();
//    matrix.arr_.resize(matrix.n);
//    matrix.free_col.resize(matrix.n);
//    for (size_t i = 0; i < matrix.n; ++i){
//        matrix.arr_[i].resize(matrix.n);
//        matrix.free_col[i] = free_column[i];
//        for (size_t j = 0; j < matrix.n; j++){
//            matrix.arr_[i][j] = arr[i][j];
//        }
//    }
//    return matrix;
//}
template <typename TMatrix>
auto FindMax(TMatrix&& matrix) {
    auto max = matrix[0][0];
    std::optional<decltype(max)> max2;
    for (size_t i = 1; i < matrix.nRows(); i++)
        if (auto& elem = matrix[i][i]; elem > max){
            max2 = max;
            max = elem;
        }
        else if (elem > max2 || !max2.has_value()){
            max2 = elem;
        }
    return std::make_tuple(max, max2.value());
}
template <typename TMatrix>
auto FindMaxVector(TMatrix&& matrix) {
    size_t max = 0;
    size_t max2;
    assert(matrix.nRows() >= 2);
    double max_value = math::Abs(matrix[0]);
    std::optional<double> max2_value;
    for (size_t index = 1; index < matrix.nRows(); index++){
        if (auto value = math::Abs(matrix[index]); value > max_value){
            max2 = max;
            max = index;
        }
        else if (!max2_value.has_value() || value > max2_value){
            max2 = index;
            max2_value = math::Abs(matrix[index]);
        }
    }
    return std::make_tuple(max, max2);
}

template<typename TMatrix>
auto GetVector(TMatrix&& matrix, size_t index){
    math::Matrix<> result(matrix.nRows(), 1);
    for (size_t i = 0; i < matrix.nRows(); i++){
        result[i][0] = matrix[i][index];
    }
    return result;
}

#define Print(matrix) std::cout << #matrix << '\n'; \
std::cout << matrix << '\n';
int main() {
    constexpr auto range = -10.0;
    constexpr utils::Traits<double> traits{
        .kMin = -10,
        .kMax = 10,
        .kSize = 3
    };
    utils::Generator<double, traits, utils::RandomSeed::No> generator;
    generator.GenerateAll();
    const auto& [vector, house_m, diag_m, result_m] = generator.GetAll();
    auto [lambda, lambda2] = FindMax(diag_m);
    auto [max_vector, max2_vector ] = FindMaxVector(house_m);
    Print(vector);
    Print(house_m);
    Print(diag_m);
    Print(result_m);
    Print(lambda);
    Print(lambda2);
    Print(GetVector(result_m, max_vector));
    Print(GetVector(result_m, max2_vector));
    constexpr math::Traits<double> traits2{
        .kMin = traits.kMin,
        .kMax = traits.kMax,
        .kEpsEigenVector = 0.00000000000001,
        .kEpsEigenLambda = 0.00000000000001,
        .kMaxCountIterations = 1000
    };
    math::Solver<double, traits2> solver(traits.kSize,
                                      result_m,
                                      lambda,
                                      GetVector(result_m, max_vector));
    solver.Solve();
    const auto& [previous_lambda_,
                 lambda_,
                 get_vector_,
                 eigen_vector_,
                 previous_eigen_vector_,
                 count_iteration] = solver.GetAll();
    Print(previous_lambda_);
    Print(lambda_);
    Print(get_vector_);
    Print(previous_eigen_vector_);
    Print(math::Normalized(previous_eigen_vector_));
    Print(eigen_vector_);
    Print(math::Normalized(eigen_vector_));
    std::cout << "count iteration: " << count_iteration;

}

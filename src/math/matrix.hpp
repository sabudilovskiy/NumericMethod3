#pragma once
#include <utility>
#include <cassert>
#include <iostream>
#include <vector>
#include <type_traits>
#include <algorithm>
#include <cmath>
#include <initializer_list>

namespace math {

    template<typename T, typename U>
    concept IsSummable = requires(T left, U right) {
        { left + right };
    };

    template<typename T, typename U>
    concept IsDeductible = requires(T left, U right) {
        { left - right };
    };

    template<typename T, typename U>
    concept IsMultiplied = requires(T left, U right) {
        { left * right };
    };

    template<typename T, typename U>
    concept IsAssignable = requires(T &left, U right) {
        { left = right };
    };
    template<typename T>
    concept IsAccumalated = requires(T &left, T right) {
        { left += right };
    };


    template<typename Info = double>
    class Matrix {
    private:
        size_t rows_count_;
        size_t columns_count_;
        std::vector<std::vector<Info>> m_cells;
        void AllocateCells(int, int);

    public:
        Matrix() : rows_count_(0), columns_count_(0) {}
        Matrix(size_t size){
            AllocateCells(size, size);
        }
        Matrix(const Matrix &);

        Matrix(int, int);
        Matrix(std::initializer_list<std::initializer_list<Info>> list){
            size_t i = 0;

            rows_count_ = list.size();
            assert(list.size() != 0);
            columns_count_ = list.begin()->size();
            AllocateCells(rows_count_, columns_count_);
            for (auto& row: list){
                size_t j = 0;
                for (auto& elem : row){
                    m_cells[i][j] = elem;
                    j++;
                }
                i++;
            }
        }
        friend bool operator==(const Matrix& left, const Matrix& right){
            if (left.nRows() != right.rows_count_ || left.nColumns() != right.columns_count_){
                return false;
            }
            for (size_t i = 0; i < left.rows_count_; i++){
                for (size_t j = 0; j < left.columns_count_; j++){
                    if (left.m_cells[i][j] != right.m_cells[i][j]){
                        return false;
                    }
                }
            }
            return true;
        }
        bool operator!=(const Matrix& matrix){
            return !(*this == matrix);
        }
        Matrix(int n_nRows, int n_nCols, Info *list) {
            int k = 0;
            AllocateCells(n_nRows, n_nCols);
            for (int i = 0; i < n_nRows; i++)
                for (int j = 0; j < n_nCols; j++) {
                    m_cells[i][j] = list[k];
                    k++;
                }
        }

        size_t nRows() const;

        size_t nColumns() const;

        decltype(auto) operator[](size_t i) {
            return m_cells[i];
        }

        decltype(auto) operator[](size_t i) const {
            return m_cells[i];
        }

        auto Transposition() const {
            auto &matrix = *this;
            Matrix t_matrix(matrix.columns_count_, matrix.rows_count_);
            for (size_t i = 0; i < matrix.rows_count_; i++)
                for (size_t j = 0; j < matrix.columns_count_; j++)
                    t_matrix[j][i] = matrix[i][j];
            return t_matrix;
        }

        Matrix &operator=(Matrix &&another) {
            std::swap(m_cells, another.m_cells);
            std::swap(this->rows_count_, another.rows_count_);
            std::swap(this->columns_count_, another.columns_count_);
            return *this;
        }

        template<typename RCell>
        requires math::IsAssignable<Info, RCell>
        Matrix<Info> &operator=(const Matrix<RCell> &right) {
            if (columns_count_ != right.columns_count_ || rows_count_ != right.rows_count_) {
                throw std::invalid_argument("loh");
            }
            for (int i = 0; i < rows_count_; i++)
                for (int j = 0; j < columns_count_; j++)
                    m_cells[i][j] = right.m_cells[i][j];
        }

        template<typename LCell, typename RCell>
        requires math::IsSummable<LCell, RCell>
        friend auto operator+(const Matrix<LCell> &left, const Matrix<RCell> &right) {
            if (left.rows_count_ != right.rows_count_ || left.columns_count_ != right.columns_count_)
                throw std::invalid_argument("loh");
            Matrix<decltype(std::declval<LCell>() + std::declval<RCell>())> res(left.rows_count_, left.columns_count_);
            for (int i = 0; i < left.rows_count_; i++)
                for (int j = 0; j < left.columns_count_; j++)
                    res.m_cells[i][j] = left.m_cells[i][j] + right.m_cells[i][j];
            return res;
        }

        template<typename LCell, typename RCell>
        requires math::IsDeductible<LCell, RCell>
        friend auto operator-(const Matrix<LCell> &left, const Matrix<RCell> &right) {
            if (left.rows_count_ != right.rows_count_ || left.columns_count_ != right.columns_count_)
                throw std::invalid_argument("loh");
            Matrix<decltype(std::declval<LCell>() - std::declval<RCell>())> res(left.rows_count_, left.columns_count_);
            for (int i = 0; i < res.rows_count_; i++)
                for (int j = 0; j < res.columns_count_; j++)
                    res.m_cells[i][j] = left.m_cells[i][j] - right.m_cells[i][j];
            return res;
        }

        template<typename Cell, typename Denominator>
        requires math::IsDeductible<Cell, Denominator>
        friend auto operator/(Matrix<Cell> &left, const Denominator &denominator) {
            auto copy = left;
            for (auto &row: copy.m_cells) {
                for (auto &elem: row) {
                    elem = elem / denominator;
                }
            }
            return copy;
        }

        template<typename Cell, typename Denominator>
        requires math::IsDeductible<Cell, Denominator>
        friend auto operator/=(Matrix<Cell> &left, const Denominator &denominator) {
            left = left / denominator;
            return left;
        }

        template<typename LCell, typename RCell>
        requires math::IsMultiplied<LCell, RCell>
        friend auto operator*(const Matrix<LCell> &left, const Matrix<RCell> &right) {
            if (left.columns_count_ != right.rows_count_) throw std::invalid_argument("loh");
            Matrix<decltype(std::declval<LCell>() * std::declval<RCell>())> res(left.rows_count_, right.columns_count_);
            {
                auto n = left.rows_count_;
                auto m = left.columns_count_;
                auto p = right.columns_count_;
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < p; j++) {
                        res.m_cells[i][j] = 0;
                        for (int k = 0; k < m; k++) {
                            res.m_cells[i][j] += left.m_cells[i][k] * right.m_cells[k][j];
                        }

                    }
                }

            }
            return res;
        }

        template<typename Cell, typename Right>
        requires math::IsMultiplied<Cell, Right>
        friend auto operator*(const Matrix<Cell> &left, const Right &right) {
            Matrix<decltype(std::declval<Cell>() * std::declval<Right>())> res(left.rows_count_, left.columns_count_);
            {
                for (int i = 0; i < left.rows_count_; i++) {
                    for (int j = 0; j < left.columns_count_; j++) {
                        res.m_cells[i][j] = left.m_cells[i][j] * right;
                    }
                }
            }
            return res;
        }

        template<typename Left, typename Cell>
        requires math::IsMultiplied<Left, Cell>
        friend auto operator*(const Left &left, const Matrix<Cell> &right) {
            Matrix<decltype(std::declval<Left>() * std::declval<Cell>())> res(right.rows_count_, right.columns_count_);
            {
                for (int i = 0; i < right.rows_count_; i++) {
                    for (int j = 0; j < right.columns_count_; j++) {
                        res.m_cells[i][j] = left * right.m_cells[i][j];
                    }
                }
            }
            return res;
        }

        friend std::istream &operator>>(std::istream & in , Matrix & M){
            for (int i = 0; i < M.rows_count_; i++)
                for (int j = 0; j < M.columns_count_; j++)
                    in >> M.m_cells[i][j];
            return in;
        }

        friend std::ostream &operator<<(std::ostream & out, const Matrix & M){
            for (int i = 0; i < M.rows_count_; i++) {
                for (int j = 0; j < M.columns_count_; j++)
                    out << M.m_cells[i][j] << '\t';
                out << '\n';
            }
            return out;
        }
    };

    template<typename Info>
    Matrix<Info>::Matrix(const Matrix<Info> &M) {
        AllocateCells(M.rows_count_, M.columns_count_);
        for (int i = 0; i < rows_count_; i++)
            for (int j = 0; j < columns_count_; j++)
                m_cells[i][j] = M.m_cells[i][j];
    }

    template<typename Info>
    Matrix<Info>::Matrix(int n_nRows, int n_nCols) {
        AllocateCells(n_nRows, n_nCols);
        for (int i = 0; i < n_nRows; i++)
            for (int j = 0; j < n_nCols; j++)
                m_cells[i][j] = 0;
    }


    template<typename Info>
    void Matrix<Info>::AllocateCells(int nRows, int nCols) {
        rows_count_ = nRows;
        columns_count_ = nCols;

        m_cells.resize(nRows);
        std::for_each(
                m_cells.begin(),
                m_cells.end(),
                [nCols](std::vector<Info> &elem) { elem.resize(nCols); });
    }

    template<typename Info>
    size_t Matrix<Info>::nRows() const {
        return rows_count_;
    }

    template<typename Info>
    size_t Matrix<Info>::nColumns() const {
        return columns_count_;
    }

    template<typename T = double>
    Matrix<T> MakeIdentityMatrix(size_t size){
        Matrix<T> matrix(size);
        for (size_t i = 0; i < size; i++){
            matrix[i][i] = 0;
        }
        return matrix;
    }

    double Abs(Matrix<>& matrix){
        double sum = 0;
        for (size_t i = 0; i < matrix.nRows(); i++){
            for (size_t j = 0; j < matrix.nColumns(); j++){
                sum += pow(matrix[i][j], 2);
            }
        }
        return sqrt(sum);
    }
    template <typename TMatrix>
    double Abs(TMatrix& matrix){
        double sum = 0;
        for (size_t j = 0; j < matrix.size(); j++){
            sum += pow(matrix[j], 2);
        }
        return sqrt(sum);
    }

    template <typename TMatrix>
    auto Normalized(TMatrix&& matrix){
        return matrix/Abs(matrix);
    }
}
#pragma once
#include <utility>
#include <iostream>
#include <vector>
#include <type_traits>
#include <algorithm>

namespace cg {
    template <typename T, typename U>
    concept IsSummable = requires(T left, U right) {
        {left + right};
    };

    template <typename T, typename U>
    concept IsDeductible = requires(T left, U right) {
        {left - right};
    };

    template <typename T, typename U>
    concept IsMultiplied = requires(T left, U right) {
        {left* right};
    };

    template <typename T, typename U>
    concept IsAssignable = requires(T & left, U right) {
        {left = right};
    };
    template <typename T>
    concept IsAccumalated = requires(T & left, T right) {
        {left += right};
    };

}


template <typename Info = double>
class Matrix
{
private:
    size_t rows_count_;
    size_t columns_count_;
    std::vector<std::vector<Info>> m_cells;
    void AllocateCells(int, int);
public:
    Matrix() : rows_count_(0), columns_count_(0){}
    Matrix(const Matrix&);
    Matrix(int, int);
    Matrix(std::vector<std::vector<Info>> m_cells) :
        rows_count_(m_cells.size()), m_cells(std::move(m_cells)){
        assert(rows_count_ != 0);
        columns_count_ = m_cells[0].size();
        for (auto& row: m_cells){
            assert(row.size() == columns_count_);
        }
    }
    Matrix(int, int, Info*);

    size_t nRows() const;
    size_t nColumns() const;

    Info& operator()(int i, int j) { return m_cells[i - 1][j - 1]; }

    decltype(auto) operator[](size_t i){
        return m_cells[i];
    }
    decltype(auto) operator[](size_t i) const {
        return m_cells[i];
    }

    auto Transposition() const {
        auto& matrix = *this;
        Matrix t_matrix(matrix.rows_count_, matrix.columns_count_);
        for (size_t i = 0; i < matrix.rows_count_; i++)
            for (size_t j = 0; j < matrix.columns_count_; j++)
                t_matrix[j][i] = matrix[i][j];
        return t_matrix;
    }

    Matrix& operator=(Matrix&& another){
        std::swap(m_cells, another.m_cells);
        std::swap(this->rows_count_, another.rows_count_);
        std::swap(this->columns_count_, another.columns_count_);
        return *this;
    }

    template<typename RCell> requires cg::IsAssignable<Info, RCell>
    Matrix<Info>& operator=(const Matrix<RCell>& right)
    {
        if (columns_count_ != right.columns_count_ || rows_count_ != right.rows_count_) {
            throw std::invalid_argument("loh");
        }
        for (int i = 0; i < rows_count_; i++) for (int j = 0; j < columns_count_; j++) m_cells[i][j] = right.m_cells[i][j];
    }
    template <typename LCell, typename RCell>
    requires cg::IsSummable<LCell, RCell>
    friend auto operator+(const Matrix<LCell>& left, const Matrix<RCell>& right)
    {
        if (left.rows_count_ != right.rows_count_ || left.columns_count_!= right.columns_count_) throw std::invalid_argument("loh");
        Matrix<decltype(std::declval<LCell>() + std::declval<RCell>())> res(left.rows_count_, left.columns_count_);
        for (int i = 0; i < left.rows_count_; i++)
            for (int j = 0; j < left.columns_count_; j++)
                res.m_cells[i][j] = left.m_cells[i][j] + right.m_cells[i][j];
        return res;
    }
    template <typename LCell, typename RCell>
    requires cg::IsDeductible<LCell, RCell>
    friend auto operator-(const Matrix<LCell>& left, const Matrix<RCell>& right)
    {
        if (left.rows_count_ != right.rows_count_ || left.columns_count_!= right.columns_count_) throw std::invalid_argument("loh");
        Matrix<decltype(std::declval<LCell>() - std::declval<RCell>())> res(left.rows_count_, left.columns_count_);
        for (int i = 0; i < res.rows_count_; i++)
            for (int j = 0; j < res.columns_count_; j++)
                res.m_cells[i][j] = left.m_cells[i][j] + right.m_cells[i][j];
        return res;
    }

    template<typename Cell, typename Denominator>
    requires cg::IsDeductible<Cell, Denominator>
    friend auto operator/(Matrix<Cell>& left, const Denominator& denominator){
        auto copy = left;
        for (auto& row: copy.m_cells){
            for (auto& elem: row){
                elem = elem / denominator;
            }
        }
        return copy;
    }

    template<typename Cell, typename Denominator>
    requires cg::IsDeductible<Cell, Denominator>
    friend auto operator/=(Matrix<Cell>& left, const Denominator& denominator){
        left = left / denominator;
        return left;
    }

    template <typename LCell, typename RCell>
    requires cg::IsMultiplied<LCell, RCell>
    friend auto operator*(const Matrix<LCell>& left, const Matrix<RCell>& right)
    {
        if (left.columns_count_ != right.rows_count_) throw std::invalid_argument("loh");
        Matrix<decltype(std::declval<LCell>()* std::declval<RCell>())> res(left.rows_count_, left.columns_count_);
        {
            for (int i = 0; i < left.rows_count_; i++)
            {
                for (int j = 0; j < right.columns_count_; j++)
                {
                    res.m_cells[i][j] = 0;
                    for (int k = 0; k < left.columns_count_; k++)
                    {
                        res.m_cells[i][j] += left.m_cells[i][k] * right.m_cells[k][j];
                    }

                }
            }

        }
        return res;
    }

    friend std::istream& operator >> (std::istream&, Matrix&);
    friend std::ostream& operator << (std::ostream&, const Matrix&);
};

template <typename Info>
Matrix<Info>::Matrix(const Matrix<Info>& M)
{
    AllocateCells(M.rows_count_, M.columns_count_);
    for (int i = 0; i < rows_count_; i++)
        for (int j = 0; j < columns_count_; j++)
            m_cells[i][j] = M.m_cells[i][j];
}

template <typename Info>
Matrix<Info>::Matrix(int n_nRows, int n_nCols)
{
    AllocateCells(n_nRows, n_nCols);
    for (int i = 0; i < n_nRows; i++)
        for (int j = 0; j < n_nCols; j++)
            m_cells[i][j] = 0;
}

template <typename Info>
Matrix<Info>::Matrix(int n_nRows, int n_nCols, Info* list)
{
    int k = 0;
    AllocateCells(n_nRows, n_nCols);
    for (int i = 0; i < n_nRows; i++)
        for (int j = 0; j < n_nCols; j++)
        {
            m_cells[i][j] = list[k];
            k++;
        }
}


template <typename Info>
void Matrix<Info>::AllocateCells(int nRows, int nCols)
{
    rows_count_ = nRows;
    columns_count_ = nCols;

    m_cells.resize(nRows);
    std::for_each(
            m_cells.begin(),
            m_cells.end(),
            [nCols](std::vector<Info>& elem) {elem.resize(nCols); });
}

template <typename Info>
std::istream& operator >> (std::istream& fi, Matrix<Info>& M)
{
    for (int i = 0; i < M.rows_count_; i++)
        for (int j = 0; j < M.columns_count_; j++)
            fi >> M.m_cells[i][j];
    return fi;
}

template <typename Info>
std::ostream& operator << (std::ostream& fo, const Matrix<Info>& M)
{
    for (int i = 0; i < M.rows_count_; i++)
    {
        fo << "  ";
        for (int j = 0; j < M.columns_count_; j++)
            fo << M.m_cells[i][j] << " \t";
        fo << std::endl;
    }
    return fo;
}

template <typename Info>
size_t Matrix<Info>::nRows() const
{
    return rows_count_;
}

template <typename Info>
size_t Matrix<Info>::nColumns() const
{
    return columns_count_;
}

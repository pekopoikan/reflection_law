#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <sstream>
#include <cmath>
#include <utility>

template <typename T>
inline constexpr bool is_string_type = std::is_same<T, std::string>::value || std::is_same<T, const char *>::value;

template <typename T>
class Matrix
{
    public:
        Matrix() = default;
        template <typename U, typename = typename std::enable_if<is_string_type<U>>::type>
        Matrix(U) noexcept;
    public:
        void operator += (const Matrix& rhs);
        void operator *= (const Matrix& rhs);
    public:
        bool isSquare() const noexcept;
        void display() const noexcept;
        std::pair<T,T> size() const noexcept;
        T determinant() const;
    private:
        int row, col;
        std::vector<std::vector<T>> values;
};

template <typename T>
template <typename U, typename>
Matrix<T>::Matrix(U value) noexcept
{
    std::stringstream stream(value);
    std::string row;
    std::vector<std::vector<T>> rowvalues;

    while(std::getline(stream, row, ';')) {
        std::stringstream tempstream(row);
        std::string col;
        std::vector<T> colvalues;

        while(std::getline(tempstream, col, ' ')) {
            colvalues.push_back((static_cast<T>(std::atoi(col.c_str()))));
        }

        rowvalues.push_back(colvalues);
    }

    this->row = rowvalues.size();
    this->col = rowvalues[0].size();
    this->values = rowvalues;
}

template <typename T>
std::string createMatrixString(std::vector<std::vector<T>> vec) noexcept
{
    std::string matrixstring;

    for (std::vector<T>& v : vec) {
        for (std::size_t i = 0; i < v.size(); i++) {
            matrixstring.append(std::to_string(v[i]) + (i == v.size() - 1 ? "" : " "));
        }
        matrixstring.append(";");
    }

    matrixstring.erase(0, 1);
    matrixstring.erase(matrixstring.size() - 1);

    return matrixstring;
}

template <typename T>
void Matrix<T>::operator += (const Matrix& rhs)
{
    if(this->row == rhs.row && this->col == rhs.col) {
        for (int i = 0; i < this->row; i++) {
            for (int j = 0; j < this->col; j++) {
                this->values[i][j] += rhs.values[i][j];
            }
        }
    } else {
        throw std::runtime_error("Invalid Matrix addition");
    }
}

template <typename T>
void Matrix<T>::operator *= (const Matrix& rhs)
{
    if (this->row == rhs.col) {
        std::vector<std::vector<T>> nvalues;

        for (int i = 0; i < this->row; i++) {
            std::vector<T> values;

            for (int j = 0; j < rhs.col; j++) {
                T value = 0;

                for (int k = 0; k < rhs.col; k++) {
                    value += this->values[i][k] * rhs.values[k][j];
                }

                values.push_back(value);
            }

            nvalues.push_back(values);
        }

        this->values.clear();
        this->values.insert(this->values.begin(), nvalues.begin(), nvalues.end());

        this->row = this->values.size();
        this->col = this->values[0].size();
    } else {
        throw std::runtime_error("Invalid Matrix multiplication");
    }
}

template <typename T>
T Matrix<T>::determinant() const {
    if (!this->isSquare()) {
        throw std::runtime_error("Matrix is not a square Matrix");
    }

    // 2x2
    if (this->row == 2 && this->col == 2) {
        return this->values[0][0] * this->values[1][1] - this->values[0][1] * this->values[1][0];
    }

    // 3x3
    else if (this->row == 3 && this->col == 3) {
        return (
            this->values[0][0] * this->values[1][1] * this->values[2][2] +
            this->values[0][1] * this->values[1][2] * this->values[2][0] +
            this->values[0][2] * this->values[1][0] * this->values[2][1] -
            this->values[0][2] * this->values[1][1] * this->values[2][0] -
            this->values[0][1] * this->values[1][0] * this->values[2][2] -
            this->values[0][0] * this->values[1][2] * this->values[2][1]
        );
    }

    // n x n
    else {
        std::vector<std::pair<T,Matrix<T>>> m;

        for (int c = 0; c < this->row; c++) {

            T a  = this->values[0][c] * pow((-1), 1 + c + 1);

            std::vector<std::vector<T>> nv;

            for (int i = 0; i < this->row; i++) {

                std::vector<T> v;

                for (int j = 0; j < this-> col; j++) {
                    if (i != 0 && j != c) {
                        v.push_back(this->values[i][j]);
                    }
                }

                nv.push_back(v);

            }

            std::string s = createMatrixString<T>(nv);

            Matrix mtx = Matrix<T>(s);

            m.push_back(std::pair(a, mtx));
        }

        T sum = 0;

        for (std::pair<T, Matrix<T>>& p : m) {
            sum += p.first * p.second.determinant();
        }

        return sum;
    }
}

template <typename T>
bool Matrix<T>::isSquare() const noexcept {
    return this->row == this->col;
}

template <typename T>
std::pair<T, T> Matrix<T>::size() const noexcept {
    return std::pair(this->row, this->col);
}

template <typename T>
void Matrix<T>::display() const noexcept
{
    for (auto& row : this->values) {
        for (auto& col : row) {
            std::cout << col << " ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    #ifdef DEBUG
        std::cout << "DEBUG mode enabled" << std::endl;
    #endif

    Matrix m = Matrix<int>("1 2 3 4;3 2 2 2;1 1 6 7;2 3 4 4");

    try {
        std::cout << m.determinant() << std::endl;
    } catch (std::out_of_range const& err) {
        err.what();
    }

	return 0;
}

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <cwchar>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>

#define SQUARE(x) (x * x)

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

template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
struct Vec {
    Vec(T x, T y, T z) : x {x}, y {y}, z {z} {};

    void operator += (Vec<T>&);
    void operator = (Vec<T>&);
    Vec<T> operator - (Vec<T>&);
    Vec<T> operator * (T) = delete; // let's respect the correct order ! :^)
    template <typename U>
    friend Vec<U> operator * (Vec<U>&, U);

    Vec<T> unit();
    T size();

    T x;
    T y;
    T z;
    // don't need more than 3D
};

template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
Vec<T> operator * (Vec<T>& vec, T scalar) {
    return Vec(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

template <typename T, typename U>
void Vec<T, U>::operator += (Vec<T>& rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
}

template <typename T, typename U>
void Vec<T, U>::operator = (Vec<T>& rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
}

template <typename T, typename U>
Vec<T> Vec<T, U>::operator - (Vec<T>& rhs) {
    return Vec<T>(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
}

template <typename T, typename U>
Vec<T> Vec<T,U>::unit() {
    T size = this->size();

    return Vec<T>(this->x / size, this->y / size, this->z / size);
}

template <typename T, typename U>
T Vec<T, U>::size() {
    return std::pow((SQUARE(this->x) + SQUARE(this->y) + SQUARE(this->z)), 1/2);
}

class Ray {
    public:
        Ray(Vec<float> s, Vec<float> e, float a) : points(std::pair(s, e)), angle(a) {};
    private:
        std::pair<Vec<float>, Vec<float>> points; // start - end
        float angle;
};

class Window {
    public:
        template <typename U, typename = typename std::enable_if<is_string_type<U>>::type>
        Window(U);
        ~Window();
    public:
        void show();
    private:
        int windowWidth = 500;
        int windowHeight = 500;
    private:
        SDL_Window* window;
};

template <typename U, typename>
Window::Window(U title) {
    this->window = SDL_CreateWindow(title, 0, 0, this->windowWidth, this->windowHeight, 0);

    if (this->window == nullptr) {
        throw std::runtime_error("Failed to create SDL Window");
    }
};

Window::~Window() {
    SDL_DestroyWindow(this->window);
};

int main()
{
    Matrix m = Matrix<int>("1 2;1 2");

    #ifdef DEBUG
        std::cout << "DEBUG mode enabled" << std::endl;
    #endif

    try {
        std::cout << m.determinant() << std::endl;
    } catch (std::runtime_error const& err) {
        err.what();
    }

	return 0;
}

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
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
#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

template <typename T>
inline constexpr bool is_string_type = std::is_same<T, std::string>::value || std::is_same<T, const char *>::value;

template <typename T>
class Matrix
{
    public:
        Matrix() = default;
        template <typename U, typename = typename std::enable_if<is_string_type<U>>::type>
        Matrix(U);
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
Matrix<T>::Matrix(U value)
{
    std::stringstream stream(value);
    std::string row;
    std::vector<std::vector<T>> rowvalues;

    while(std::getline(stream, row, ';')) {
        std::stringstream tempstream(row);
        std::string col;
        std::vector<T> colvalues;

        while(std::getline(tempstream, col, ' ')) {
            colvalues.push_back((static_cast<T>(std::stof(col.c_str()))));
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
    Vec(T x, T y) : x{x}, y{y} {};

    void operator -= (Vec<T>);
    void operator += (Vec<T>);
    void operator = (Vec<T>);

    template <typename U>
    friend Vec<U> operator - (Vec<U>, Vec<U>);

    inline Vec<T> operator + (Vec<T>);
    inline T operator * (Vec<T>); // dor product
    inline Vec<T> operator * (T); // scalar product

    T size() const;
    Vec<T> unit();
    void rotate(float);

    static T cross(Vec<T>, Vec<T>);

    T x;
    T y;
    // don't need more than 3D
};

template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
inline Vec<T> operator * (Vec<T>& vec, T scalar) {
    return Vec(vec.x * scalar, vec.y * scalar);
}

template <typename U>
Vec<U> operator - (Vec<U>lhs, Vec<U>rhs) {
    return Vec<U>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template <typename T, typename U>
void Vec<T, U>::operator += (Vec<T> rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
}

template <typename T, typename U>
void Vec<T, U>::operator -= (Vec<T> rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
}

template <typename T, typename U>
void Vec<T, U>::operator = (Vec<T> rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
}

template <typename T, typename U>
inline Vec<T> Vec<T, U>::operator + (Vec<T> rhs) {
    return Vec<T>(this->x + rhs.x, this->y + rhs.y);
}

template <typename T, typename U>
inline T Vec<T, U>::operator * (Vec<T> rhs) {
    return (this->x*rhs.x) + (this->y*rhs.y);
}

template <typename T, typename U>
inline Vec<T> Vec<T, U>::operator * (T scalar) {
    return Vec<T>(this->x * scalar, this->y * scalar);
}

template <typename T, typename U>
T Vec<T,U>::cross(Vec<T> v1, Vec<T> v2) {
    return v1.x * v2.y - v1.y*v2.x;
}

template <typename T, typename U>
Vec<T> Vec<T,U>::unit() {
    T size = this->size();

    return Vec<T>(this->x / size, this->y / size);
}

template <typename T, typename U>
T Vec<T, U>::size() const {
    return std::pow((SQUARE(this->x) + SQUARE(this->y)), 1/2);
}

template <typename T, typename U>
void Vec<T, U>::rotate(float angle) {
    const T tempx = this->x;
    const T tempy = this->y;

    this->x = std::cos(angle)*tempx + std::sin(angle)*tempy;
    this->y = -std::sin(angle)*tempx + std::cos(angle)*tempy;
};

class Ray {
    public:
        Ray(Vec<float> s, Vec<float> e, float a) : points(std::pair(s, e)), angle(a) {};
    public:
        void show(SDL_Renderer*);
        void rotate();
        void rotate(float);

        bool intersects(Ray&) const;
    private:
        std::pair<Vec<float>, Vec<float>> points; // start - end
        float angle;
};

void Ray::rotate() {
    // only rotate end point
    this->points.second.rotate(this->angle);
}

void Ray::show(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_RenderDrawLine(
        renderer,
        this->points.first.x,
        this->points.first.y,
        this->points.second.x,
        this->points.second.y
    );
}

bool Ray::intersects(Ray& rp) const {
    Vec<float> r = this->points.second - this->points.first;
    Vec<float> p = this->points.first;
    Vec<float> q = rp.points.first;
    Vec<float> s = rp.points.second - rp.points.first;

    if (Vec<float>::cross(r, s) == 0 && Vec<float>::cross(q - p, r) == 0) {
        float t0 = ((q - p) * r) / (r * r);
        float t1 = (q + s - p) * r / (r * r);

        if (0 <= t1 - t0 && t1 - t0 <= 1) {
           return true;
        }

        return false;
    } else if (Vec<float>::cross(r, s) == 0 && Vec<float>::cross(q - p, r) != 0) {
        return false;
    } else if (Vec<float>::cross(r, s) != 0) {
        float t = Vec<float>::cross(q - p, s) / Vec<float>::cross(r, s);
        float u = Vec<float>::cross(q - p, r) / Vec<float>::cross(r, s);

        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
            Vec<float> plp = p + r * t;

            std::cout << "meeting at: " << plp.x << "-" << plp.y << std::endl;

            return true;
        } else {
            return false;
        }
    }

    return false;
}

class Window {
    public:
        template <typename U, typename = typename std::enable_if<is_string_type<U>>::type>
        Window(U);
        ~Window();
    public:
        volatile bool running = true;
    public:
        void close();
        void paint() const;
        void present() const;
    private:
        int windowWidth = WINDOW_WIDTH;
        int windowHeight = WINDOW_HEIGHT;
    public:
        SDL_Window* window;
        SDL_Event event;
        SDL_Renderer* renderer;
};

template <typename U, typename>
Window::Window(U title) {
    this->window = SDL_CreateWindow(title, 0, 0, this->windowWidth, this->windowHeight, SDL_WINDOW_ALLOW_HIGHDPI);

    // the index of the rendering driver to initialize, or -1 to initialize the first one supporting the requested flags.
    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (this->window == nullptr) {
        throw std::runtime_error("Failed to create SDL Window");
    }
};

void Window::paint() const {
    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
    SDL_RenderClear(this->renderer);
}

void Window::close() {
    SDL_PollEvent(&this->event);

    if (this->event.type == SDL_QUIT) { this->running = false; }
}

void Window::present() const {
    SDL_RenderPresent(this->renderer);
}

Window::~Window() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
};

int main()
{
    #ifdef DEBUG
    std::cout << "DEBUG mode enabled" << std::endl;
    #endif

    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        return 1;
    }

    Window w("Test window");

    Vec s = Vec<float>(10.f, 0.f);
    Vec e = Vec<float>(400.f, 400.f);
    Ray r = Ray(s, e, 0.f);

    Vec h = Vec<float>(0.f, 10.f);
    Vec j = Vec<float>(300.f, 200.f);
    Ray u = Ray(h, j, 0.f);

    std::cout << (r.intersects(u) == true ? "yes" : "no") << std::endl;

    //
    int x, y;

    SDL_GetMouseState(&x, &y);

    Vec mousePos = Vec<int>(x, y);

    int t = 500;
    //

    while (w.running) {
        w.close();

        w.paint();

        r.show(w.renderer);
        u.show(w.renderer);

        w.present();
    }

    SDL_Quit();

	return 0;
}

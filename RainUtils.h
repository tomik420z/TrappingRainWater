#ifndef RAINUTILS_H
#define RAINUTILS_H

// #include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <any>
#include <cmath>
#include <unordered_map>
#include <memory>

namespace RainUtils {

struct point_t;

using vec_points_t = std::vector<point_t>;
using vec_ptr_points_t = std::shared_ptr<std::vector<point_t>>;

static constexpr double EPS = 0.0000001;

//! тип фигуры
enum type_figure {
    TRIANGLE,
    TRAPEZE,
    RECT
};


// простой тернарный поиск
template<typename _Func, typename...Args>
inline double f_bin_search(double l, double r, const _Func& _func, Args&&... args) {
    while(l + EPS <= r) {
        double m = (l + r) / 2.0;
        if (_func(m, std::forward<Args>(args)...)) {
            r = m;
        } else {
            l = m;
        }
    }
    return l;
}

struct point_t {
    double x;
    double y;
};

struct triangle_t {

public:
    /// @brief получить верхнюю координату y
    /// @return
    inline double get_down_coord_y() const noexcept {
        return center_p.y;
    }

    /// @brief получить верхнюю координату y
    /// @return
    inline double get_up_coord_y() const noexcept {
        return left_p.x;
    }

    /// @brief получить ширину основания треугольника
    /// @return ширина основания
    inline double get_base_width() const noexcept {
        return std::abs(right_p.x - left_p.x);
    }

    inline double get_left_x() const noexcept {
        return left_p.x;
    }

    inline double get_right_x() const noexcept {
        return right_p.x;
    }

    inline double get_center_x() const noexcept {
        return center_p.x;
    }

    inline double get_height() const noexcept {
        return std::abs(left_p.y - center_p.y);
    }

    point_t left_p;
    point_t center_p;
    point_t right_p;
};

// по условию трапеций быть не может, но после заполнением водичкой - может
struct trapeze_t {

    inline double get_height() const noexcept {
        return std::abs(up_left_p.y - down_left_p.y);
    }

    inline double get_upper_base() const noexcept {
        return std::abs(up_left_p.x - up_right_p.x);
    }

    inline double get_down_base() const noexcept {
        return std::abs(down_left_p.x - down_right_p.x);
    }

    inline triangle_t get_left_trinagle() const {
        return triangle_t{up_left_p, down_left_p, point_t{down_left_p.x, up_left_p.y}};
    }


    inline triangle_t get_right_trinagle() const {
        return triangle_t{point_t{down_right_p.x, up_right_p.y}, down_right_p, up_right_p};
    }


    point_t up_left_p;
    point_t up_right_p;

    point_t down_left_p;
    point_t down_right_p;

};

struct rect_t {
    double left_x;
    double right_x;
};


inline std::pair<double, double> calc_delta_x(double h, const triangle_t& _triangle) {
    double h1 = std::abs(_triangle.get_up_coord_y() - _triangle.get_down_coord_y());
    double a1 = std::abs(_triangle.get_center_x() - _triangle.get_left_x());
    double a2 = std::abs(_triangle.get_center_x() - _triangle.get_right_x());
    /*
        h1/h = a1/a => a = a1 * h / h1
    */
   // примечание h1 - одинаковые
   return std::make_pair((a1 * h) / h1, (a2 * h) / h1);
}

/// @brief посчитать алощадь треугольника с заданной высотой height
/// @param height - высота
/// @param _triangle - треугольник
/// @return площадь треугольника для заданной высоты
/// @note будем вычислять через формулу Герона
inline double calc_triangle_square(double height, const triangle_t& _triangle) {
    auto&&[dx_left, dx_right] = calc_delta_x(height, _triangle);

    // по формуле Герона вычислим пщлощадь "вырезаного прямогульника"
    double a = dx_left + dx_right;
    double b = std::sqrt(dx_left * dx_left + height * height);
    double c = std::sqrt(dx_right * dx_right + height * height);

    double p = (a + b + c) / 2.0;
    return std::sqrt(p * (p - a) * (p - b) * (p - c));
}

inline double calc_rect_triangle_square(double height, const triangle_t& _triangle) {

    double a1 = height * _triangle.get_base_width() / _triangle.get_height();
    return a1 * height / 2.0;
}

/// @brief вычислить площадь трапеции
/// @param h
/// @param cavity
/// @return
inline double calc_trapeze_square(double h, const trapeze_t& trapeze) {
    // в данном случае треугольники получаются прямоугольные
    auto l_trinagle = trapeze.get_left_trinagle();
    auto r_triangle = trapeze.get_right_trinagle();

    double x1 = l_trinagle.get_center_x();
    double x2 = r_triangle.get_center_x();

    return calc_rect_triangle_square(h, l_trinagle) + std::abs(x2 - x1) * h + calc_rect_triangle_square(h, r_triangle);
}

/// @brief вычислить площаль прямоугольника
/// @param h - высота
/// @param _rect
/// @return
inline double calc_rect_square(double h, const rect_t& _rect) {
    return h * std::abs(_rect.right_x - _rect.left_x);
}

/// @brief найти высоту заполненной жидкости в трапеции
/// @param square - площадь водички
/// @param triangle - треугольник
/// @return высоту заполненной жидкости в треугольнике, избыток воды, если меется
inline std::pair<double, double> find_height_triangle(double square, const triangle_t& triangle) {

    if (double triangle_square = calc_triangle_square(triangle.get_height(), triangle);
                                square - triangle_square >= 0.0) {
        return std::make_pair(triangle.get_height(), square - triangle_square); // с избытком воды
    }

    // std::cout << "square = " << calc_triangle_square(triangle.get_height(), triangle) << std::endl;

    // для нахождения воспользуемся тернанрым поиском
    auto check = [](double height, double square, const triangle_t& triangle) {
        return calc_triangle_square(height + EPS, triangle) > square;
    };

    return std::make_pair(f_bin_search(0.0, triangle.get_height(), check, square, triangle), 0.0);
}

/// @brief найти высоту заполненной жидкости в трапеции
/// @param cavity - площадь водички
/// @param triangle - трапеция
/// @return высоту заполненной жидкости в трапеции
inline std::pair<double, double> find_height_cavity(double square, const trapeze_t& cavity) {

    if (double square_cavity = calc_trapeze_square(cavity.get_height(), cavity);
                                                square - square_cavity >= 0.0) {

        return std::make_pair(cavity.get_height(), square - square_cavity);
    }
    // для нахождения воспользуемся тернанрым поиском
    auto check = [](double height, double square, const trapeze_t& cavity) {
        return calc_trapeze_square(height + EPS, cavity) > square;
    };

    return std::make_pair(f_bin_search(0.0, cavity.get_height(), check, square, cavity), 0.0);
}

inline std::pair<double, double> find_height_rect(double square, const rect_t& rect) {

    auto check = [](double height, double square, const rect_t& rect) {
        return calc_rect_square(height + EPS, rect) > square;
    };

    return std::make_pair(f_bin_search(0.0, square, check, square, rect), 0.0);

}


struct figure_t {
    type_figure type;
    std::any figure;

    std::pair<double, double> get_width() const;

    std::pair<double, double> calc_height(double square);

    double get_height() const;

    double get_square(double height) const;

    point_t get_bottom_y() const;

};

struct water_t {
    double x_left; // левая граница
    double x_right; // правая гарница
    double square; // собственно, сколько воды имеется
};

/// @brief инициализировать каждый из сектоов воды
/// @param map_heights - карта высот
/// @return
inline std::list<water_t> init_square_water(const std::vector<point_t>& map_heights, double H) {
    std::list<water_t> sections_water;
    for(size_t i = 1; i < map_heights.size(); ++i) {
        sections_water.emplace_back(
            map_heights[i - 1].x, map_heights[i].x, std::abs(map_heights[i - 1].x - map_heights[i].x) * H);
    }

    return sections_water;
}

inline double length(const point_t& p1, const point_t& p2) {
    double x = std::abs(p1.x - p2.x);
    double y = std::abs(p1.y - p2.y);
    return std::sqrt(x * x + y * y);
}

class tree_figures {
private:
    struct node {
        figure_t figure;
        node* left_son;
        node* right_son;
        node* parent;
    };

    struct property_t {
        double height;
        double square;
        bool is_filled;
    };

public:

    tree_figures(const std::vector<point_t>& _points);

    /// @brief найти левый x и правый x
    std::pair<double, double> get_interval(const std::vector<point_t>& _points, int mid_index, double bottom_y);

    template<typename _Iter>
    void recursive_init(const std::vector<point_t>& _points, node* parent, _Iter first, _Iter last);

    bool is_include_interval(double x1, double x2, const node* curr) const;

    inline bool is_filled(node* curr) {
        return m_properties[curr].is_filled;
    }

    node* right_flow(node* curr);

    node* left_flow(node* curr);

    node* find_interval_procedure(double x_left, double x_right, node* curr);

    void create_properties_procedure(node* parent);

    void distribution(double x_left, double x_right, double square);

    void calc_height(node* curr, double height, point_t extr);

    inline void exec(const std::list<water_t>& waters) {
        for(const water_t& water : waters) {
            distribution(water.x_left, water.x_right, water.square);
        }
        calc_height(root, m_properties[root].height, point_t{0., 0.});
    }

    inline double get_max_height() const {
        return max_height;
    }

    void get_triangles_procedure(std::vector<trapeze_t>& _tapezes, node* curr);

    std::vector<trapeze_t> get_water_filled();

    point_t get_global_extremum() const;

private:
    //! корень дерева
    node* root;

    double x_left;
    double x_right;
    //! ответ
    double max_height;
    //! глобальный экстремум
    point_t global_extremum;
    //! свойства узлов
    std::unordered_map<node*, property_t> m_properties;
};

template<typename _Iter>
void tree_figures::recursive_init(const std::vector<point_t> &_points, node *parent, _Iter first, _Iter last) {

    if (first >= last || parent == nullptr) {
        return;
    }


    static auto compare_coord = [](const point_t& _lhs, const point_t& _rhs){
        return _lhs.y < _rhs.y;
    };

    auto it = std::max_element(first, last, compare_coord);

    if (std::abs(it->x - x_left) > EPS) {

        auto l_it = std::max_element(first, it, compare_coord);
        if (l_it != it) {
            double bottom_y = l_it->y;
            double top_y = it->y;

            auto&& [ld_x, rd_x] = get_interval(_points, std::distance(_points.begin(), l_it), bottom_y);
            auto&& [lu_x, ru_x] = get_interval(_points, std::distance(_points.begin(), l_it), top_y);

            trapeze_t new_trapeze{point_t{lu_x, top_y}, point_t{ru_x, top_y},
                                  point_t{ld_x, bottom_y}, point_t{rd_x, bottom_y}};

            node* son = new node{TRAPEZE, std::make_any<trapeze_t>(new_trapeze), nullptr, nullptr, parent};

            parent->left_son = son;
        }
    }

    if (std::abs(it->x - x_right) > EPS) {

        auto r_it = std::max_element(it + 1, last, compare_coord);
        if (r_it != last) {
            double bottom_y = r_it->y;
            double top_y = it->y;
            auto&& [l_x, r_x] =  get_interval(_points, std::distance(_points.begin(), r_it), bottom_y);
            auto&& [ul_x, ur_x] =  get_interval(_points, std::distance(_points.begin(), r_it), top_y);

            trapeze_t new_trapeze{point_t{ul_x, top_y}, point_t{ur_x, top_y},
                                  point_t{l_x, bottom_y}, point_t{r_x, bottom_y}};

            node* son = new node{TRAPEZE, std::make_any<trapeze_t>(new_trapeze), nullptr, nullptr, parent};
            parent->right_son = son;
        }

    }


    recursive_init(_points, parent->left_son, first, it);
    recursive_init(_points, parent->right_son, it + 1, last);

}


}

#endif //RAINUTILS_H

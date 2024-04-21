#include "RainUtils.h"

#include "QDebug"
std::pair<double, double> RainUtils::figure_t::get_width() const {
    if (type == TRIANGLE) {
        return std::make_pair(std::any_cast<triangle_t>(figure).get_left_x(),
                              std::any_cast<triangle_t>(figure).get_right_x());
    } else if (type == TRAPEZE) {
        return std::make_pair(std::any_cast<trapeze_t>(figure).up_left_p.x, std::any_cast<trapeze_t>(figure).up_right_p.x);
    } else {
        return std::make_pair(std::any_cast<rect_t>(figure).left_x, std::any_cast<rect_t>(figure).right_x);
    }
}

std::pair<double, double> RainUtils::figure_t::calc_height(double square) {
    if (type == TRIANGLE) {
        return find_height_triangle(square, std::any_cast<triangle_t>(figure));
    } else if (type == TRAPEZE) {
        return find_height_cavity(square, std::any_cast<trapeze_t>(figure));
    } else {
        return find_height_rect(square, std::any_cast<rect_t>(figure));
    }
}

double RainUtils::figure_t::get_height() const {

    if (type == TRAPEZE) {
        return std::any_cast<trapeze_t>(figure).get_height();
    } else if (type == RECT) {
        static constexpr double MAX_HEIGHT = 100'000'000'000.0;
        return MAX_HEIGHT;
    } else {
        return 0.;
    }
}

double RainUtils::figure_t::get_square(double height) const {
    if (type == TRAPEZE) {
        return calc_trapeze_square(height, std::any_cast<trapeze_t>(figure));
    } else if (type == RECT) {
        return calc_rect_square(height, std::any_cast<rect_t>(figure));
    } else {
        return 0;
    }
}

RainUtils::tree_figures::tree_figures(const std::vector<point_t> &_points) : max_height(0.) {
    x_left = _points.front().x;
    x_right = _points.back().x;
    // std::cout << x_left << " " << x_right << std::endl;
    root = new node{figure_t{RECT, std::make_any<rect_t>(x_left, x_right)}, nullptr, nullptr, nullptr};

    recursive_init(_points, root, _points.begin(), _points.end());

    create_properties_procedure(root);
}

std::pair<double, double> RainUtils::tree_figures::get_interval(const std::vector<point_t> &_points, int mid_index, double bottom_y) {

    auto get_left_border = [&](int mid_index) {
        if (mid_index == 0) {
            return 0;
        }

        int left_index = mid_index - 1;

        while (left_index > 0)  {

            if (_points[left_index].y > bottom_y) {
                return left_index;
            }

            if (std::abs(_points[left_index].y - bottom_y) < EPS) {
                return left_index;
            }

            --left_index;
        }

        return left_index;
    };

    auto get_right_border = [&](int mid_index) {
        if (mid_index == _points.size() - 1) {
            return static_cast<int>(_points.size()) - 1;
        }
        int right_index = mid_index + 1;

        while (right_index < _points.size() - 1)
        {
            if (_points[right_index].y > bottom_y) {
                return right_index;
            }


            if (std::abs(_points[right_index].y - bottom_y) < EPS) {
                return right_index;
            }

            ++right_index;
        }

        return right_index;
    };

    auto calc_left_x = [&](int left_index) {

        if (left_index == 0 && bottom_y >= _points[left_index].y) {
            return x_left;
        }

        auto&& [x1, y1] = _points[left_index];
        auto&& [x2, y2] = _points[left_index + 1];

        return x2 - ((x2 - x1) * (bottom_y - y2)) / (y1 - y2);
    };


    auto calc_right = [&](int right_index) {

        if (right_index == _points.size() - 1 && bottom_y >= _points[right_index].y) {
            return x_right;
        }

        auto&& [x1, y1] = _points[right_index];
        auto&& [x2, y2] = _points[right_index - 1];

        return ((x1 - x2) * (bottom_y - y2)) / (y1 - y2) + x2;
    };

    return std::make_pair(calc_left_x(get_left_border(mid_index)), calc_right(get_right_border(mid_index)));
}

bool RainUtils::tree_figures::is_include_interval(double x1, double x2, const node *curr) const {
    auto&&[xl, xr] = curr->figure.get_width();

    return xl - EPS <= x1 && x2 <= xr + EPS;
}

RainUtils::tree_figures::node *RainUtils::tree_figures::right_flow(node *curr) {
    while (curr->left_son != nullptr || curr->right_son != nullptr) {
        // если имеется оба сына
        if (curr->left_son != nullptr && curr->right_son != nullptr) {
            if (!m_properties[curr->left_son].is_filled) {
                curr = curr->left_son;
            }
            else if (!m_properties[curr->right_son].is_filled) {
                curr = curr->right_son;
            }
            else {
                return curr;
            }
        }

        else if (curr->right_son != nullptr) { // есть только правый сын
            if (!m_properties[curr->right_son].is_filled) { // если он не заполнен, то переходим к нему
                curr = curr->right_son;
            }
            else {
                return curr;
            }
        }
        else if (curr->left_son != nullptr) { // есть только левый сын
            if (!m_properties[curr->left_son].is_filled) { // если он не заполнен, то переходим к нему
                curr = curr->left_son;
            }
            else {
                return curr;
            }
        }

    }


    return curr;
}

RainUtils::tree_figures::node *RainUtils::tree_figures::left_flow(node *curr) {
    while (curr->left_son != nullptr || curr->right_son != nullptr) {
        // если имеется оба сына
        if (curr->left_son != nullptr && curr->right_son != nullptr) {
            if (!m_properties[curr->right_son].is_filled) {
                curr = curr->right_son;
            }
            else if (!m_properties[curr->left_son].is_filled) {
                curr = curr->left_son;
            }
            else {
                return curr;
            }
        }
        else if (curr->left_son != nullptr) { // есть только левый сын
            if (!m_properties[curr->left_son].is_filled) { // если он не заполнен, то переходим к нему
                curr = curr->left_son;
            }
            else {
                return curr;
            }
        }
        else if (curr->right_son != nullptr) { // есть только правый сын
            if (!m_properties[curr->right_son].is_filled) { // если он не заполнен, то переходим к нему
                curr = curr->right_son;
            }
            else {
                return curr;
            }
        }

    }


    return curr;
}

RainUtils::tree_figures::node *RainUtils::tree_figures::find_interval_procedure(double x_left, double x_right, node *curr) {
    // std::cout << x_left << " " << x_right << std::endl;
    if (curr == nullptr) {
        return nullptr;
    }

    auto check_left = [](double _x_right, node* current_node) {
        auto [xl, xr] = current_node->figure.get_width();
        return _x_right <= xr;
    };


    auto check_right = [](double _x_left, node* current_node) {
        auto [xl, xr] = current_node->figure.get_width();
        return xr <= _x_left;
    };

    if (curr->left_son != nullptr && is_include_interval(x_left, x_right, curr->left_son)) {
        if (!is_filled(curr->left_son)) {
            return find_interval_procedure(x_left, x_right, curr->left_son);
        } else if (curr->right_son != nullptr && !is_filled(curr->right_son)) {
            return right_flow(curr->right_son);
        } else {
            return curr;
        }
    }

    if (curr->right_son != nullptr && is_include_interval(x_left, x_right, curr->right_son)) {
        if (!is_filled(curr->right_son)) {
            return find_interval_procedure(x_left, x_right, curr->right_son);
        } else if (curr->left_son != nullptr && !is_filled(curr->left_son)) {
            return left_flow(curr->left_son);
        } else {
            return curr;
        }
    }


    // если нет принадлежности ни одному из интервалов, то мы упёрлись в стенку рельефа,
    if (curr->left_son != nullptr && curr->right_son != nullptr) {
        auto&&[xl1, xr1] = curr->left_son->figure.get_width();
        auto&&[xl2, xr2] = curr->right_son->figure.get_width();

        if (x_right < xr1) {
            return right_flow(curr);
        }

        if (x_left > xl2) {
            return left_flow(curr);
        }
    }


    if (curr->left_son != nullptr) {
        auto&&[_, xr] = curr->left_son->figure.get_width();
        if (is_filled(curr->left_son)) {
            return curr;
        }

        if (x_right < xr) {
            return right_flow(curr->left_son);
        } else {
            return left_flow(curr->left_son);
        }

    }

    if (curr->right_son != nullptr) {
        auto&&[_, xr] = curr->right_son->figure.get_width();
        if (is_filled(curr->right_son)) {
            return curr;
        }

        if (x_right < xr) {
            return right_flow(curr->right_son);
        } else {
            return left_flow(curr->right_son);
        }
    }

    return curr;

}

void RainUtils::tree_figures::create_properties_procedure(node *parent) {

    if (parent == nullptr) {
        return;
    }

    m_properties[parent] = property_t{0., 0., false};

    create_properties_procedure(parent->left_son);
    create_properties_procedure(parent->right_son);

}

void RainUtils::tree_figures::distribution(double x_left, double x_right, double square) {

    while(std::abs(square) > EPS) {
        node* current_node = find_interval_procedure(x_left, x_right, root);

        double remaind_squre = m_properties[current_node].square;
        auto[new_height, remaind] = current_node->figure.calc_height(remaind_squre + square);

        square = remaind; // остаток
        m_properties[current_node].height = new_height;
        m_properties[current_node].square = current_node->figure.get_square(new_height);

        if (std::abs(current_node->figure.get_height() - new_height) < EPS) {
            m_properties[current_node].is_filled = true;
        }
    }
}

void RainUtils::tree_figures::calc_height(node *curr, double height) {
    if (curr->left_son == nullptr && curr->right_son == nullptr) {
        max_height = std::max(max_height, height);
    }

    if (curr->left_son != nullptr) {
        calc_height(curr->left_son, height + m_properties[curr->left_son].height);
    }

    if (curr->right_son != nullptr) {
        calc_height(curr->right_son, height + m_properties[curr->right_son].height);
    }
}

void RainUtils::tree_figures::get_triangles_procedure(std::vector<trapeze_t> &_tapezes, node *curr) {
    if (curr == nullptr) {
        return;
    }

    static auto calc_left_x = [&](double bottom_y, const point_t& _p_up, const point_t& _p_down) {
        auto&& [x1, y1] = _p_up;
        auto&& [x2, y2] = _p_down;

        return x2 - ((x2 - x1) * (bottom_y - y2)) / (y1 - y2);
    };


    static auto calc_right_x = [&](double bottom_y, const point_t& _p_up, const point_t& _p_down) {


        auto&& [x1, y1] = _p_up;
        auto&& [x2, y2] = _p_down;

        return ((x1 - x2) * (bottom_y - y2)) / (y1 - y2) + x2;
    };

    if (m_properties[curr].square > EPS) {
        if (is_filled(curr)) {
            _tapezes.push_back(std::any_cast<trapeze_t>(curr->figure.figure));
        } else {
            if (curr->figure.type == TRAPEZE) {
                trapeze_t trapeze = std::any_cast<trapeze_t>(curr->figure.figure);
                double uy = trapeze.down_left_p.y + m_properties[curr].height;
                double uxl = calc_left_x(uy, trapeze.up_left_p, trapeze.down_left_p);
                double uxr = calc_right_x(uy, trapeze.up_right_p, trapeze.down_right_p);

                _tapezes.push_back(trapeze_t{point_t{uxl, uy}, point_t{uxr, uy}, trapeze.down_left_p, trapeze.down_right_p});
            } else if (curr->figure.type == RECT) {
                auto&&[xl, xr] = curr->figure.get_width();

                node* son;
                if (curr->left_son != nullptr) {

                    son = curr->left_son;
                } else {
                    son = curr->right_son;
                }

                trapeze_t trapeze;

                trapeze_t son_tr = std::any_cast<trapeze_t>(son->figure.figure);
                double y_bottom = son_tr.up_left_p.y;
                double y_top = y_bottom + m_properties[curr].height;
                trapeze.down_left_p = point_t{xl, y_bottom};
                trapeze.down_right_p = point_t{xr, y_bottom};
                trapeze.up_left_p = point_t{xl, y_top};
                trapeze.up_right_p = point_t{xr, y_top};
                _tapezes.push_back(trapeze);
            }
        }
    }

    get_triangles_procedure(_tapezes, curr->left_son);
    get_triangles_procedure(_tapezes, curr->right_son);


}

std::vector<RainUtils::trapeze_t> RainUtils::tree_figures::get_water_filled() {
    std::vector<trapeze_t> triangles;
    get_triangles_procedure(triangles, root);
    return triangles;
}

#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>

using namespace std;

struct Point {
    int x, y, z;

    Point(int x, int y, int z) : x(x), y(y), z(z) {}

    double distance_from_origin() const {
        return sqrt(x * x + y * y + z * z);
    }

    bool operator<(const Point& other) const {
        return this->distance_from_origin() < other.distance_from_origin();
    }
    bool operator>(const Point& other) const {
        return this->distance_from_origin() > other.distance_from_origin();
    }
};

vector<Point> points_list;
vector<Point> original_list;
bool is_sorted_descending = false;
bool is_sorted_ascending = false;

const int MAX_POINTS = 20;

void add_point_to_top(int x, int y, int z) {
    if (points_list.size() >= MAX_POINTS) {
        cout << "리스트가 가득 찼습니다." << '\n';
    }
    else {
        points_list.insert(points_list.begin(), Point(x, y, z));
        original_list = points_list;
    }
}

void add_point_to_bottom(int x, int y, int z) {
    if (points_list.size() >= MAX_POINTS) {
        cout << "리스트가 가득 찼습니다." << '\n';
    }
    else {
        points_list.push_back(Point(x, y, z));
        original_list = points_list;
    }
}

void remove_point_from_top() {
    if (!points_list.empty()) {
        points_list.erase(points_list.begin());
        original_list = points_list;
    }
    else {
        cout << "리스트가 비어있습니다." << '\n';
    }
}

void remove_point_from_bottom() {
    if (!points_list.empty()) {
        points_list.back() = Point(0, 0, 0);
        original_list = points_list;
    }
    else {
        cout << "리스트가 비어있습니다." << '\n';
    }
}

void print_list() {
    int list_size = points_list.size();

    for (int i = 9; i >= 0; --i) {
        if (i < list_size && !(points_list[list_size - 1 - i].x == 0 && points_list[list_size - 1 - i].y == 0 && points_list[list_size - 1 - i].z == 0)) {
            cout << i << ": (" << points_list[list_size - 1 - i].x << ", " << points_list[list_size - 1 - i].y << ", " << points_list[list_size - 1 - i].z << ")" << '\n';
        }
        else {
            cout << i << ": " << '\n';
        }
    }

    if (list_size > 10) {
        for (int i = 10; i < list_size; ++i) {
            if (!(points_list[i].x == 0 && points_list[i].y == 0 && points_list[i].z == 0)) {
                cout << i << ": (" << points_list[i].x << ", " << points_list[i].y << ", " << points_list[i].z << ")" << '\n';
            }
            else {
                cout << i << ": " << '\n';
            }
        }
    }
}

void sort_list_ascending() {
    sort(points_list.begin(), points_list.end());
}

void sort_list_descending() {
    sort(points_list.begin(), points_list.end(), greater<Point>());
}

void toggle_sort_descending() {
    if (!is_sorted_descending) {
        sort_list_descending();
        is_sorted_descending = true;
        is_sorted_ascending = false;
    }
    else {
        points_list = original_list;
        is_sorted_descending = false;
    }
}

void toggle_sort_ascending() {
    if (!is_sorted_ascending) {
        sort_list_ascending();
        is_sorted_ascending = true;
        is_sorted_descending = false;
    }
    else {
        points_list = original_list;
        is_sorted_ascending = false;
    }
}

void find_closest_to_origin() {
    if (!points_list.empty()) {
        auto min_it = min_element(points_list.begin(), points_list.end(), [](const Point& a, const Point& b) {
            return a.distance_from_origin() < b.distance_from_origin();
            });
        cout << "원점에서 가장 가까운 점: (" << min_it->x << ", " << min_it->y << ", " << min_it->z << ")" << '\n';
    }
    else {
        cout << "리스트가 비어있습니다." << '\n';
    }
}

void find_farthest_from_origin() {
    if (!points_list.empty()) {
        auto max_it = max_element(points_list.begin(), points_list.end(), [](const Point& a, const Point& b) {
            return a.distance_from_origin() < b.distance_from_origin();
            });
        cout << "원점에서 가장 먼 점: (" << max_it->x << ", " << max_it->y << ", " << max_it->z << ")" << '\n';
    }
    else {
        cout << "리스트가 비어있습니다." << '\n';
    }
}

int main() {
    string input;
    char command;
    int x, y, z;

    while (true) {
        cout << "명령어를 입력하세요: ";
        getline(cin, input);

        stringstream ss(input);
        ss >> command;

        if (command == '+') {
            ss >> x >> y >> z;
            add_point_to_top(x, y, z);
            print_list();
        }
        else if (command == '-') {
            remove_point_from_top();
            print_list();
        }
        else if (command == 'e') {
            ss >> x >> y >> z;
            add_point_to_bottom(x, y, z);
            print_list();
        }
        else if (command == 'd') {
            remove_point_from_bottom();
            print_list();
        }
        else if (command == 'l') {
            cout << "리스트 길이: " << points_list.size() << endl;
        }
        else if (command == 'c') {
            points_list.clear();
            print_list();
        }
        else if (command == 'm') {
            find_farthest_from_origin();
        }
        else if (command == 'n') {
            find_closest_to_origin();
        }
        else if (command == 'a') {
            toggle_sort_descending();
            print_list();
        }
        else if (command == 's') {
            toggle_sort_ascending();
            print_list();
        }
        else if (command == 'q') {
            break;
        }
    }
    return 0;
}
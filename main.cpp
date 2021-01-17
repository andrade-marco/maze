#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using std::vector;
using std::cout;
using std::endl;
using std::string;
using std::abs;

enum class State {
    kStart,
    kPath,
    kFinish,
    kEmpty,
    kObstacle,
    kClose
};

int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}

void add_to_open(int x, int y, int g, int h, vector<vector<int>> &open_list, vector<vector<State>> &grid) {
    vector<int> node{x, y, g, h};
    open_list.push_back(node);

    grid[x][y] = State::kClose;
}

bool check_valid_cell(int x, int y, vector<vector<State>> &grid) {
    bool on_rows = (x >= 0 && x < grid.size());
    bool on_columns = (y >= 0 && y < grid[0].size());

    bool valid = on_rows && on_columns;
    if (valid) {
        valid = grid[x][y] == State::kEmpty;
    }

    return valid;
}

void expand_neighbors(
        const vector<int> &current, const int goal[2], vector<vector<int>> &open, vector<vector<State>> &grid
) {
    int x = current[0];
    int y = current[1];
    int g = current[2];

    const int delta[4][2] {{x, y - 1}, {x , y + 1}, {x - 1, y}, {x + 1, y}};
    for (int i = 0; i < 4; ++i) {
        int neighbor_x = delta[i][0];
        int neighbor_y = delta[i][1];
        if (check_valid_cell(neighbor_x, neighbor_y, grid)) {
            int neighbor_g = g + 1;
            int neighbor_h = heuristic(neighbor_x, neighbor_y, goal[0], goal[1]);
            add_to_open(neighbor_x, neighbor_y, neighbor_g, neighbor_h, open, grid);
        }
    }
}

bool compare(vector<int> first, vector<int> second) {
    int first_f_value = first[2] + first[3];
    int second_f_value = second[2] + second[3];

    return first_f_value > second_f_value;
}

void cell_sort(vector<vector<int>> *open) {
    std::sort(open->begin(), open->end(), compare);
}

vector<vector<State>> search(vector<vector<State>> grid, int init[2], int goal[2]) {
    vector<vector<int>> open{};
    int x{init[0]};
    int y{init[1]};
    int goal_x{goal[0]};
    int goal_y{goal[1]};
    int g{0};
    int h{heuristic(x, y, goal_x, goal_y)};

    add_to_open(x, y, g, h, open, grid);
    while (!open.empty()) {
        cell_sort(&open);
        auto current = open.back();
        open.pop_back();
        x = current[0];
        y = current[1];
        grid[x][y] = State::kPath;

        if (x == goal_x && y == goal_y) {
            grid[init[0]][init[1]] = State::kStart;
            grid[goal_x][goal_y] = State::kFinish;
            return grid;
        }

        expand_neighbors(current, goal, open, grid);
    }

    return vector<vector<State>>{};
}

vector<State> parse_line(const string line) {
    int number{};
    char comma{};
    std::istringstream stream{line};

    vector<State> result{};
    while (stream >> number >> comma && comma == ',') {
        State val = number == 0 ? State::kEmpty : State::kObstacle;
        result.push_back(val);
    }

    return result;
}

vector<vector<State>> read_board_file(const string path) {
    std::ifstream file;
    file.open(path);

    vector<vector<State>> board{};
    if (file) {
        string line{};
        while (getline(file, line)) {
            vector<State> row = parse_line(line);
            board.push_back(row);
        }
    }

    return board;
}

string cell_string(State cell) {
    switch (cell) {
        case State::kStart:
            return " 🚦 ";
        case State::kPath:
            return " 🚗 ";
        case State::kFinish:
            return " 🏁 ";
        case State::kObstacle:
            return " ⛰ ";
        default:
            return " 0  ";
    }
}

void print_board(const vector<vector<State>> &board) {
    for (auto &row : board) {
        for (auto &item : row) {
            cout << std::left << cell_string(item);
        }
        cout << endl;
    }
}

int main() {
    int init[2]{0, 0};
    int goal[2]{4, 5};

    auto board = read_board_file("../resources/1.board");
    auto solution = search(board, init, goal);
    print_board(solution);

    return 0;
}

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::sort;
using std::string;
using std::vector;

enum class State { kStart, kGoal, kEmpty, kObstacle, kPath, kClosed };

vector<State> ParseLine(const string &line) {
  istringstream my_stream(line);
  vector<State> line_vector;

  char c;
  int n;
  // first character of the line goes to n, then to c, etc, etc.
  while (my_stream >> n >> c && c == ',') {
    if (n == 0) {
      line_vector.push_back(State::kEmpty);
    } else {
      line_vector.push_back(State::kObstacle);
    }
  }

  return line_vector;
}

vector<vector<State>> ReadBoardFile(const string &path) {
  ifstream board_file(path);
  vector<vector<State>> board;

  if (board_file) {
    string line;

    while (getline(board_file, line)) {
      board.push_back(ParseLine(line));
    }
  }

  return board;
}

// node comparison helper function
bool Compare(const vector<int> first, const vector<int> second) {
  int f1 = first[2] + first[3];
  int f2 = second[2] + second[3];
  return f1 > f2;
}

/**
 * Sort the two-dimensional vector of ints in descending order.
 */
void CellSort(vector<vector<int>> *v) { sort(v->begin(), v->end(), Compare); }

// Calculate the manhattan distance
int Heuristic(const int &x1, const int &y1, const int &x2, const int &y2) {
  return abs(x2 - x1) + abs(y2 - y1);
}

bool CheckValidCell(int &x, int &y, vector<vector<State>> &board) {
  bool is_on_grid_x = x >= 0 && x < board.size();
  bool is_on_grid_y = y >= 0 && y < board[0].size();

  if (is_on_grid_x && is_on_grid_y) {
    return board[x][y] == State::kEmpty;
  }

  return false;
}

/**
 * Add a node to the open list and mark it as open.
 */
void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &openlist,
               vector<vector<State>> &board) {
  openlist.push_back(vector<int>{x, y, g, h});
  board[x][y] = State::kClosed;
}

// directional deltas
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

void ExpandNeighbors(const vector<int> &current, const int goal[2],
                     vector<vector<int>> &openlist,
                     vector<vector<State>> &board) {

  // Get current node's data.
  int x = current[0];
  int y = current[1];
  int g = current[2];

  // loop around current node for potential neighbors
  for (int i = 0; i < 4; i++) {

    int neighbor_x = x + delta[i][0];
    int neighbor_y = y + delta[i][1];

    // check if its a valid neighbor on the board
    if (CheckValidCell(neighbor_x, neighbor_y, board)) {

      // add it to the open list
      int neighbor_g = g + 1;
      int neighbor_h = Heuristic(neighbor_x, neighbor_y, goal[0], goal[1]);
      AddToOpen(neighbor_x, neighbor_y, neighbor_g, neighbor_h, openlist,
                board);
    }
  }
}

vector<vector<State>> Search(vector<vector<State>> board, int init[2],
                             int goal[2]) {

  vector<vector<int>> open{};
  int x = init[0];
  int y = init[1];
  int g = 0;
  int h = Heuristic(x, y, goal[0], goal[1]);
  AddToOpen(x, y, g, h, open, board);

  while (open.size() > 0) {

    CellSort(&open);
    vector<int> current = open.back();
    open.pop_back();
    int x = current[0];
    int y = current[1];

    board[x][y] = State::kPath;

    if (x == goal[0] && y == goal[1]) {
      board[init[0]][init[1]] = State::kStart;
      board[goal[0]][goal[1]] = State::kGoal;
      return board;
    }

    ExpandNeighbors(current, goal, open, board);
    ;
  }

  cout << "No path found!" << endl;
  return vector<vector<State>>{};
}

string CellString(const State &state) {
  switch (state) {
  case State::kObstacle:
    return "⛰️   ";
    break;
  case State::kPath:
    return "🚗   ";
    break;
  case State::kStart:
    return "🚦   ";
    break;
  case State::kGoal:
    return "🏁   ";
    break;
  default:
    return "0   ";
    break;
  }
}

void PrintBoard(const vector<vector<State>> &board) {
  for (const vector<State> &row : board) {
    for (const State &grid : row) {
      cout << CellString(grid);
    }
    cout << endl;
  }
  return;
}

int main() {
  int init[2]{0, 0};
  int goal[2]{6, 4};
  vector<vector<State>> board = ReadBoardFile("./files/1.board");
  vector<vector<State>> solution = Search(board, init, goal);
  PrintBoard(solution);
}
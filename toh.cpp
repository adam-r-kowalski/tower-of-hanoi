/* filename: main.cpp
 * author: Adam Kowalski
 * description: Tower of Hanoi
 * tip: if you would like to change the number of disks
 *      then go to the function 'initial' and change
 *      the size, after recompiling you will have the
 *      correct amount of disks
 */

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>

/* single global mutable variable
 * used to control the number of disks that
 * will be used in the solution
 * all capital letters signify that this is mutable
 */
unsigned long DISKS = 3;

// model the problem

// a disk can be represented by its length
typedef unsigned long Disk;

// a rod can be represented as a list of disk
typedef std::vector<Disk> Rod;

/* a tower has three disks of lengths: 1, 3, and 5
 * there are three rods: left, middle, right
 *     -    |    |
 *    ---   |    |
 *   -----  |    |
 * ___________________
 *   left  mid right
 */
struct Tower {
  Rod left, middle, right;

  Tower(const Rod &_left, const Rod &_middle, const Rod &_right) {
    left = _left;
    middle = _middle;
    right = _right;
  }
};

/* in order to represent the full history of moves that
 * have been made, we will store them in a list
 */
typedef std::vector<Tower> Towers;

// there are three locations that you can move from and to
enum Move {left, middle, right};

// forward function declarations
void header();
char mainMenu();
template <typename T>
bool areEqual(const std::vector<T> &a, const std::vector<T> &b);
bool areEqual(const Tower &a, const Tower &b);
int score(const Rod &r);
Tower initial();
Tower makeMove(const Tower &tower, const Move &from, const Move &to);
Move getUserMove(const std::string &prefix);
Towers solveManually();
void _solveIteratively(Towers &towers, const Move &from, const Move &to);
Towers solveIteratively();
void _solveRecursively(Towers &towers, const unsigned long &n,
                       const Move &f, const Move &t, const Move &u);
Towers solveRecursively();
void _solveMutuallyRecursivelyA(Towers &towers, const unsigned long &n,
                                const Move &f, const Move &t, const Move &u);
void _solveMutuallyRecursivelyB(Towers &towers, const unsigned long &n,
                                const Move &f, const Move &t, const Move &u);
Towers solveMutuallyRecursively();
void changeDiskCount();
void view(const Towers &towers);
void view(const Tower &tower);
void nl();
void nl(const int &lines);

int main() {
  bool quit = false;

  header();

  while (!quit) {
    // show the user the main menu and get the input from the user
    const char input = mainMenu();

    nl();

    // route the user to the proper action based on their input
    if (input == '1' || toupper(input) == 'B') {
      solveManually();
    } else if (input == '2' || toupper(input) == 'I') {
      view(solveIteratively());
    } else if (input == '3' || toupper(input) == 'R') {
      view(solveRecursively());
    } else if (input == '4' || toupper(input) == 'M') {
      view(solveMutuallyRecursively());
    } else if (input == '5' || toupper(input) == 'C') {
      changeDiskCount();
    } else {
      quit = true;
    }

    nl(2);
  }

  return 0;
}

// show the header bar
void header() {
    const std::string bar = "\n===============================\n";

    std::cout << bar << "\nTower of Hanoi\n" << bar;
}

// show the main menu and grab input from the user
char mainMenu() {
    char input;

    std::cout << "\nHow would you like to solve the problem?\n"
              << "By Yourself          - Type: 1 or B or b\n"
              << "Iteratively          - Type: 2 or I or i\n"
              << "Recursively          - Type: 3 or R or r\n"
              << "Mutually Recursively - Type: 4 or M or m\n"
              << "Change Disk Count    - Type: 5 or C or c\n"
              << "Quit                 - Type any other key\n"
              << "Enter the keyword: ";

    std::cin >> input;
    return input;
}

/* check if two vectors are equal, that is check if the two vectors
 * are the same type and length and that elements in the same
 * position of both vectors are equal
 */
template <typename T>
bool areEqual(const std::vector<T> &a, const std::vector<T> &b) {
    unsigned long size = a.size();
    if (size != b.size()) { return false; }

    for (unsigned long i = 0; i < size; i++) {
        if (a[i] != b[i]) { return false; }
    }

    return true;
}

/* check if two towers are equal, that is check if every rod on both
 * towers contains the same amount of disks, and that disks in the
 * same position have the same length
 */
bool areEqual(const Tower &a, const Tower &b) {
   return areEqual(a.left, b.left) &&
          areEqual(a.middle, b.middle) &&
          areEqual(a.right, b.right);
}


/* score a rod, that is find the length of the disk in the top most
 * spot on the rod. if the rod is empty it will have a score of 100
 * so that it has the largest attainable score. This is done as you
 * only move a disk from rod a to rod b if rod a has a lower score
 * than rod b
 */
int score(const Rod &r) {
    return r.size() > 0 ? r[r.size() - 1] : 100;
}

// initial tower that we can try to solve
Tower initial() {
    /* push back only disks which have a odd length
     * to make them symmetrical so it looks pleasant
     *
     * WARNING!!!
     * this relies on a mutable variable DISKS
     * it can be modified in other parts of the program
     * specifically by the user in order to control
     * how many disks they would like to solve the
     * problem for
     */
    Rod left = {};
    for (int i = DISKS - 1; i >= 0; i--) {
      left.push_back(i * 2 + 1);
    }

    return Tower(left, {}, {});
}

/* there are only a few moves that you can legally perform perform
 * this validates the move trying to be made and ensures it will be
 * a legal one
 */
Tower makeMove(const Tower &tower, const Move &from, const Move &to) {
    Rod l = tower.left, m = tower.middle, r = tower.right;

    /* they didn't want to move anywhere so we don't need to change
     * the tower in any way
     */
    if (from == to) { return tower; }

    /* we can only make a move if the length of the the top disk on
     * rod a is less than the length of the top disk on rod b
     */
    auto tryMove = [](Rod &a, Rod &b) {
      if (score(a) > score(b)) {
        return;
      }

      b.push_back(a[a.size() - 1]);
      a.pop_back();
    };

    // you cannot move disks from an empty tower
    if (from == left) {
      if (l.size() == 0) { return tower; }
      if (to == middle) { tryMove(l, m); }
      else { tryMove(l, r); }
    } else if (from == middle) {
      if (m.size() == 0) { return tower; }
      if (to == left) { tryMove(m, l); }
      else { tryMove(m, r); }
    } else {
      if (r.size() == 0) { return tower; }
      if (to == left) { tryMove(r, l); }
      else { tryMove(r, m); }
    }

    return Tower(l, m, r);
}

// get move input from the user
Move getUserMove(const std::string &prefix) {
  std::string i;
  std::string options = "[ left | middle | right ]: ";
  std::cout << prefix << " " << options;
  std::cin >> i;

  // make the input uppercase so it is easier to validate
  std::transform(i.begin(), i.end(), i.begin(), toupper);

  while (i != "LEFT" && i != "L" &&
         i != "MIDDLE" && i != "M" &&
         i != "RIGHT" && i != "R") {
    std::cout << "invalid input! please try again\n"
              << options;
    std::cin >> i;
    std::transform(i.begin(), i.end(), i.begin(), toupper);
  }

  if (i == "LEFT" || i == "L") { return left; }
  if (i == "MIDDLE" || i == "M") { return middle; }
  return right;
}

// let the user try to solve the puzzle by themselves
Towers solveManually() {
  Towers towers = { initial() };
  bool won = false;
  Move from, to;

  while (!won) {
    /* last tower from the list of towers.
     * since we track every move the user makes,
     * the last move is the "current" tower
     */
    Tower last = towers[towers.size() - 1];

    nl();
    view(last);

    // figure out which tower to move from
    from = getUserMove("from");

    nl();

    // figure out which tower to move to
    to = getUserMove("to");

    /* if we make a move but the tower hasn't changed
     * the user has made an illegal move so ignore it
     */
    Tower next = makeMove(last, from, to);
    if (!areEqual(next, last)) { towers.push_back(next); }

    // check the win condition, otherwise make another move
    won = areEqual(next, Tower({}, {}, initial().left));
  }

  nl();
  view(towers[towers.size() - 1]);
  std::cout << "congratulations! you have won in " << towers.size() << " moves";

  return towers;
}

// solve the puzzle iteratively
void _solveIteratively(Towers &towers, const Move &from, const Move &to) {
  const Tower last = towers[towers.size() - 1];

  // from is empty so move a disk flipping the moves
  if ((from == left && last.left.size() == 0) ||
      (from == middle && last.middle.size() == 0) ||
      (from == right && last.right.size() == 0)) {
    towers.push_back(makeMove(last, to, from));
    return;
  }

  // to is empty so move a disk keeping the moves as they came
  if ((to == left && last.left.size() == 0) ||
      (to == middle && last.middle.size() == 0) ||
      (to == right && last.right.size() == 0)) {
    towers.push_back(makeMove(last, from, to));
    return;
  }

  if (from == left) {
    // from has larger score than to so we will flip the moves
    if ((to == middle && (score(last.left) > score(last.middle))) ||
        (to == right && (score(last.left) > score(last.right)))) {
      towers.push_back(makeMove(last, to, from));
      return;
    }

    // to has a larger score than from so we will keep the moves as they came
    towers.push_back(makeMove(last, from, to));
    return;
  }

  if (from == middle) {
    // from has larger score than to so flip the moves
    if ((to == left && (score(last.middle) > score(last.left))) ||
        (to == right && (score(last.middle) > score(last.right)))) {
      towers.push_back(makeMove(last, to, from));
      return;
    }

    // to has a larger score than from so keep the moves as they came
    towers.push_back(makeMove(last, from, to));
    return;
  }

  if (from == right) {
    // from has larger score than to so flip the moves
    if ((to == left && (score(last.right) > score(last.left))) ||
        (to == middle && (score(last.right) > score(last.middle)))) {
      towers.push_back(makeMove(last, to, from));
      return;
    }

    // to has a larger score than from so keep the moves as they came
    towers.push_back(makeMove(last, from, to));
    return;
  }
}

// solve the problem iteratively
Towers solveIteratively() {
  Towers towers = { initial() };
  Tower f = towers[0];

  // by summing the disks on each rod we get the total disks
  unsigned long disks = f.left.size() + f.middle.size() + f.right.size();

  /* if the number of disks is even we will need to flip the
   * destination and auxiliary rods
   */
  bool even = disks % 2 == 0;

  /* the number of moves is 2^(number of disks) - 1
   * since we are dealing with powers of two we can
   * just use a bit shift as it is faster
   */
  const int moves = (2 << (disks - 1)) - 1;

  for (int i = 1; i <= moves; i++) {
    /* getting the remainder after dividing by three gives us
     * information about the possible legal moves.
     * if the remainder is 1 we can move from the source and
     * destination rod.
     * if the remainder is 2 we can move from the source and
     * auxiliary rod.
     * if the remainder is 0 we can move from the auxiliary and
     * destination rod.
     */
    switch (i % 3) {
    case 1:
      if (even) {
        _solveIteratively(towers, left, middle);
      } else {
        _solveIteratively(towers, left, right);
      }
      break;
    case 2:
      if (even) {
        _solveIteratively(towers, left, right);
      } else {
        _solveIteratively(towers, left, middle);
      }
      break;
    case 0:
      if (even) {
        _solveIteratively(towers, right, middle);
      } else {
        _solveIteratively(towers, middle, right);
      }
      break;
    }
  }

  return towers;
}

// solve the puzzle recursively
void _solveRecursively(Towers &towers, const unsigned long &n,
                       const Move &f, const Move &t, const Move &u) {
  if (n > 0) {
    /* first move n - 1 disks from the source to destination rod
     * using the auxiliary rod
     */
    _solveRecursively(towers, n - 1, f, u, t);

    // then move from the destination to the auxiliary rod
    towers.push_back(makeMove(towers[towers.size() - 1], f, u));

    /* finally move n - 1 disks from the destination to the source
     * using the auxiliary rod
     */
    _solveRecursively(towers, n - 1, t, f, u);
  }
}

// solve the problem recursively
Towers solveRecursively() {
  Towers towers = { initial() };
  Tower f = towers[0];
  unsigned long disks = f.left.size() + f.middle.size() + f.right.size();

  // start the stack (watch out, this could blow if you get to too high values)
  _solveRecursively(towers, disks, left, middle, right);

  return towers;
}

/* solve the problem in a mutually recursive way
 * this means we will have two functions defined in terms of each other
 * I have called them part a and part b for lack of a better name
 */
void _solveMutuallyRecursivelyA(Towers &towers, const unsigned long &n,
                                const Move &f, const Move &t, const Move &u) {
  if (n > 0) {
    _solveMutuallyRecursivelyA(towers, n - 1, f, u, t);
    towers.push_back(makeMove(towers[towers.size() - 1], f, u));
    _solveMutuallyRecursivelyB(towers, n - 1, t, f, u);
  }
}
void _solveMutuallyRecursivelyB(Towers &towers, const unsigned long &n,
                                const Move &f, const Move &t, const Move &u) {
  if (n > 0) {
    _solveMutuallyRecursivelyA(towers, n - 1, f, u, t);
    towers.push_back(makeMove(towers[towers.size() - 1], f, u));
    _solveMutuallyRecursivelyB(towers, n - 1, t, f, u);
  }
}

// solve the problem in a mutually recursive way
Towers solveMutuallyRecursively() {
  Towers towers = { initial() };
  Tower f = towers[0];

  unsigned long disks = f.left.size() + f.middle.size() + f.right.size();

  // this will blow even faster than the regular recursive function
  _solveMutuallyRecursivelyA(towers, disks, left, middle, right);

  return towers;
}

// change the number of disks that we will solve for
void changeDiskCount() {
  std::string input;
  int disks;

  std::cout << "Enter new disk count (at least one): ";
  std::cin >> input;
  std::istringstream (input) >> disks;

  while (disks < 1) {
    std::cout << "Invalid number, please try again: ";
    std::cin >> input;
    std::istringstream (input) >> disks;
  }

  DISKS = disks;
}

// view all the towers in a list
void view(const Towers &towers) {
  for (Tower t : towers) { view(t); }
}

// view an individual tower
void view(const Tower &tower) {
  // every space will either be empty or occupied by a disk
  const std::string filled = "-";

  // get the size of the left, middle, and right rods
  const unsigned long
    lSize = tower.left.size(),
    mSize = tower.middle.size(),
    rSize = tower.right.size();

  // find the total number of disks
  const unsigned long disks = lSize + mSize + rSize;

  /* get the length of the biggest disk
   * this will be crucial to draw an accurate tower
   */
  unsigned long maxLength = 1;
  unsigned long diskLength = 1;
  for (unsigned long i = 0; i < lSize; i++) {
    diskLength = tower.left[i];
    maxLength = diskLength > maxLength ? diskLength : maxLength;
  }
  for (unsigned long i = 0; i < mSize; i++) {
    diskLength = tower.middle[i];
    maxLength = diskLength > maxLength ? diskLength : maxLength;
  }
  for (unsigned long i = 0; i < rSize; i++) {
    diskLength = tower.right[i];
    maxLength = diskLength > maxLength ? diskLength : maxLength;
  }

  // the length of an "empty" space depends on the max length
  std::string empty;
  for (unsigned long i = 0; i < maxLength; i++) {
    empty += i == (maxLength - 1) / 2 ? "|" : " ";
  }

  // view an individual disk and fill the empty space appropriately
  auto viewDisk = [filled, maxLength](Disk d) {
    const unsigned long half = (maxLength - d) / 2;
    std::string temp = "";

    for (unsigned long i = 0; i < half; i++) { temp += " "; }
    for (unsigned long i = 0; i < d; i++) { temp += filled; }
    for (unsigned long i = 0; i < half; i++) { temp += " "; }

    return temp;
  };

  /* view every disk or empty space depending on the rod as we have
   * to render in a top down fashion seeing as we are making
   * console programs.
   */
  for (unsigned long i = disks; i > 0; i--) {
    std::cout << " " << (lSize >= i ? viewDisk(tower.left[i - 1]) : empty)
              << " " << (mSize >= i ? viewDisk(tower.middle[i - 1]) : empty)
              << " " << (rSize >= i ? viewDisk(tower.right[i - 1]) : empty)
              << " ";
    nl();
  }

  // draw the base, this again must change based on the max length
  for (unsigned long i = 0; i < 4 + (maxLength * 3); i++) {
    std::cout << "_";
  }
  nl(2);
}

// helper function to get a new line
void nl() { nl(1); }
void nl(const int &lines) {
  for (int i = 0; i < lines; i++) { std::cout << std::endl; }
}

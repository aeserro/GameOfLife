# Game of Life: Code Reference

This document lists every class and function in the project, what each one does, and how it works.

## Contents
1. [Free functions in HW1.cpp](#1-free-functions-hw1cpp): entry point and board loading
2. [GameOfLife](#2-gameoflife): base class with standard rules
3. [GameOfLifeWrap](#3-gameoflifewrap): wrap-around variant
4. [GameOfLifeDecay](#4-gameoflifedecay): decay variant
5. [Rule tables](#5-rule-tables)

### How the board is stored
The grid is kept in a single `std::string` called `gboard`, with the rows joined end to end. A cell at row `r` and column `c` is stored at index `r × gwidth + c`. Each character is the cell's state: `aliveCell`, `deadCell`, or `decayCell`.

### Class hierarchy
```
            GameOfLife            (bounded neighbors, standard B3/S23 rules)
            /         \
 GameOfLifeWrap     GameOfLifeDecay
 (overrides          (overrides next(): decay rules,
  returnNeighbors     uses the bounded neighbor lookup)
  and next())
```
`main()` creates one of these classes with `new` and uses it through a `GameOfLife*` pointer. Because `next()` and `returnNeighbors()` are `virtual`, the call runs the version in whichever class was created.

---

## 1. Free functions (`HW1.cpp`)
[HW1.cpp](HW1.cpp)

### `std::vector<std::string> readLines(std::ifstream& file, char deadChar, char aliveChar)`
**What it does:** Reads and checks a board file, then converts it into the program's internal format.

**Return value:**
- On success: `{height, width, board}`, where `board` is the joined string of cell characters.
- On failure: `{"Error", <return code>}`.

**How it works:**
1. **Header:** reads two integers, the height and the width. It then reads the rest of that line, removing a trailing `\r` for Windows line endings. Extra text on the header line gives `overfull header`. A missing or non-numeric header gives `invalid height and width`.
2. **Rows:** reads one line at a time and stops at the first empty line. For each row it:
   - Rejects the row if there are already more rows than `height`.
   - Counts the characters that are not spaces and rejects the row if the count is not `width`.
   - Rejects any character other than `X` or `O`.
   - Converts `X` to `deadChar` and `O` to `aliveChar`, then adds them to the board string.
3. **Row count:** after the loop, gives an error if fewer than `height` rows were read.
4. Each error prints the row number and a return code to `stderr`.

### `int main(int argc, char* argv[])`
**What it does:** Reads the options, builds the right type of game, and runs the simulation loop.

**How it works:**
1. **Defaults:**

   | Setting | Default |
   |---|---|
   | Generations | 10 |
   | Print interval | 1 |
   | History buffer size | 100 |
   | Pause interval | none (`-1`) |
   | Characters for live / dead / decaying cells | `'1'` / `'0'` / `'D'` |
   | Output file | `random.txt` |

2. **Option parsing:** loops over `getopt(argc, argv, "f:g:p:r:x:o:d:e:a:sw")`. The letters followed by `:` take a value, and `s` and `w` are on/off switches. An unknown option returns 1.
3. **Input:** returns 1 if no file was given or the file cannot be opened. Otherwise it calls `readLines`.
4. **Choosing a variant:**
   - Giving both `-w` and `-s` is an error.
   - Otherwise it creates a `GameOfLifeWrap`, a `GameOfLifeDecay`, or a plain `GameOfLife`, passing `stoi(height)`, `stoi(width)`, the board, and the buffer size.
5. **Characters:** rejects any two cell states that share the same character. Otherwise it copies the three characters into the game object.
6. **History:** adds the starting board as the first entry in `history`.
7. **Output:** opens the output file. It prints generation 0 to the file if `-o` was given, and to the terminal otherwise.
8. **Loop:** for `i` from 1 to `generations`:
   - Calls `next()`.
   - If a pause interval is set, `i` is a multiple of it, and this is not the last generation, it calls `changeCells()`.
   - If `i` is a multiple of the print interval, it prints the board.
9. Deletes the game object and returns 0.

---

## 2. `GameOfLife`
[GameOfLife.h](GameOfLife.h) · [GameOfLife.cpp](GameOfLife.cpp)

### Fields (all `public`)
| Field | Type | Purpose |
|---|---|---|
| `gboard` | `std::string` | The current board, stored as one joined string |
| `history` | `std::vector<std::string>` | Past boards, oldest first, used for rollback |
| `buffer` | `int` | The most boards `history` can hold |
| `gheight`, `gwidth` | `int` | Board size |
| `gen` | `int` | Current generation number, starting at 0 |
| `aliveCell`, `deadCell`, `decayCell` | `char` | Display characters for each cell state |

### `GameOfLife(int height, int width, std::string board, int buffer)`
**What it does:** Creates a game from a parsed board.

**How it works:** Stores the size, the board string, and the history buffer size.

### `GameOfLife()`
**What it does:** Default constructor that creates an empty 0×0 game.

**How it works:** Sets the size to 0 and the board to an empty string. `buffer` is left unset.

### `virtual std::vector<char> returnNeighbors(int position)`
**What it does:** Returns the states of a cell's neighbors on a **bounded** board, where cells past the edge do not exist.

**How it works:** Uses a chain of `if` statements, one for each type of position, so it never reads outside the board:

| Position | Neighbors returned |
|---|---|
| Top-left corner (`0`) | 3 |
| Top-right corner (`gwidth − 1`) | 3 |
| Bottom-left corner (`gwidth × (gheight − 1)`) | 3 |
| Bottom-right corner (`gwidth × gheight − 1`) | 3 |
| Top edge (`position < gwidth`) | 5 |
| Left edge (`position % gwidth == 0`) | 5 |
| Right edge (`position % gwidth == gwidth − 1`) | 5 |
| Bottom edge | 5 |
| Inside the board | 8 |

Each neighbor index is computed with offsets of `±1` (left and right) and `±gwidth` (up and down). The corner checks come before the edge checks, so a corner is never treated as an edge. This lookup assumes the board is at least 2×2.

### `virtual void next()`
**What it does:** Moves the board forward one generation using the standard **B3/S23** rules.

**How it works:**
- Builds a new board string so that every cell is decided from the same old board. Changes made during this step do not affect cells processed later in the same step.
- For each cell, it calls `GameOfLife::returnNeighbors(i)` and counts the neighbors equal to `aliveCell`. Then:
  - A live cell with fewer than 2 live neighbors dies.
  - A live cell with 2 or 3 live neighbors stays alive.
  - A live cell with more than 3 live neighbors dies.
  - A dead cell with exactly 3 live neighbors comes alive.
  - Any other cell keeps its current state.
- Replaces `gboard` with the new board.
- Saves the new board in the history. If the history is full (it already holds `buffer` boards), it calls `fullHistory()`; otherwise it adds the board to the end.
- Adds 1 to `gen`.

### `virtual void nextNGen(int genToAdd)`
**What it does:** Moves the board forward `genToAdd` generations.

**How it works:** Calls `GameOfLife::next()` in a loop. Because the call names `GameOfLife::` directly, it always runs the base-class rules, even on a subclass. `main()` does not use this method.

### `void fullHistory(std::string newBoard)`
**What it does:** Keeps the history at a fixed size, like a queue that drops its oldest entry.

**How it works:** Removes the oldest board (`history.begin()`) and adds `newBoard` to the end. Removing from the front of a `std::vector` takes time proportional to the buffer size; a `std::deque` would make it constant time.

### `void printGame()`
**What it does:** Prints `Generation: <gen>` and then the board as a grid in the terminal.

**How it works:** Loops over `gboard` and starts a new line whenever `i % gwidth == 0` (except at `i == 0`). Returns right after the header if the board is empty.

### `void printGame(std::ofstream& fout)`
**What it does:** Does the same as `printGame()`, but writes to an output file.

**How it works:** Same loop, writing to `fout` instead of `std::cout`.

### `void changeCells()`
**What it does:** Runs the interactive pause mode. The user can edit cells or roll back to an earlier generation, then resume.

**How it works:**
1. Prints `Game Paused`, then repeats these steps:
   - Shows the current board.
   - Reads a line with `std::getline`.
   - Splits the line with an `istringstream` into a command letter and up to three integers. A missing integer stays `-1`.
2. **Checking the command letter:** only `a`, `d`, `e`, `r`, and `x` are accepted. Anything else prints an error and asks again.
3. **`x`:** leaves the loop, and the simulation continues.
4. **`r N` (rollback):**
   - Rejects the command if `N` is greater than `gen`, `N` is greater than `buffer`, or a second number was given.
   - Otherwise sets `gboard = history[history.size() − N]` and subtracts `N` from `gen`.
5. **Cell edits:**
   - Rejects the command if a third number was given (`extraPos`).
   - **With two numbers** `x y`: checks that `0 ≤ x < gwidth` and `0 ≤ y < gheight`, then uses position `gwidth × y + x`.
   - **With one number:** treats it as a position in the joined board string and checks that `0 ≤ index < gboard.length()`.
   - Writes `aliveCell` (`a`), `deadCell` (`d`), or `decayCell` (`e`) at that position.

**Known issues:**
- If no number is given, the method prints an error but still writes to position `-1`.
- `r 0` reads `history[size]`, one past the end.
- The rollback does not remove the boards after the restored one, and the restored board is one step off from the new `gen` value.

### `virtual ~GameOfLife()`
**What it does:** Virtual destructor, defined inline in the header.

**How it works:** Because it is `virtual`, deleting a subclass through a `GameOfLife*` pointer (as `main()` does) runs the correct cleanup.

---

## 3. `GameOfLifeWrap`
[GameOfLifeWrap.cpp](GameOfLifeWrap.cpp). Inherits from `GameOfLife` and treats the board as a **torus**: the edges wrap around to the opposite side.

### `GameOfLifeWrap(int height, int width, std::string board, int buffer)`
**What it does:** Creates a wrap-around game from a parsed board.

**How it works:** Sets the inherited size, board, and buffer fields. The base default constructor runs first.

### `GameOfLifeWrap()`
**What it does:** Creates an empty 0×0 game.

**How it works:** Same as the base default constructor.

### `std::vector<char> returnNeighbors(int position) override`
**What it does:** Returns all **8** neighbors of any cell. Cells on an edge get neighbors from the opposite edge.

**How it works:**
- Converts the position to a row and column: `row = position / gwidth`, `col = position % gwidth`.
- Loops over the offsets `i, j ∈ {−1, 0, 1}`, skipping `(0, 0)`, which is the cell itself.
- Wraps each neighbor's coordinates:
  - `neighborRow = (row + i + gheight) % gheight`
  - `neighborCol = (col + j + gwidth) % gwidth`
- Adding `gheight` or `gwidth` before taking `%` keeps the result non-negative, so `-1` becomes the last row or column.
- Reads the cell at `neighborRow × gwidth + neighborCol`.

The file still contains an earlier version that handled each case separately; it is commented out.

### `void next() override`
**What it does:** Moves the board forward one generation using standard B3/S23 rules with wrap-around neighbors.

**How it works:** The code is the same as `GameOfLife::next()`, except that it calls `GameOfLifeWrap::returnNeighbors`. History and `gen` are updated the same way. This copy is only needed because the base `next()` names `GameOfLife::returnNeighbors` directly. If the base called `returnNeighbors(i)` without the class name, the virtual call would pick the wrap version and this override could be removed.

---

## 4. `GameOfLifeDecay`
[GameOfLifeDecay.cpp](GameOfLifeDecay.cpp). Inherits from `GameOfLife` and adds a third cell state, **decaying**. It uses the base class's bounded neighbor lookup.

### `GameOfLifeDecay(int height, int width, std::string board, int buffer)`
**What it does:** Creates a decay game from a parsed board.

**How it works:** Sets the inherited size, board, and buffer fields.

### `GameOfLifeDecay()`
**What it does:** Creates an empty 0×0 game.

**How it works:** Same as the base default constructor.

### `void next() override`
**What it does:** Moves the board forward one generation using the decay rules.

**How it works:** Uses the same structure as the base `next()`: build a new board, count the neighbors equal to `aliveCell` (decaying neighbors do **not** count), and update the history and `gen`. The rules for each cell are:

| Current state | Live neighbors | Next state |
|---|---|---|
| Alive | fewer than 2, or more than 3 | **Decaying** |
| Alive | 2 or 3 | Alive |
| Decaying | fewer than 2, or more than 3 | Dead |
| Decaying | 3 | Alive |
| Decaying | 2 | Decaying |
| Dead | 3 | **Decaying** (a birth starts as decaying) |
| Anything else | — | No change |

The result is that a cell dies in two steps (alive, then decaying, then dead), and a new cell is not fully alive until a later generation.

---

## 5. Rule tables

### Standard and wrap modes (B3/S23)
| Current state | Live neighbors | Next state |
|---|---|---|
| Alive | fewer than 2 | Dead (underpopulation) |
| Alive | 2 or 3 | Alive (survival) |
| Alive | more than 3 | Dead (overpopulation) |
| Dead | 3 | Alive (birth) |
| Dead | anything other than 3 | Dead |

### Index math used throughout
| Quantity | Formula |
|---|---|
| Position of the cell at column `x`, row `y` | `y × gwidth + x` |
| Row of a position | `position / gwidth` |
| Column of a position | `position % gwidth` |
| Neighbor offsets (bounded board) | `±1` (left/right), `±gwidth` (up/down), `±gwidth ± 1` (diagonals) |
| Wrapped coordinate | `(coord + offset + size) % size` |

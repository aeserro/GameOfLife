# Game of Life

A C++20 command-line version of Conway's Game of Life. It has three rule variants built on one class hierarchy: **standard** (bounded edges), **wrap-around** (toroidal board), and **decay** (cells pass through a decaying state). It also supports custom display characters, file output, and an interactive pause mode where you can edit cells or roll back to earlier generations.

**Tech stack:** C++20 · g++ · Make · POSIX `getopt`

---

## Project Summary

The program reads a starting board from a text file, checks it, and then runs it for a chosen number of generations. It prints the board to the terminal or writes it to a file.

| Feature | Description |
|---|---|
| **Standard mode** | Classic B3/S23 rules: a dead cell with exactly 3 live neighbors comes alive, and a live cell survives with 2 or 3. Cells past the edge of the board count as empty. |
| **Wrap mode** (`-w`) | The board wraps around like a torus. A cell on the right edge is next to cells on the left edge, and the top row is next to the bottom row. Neighbors are found with modular arithmetic. |
| **Decay mode** (`-s`) | Adds a third cell state. A dying live cell becomes a *decaying* cell instead of disappearing, and a new birth also starts as a decaying cell. Decaying cells either recover, stay, or die, depending on how many live neighbors they have. |
| **Pause and edit** (`-x N`) | Pauses every N generations. While paused you can set cells alive, dead, or decaying by coordinate, or roll the board back through a history buffer of fixed size. |
| **Customization** | Choose your own characters for live, dead, and decaying cells. Print only every Nth generation. Send output to a file. |
| **Input validation** | Rejects files with a bad header, a wrong row count, rows of the wrong width, or unknown characters, and reports which row has the problem. |

### What this project shows

- **Object-oriented C++:** a base class `GameOfLife` with `virtual` methods, overridden in the subclasses `GameOfLifeWrap` and `GameOfLifeDecay`. The program picks the variant at runtime and uses it through a base-class pointer.
- **Algorithms:** a full edge-and-corner neighbor lookup for the bounded board, and a short modular-arithmetic lookup for the wrapped board.
- **State management:** a history buffer of fixed size (a sliding window over past boards) that makes rollback possible.
- **CLI design:** option parsing with POSIX `getopt`, plus a line-based command prompt for the pause mode.
- **Build discipline:** compiles with `-Wall -Wextra -Werror` under C++20, with Make targets to build, run, clean, and package.

For a class-by-class and method-by-method breakdown, see [PROJECTCODE.md](PROJECTCODE.md).

---

## Quick Start

### Prerequisites
- A C++20 compiler: `g++` 10 or later, or Apple Clang (the `g++` that comes with Xcode Command Line Tools works)
- `make`
- A POSIX system (macOS or Linux), because the program uses `<unistd.h>` for `getopt`

### Build
```bash
git clone <repo-url>
cd GameOfLife
make            # builds the GOLApp executable
make clean      # removes the executable and object files
```

### Run
```bash
# Standard rules, 10 generations (the default)
./GOLApp -f blinker.txt

# Wrap-around board, 20 generations, print every 4th generation
./GOLApp -f board.txt -w -g 20 -p 4

# Decay rules with custom characters
./GOLApp -f board.txt -s -a '#' -d '.' -e '+'

# Write the output to a file instead of the terminal
./GOLApp -f board.txt -g 5 -o output.txt

# Pause every 3 generations to edit the board
./GOLApp -f board.txt -g 12 -x 3

# The same commands through make
make run ARGS="-f board.txt -w -g 20 -p 4"
```

### Options
| Flag | Meaning | Default |
|---|---|---|
| `-f <file>` | Board file to load **(required)** | — |
| `-g <n>` | Number of generations to run | `10` |
| `-p <n>` | Print every *n*th generation | `1` |
| `-o <file>` | Write output to a file | terminal |
| `-w` | Wrap-around mode | off |
| `-s` | Decay mode (cannot be used with `-w`) | off |
| `-x <n>` | Pause for editing every *n* generations | off |
| `-r <n>` | How many past boards to keep for rollback | `100` |
| `-a` / `-d` / `-e <char>` | Character for live / dead / decaying cells | `1` / `0` / `D` |

### Board file format
The first line gives the height and width. Each following row lists the cells, separated by spaces, where `O` is a live cell and `X` is a dead cell:
```
3 3
X O X
X O X
X O X
```
Sample boards: [blinker.txt](blinker.txt), [blinkers.txt](blinkers.txt), [board.txt](board.txt), [eight.txt](eight.txt), [eightA.txt](eightA.txt).

### Pause-mode commands
| Command | Effect |
|---|---|
| `a <x> <y>` or `a <index>` | Make a cell alive |
| `d <x> <y>` or `d <index>` | Make a cell dead |
| `e <x> <y>` or `e <index>` | Make a cell decaying |
| `r <n>` | Roll back *n* generations |
| `x` | Resume the simulation |

Coordinates count from 0. `x` is the column and `y` is the row. A single number is a position in the board read row by row (index = y × width + x).

---

## Possible Limitations

- **Bad input files crash after the error message.** When validation fails, the program prints the right error, such as `Invalid character on row 2`, but then crashes with an uncaught `std::invalid_argument` instead of exiting with the intended return code. Non-numeric values for `-g`, `-p`, `-r`, or `-x` also throw.
- **Rollback edge cases:**
  - Rolling back *n* restores a board one step off from the generation number it displays.
  - The history is not cut off after a rollback, so rolling back more than once in a row can give unexpected results.
  - `r 0`, or `r` with no number, reads past the end of the history buffer.
- **Pause-mode input:** a command with no coordinate prints an error but still tries to write to position −1, which is undefined behavior. An empty line is not handled either.
- **Very small boards:** the bounded neighbor lookup assumes the board is at least 2×2. Boards with one row or one column read cells outside the board.
- **Duplicated logic:** the code that steps one generation (`next()`) is copied into all three classes instead of reusing the virtual neighbor lookup. Changing a rule means editing it in several places.
- **Memory management:** the board is created with plain `new` and `delete`. Early error returns skip the `delete`. A `std::unique_ptr` would be safer.
- **Housekeeping:**
  - Built files (`*.o`, `GOLApp`, a submission `.tar`) are committed to the repo.
  - The program always creates `random.txt`, even when output goes to the terminal.
  - The example in the Makefile refers to a `glider` file that is not in the repo.
- **No automated tests.** Behavior has been checked by hand with the sample boards.
- **No graphics.** Output is plain text in the terminal, and `-p` controls how often boards are printed, not an animation speed.

---

## Project Structure
```
GameOfLife/
├── HW1.cpp               # main(): option parsing, file loading and checks, simulation loop
├── GameOfLife.h          # Declarations for GameOfLife, GameOfLifeDecay, GameOfLifeWrap
├── GameOfLife.cpp        # Base class: bounded neighbors, standard rules, printing, pause mode
├── GameOfLifeWrap.cpp    # Wrap-around neighbor lookup and step function
├── GameOfLifeDecay.cpp   # Decay rule set
├── Makefile              # build / run / clean / package
└── *.txt                 # Sample boards and saved runs
```

*Built as a C++ course project (HW6).*

# BeitaGo

## What is it?
BeitaGo is a lightweight Go AI written in C++ using deep learning algorithms. Its main goal is to be lightweight enough to be trained and run on consumer grade hardware, while simultaneously playing the game at a high level. The codebase uses Google Test for unit testing, dlib for deep learning, and Qt for a supplied front-end.

## How to compile?
You'll need to perform the following to get this program working:

- Clone the repository (and make sure that you recursively clone submodules to get Google Test and dlib).
```
git clone --recurse-submodules https://github.com/Biendeo/BeitaGo.git
```
- Run CMake in the root folder of the project. Supply any extra arguments as you see fit:
    - Define `BEITAGO_QT` to also compile BeitaGoQt. You only need the Qt libraries should you define this term.
      - If you do want the Qt build, you'll need to download a version of [Qt 5](https://www.qt.io/). I've tested this using Qt 5.10.1, but older versions may work. Then, add the Qt path into your `CMAKE_PREFIX_PATH` variable, and it should detect it.
    - Define `USE_AVX_INSTRUCTIONS` to slightly speed up dlib's execution on the CPU.
    - Define `DLIB_USE_CUDA` if you have CUDA drivers installed and cuDNN libraries available to greatly speed up dlib's training.
    - Define `CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS` to get CMake to export the engine as a dynamic DLL properly for Visual Studio builds.
- Build the project using your toolset.

## What gets output?
The project currently produces following outputs:
- BeitaGoEngine.dll (the naming depends on the build configuration) contains the main engine functionality of the program (the board, win states, etc.). This comes with its accompanying static library. The `include/` folder contains all the relevant include files.
- BeitaGoTest is the Google Test program that'll run through all the tests just to verify that the engine functions properly.
- BeitaGoASCII is a command-line application that allows users to interact with the engine. This is mostly to quickly test features and is not the main target.
- BeitaGoQt provides a user interface to interact with the game engine to play the game.
- BeitaGoGTP provides a terminal-based interface to communicating with the AI. It can be piped into any program that accepts GTP input such as [Sabaki](https://github.com/SabakiHQ/Sabaki).
- TestDLIB is a small program I've made just to demonstrate dlib's features. It should be replaced by an actual training program eventually.

## Roadmap
These are all the features that I intend to make:

### Engine
- [x] Board state
- [x] Player interaction
- [x] Undo/redo history
- [x] Scoring and win state
- [ ] Handicap
- [x] AI

### AI
- [x] Place tiles on the board
- [x] Manage game state
- [x] Deep learning training
- [ ] Playing at a higher level
- [x] Knowing when to pass/resign

### Qt UI (not the main focus)
- [x] Draw board
- [x] Draw board pieces
- [x] User interaction
- [ ] Customisable board/AI settings
- [ ] Savable sessions

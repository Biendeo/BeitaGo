# BeitaGo

## What is it?
BeitaGo is a lightweight Go client written in C++, using the Qt library for UI. Its main goal is to create a competitive Go AI while simultaneously keeping system requirements low and written with cross-platform in mind.

## How to compile?
You'll need to perform the following to get this program working:

- Clone the repository (and make sure that you recursively clone submodules to get Google Test and dlib).
```
git clone --recurse-submodules https://github.com/Biendeo/BeitaGo.git
```
- Download a version of [Qt 5](https://www.qt.io/). I've tested this using Qt 5.10.1, but older versions may work.
- Run CMake in the root folder of the project, supplying the argument `CMAKE_PREFIX_PATH` to where Qt is installed (so CMake finds the modules properly). Supply any extra arguments as you see fit.
- Build the project.

## What gets output?
The project currently produces three outputs:
- BeitaGoEngine.dll (the naming depends on the build configuration) contains the main engine functionality of the program (the board, win states, etc.). This comes with its accompanying static library. The `include/` folder contains all the relevant include files.
- BeitaGoTest is the Google Test program that'll run through all the tests just to verify that the engine functions properly.
- BeitaGoASCII is a command-line application that allows users to interact with the engine. This is mostly to quickly test features and is not the main target.
- BeitaGo also lets users interact with the engine, but it uses the Qt library to create a much friendlier interface.

## Roadmap
These are all the features that I intend to make. They're a bit rough right now, but I'll refine these later on.

### Engine
- [x] Board state
- [ ] Player interaction
- [ ] Undo/redo history
- [ ] Scoring and win state
- [ ] AI

### UI
- [x] Draw board
- [x] Draw board pieces
- [x] User interaction
- [ ] Customisable board/AI settings
- [ ] Savable sessions
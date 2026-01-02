# Game Engine

A cross-platform C game engine built with CMake.

## Building

### Prerequisites
- CMake 3.15 or higher
- A C compiler (GCC, Clang, or MSVC)

### Build Instructions

#### Linux/macOS
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

#### Windows (Visual Studio)
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

#### Windows (MinGW)
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

### Running
After building, the executable will be in `build/bin/GameEngine` (or `build/bin/GameEngine.exe` on Windows).

## Project Structure
```
.
├── CMakeLists.txt    # CMake configuration
├── src/              # Source files
│   └── main.c        # Main entry point
├── include/          # Header files (future)
└── README.md         # This file
```

## Development

The project uses C11 standard and includes:
- Cross-platform CMake configuration
- Basic game loop structure
- Platform-specific compiler flags

## Next Steps
- Add input handling
- Integrate a graphics library (SDL2, GLFW, etc.)
- Add audio support
- Implement game systems (ECS, physics, etc.)


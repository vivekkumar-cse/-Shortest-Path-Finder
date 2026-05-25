// Ronin Pathfinder Engine — entry point.
// Builds on Windows / Linux / macOS via CMake (see README.md).
#include "engine/Application.h"

int main() {
    ronin::engine::Application app;
    return app.run();
}

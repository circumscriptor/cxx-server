#include <memory>

struct Context {
    Context(int /*argc*/, char** /*argv*/) { }

    int run()
    {
        return 0;
    }
};

///
/// @brief Main entry
///
/// @param argc Number of arguments
/// @param argv Arguments
/// @return 0 on success
///
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    return std::make_unique<Context>(argc, argv)->run();
}

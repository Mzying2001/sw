#include "Test.h"

int main()
{
    int failed = 0;

    for (const auto &test : swtest::Registry()) {
        try {
            test.func();
            std::cout << "[PASS] " << test.name << "\n";
        } catch (const std::exception &ex) {
            ++failed;
            std::cerr << "[FAIL] " << test.name << "\n" << ex.what() << "\n";
        } catch (...) {
            ++failed;
            std::cerr << "[FAIL] " << test.name << "\nunknown exception\n";
        }
    }

    const int total = static_cast<int>(swtest::Registry().size());
    std::cout << total - failed << "/" << total << " tests passed\n";
    return failed == 0 ? 0 : 1;
}

#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
#include <execution>
#include <print>

namespace execution = std::execution;

void measure(auto policy, std::vector<std::uint64_t> v)
{
    const auto start = std::chrono::steady_clock::now();
    std::sort(policy, v.begin(), v.end());
    const auto finish = std::chrono::steady_clock::now();
    std::println("{}", std::chrono::duration_cast<std::chrono::milliseconds>(finish - start));
};

int main()
{
    std::vector<std::uint64_t> v(1'000'000);
    std::mt19937 gen { std::random_device {}() };
    std::ranges::generate(v, gen);

    measure(execution::seq, v);
    measure(execution::unseq, v);
    measure(execution::par_unseq, v);
    measure(execution::par, v);
    return 0;
}

#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>

int main() {
    // const int 1000 = 1000;
    // const int 1000 = 1000;
    const int size = 1000 * 1000;
    const int repetitions = 1000;

    std::vector<int> a(size, 1.0);
    std::vector<int> b(size, 2.0);
    std::vector<int> c(size, 0.0);
    std::chrono::duration<double, std::micro> duration1, duration2, duration3;
{
    auto start = std::chrono::high_resolution_clock::now();

    for (int r = 0; r < repetitions; ++r) {
for (int y = 0; y<1000; ++y)
 for (int x = 0; x<1000; ++x)
  c[y*1000 + x] = a[y*1000 + x] + b[y*1000 + x];
    }

    auto end = std::chrono::high_resolution_clock::now();

    double checksum = std::accumulate(c.begin(), c.end(), 0.0);

    duration1 = end - start;

    std::cout << "Checksum: " << checksum << "\n";
    std::cout << "Elapsed time xy: " << duration1.count()/repetitions << " microseconds\n";
}
{
    auto start = std::chrono::high_resolution_clock::now();
    for (int r = 0; r < repetitions; ++r) {
for (int x = 0; x<1000; ++x) //~|\label{line:optimised-loop-x}|
 for (int y = 0; y<1000; ++y)//~|\label{line:optimised-loop-y}|
  c[y*1000 + x] = a[y*1000 + x] + b[y*1000 + x];
    }

    auto end = std::chrono::high_resolution_clock::now();

    double checksum = std::accumulate(c.begin(), c.end(), 0.0);

    duration2 = end - start;

    std::cout << "Checksum: " << checksum << "\n";
    std::cout << "Elapsed time yx: " << duration2.count()/repetitions << " microseconds\n";
}
{
    auto start = std::chrono::high_resolution_clock::now();
    for (int r = 0; r < repetitions; ++r) {
#pragma omp parallel for
for (int y = 0; y<1000; ++y)
 for (int x = 0; x<1000; ++x)
  c[y*1000 + x] = a[y*1000 + x] + b[y*1000 + x];
}

    auto end = std::chrono::high_resolution_clock::now();

    double checksum = std::accumulate(c.begin(), c.end(), 0.0);

    duration3 = end - start;

    std::cout << "Checksum: " << checksum << "\n";
    std::cout << "Elapsed time parallel: " << duration3.count()/repetitions << " microseconds\n";
}
    std::cout << "Speedup: " << duration2.count() / duration1.count() << "\n";
    std::cout << "Speedup (parallel): " << duration2.count() / duration3.count() << "\n";
    return 0;
}
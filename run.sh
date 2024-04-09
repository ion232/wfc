set -e

main() {
    mkdir -p build
    cmake -DCMAKE_BUILD_TYPE=Release -GNinja -B build -S .
    ninja -C build
    # export ASAN_OPTIONS=halt_on_error=1:symbolize=1:print_stacktrace=1:check_initialization_order=1
    # export UBSAN_OPTIONS=print_stacktrace=1
    ./build/wfc_unit_tests
    # lldb -o run ./build/wfc
}

main

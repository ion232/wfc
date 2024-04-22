#/bin/bash

set -e

readonly build_dir="build"
readonly app_name="wfc_app"
readonly unit_tests_name="wfc_unit_tests"
readonly project_name="wfc"
readonly app_bin="./$build_dir/$app_name"
readonly test_bin="./$build_dir/$unit_tests_name"
readonly cwd="$(pwd)"

cmake_build_type="Release"
cmake_generator="Ninja"

build() {
    if [ "$cmake_build_type" == "Debug" ]; then
        export ASAN_OPTIONS=halt_on_error=1:symbolize=1:print_stacktrace=1:check_initialization_order=1
        export UBSAN_OPTIONS=print_stacktrace=1
    fi

    readonly threads=$(sysctl -n hw.ncpu)

    cmake -DCMAKE_BUILD_TYPE=$cmake_build_type -G$cmake_generator -B $build_dir -S .
    cmake --build "$build_dir" "-j$threads"
}

run() {
    if [ "$cmake_generator" == "Ninja" ]; then
        $app_bin "gui" "$cwd/assets/images/village.png" 128 128
    elif [ "$cmake_generator" == "Unix Makefiles" ]; then
        $app_bin "gui" "$cwd/assets/images/village.png" 128 128
    elif [ "$cmake_generator" == "Xcode" ]; then
        open "$build_dir/$project_name.xcodeproj"
    else
        echo "Unsupported generator: $cmake_generator"
    fi
}

benchmark() {
    if [ "$cmake_generator" == "Ninja" ]; then
        $app_bin "benchmark" "$cwd/assets/images/village.png"
    elif [ "$cmake_generator" == "Unix Makefiles" ]; then
        $app_bin "benchmark" "$cwd/assets/images/village.png"
    elif [ "$cmake_generator" == "Xcode" ]; then
        open "$build_dir/$project_name.xcodeproj"
    else
        echo "Unsupported generator: $cmake_generator"
    fi
}

test() {
    $test_bin
}

clean() {
    rm -rf "$build_dir"
}

main() {
    if [ "$#" -gt 3 ] || [ "$#" -lt 1 ]; then
        echo "Usage: $0 <build|run|clean> [generator] [build_type]"
        return
    fi

    if [ "$#" -gt 1 ]; then
        cmake_generator="$2"
    fi

    if [ "$#" -gt 2 ]; then
        cmake_build_type="$3"
    fi

    if [ "$1" == "build" ]; then
        build
    elif [ "$1" == "run" ]; then
        build && run
    elif [ "$1" == "benchmark" ]; then
        build && benchmark
    elif [ "$1" == "test" ]; then
        build && test
    elif [ "$1" == "clean" ]; then
        clean
    else
        echo "Invalid argument: $1"
    fi
}

main $@

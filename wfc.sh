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

dependencies() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # ion232: For SDL3.
        sudo apt-get update
        sudo apt-get install build-essential git make \
        pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
        libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
        libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev \
        libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
        libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev fcitx-libs-dev \
        libpipewire-0.3-dev libwayland-dev libdecor-0-dev
        sudo apt-get install g++ clang
        echo "[wfc] You will need to switch to clang for Debug ASAN builds."
        echo "[wfc] You may stick with g++ if only building for Release. (e.g. ./wfc.sh run)"
        sudo update-alternatives --config cc
        sudo update-alternatives --config c++
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "[wfc] Please ensure you have Xcode 15+ installed."
        brew install make ninja cmake
    fi
}

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
        $app_bin "gui" "$cwd/assets/images/flowers.png" 64 64
    elif [ "$cmake_generator" == "Unix Makefiles" ]; then
        $app_bin "gui" "$cwd/assets/images/flowers.png" 64 64
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

    if [ "$1" == "dependencies" ]; then
        dependencies
    elif [ "$1" == "build" ]; then
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

set -e

main() {
    mkdir -p .build
    cmake -B .build -S .
    make -C .build
    ./.build/wfc
}

main

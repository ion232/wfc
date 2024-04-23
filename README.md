# Wfc

This is yet another wfc ([wave function collapse](https://github.com/mxgmn/WaveFunctionCollapse)) library. I used [this great paper](https://escholarship.org/uc/item/3rm1w0mn) to understand its workings and write my implementation. This library is specifically designed for my own use case, but I've open sourced it in case it's useful to anyone - especially for learning, as most implementations are pretty unintelligible. There are no dependencies for the library itself except the C++20 standard library.

<p align="center"><img alt="demo" src="./.github/assets/wfc.gif"></p>

## Build setup

This codebase has only been tested on Linux and macOS, I've yet to test it on Windows.
**For macOS, you must have Xcode 15+ installed for C++20 support.**

```bash
git clone --recurse-submodules git://github.com/ion232/wfc.git
./wfc.sh dependencies
```

## Running

Once you have the required dependencies, use `wfc.sh` to run commands.

- `./wfc.sh build` will build all targets.
- `./wfc.sh run` will run a gui demo of continuous wfc.
- `./wfc.sh benchmark` will run a benchmark of the wfc library for increasing output sizes.
- `./wfc.sh test` will run the unit tests.
- `./wfc.sh clean` will remove the build directory.

## Is it fast?

- I've made various optimisations guided by Instruments on macOS, such as the custom bitset class for variables, which constitutes most of the computation footprint. It's fast enough for my use case, but there's definitely room for further improvement. Some performance has been sacrificed for generality and comprehensibility.

- `./wfc benchmark` for `village.png` on my standard m1 macbook gives the following rough numbers. As you can see, the time taken is roughly proportional to the size of the image, until (presumably) the program runs out of good cache to use and has to resort to slower cache or main memory. Multithreading, exploiting symmetry, more bitset improvements, etc, are not implemented but would all improve performance.
```
Image: "/Users/ion/Repos/wfc/assets/images/village.png"
8x8 t=1ms
16x16 t=10ms
32x32 t=50ms
64x64 t=210ms
128x128 t=915ms
256x256 t=4989ms
512x512 t=33845ms
```

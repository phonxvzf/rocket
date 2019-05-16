# Rocket
A real-time rocket launching simulator.

See [demonstration video](https://www.youtube.com/watch?v=HGBc8anXR54).

## Dependencies
1. SDL2
2. SDL2_image
3. cmake
4. C++ compiler with C++14 support

## Building
1. Choose `SIM_SIZE` of your choice. The default is 200x200.
2. `SIM_SIZE=200 cmake .`
3. `make`

## Running
```
$ ./rocket
```

*Note that the simulation starts at paused state.*

## Instructions
- `r` to reset
- `p` to pause/continue
- `q` to quit
- `space` to toggle pressure view

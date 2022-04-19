# kaji sdl example

Example project showcasing an IPC scenario where a SDL2 application can be controlled through a cli application. 

## Getting started

### Buiding

First prepare the cmake project. For example by putting the generated code in 'staging'.
```bash
cmake -B staging
```

Then start the build via:
```bash
cmake --build staging
```

### Running

The SDL application can then be started via:
```bash
staging/./kaji-sdl
```

While the SDL application is running, open another terminal and start:
```bash
staging/./kaji-cli
```

# C function defintion finder

This is a small C function definition finding program I made for personal use.

The main purpose of this is to find the location of the definition and the name of the function.

## Build

```console
./build.sh
```

## Usage

```console
$ ./main raylib.h
raylib.h:958:11: void InitWindow()
raylib.h:959:11: void CloseWindow()
raylib.h:960:11: bool WindowShouldClose()
raylib.h:961:11: bool IsWindowReady()
raylib.h:962:11: bool IsWindowFullscreen()
raylib.h:963:11: bool IsWindowHidden()
... etc
```

## Current Limitations

Currently this program does not support function declartaions such as these:

```c
struct { int a, b; } returns_struct();

enum {
    TEST_1,
    TEST_2
} returns_enum();

```

Don't do this anyway, whats wrong with you?

Also this program is a pretty slow, don't expect it to operate in high performance cases.
Scanning raylib.h takes 300ms on my machine.

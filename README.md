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
raylib.h:973:11: void InitWindow(int width , int height , const char * title )
raylib.h:974:11: void CloseWindow(void )
raylib.h:975:11: bool WindowShouldClose(void )
raylib.h:976:11: bool IsWindowReady(void )
raylib.h:977:11: bool IsWindowFullscreen(void )
raylib.h:978:11: bool IsWindowHidden(void )
raylib.h:979:11: bool IsWindowMinimized(void )
raylib.h:980:11: bool IsWindowMaximized(void )
raylib.h:981:11: bool IsWindowFocused(void )
raylib.h:982:11: bool IsWindowResized(void )
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

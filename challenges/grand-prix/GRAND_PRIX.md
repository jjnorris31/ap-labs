Multithreaded Grand Prix Game
===================================

This is an implementation of a a multithreaded grand prix simulator where every car will be an independent entity. 
It will be simulating an N laps race.


Features
--------

- The race's lap can be static or automatically generated.
- Racers number can be configured on start.
- Define a random max speed for each car.
- You will emulate the increasing speed behaviour that any car has.
- If a car detect another car on his route and it's slower, it must slow down its speed or try to rebase it.
- Each racer behaviour will be implemented as a separated thread.
- Cars threads must use the same map or city layout data structure resource.
- Define how many laps before starting the race.
- Display each car's speed, position, racing time and lap.
- At the end, display the top 3 winners.

Dependency Installation
-----------------------

### On __Windows__ based systems

1. Install MSYS2 from [MSYS2 installer](http://www.msys2.org/)
    * Version: latest (at time of writing 2.0)
    * Architecture: x86_64
    * Destination Folder: Select a folder that your Windows user owns
    
2. Install OpenGL from [OpenGL Wiki](https://www.khronos.org/opengl/wiki/Getting_Started#Windows)
    * Version: 3.3 (see appropiate windows drivers for your graphics hardware)
    * Architecture: x86_64
    * Destination Folder: Select a folder that your Windows user owns

### On __Linux__ based systems

On __CentOS/Fedora-like distributions__, you need the following packages:\
`libX11-devel libXcursor-devel libXrandr-devel libXinerama-devel mesa-libGL-devel libXi-devel`

On __Ubuntu/Debian-like Linux distributions__, you need the following packages:\
`libgl1-mesa-dev xorg-dev`

See [here](http://www.glfw.org/docs/latest/compile.html#compile_deps) for full details:

Compilation
-----------

__*Clone repository into the $GOPATH/src/ folder*__

### On __Linux__ or __MacOs__

```bash
# To compile the go proyect
make

# To remove all files (including executable) created after compilation.
make clean
```

### On __Windows__

```bash
# To get dependencies
go get -v

# To build the proyect
go build
```

How to run
----------

### On __Linux__

```bash
# -cars=num where num = number of cars (from 1 up to 10)
# -laps=num where num = number of laps (from 1 up to 10)
./CityTraffic [-ncars=4] [-laps=4]
```
### On __Windows__

```bash
# -ncars=num where num = number of cars (from 1 up to 12)
# -laps=num where num = number of laps (from 1 up to 10)
city-traffic.exe [-ncars=4] [-laps=4]
```

License
-------

The project is licensed under the MIT license.
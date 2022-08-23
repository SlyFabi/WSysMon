# WSysMon
A windows task manager clone for Linux.

<img src="screenshots/scr0.png" width="600" height="800"><img src="screenshots/scr1.png" width="600" height="800">


# Building
The following dependencies are required:
* `libgtkmm-3.0`

```
mkdir build
cmake -B ./build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build --config Release
```

# Limitations
* Apps Category only works on X11
* Only NVIDIA GPUs using the proprietary driver are detected

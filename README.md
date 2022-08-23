# WSysMon
A windows task manager clone for Linux.

<img src="screenshots/scr0.png" style="max-width: 70%; height: auto;">
<img src="screenshots/scr1.png" style="max-width: 70%; height: auto;">


# Installation
On Arch based distros (AUR):
```
$ yay -S wsysmon-git
```

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

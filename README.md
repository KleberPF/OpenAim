# OpenAim

OpenAim is a small aim trainer made from scratch in C++, using OpenGL as the rendering backend.

## Building on Linux

```
git clone --recurse-submodules https://github.com/KleberPF/OpenAim.git
cd OpenAim
mkdir build
cd build
cmake ..
make
```

To run the game

```
./OpenAim
```

## Building on Windows

```
git clone --recurse-submodules https://github.com/KleberPF/OpenAim.git
```

Inside Visual Studio, open the CMake project and select the OpenAim.exe target

![image](https://github.com/KleberPF/OpenAim/assets/43550602/17f8bb7a-f460-45a6-9fbf-c8fc51befc11)

![image](https://github.com/KleberPF/OpenAim/assets/43550602/afa865c6-c61e-49b0-b809-b38d74a1b7e3)

## Media

### Main Menu
![image](https://github.com/KleberPF/OpenAim/assets/43550602/7e19f89f-596f-4de0-90f9-36026a27ece2)

### Settings
![image](https://github.com/KleberPF/OpenAim/assets/43550602/8660a748-d1b4-4350-84ac-b955207169e9)

### Demo
https://github.com/KleberPF/OpenAim/assets/43550602/8ee21462-c848-48d8-a98f-80ac2731eeed

## Scenario File Syntax

Each scenario is represented by a JSON file inside `/resources/scenarios`. The name shown in game for each scenario is the file name of that scenario. This is an example JSON:

```
{
    "weapon": "machine_gun",
    "playerPos": "0.0 1.5 8.0",
    "targets": [
        {
            "shape": "box",
            "scale": "0.3 3.0 0.3",
            "spawnCoords": "0.0 1.55 -8.0",
            "onDestroy": "die",
            "health": 200,
            "moves": true,
            "movementAmplitude": 5,
            "movementSpeed": 2
        }
    ]
}
```

Here are all the possible fields with their respective meanings:

* `"weapon"`: either `"pistol"` or `"machine_gun"` (case insensitive). Selects the type of weapon to be used in the scenario.
* `"playerPos"`: starting player position represented by a string with 3 numbers for the x, y and z coordinates. For now, player movement is not supported through scenario files.
* `"targets"`: JSON array containing the definition for each target

Target fields:

* `"shape"`: either `"ball"` or `"box"` (case insensitive).
* `"scale"`: scaling factor for the target. Can be either a string with 3 numbers for a box with sides with different lengths or one number for a ball.
* `"randomSpawn"` (optional): `true` by default. If true, the target's initial position is a random value that falls within `"minCoords"` and `"maxCoords"`
* `"minCoords"`: required if `"randomSpawn"` is `true`. The smallest possible coordinate the target can have when spawning randomly.
* `"maxCoords"`: required if `"randomSpawn"` is `true`. The largest possible coordinate the target can have when spawning randomly.
* `"health"` (optional): the target's health. 1 by default.
* `"moves"` (optional): `false` by default. Sets if the target moves or not. For now, movement only happens horizontally.
* `"movementAmplitude"`: required if `"moves"` is set. Represents how many units the target oscillates. If set to x, the target moves from -x to x, relative to its starting position.
* `"movementSpeed"`: required if `"moves"` is set. Represents the speed of the movement of the target.
* `"onDestroy"`: either `"die"` or `"move"`. Sets the behavior of the target when destroyed, where `"die"` makes it disappear while `"move"` moves it to a new random location.


cmake
```
cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S source -B bin
cmake --build bin
```


## Modules
- RENDERER
  - WORLD
  - Objects
- INPUT
  - USER
    - KEYBOARD
    - MOUSE/MOUSEPAD/TRACKPAD
  - FILES
    - IMAGES
    - TEXT
      - CONFIG
      - SHADER
      - ETC.
  - API
    - connections to web??
- ADDON MANAGER


TILE (square 2 triangles)
- Postition
  - x,z
  - 6x height
  - slope
- Structure
  - farmland.
  - settlement.
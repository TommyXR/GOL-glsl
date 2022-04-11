# Game of life compute shader edition
___


To compile, you need GLFW installed on your system. Then you can just run 
```bash
mkdir build && cd build
CC=clang CXX=clang++ cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja
cd ..  && ./build/slime
```


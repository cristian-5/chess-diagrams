clang++ -std=c++20 -O3 -c lodepng.cpp -o lodepng.obj
clang++ -std=c++20 -O3 -c route66.cpp -o route66.obj
clang++ -std=c++20 -O3 -c main.cpp -o main.obj
clang++ -std=c++20 -o main.exe main.obj route66.obj lodepng.obj
del *.obj
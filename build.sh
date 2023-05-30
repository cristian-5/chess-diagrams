clang++ -std=c++20 -O3 -c lodepng.cpp -o lodepng.o
clang++ -std=c++20 -O3 -c route66.cpp -o route66.o
clang++ -std=c++20 -O3 -c main.cpp -o main.o
clang++ -std=c++20 -o main main.o route66.o lodepng.o
rm *.o
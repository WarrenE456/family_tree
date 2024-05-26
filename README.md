Family Tree
Given the genotype of a male and female, this program can generate a pedigree of their possible offspring.
# Requirements
To run this program, you must have CMake, Make, and Git. Additionally, you must set up termio.h. This program was developed and tested on Debian Linux, however, it should work on all Unix-based operating systems hopefully.
# How to use
Once all the requirements are met, clone the repository, change directory into it, make an out directory, change to the out directory, run Cmake and Make, then run the executable 'tree'. The program will prompt you to create some traits and assign those traits to the initial male and female. After that, the controls will be listed at the top of the screen.
```
git clone https://github.com/WarrenE456/family_tree.git
cd family_tree
mkdir out
cd out
cmake ..
make
./tree
```

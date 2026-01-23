# lc3-vm
makeing A lc3 Virtual Machine


### How To Compile

```c
//Step 1: compile .c → .o
gcc -c src/*.c -Iinclude // I{$} this for Header folder name

//Step 2: link object files
gcc *.o -o program
```
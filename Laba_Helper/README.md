# Laba Helper - the best choice for building graphs

## How to install
```
git clone git@github.com:KetchuppOfficial/Labs_In_General_Physics.git
cd ./Labs_In_General_Physics/Laba_Helper
```

## How to use

1) Build & Run
```
cmake -B=./build
cd build
make
./Laba_Helper
```

2) You will see such menu:
```
username@machine:~/Labs_In_General_Physics/Laba_Helper/build$
Laba_Helper - the best choice for labs in general physics

Available modes:
1) Build a graph;
2) Calculate error

Desired mode:
```

3) Follow the instructions and you will recieve your graph:
```
username@machine:~/Labs_In_General_Physics/Laba_Helper/build$
Laba_Helper - the best choice for labs in general physics

Available modes:
1) Build a graph;
2) Calculate error

Desired mode: 1
Type the name of file specifying the directory: ../test.lab

*********** Chi-square coefficients ***********
Approximation line: y = kx + b
k = 0.000649 +- 0.000016;
b = -0.001092 +- 0.001678
***********************************************

Printing graph...
```

*) Handling input mistakes
```
username@machine:~/Labs_In_General_Physics/Laba_Helper/build$
Laba_Helper - the best choice for labs in general physics

Available modes:
1) Build a graph;
2) Calculate error

Desired mode: ifdf
"ifdf" is not a number. Please, try again.

Desired mode: 1215dgdg
You have written a number and some inappropriate symbols after that. Please, try again.

Desired mode: 7
There is no mode with number 7. Please, try again.

Desired mode: 1
Type the name of file specifying the directory:
```

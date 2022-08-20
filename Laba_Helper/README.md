# Laba Helper - the best choice for building graphs

## How to install
```
git clone git@github.com:KetchuppOfficial/Labs_In_General_Physics.git
cd ./Labs_In_General_Physics/Laba_Helper
```

## How to use

1) Build & Run
```
cmake -B build
cd build
cmake --build .
./Laba_Helper
```

2) You will see such menu:

![picture_1](/Laba_Helper/pictures/picture_1.png)

3) Follow the instructions and you will recieve your graph:

![picture_2](/Laba_Helper/pictures/picture_2.png)

## Format of the description file

When this program asks you to "type the name of a file specifying the directory", it expects you to specify the path to a **description file** of your future graph.

**Description file** is a text file that describes how your graph should look and what data it should represent. Every desctiprion file consists of *labels*, *strings* and *numbers*. We will call them all together as *structural units*.

A structural unit is sequence of characters that has a specific semantic meaning. Structural units are separated from each other by at least one whitespace character.

Let's discuss types of structural units in turn.

### Labels

A label shows the semantic meaning of a structural unit that follows it (there is only one exception that is **No_Line** label). 

The name of a label has to end in a colon.

Labels can be placed in any order.

There are 13 labels:

1) **Graph_Title**
```
    Next structural unit: a string representing the title of the graph (see Graph Example, 1).
```

2) **Dot_Label**
```
    Next structural unit: a string representing the label for dots on the graph (see Graph Example, 2).
```

3) **Dot_Colour**
```
    Next structural unit: a string representing the colour of dots on the graph.

    Dot_Colour label is optional. Its absence sets the colour of dots to blue.
```

4) **No_Line**
```
    This label has to be the last structural unit of a desctiption file or has to be followed by another label.

    If No_Line label is presented, there will be no a line on graph (just dots).

    No_Line label is optional.

    No_Line and Line_Colour labels cannot be used simultaneously in a description file.
```

5) **Line_Colour**
```
    Next structural unit: a string representing the colour of the line on the graph.

    Line_Colour and No_Line labels cannot be used simultaneously in a description file.

    Line_Colour label is optional. Its absence sets the colour of the line to green.
```

6) **X_Data**
```
    Next structural unit: a number representing a value on the abscissa axis.

    This label can be followed by more than one number.
    
    A quantity of following numbers has to be same as that of X_Error, Y_Data and Y_Error labels.
    
    A quantity of following numbers has to be not less than 3.
```

7) **X_Error**
```
    Next structural unit: a number representing a margin of error of the respective value on the abscissa axis.

    This label can be followed by more than one number.
    
    A quantity of following numbers has to be same as that of X_Data, Y_Data and Y_Error labels.
    
    A quantity of following numbers has to be not less than 3.
```

8) **X_Title**
```
    Next structural unit: a string representing the title of the abscissa axis (see Graph Example, 3).
```

9) **Y_Data**
```
    Next structural unit: a number representing a value on the ordinate axis.

    This label can be followed by more than one number.
    
    A quantity of following numbers has to be same as that of X_Data, X_Error, and Y_Error labelas.

    A quantity of following numbers has to be not less than 3.
```

10) **Y_Error**
```
    Next structural unit: a number representing a margin of error of the respective value on the ordinate axis.

    This label can be followed by more than one number. 
    
    A quantity of following numbers has to be same as that of X_Data, X_Error and Y_Data labels.
    
    A quantity of following numbers has to be not less than 3.
```

11) **Y_Title**
```
    Next structural unit: a string representing the title of the ordinate axis (see Graph Example, 4).
```

12) **Error_Colour**
```
    Next structural unit: a string representing the colour of error crosses.

    Error_Colour label is optional. Its absence sets the colour of error crosses to red.
```

13) **Image_Name**
```
    Next structural unit: a string representing the path to the future graph.
    
    The name of the graph-file has to be specified too.
```

If a label is followed by an unexpectec structural unit, the program terminated with an error and an error report is shown on the screen (more on than later).

### Strings

A string is a sequence of more than 1 characters surrounded by double quotes. Thus, empty strings are not strings and are forbidden.
```
    "Hello, World!"    // a string
    "./test_graph.png" // also a string
    ""                 // empty string: forbidded
```

### Numbers

A number is almost a C number of type **double**. Even if all fractional digits are zeros a number has to end in a decimal point and at least one trailing zero.
```
    3.14 // a number
    2.72 // also a number
    5    // not a number: no decimal point 
         // and trailing zeros
    5.   // not a number: no trailing zeros
    5.0  // a number
    5.00 // also a number
```

### Graph Example
![graph_example](/Laba_Helper/pictures/graph_example.png)

## Comments

Apart from discussed structural units you can write comments in description files. These comments behave exactly like those in C++: all characters starting with two slashes in a row and ending with EOL (end of line) character are ignored.
```
Graph_Title: // this is comment #1
    "U(I)"   // this is comment #2
```

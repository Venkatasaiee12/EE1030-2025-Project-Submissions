
# Requirements
1. Compiler
    - GCC or Clang
2. Header Files
    - As this code uses external libraries, you need to download them from the  internet. However they are already present in the `c_libs` folder

## How to Run
### Compilation
```bash
gcc -o main main.c -lm
```
> main.c can be found in `c_main` folder

The images are in the `figs` folder. Choose an Image from them by navigating into that folder.
### Usage after Compilation
1. If it is grayscale,
```bash
./main  path_to_your_image name_of_the_output_file  
```
2. If it is color,
```bash
./main  path_to_your_image name_of_the_output_file 3  
```
> No need to specify extension in the `name_of_the_output_file`, it will be added based upon the original file extension as jpg/png
### Analysing the Output 
1. If it is grayscale,

    k= `5/20/50/100` 
    > Displays the current k value the code is working upon. As only these k values are defined in the code, only these values will be displayed. If necessary, you are free to change them in the code.

    `Error: ` 
    > Displays Frobenius Error.

    `Total Reconstruction Time: ` 
    > Time taken to reconstruct the matrix.

    `Saved: ` 
    > Shows the name of the saved image.

2. If it is color,

    k= `5/20/50/100` 
    > Displays the current k value

    `Error in Channel 1-3: ` 
    > Displays Frobenius Error in each channel starting from 1 to 3 .

    `Total Reconstruction Time: ` 
    > Time taken to reconstruct the matrix.

    `Saved: ` 
    > Shows the name of the saved image.
    
### Comparing the Reconstruction time with Python's numpy library

1. Run the main.py inside the `python_driver`.
    > You may need to change the name of the input image file manually.
2. The output will be the Total Reconstruction Time for different values of k.

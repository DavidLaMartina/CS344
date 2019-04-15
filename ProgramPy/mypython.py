# David LaMartina
# lamartid@oregonstate.edu
# CS344 Spr2019
# Program Py - Python Exploration
# Due May 20, 2019

import sys
from random import choice, randint
from string import ascii_lowercase

# Set up assigned parameters
numChars = 10
numIter  = 3
minInt   = 1
maxInt   = 42

# Generate 10-char random lowercase strings w/ newline, write & output
for iteration in range(0, numIter):
    string = "".join(choice(ascii_lowercase) for x in range(0, numChars)) + '\n'
    fileName = "myOut" + str(iteration)

    outputFile = open(fileName, "w+")
    outputFile.write(string)
    sys.stdout.write(string)
    outputFile.close()

# Output 2 rand nums [1...42], output on lines 4 & 5, output product line 6
num1 = randint(minInt, maxInt)
num2 = randint(minInt, maxInt)
prod = num1 * num2

sys.stdout.write(str(num1) + '\n')
sys.stdout.write(str(num2) + '\n')
sys.stdout.write(str(prod) + '\n')

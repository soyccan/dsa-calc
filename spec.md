# DSA 2020 HW 3.4 Specifications
In this spec, first we will tell you how the TAs are going to test your program. Then, we will tell you how the total points are split into different test cases, what kind of inputs will be in every test case, and provide a sample input/output of every test case.

## Testing
You should provide your own Makefile.
The TAs will run `make` to compile your source code **on the CSIE workstation** and use the following command to test your program:
```
./hw3_1
```
and
```
./hw3_2
```
Your program should read from the standard input, and write on the standard output.

## Integer Calculator (60%)

### Written Part (15%)
Please prepare **3** test cases **in the written part** that help the TAs verify all the operations are implemented correctly. That is, print out the input and the corresponding output of your program. The output should include
- your stack operations on how to transform the infix expression to a postfix one,
-  the corresponding postfix expression, and
-   the evaluated result, which should be exactly the same as the result computed by a usual C statement on the same expression.

Since the written part will be graded manually, there is no strict restriction on the ouput format. However, note that given a specific input, there is only one correct postfix expression and evaluated result. Also, the test cases and the outputs you provided in the written part **should not exceed 1 page in total**.

### Coding Part (45%)
The coding part will be graded automatically, where the judge will **only check the correctness of the evaluated result**. Your program can print only the evaluated result in the output, or have other information printed before the evaluated result (like the stack operations and the postfix expression). We will take **the line starting with `RESULT:`** of your output as the evaluated result. The evaluated result should be exactly the same as the result computed by a usual C statement on the same expression, to be viewed as a correct answer. The time limit is 10 seconds per test case.

#### Special Notes
- Your program is prohibited to call other processes, or the Python library. So you cannot, for example, write a new C file based on the input expression, execute it, and print out its output. These kinds of behaviors are strictly prohibited and you might get a zero for the entire homework if you do this.
- Since the input will contain characters from only numbers and the needed operators, there will be no spaces in the input expression. Therefore, you do not need to worry about expressions like `1++-1` or `1+ +-1`. The former is not a valid C statement, and the latter contains a space, so both cannot be in the input.
- We guarantee that under a standard evaluation on the correct postfix expression of the input expression, there will be no overflow or any errors/warnings (divide by 0, etc.) produced during the evaluation.
- We guarantee that the input expression is a valid C statement, where compilation will not produce any warnings or errors.

### Test case 1 (15%)
The input expression will only contain the binary operators `+,-,*,/,%`, parentheses `()`, and characters from numbers (digits `0~9`).

#### Sample Input
```
2*(1+2*3)-20/3%4
```
#### Sample Output
```
RESULT: 12
```
The correct postfix expression of the input expression is `2 1 2 3 * + * 20 3 / 4 % -`.

### Test case 2 (15%)
The input expression will **not** contain the unary operators `+,-`.
#### Sample Input
```
2+!~(2-3)*5==7
```
#### Sample Output
```
RESULT: 1
```
The correct postfix expression of the input expression is `2 2 3 - ~ ! 5 * + 7 ==`.
### Test case 3 (15%)
No special restrictions.
#### Sample Input
```
1+-+1
```
#### Sample Output
```
RESULT: 0
```
The correct postfix expression of the input expression is `1 1 + - +`.

## Scientific Calculator (50%)
### Written Part (15%)
Please prepare **3** test cases **in the written part** that help the TAs verify that all the operations are implemented correctly. That is, print out the input and the corresponding output of your program. The output should include
- your stack operations on how to transform the infix expression to a postfix one,
-  the corresponding postfix expression, and
-   the evaluated result, which should be exactly the same as the result computed by a usual C statement on the same expression if the numbers are all type *double*, rounded to exactly 6 digits after the decimal point.

Since the written part will be graded manually, there is no strict restriction on the ouput format. However, note that given a specific input, there is only one correct postfix expression and evaluated result. Also, the test cases and the outputs you provided in the written part **should not exceed 1 page in total**.

### Coding Part (35%)
The coding part will be graded automatically, where the judge will **only check the correctness of the evaluated result**. Your program can print only the evaluated result in the output, or have other information printed before the evaluated result (like the stack operations and the postfix expression). We will take **the last line** of your output as the evaluated result. The evaluated result should be exactly the same as the result computed by a usual C statement on the same expression if the numbers are all type *double*, rounded to exactly 6 digits after the decimal point, to be viewed as a correct answer. The time limit is 10 seconds per test case.

#### Special Notes
- All special notes in the Integer Calculation section.
- You don't need to worry about floating point errors. We will guarantee that under a standard evaluation on the correct postfix expression of the input expression, the floating point error accumulated will not be too large that the evaluated result rounded to 6 digits after the decimal point differs from the judge's answer.
- Please use the `math.h` or `cmath` library to implement the function calls.

### Test case 1 (15%)
The input expression will **not** contain function calls.

#### Sample Input
```
-4.1436*(5.24-19*0.99)
```
#### Sample Output
```
RESULT: 56.228652
```
The correct postfix expression of the input expression is `4.1436 - 5.24 19 0.99 * - *`.

### Test case 2 (20%)
No special restrictions.
#### Sample Input
```
log(pow(sin(100),2)+pow(cos(100),2))
```
#### Sample Output
```
RESULT: 0.000000
```
The correct postfix expression of the input expression is `100 sin 2 pow 100 cos 2 pow + log`.
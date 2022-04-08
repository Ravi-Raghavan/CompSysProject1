# CompSysProject1
This is Group 9's Submission of Project 1 with all of the C Programs associated with Problem 1 and 2 of this Project.

# Problem 1 Information 
Note: For Question 1 since we are just using 1 process, I made the output be written to an outputFile called output_Problem1_Question1.txt. For the rest of the programs, since we were using multiple processes, I made the output written to both a output File and the terminal. The reason I did this is so that I could debug things more easily(i.e. more debugging was needed for program that generated more processes).  
# Question 1 of Problem 1
How to execute this program: 
Required Inputs: 
- The 1st Input(argv[1]) must be equal to the number of positive integers to place in the File
- The 2nd input(argv[2]) must be equal to the number of Hidden Keys(i.e. integers whose value is -1) to place in the File
- The 3rd Input (argv[3]) must be equal to the number of Hidden Keys, out of the total number of Hidden Keys in the File, to identify. Hence, argv[3] <= argv[2]

Note: Instead of manually generating text files populated with numbers during every test run, I had my program generate a test file programmatically. In other words, my program will generate the Input text file for you and will populate it with numbers for you. I included C code within my programs(i.e. I had a function called generateTestFile()) that will generate a test file based on the values of the 1st and 2nd Inputs to the Program(i.e. argv[1] and argv[2]). I used the rand() function in C to generate a random number from 0 to RAND_MAX(i.e. 2147483647 on my Operating System). However, it is well known that, in C programming, if we add extremely large numbers, due to the fact that integers are only 4 bytes, if the sum exceeds 2147483647, the C library will be forced to truncate the result and, thus, the sum will not accurately represent the actual sum. Hence, to get around this, whenever I generated random Numbers, I used the following piece of code: rand() % MAX where MAX = RAND_MAX / L. This way, in the worst case, even if every number we generate is equal to RAND_MAX / L. The total sum of all the L positive integers cannot exceed RAND_MAX

So, when you run the program, you do not need to actually generate the numbers.txt file yourself. Upon execution of the program with the aforementioned inputs, the numbers.txt file will automatically be generated, the numbers.txt file will automatically be populated with random numbers as discussed previously, the numbers from the numbers.txt will automatically be read into an Array by the program, and the computations/identification of the hidden numbers will be performed by the program itself 

# Question 2 of Problem 1
How to execute this program: 
Required Inputs: 
- The 1st Input(argv[1]) must be equal to the number of positive integers to place in the File
- The 2nd input(argv[2]) must be equal to the number of Hidden Keys(i.e. integers whose value is -1) to place in the File
- The 3rd Input (argv[3]) must be equal to the maximum number of processes that can be in our process tree. Please note that this INCLUDES the main process. So, if you want your process tree corresponding to this program to have a maximum of 10 processes, set argv[3] to 10. This means that a total of 9 processes will be generated via the fork() call. The main process already exists when we execute this program!
- The 4th Input (argv[4]) must be equal to the number of Hidden Keys, out of the total number of Hidden Keys in the File, to identify. Hence, argv[4] <= argv[2]

Note: Instead of manually generating text files populated with numbers during every test run, I had my program generate a test file programmatically. In other words, my program will generate the Input text file for you and will populate it with numbers for you. I included C code within my programs(i.e. I had a function called generateTestFile()) that will generate a test file based on the values of the 1st and 2nd Inputs to the Program(i.e. argv[1] and argv[2]). I used the rand() function in C to generate a random number from 0 to RAND_MAX(i.e. 2147483647 on my Operating System). However, it is well known that, in C programming, if we add extremely large numbers, due to the fact that integers are only 4 bytes, if the sum exceeds 2147483647, the C library will be forced to truncate the result and, thus, the sum will not accurately represent the actual sum. Hence, to get around this, whenever I generated random Numbers, I used the following piece of code: rand() % MAX where MAX = RAND_MAX / L. This way, in the worst case, even if every number we generate is equal to RAND_MAX / L. The total sum of all the L positive integers cannot exceed RAND_MAX

So, when you run the program, you do not need to actually generate the numbers.txt file yourself. Upon execution of the program with the aforementioned inputs, the numbers.txt file will automatically be generated, the numbers.txt file will automatically be populated with random numbers as discussed previously, the numbers from the numbers.txt will automatically be read into an Array by the program, and the computations/identification of the hidden numbers will be performed by the program itself 

# Question 3 of Problem 1
How to execute this program: 
Required Inputs: 
- The 1st Input(argv[1]) must be equal to the number of positive integers to place in the File
- The 2nd input(argv[2]) must be equal to the number of Hidden Keys(i.e. integers whose value is -1) to place in the File
- The 3rd Input (argv[3]) must be equal to the maximum number of processes that can be in our process tree. Please note that this DOES NOT INCLUDE the main process. So, if you want your process tree corresponding to this program to have a maximum of 10 processes, set argv[3] to 9. This means that a total of 9 processes will be generated via the fork() call. The main process already exists when we execute this program!
- The 4th Input (argv[4]) must be equal to the fixed number of child processes that each process can have
- The 5th Input (argv[5]) must be equal to the number of Hidden Keys, out of the total number of Hidden Keys in the File, to identify. Hence, argv[5] <= argv[2]

Note: Instead of manually generating text files populated with numbers during every test run, I had my program generate a test file programmatically. In other words, my program will generate the Input text file for you and will populate it with numbers for you. I included C code within my programs(i.e. I had a function called generateTestFile()) that will generate a test file based on the values of the 1st and 2nd Inputs to the Program(i.e. argv[1] and argv[2]). I used the rand() function in C to generate a random number from 0 to RAND_MAX(i.e. 2147483647 on my Operating System). However, it is well known that, in C programming, if we add extremely large numbers, due to the fact that integers are only 4 bytes, if the sum exceeds 2147483647, the C library will be forced to truncate the result and, thus, the sum will not accurately represent the actual sum. Hence, to get around this, whenever I generated random Numbers, I used the following piece of code: rand() % MAX where MAX = RAND_MAX / L. This way, in the worst case, even if every number we generate is equal to RAND_MAX / L. The total sum of all the L positive integers cannot exceed RAND_MAX

So, when you run the program, you do not need to actually generate the numbers.txt file yourself. Upon execution of the program with the aforementioned inputs, the numbers.txt file will automatically be generated, the numbers.txt file will automatically be populated with random numbers as discussed previously, the numbers from the numbers.txt will automatically be read into an Array by the program, and the computations/identification of the hidden numbers will be performed by the program itself 

# Variation Problem of Problem 1
How to execute this program: 
Required Inputs: 
- The 1st Input(argv[1]) must be equal to the number of positive integers to place in the File
- The 2nd input(argv[2]) must be equal to the number of Hidden Keys(i.e. integers whose value is -1) to place in the File
- The 3rd Input (argv[3]) must be equal to the maximum number of processes that can be in our process tree. Please note that this INCLUDES the main process. So, if you want your process tree corresponding to this program to have a maximum of 10 processes, set argv[3] to 10. This means that a total of 9 processes will be generated via the fork() call. The main process already exists when we execute this program!
- The 4th Input (argv[4]) must be equal to the fixed number of child processes that each process can have

Note: Instead of manually generating text files populated with numbers during every test run, I had my program generate a test file programmatically. In other words, my program will generate the Input text file for you and will populate it with numbers for you. I included C code within my programs(i.e. I had a function called generateTestFile()) that will generate a test file based on the values of the 1st and 2nd Inputs to the Program(i.e. argv[1] and argv[2]). I used the rand() function in C to generate a random number from 0 to RAND_MAX(i.e. 2147483647 on my Operating System). However, it is well known that, in C programming, if we add extremely large numbers, due to the fact that integers are only 4 bytes, if the sum exceeds 2147483647, the C library will be forced to truncate the result and, thus, the sum will not accurately represent the actual sum. Hence, to get around this, whenever I generated random Numbers, I used the following piece of code: rand() % MAX where MAX = RAND_MAX / L. This way, in the worst case, even if every number we generate is equal to RAND_MAX / L. The total sum of all the L positive integers cannot exceed RAND_MAX

So, when you run the program, you do not need to actually generate the numbers.txt file yourself. Upon execution of the program with the aforementioned inputs, the numbers.txt file will automatically be generated, the numbers.txt file will automatically be populated with random numbers as discussed previously, the numbers from the numbers.txt will automatically be read into an Array by the program, and the computations/identification of the hidden numbers will be performed by the program itself 

# Problem 2
How to execute this program: 
No Arguments Needed. Simply execute as normal!

Some additional information is that I generated process trees in the main process just to analyze how the process trees were working so that I could answer the accompanying questions for this Problem. Feel free to comment them out if it is making the output terminal cluttered 

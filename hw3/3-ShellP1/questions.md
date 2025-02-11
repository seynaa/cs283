1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is a good choice because it reads a whole line of input from stdin and makes sure that it doesn't overflow the buffer. 

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  You need to use malloc() to allocate memory for cmd_buff in dsh_cli.c because it lets you use dynamic memory allocation when the size of the input can vary. 


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  It is necessary to trim leading and trailing spaces from each command before storing it because this makes sure that the command is parsed correctly. 

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  echo "Hi" > output.txt : if the file already exist we need to see if we want to overwrite or append data to it. 
    sort < input.txt : the shell will have to open, read, and use that as input, will cause errors if the file doesn't exist
    echo " seyna" >> output.txt : open the file and make sure sure that no data is lost when different commands are appending to the same file. 

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection lets the shell direct input or output files and changes the flow of data. 
    Piping connect the output of one command to the input of another command. 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Keeping STDERR and STDOUT in different shells is important because it lets users capture and redirect normal output and error. 

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Users can merge the two using 2>&1. 
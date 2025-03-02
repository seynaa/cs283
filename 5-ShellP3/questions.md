1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

The waitpad() function is called on every child process to make sure that the parent shell waits for the child to be completed. Fork() makes a child process for each command in the pipeline, after each child process has been forked the parent waits for the child process to finish using waitpid(). If waitpif() is not called, the parent shell can accept new input and execute new command while the child is running. 

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

After usinhg dup2() to redirect file descriptors, you need to make sure to close the unused pipe ends because the file descriptors for the pipe will stay open unless closed. If the process a process does not close unused pipe ends it could stay open for reading and writing, this will cause the process to be left hanging. 

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is built-in because it changes the parent shell's working directory. There is another process running in a seperate memory space, any chnages made would affect the child process and not persist in the shell. This makes cd built-in makingh sure that the change applies to the shell itself. 

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

Instead a a fixed cmd_max array i woulduse dynamic memory to store commands. The trade-offs would be memory management, performance, and how complexity it is. 

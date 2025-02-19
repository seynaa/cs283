1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  We use fork() to make a child process before calling execvp(). This makes sure that the new program replaces only the child, leaving the parent running.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  if fork() fails, it return -1, this means that a new process could not be created due to system limitations. My implementation prevents the shell from crashing and lets it continue running. 

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() looks for the specified command in the directorieslisted in the Path environment variable. 

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The wait() function makes the parent process wait for the child processto finish. If we didn't call it the child process would run in the back and the shell might immediately return the prompt.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() extracts the exit status of a terminated child ptocess. It helps in checking whether a command completed correctly. 

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  By preserving spaces in the quotes and removing the enclosing quotes. This is necessary because some commands need arguments with spaces. 

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I changed how quoted arguments are being handled by trimming trailing quotes. A challenge I faced wwas handling cases where the quote was nested.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are a form of asynchronous interprocess communication used to notify a process about events. They require no communication setup and can be sent between unrelated processes. 

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:     
    - SIGKILL: Immediately terminates a process and cannot be caught or ignored. Used to forcefully stop unresponsive processes (kill -9 <pid>).
    - SIGTERM: Requests a process to terminate, allowing it to clean up resources before exiting. Used for controlled shutdowns (kill <pid>).
    - SIGINT: Sent when the user presses Ctrl+C in a terminal. It interrupts and terminates a foreground process.


- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: When a process receives "SICSTOP", it is suspended until resumed. It cannot be caught or ignored like SIGNT because it is designed to always suspend execution. 

z-Text: a bare-bones text-editor

-basically an implementation of the stack interface using a dynamic array

-generates a blank window via the NCurses API that is the size of the user's monitor, and sends
user input to it one character at a time, simulating a real-world text-editor

-supports automatic newline generation and newline removal (in case the user
wants to retrace their 'typing-steps'.

-to run, simply compile using your favorite c++ compiler, so long as you have NCurses installed on the machine

-TODO: add a menu for saving the file, loading an existing file

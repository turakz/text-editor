/******************C-STYLE CHARACTER BUFFER IMPLEMENTED AS ARRAY STACK*******************
 ****************************************************************************************/
#include <iostream>
#include <ncurses.h>
#include <algorithm>

using namespace std;

struct Stack{

    int len;
    int top;
    int last_char_index;
    int line_count;
    int* buffer;
    int x;
    int y;
    int height;
    int width;


    //character buffer
    Stack()
        :len(50), top(len), last_char_index(0), line_count(0), x(0), y(0){
            
            //initialize buffer
            buffer = new int[len];
            for (int i = 0; i < len; i++){
                buffer[i] = 0;
            }

            //screen dimensions -> (y, x)
            getmaxyx(stdscr, height, width);

        }

    ~Stack(){
        delete [] buffer;
    }

    void push(int);
    void pop();
    int peek();
    int size();
    void resize();
    void printBufferContents();
    bool empty();
    bool full();
    
    //push() and pop() cursor management
    void gotoNewLine();
    void gotoPreviousLine();
    bool newLine(char c);
    bool startOfLine();
    bool endOfLine();
    void addChar(char c);
    void deleteChar();


};

using namespace std;

int main(){

    //initalize screen
    initscr();
    
    //initialize buffer
    Stack s;
    
    //disable line buffering -- read characters as they're pressed
    cbreak();
    noecho();
    keypad(stdscr, true);
    
    int c;
    
    //read chars into buffer until DELETE is pressed
    while(c != KEY_DC){
        
        refresh();
        c = getch();
    
        switch(c){

            /*FORMATTING*/

            /*BUFFER INPUT*/

            //handle in-place deletions
            case 263:
                s.deleteChar();
                break;

            //read everything else into buffer
            default:
                s.addChar(c);
                break;

        }

    }
    clear();
    refresh();
    printw("%s%d%s", "stack is empty: ", s.empty(), "\nstack contents:\n\n");
    s.printBufferContents();
    printw("%s", "\n\npress enter to exit...");
    getch();
    endwin();

    return 0;
}


void Stack::push(int c){
    //we need to resize here
    if (full()){
        resize();
        buffer[--top] = c;
        ++last_char_index;
    }
    else{
        buffer[--top] = c;
        ++last_char_index;
    }
}

void Stack::pop(){
    if (empty()){
        clear();
        mvprintw(0, 0, "%s", "error: pop(): stack is EMPTY - press DELETE");
        last_char_index = 0;
    }
    else{
        buffer[top++] = 0;
        --last_char_index;
    }
}

int Stack::peek(){
    return buffer[top];
}

int Stack::size(){
    return len;
}

void Stack::printBufferContents(){
    for (int i = len - 1; i > top; i--){
        if (char(buffer[i]) == '\n'){
            printw("%s", "<newline>");
        }
        else{
            printw("%c", buffer[i]);
        }
    }

}

bool Stack::empty(){
    return (top > len);
}

bool Stack::full(){
    return (top == 0);
}

void Stack::resize(){

    //compute new size
    int new_len = 2*len;

    //create place to store existing buffer
    int* new_buffer = new int[new_len];

    //initialize new buffer so we don't get corrupted values due to function scope
    for (int i = 0; i < new_len; i++){
        new_buffer[i] = 0;
    }

    //copy existing buffer over to local buffer
    copy(buffer, (buffer+len), (new_buffer+len));

    //handle memory allocations and update the address buffer points to
    delete [] buffer;
    buffer = new_buffer;

    //safeguard against using incorrect reference
    new_buffer = nullptr;

    //update our member vars
    top = new_len - len;
    len = new_len;
    

}

//these functions handle cursor placement
void Stack::gotoNewLine(){
    x = 0;
    ++y;
    line_count = y;
}

void Stack::gotoPreviousLine(){
    --line_count;
    --y;
    x = last_char_index;
    move(y, --x);
}

bool Stack::newLine(char c){
    return (c == '\n');
}

bool Stack::startOfLine(){
    return(x == 0);
}

bool Stack::endOfLine(){
    return (x == width-2);
}

void Stack::addChar(char c){

    //handle user newline creation (carriage return) - x coordinate resets or advances
    if (endOfLine()){
        gotoNewLine();
    }
    else if (newLine(c)){
        gotoNewLine();
        addch(c);
        push(c);
    
    }
    else{
        ++x;
        addch(c);
        push(c);
    }
    refresh();
}

void Stack::deleteChar(){


    //backtrack to line above and manage cursor for deletes
    if (startOfLine() && y != 0){
        //backtrack
        gotoPreviousLine();
        //pop newline character and delete it
        pop();
        delch();
        //pop whatever character is flush with the width and delete it
        pop();
        delch();
    }
    //delete in place on current line
    else{
        move(y, --x);
        delch();
        pop();
    }
    refresh();
}

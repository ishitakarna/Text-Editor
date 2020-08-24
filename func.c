#include "func.h"
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int maxRows, maxCols;
int cRow, cCol;
WINDOW* win;
int tabglobal = 4, indentglobal = 0;
int flag = 0, initr = 0, initc = 0, right = 0, left = 0, righta = 0;
char copy1[1500];

/*------------------------ FILE HANDLING -------------------------*/
int fileExists(char *file) {
	FILE *fp;
	fp = fopen(file, "r");
	if(fp == NULL)
		return -1;
	else {
		fclose(fp);
		return 0;
	}
}

FILE* openNewFile(buffer *buf, FILE *fp) {
	node *temp;
	char ch, fname[20];
	int c = 0, i = 0, exists = 0, mode;
	FILE *fp_new;
	
	callBox();
	
	//To write data from the data structure into old file and close file
	temp = buf->head;
	while(temp != NULL) {
		while(temp->p[i] != '\0') {
			ch = (char)temp->p[i];
			fwrite(&ch, 1, 1, fp);
			i++;
		}
		temp = temp->next;
		i = 0;
	}
	fclose(fp);
	destroyBuffer(buf); //To empty the buffer data structure

	wclear(win);
	callBox();
	mvwprintw(win, 1, 1, "Enter the name of the file to be Opened:\t");

	//To accept user input
	wrefresh(win);
	i = 0;
	wmove(win, 2, 1);
	wrefresh(win);
	fname[0] = '\0';
	while((ch = getch()) != '\n') {
		if(ch == 7) {
			i--;
			fname[i] = '\0';
			wclear(win);
			callBox();
			mvwprintw(win, 1, 1, "Enter the name of the file to be Opened:\t");
			wrefresh(win);
		}
		else {
			fname[i] = ch;
			fname[i + 1] = '\0';
			i++;
		}
		mvwprintw(win, 2, 1, "%s", fname);
		wrefresh(win);
	}	

	exists = fileExists(fname);
	if(exists == -1) { //File does not exist
		fp_new = fopen(fname, "w+");
		if(fp_new == NULL)
			return NULL;
		mode = 0;
		win = initNcurses(mode);
	}
	else { //File exists
		fp_new = fopen(fname, "r+");
		if(fp_new == NULL)
			return NULL;
		mode = 1;
		win = initNcurses(mode);
		while((ch = fgetc(fp_new)) != EOF) {
			c = (int)ch;
			insertBuffer(buf, c);
		}
		fclose(fp_new);
		//To erase the previous contents of the file in order to rewrite
		fp_new = fopen(fname, "w");
		move(0,0);
	}
	refresh();
	return fp_new;
}

int saveAs(buffer *buf) { 
	//Saves contents of the data structure
	char ch, fname[20];
	node *temp;
	int i = 0;
	FILE *fp;
	
	wclear(win);
	callBox();
	
	mvwprintw(win, 1, 1, "Enter the name of the file :\t");

	//To accept user input
	wrefresh(win);
	i = 0;
	wmove(win, 2, 1);
	wrefresh(win);
	fname[0] = '\0';
	while((ch = getch()) != '\n') {
		if(ch == 7) {
			i--;
			fname[i] = '\0';
			wclear(win);
			callBox();
			mvwprintw(win, 1, 1, "Enter the name of the file :\t");
			wrefresh(win);
		}
		else {
			fname[i] = ch;
			fname[i + 1] = '\0';
			i++;
		}
		mvwprintw(win, 2, 1, "%s", fname);
		wrefresh(win);
	}
	
	if(fname[0] == '\0')
		return -1;	
	
	fp = fopen(fname, "w+");
	if(fp == NULL) {
		mvwprintw(win, 1, 1, "Error : Data not saved\n");
		return -1;
	}
	i = 0;
	temp = buf->head;
	while(temp != NULL) {
		while(temp->p[i] != '\0') {
			ch = (char)temp->p[i];
			fputc(ch, fp);
			i++;
		}
		temp = temp->next;
		i = 0;
	}
	fclose(fp);
	wclear(win);
	callBox();
	mvwprintw(win, 1, 1, "File successfully saved");
	wrefresh(win);
	getch();

	return 0;
}

void readFileToBuffer(buffer *buf, FILE *fp) {
	node *temp, *temp2;
	int c, i = 0, fileflag = 0;
	temp = buf->head;
	while((c = getc(fp)) != EOF) {
		fileflag = 1; 
		if(buf->head == NULL) {
			temp2 = (node*)malloc(sizeof(node));
			buf->bufRows++;
			temp2->p = (int*)malloc(sizeof(int)*maxCols);
			temp = buf->head = buf->tail = buf->start = buf->end = temp2;
			temp->next = temp->prev = NULL;
		}
		if(c != '\0') {
			temp->p[i] = c;
			temp->p[i + 1] = '\0';
		}
		i++;
		if(c == '\n' || c == '\0' || i > (maxCols - 1)) {
			temp2 = (node*)malloc(sizeof(node));
			buf->bufRows++;
			temp2->p = (int*)malloc(sizeof(int)*maxCols);
			temp2->next = NULL;
			temp2->prev = temp;
			temp->next = temp2;
			buf->tail = temp2;
			
			if(buf->bufRows < maxRows)
				buf->end = buf->tail;

			temp = temp->next;
			i = 0;
		}
	}
	if(fileflag == 1) //To overcome condition when file exists but contains no character
		printBuffer(buf);
}
	
/*----------------------- INITIALIZE NCURSES -----------------------*/
WINDOW* initNcurses(int mode) {
	int heightWin, widthWin, startRowWin, startColWin;

	initscr();
	noecho();
	raw();
	keypad(stdscr, TRUE);
	keypad(win, TRUE);
	mousemask(BUTTON1_PRESSED | BUTTON_CTRL, NULL);
	clear();
	move(0,10);

	if(mode == 0)
		printw("Pluto version - 0.0.0 Press any character to begin editing!");
	else
		printw("Pluto version - 0.0.0 Press any character to continue editing!");
	
	addTilda();
	getch();
	clear();
	addTilda();
	
	//To create a new window
	getyx(stdscr, cRow, cCol);
	getmaxyx(stdscr, maxRows, maxCols);
	heightWin = 5;
	widthWin = maxCols;
	startColWin = 0;
	startRowWin = maxRows-5;
	win = newwin(heightWin, widthWin, startRowWin, startColWin);
	refresh();
	move(0,0);
	
	return win;
}

void addTilda() {
	int i;
	getyx(stdscr, cRow, cCol);
	i = cRow + 1;
	getmaxyx(stdscr, maxRows, maxCols);
	while(i < maxRows) {
		move(i, 0);
		wprintw(stdscr, "~");
		i++;
	}
	wmove(stdscr, 0, 0);
}

void callBox() {
	//To make the window in the bottom visible
	refresh();
	box(win, 0, 0);
	wrefresh(win);	
}

void highlight(buffer *buf, int startRow, int endRow, int startCol, int endCol) {
	int i = 0;
	node *temp;

	temp = buf->start;	
	while(i != startRow) {
		temp = temp->next;
		i++;
	}

	attron(A_BOLD|A_STANDOUT); //To switch on highlight attribute
	for(i = startCol; i <= endCol; i++) 
		mvprintw(startRow, i, "%c", temp->p[i]);
	attroff(A_BOLD|A_STANDOUT);
}

void resizeTerminal(buffer *buf) {
	node *temp;

	endwin();
	refresh();
	clear();
	getmaxyx(stdscr, maxRows, maxCols);
	
	temp = buf->head;
	buf->start = buf->head;
	while(temp != NULL) {
		temp->p = realloc(temp->p, sizeof(int)*maxCols);
		temp = temp->next;
	}
	printBuffer(buf);
}
	
/*---------------------- DATA STRUCTURE HANDLING ------------------------*/
void initBuffer(buffer *buf) {
	buf->head = buf->tail = buf->start = buf->end = NULL;
	buf->bufRows = -1;
}

void insertBuffer(buffer *buf, int c) {
	node *temp, *temp2, *temp3, *nextNode;
	int i, j, count = 0, row, col, ctemp;
	int cnt = 0, spacecnt = 0;

	righta = 0;

	//Condition for first character entered in editor
	if((buf->head == buf->tail) && (buf->head == NULL)) {
		temp = (node*)malloc(sizeof(node));
		buf->bufRows++;
		temp->next = temp->prev = NULL;
		buf->head = buf->tail = temp;
		buf->start = buf->head;
		buf->end = buf->tail;
		temp->p = (int*)malloc(sizeof(int)*maxCols);
		temp->p[0] = c;
		temp->p[1] = '\0';
		printBuffer(buf);
	}
	else {
		getyx(stdscr, cRow, cCol);
		row = cRow;
		col = cCol;
		temp = buf->start;
		i = 0;
		while(i != row) {
			//Condition when input reaches end of line, to create a new line
			if(temp->next == NULL) {
				temp2 = (node*)malloc(sizeof(node));
				buf->bufRows++;
				temp2->p = (int*)malloc(sizeof(int)*maxCols);
				temp2->p[0] = '\0';
				temp2->next = NULL;
				temp2->prev = temp;
				temp->next = temp2;
				buf->tail = temp2;
				
				if(row != maxRows)
					buf->end = buf->tail;
				temp = temp2;
			}
			else 
				temp = temp->next;
			i++;
		}
		//Assert : temp points to the line where the character has to be inserted
		//Condition for insertion taking place at the end of the line
		if(temp->p[col] == '\0') {
			//For automatic indentation
			if(c == '\n' && indentglobal == 1) {
				for(cnt = 0; temp->p[cnt] != '\0'; cnt++);
				if(cnt >= tabglobal) {
					for(cnt = 0; temp->p[cnt] != '\0'; cnt++) {
						if(temp->p[cnt] != 32)
							break;
						spacecnt++;
					}
					spacecnt = spacecnt / tabglobal;
					if(spacecnt > 0) {
						temp->p[col] = c;
						temp->p[col + 1] = '\0';
						move(row + 1, 0);
						for(cnt = 0; cnt < spacecnt; cnt++)
							insertTab(buf);
						return;
					}
				}
			}
						
						
			//Enter pressed at the end of the last line
			if((cRow + 1) == maxRows && c == '\n') {
				temp->p[col] = c;
				temp->p[col + 1] = '\0';
				
				//Not the last line in the data structure
				if(temp->next != NULL) {
					buf->start = buf->start->next;
					buf->end = buf->end->next;
					printBuffer(buf);
				}
				//Last line in the data structure
				else {
					temp2 = (node*)malloc(sizeof(node));
					buf->bufRows++;
					temp2->p = (int*)malloc(sizeof(int)*maxCols);
					temp2->p[0] = '\0';
					temp2->next = NULL;
					temp2->prev = temp;
					temp->next = temp2;
					buf->tail = buf->end = temp2;
					buf->start = buf->start->next;
					printBuffer(buf);
				}
				return;
			}
			if(col != (maxCols - 2)) {
				temp->p[col] = c;
				temp->p[col + 1] = '\0';
				printBuffer(buf);
			}
			//Overflow condition for the line
			else {
				temp->p[col] = '\n';
				temp->p[col + 1] = '\0';
				printBuffer(buf);
				move(row + 1, 0);
				insertBuffer(buf, c);
			}
		}
		else {
			//Condition when insertion takes place in the middle of a line
			//For enter
			if(c == 10) {
				temp3 = (node*)malloc(sizeof(node));
				buf->bufRows++;
				temp3->p = (int*)malloc(sizeof(int)*maxCols);
				//Copying chars from original line to new line after cursor position
				j = 0;
				for(i = col; temp->p[i] != '\0'; i++) {
					temp3->p[j] = temp->p[i];
					j++;
				}
				temp3->p[j] = '\0';
				temp->p[col] = c; //Inserting \n for printBuffer
				temp->p[col + 1] = '\0';

				nextNode = temp->next;
				
				//Condition for new line being the last line
				if(nextNode == NULL) {
					buf->tail = temp3;
					if(row != maxRows)
						buf->end = temp3;
					temp3->prev = temp;
					temp3->next = nextNode;
					temp->next = temp3;
					printBuffer(buf);
					move(row + 1, 0);
				}
				
				//Condition for new line being in between two lines
				else {
					temp3->prev = temp;
					temp3->next = nextNode;
					temp->next = temp3;
					nextNode->prev = temp3;
					printBuffer(buf);
					move(row + 1, 0);
				}				
			}
			
			//For any other character
			else {
				//To count the number of characters in that line
				for(count = 0; temp->p[count] != '\0'; count++);
			
				//No of characters in the line exceeds availabilty
				if(count == maxCols - 1) {
					ctemp = temp->p[count - 2]; //Last char of that line
					//Next line is present
					if(temp->next != NULL) {
						for(i = maxCols - 4; i >= col; i--)
							temp->p[i + 1] = temp->p[i];
						temp->p[col] = c;
						printBuffer(buf);
						move(row + 1, 0);
						insertBuffer(buf, ctemp);
						move(row, col + 1);
					}
					
					else {
						temp2 = (node*)malloc(sizeof(node));
						buf->bufRows++;
						temp2->p = (int*)malloc(sizeof(int)*maxCols);
						temp2->p[0] = '\0';
						temp2->next = NULL;
						temp2->prev = temp;
						temp->next = temp2;
						buf->tail = buf->end = temp2;
						for(i = maxCols - 4; i >= col ; i--)
							temp->p[i + 1] = temp->p[i];
						temp->p[col] = c;
						printBuffer(buf);
						move(row + 1, 0);
						insertBuffer(buf, ctemp);
						move(row, col + 1);
					}
				}
				else {
					//To shift the characters to the right
					for(i = count - 1; i >= col; i--) {
						temp->p[i + 1] = temp->p[i];
					}
					//To insert at the specific position
					temp->p[col] = c;
					printBuffer(buf);
					move(row, col + 1); //To move the cursor back to the insertion position in the middle
				}
			}
		}
	} 
}

void tabPrompt() {
	char ch;
	int c;	

	wclear(win);
	callBox();
	mvwprintw(win, 1, 1, "Tab width (2/4/8) : ");
	wrefresh(win);
	wmove(win, 2, 1);
	wrefresh(win);
	//Accepting user input
	while((ch = getch()) != '\n') {
		c = (int)ch;
		if(ch == 7) {
			wclear(win);
			callBox();
			mvwprintw(win, 1, 1, "Tab width (2/4/8) : ");
			wmove(win, 2, 1);
		}
		else
			mvwprintw(win, 2, 1, "%c", c);
		wrefresh(win);
	}
	c = c - 48;
	if(c != 2 && c != 4 && c != 8)
		tabglobal = 4; //Default tab value
	else
		tabglobal = c;

	wclear(win);
	callBox();
	mvwprintw(win, 1, 1, "Automatic indentation (y/n) : ");
	wrefresh(win);
	wmove(win, 2, 1);
	wrefresh(win);
	//Accepting user input
	while((ch = getch()) != '\n') {
		c = (int)ch;
		if(ch == 7) {
			wclear(win);
			callBox();
			mvwprintw(win, 1, 1, "Automatic indentation (y/n) : ");
			wmove(win, 2, 1);
		}
		else
			mvwprintw(win, 2, 1, "%c", c);
		wrefresh(win);
	}
	if(c == 'y') 
		indentglobal = 1;
	else
		indentglobal = 0;
}

void insertTab(buffer *buf) {
	int i;
	for(i = 0; i < tabglobal; i++)
		insertBuffer(buf, 32);
}

void destroyBuffer(buffer *buf) {
	node *temp, *temp2;
	temp = buf->head;
	while(temp != buf->tail) {
		temp2 = temp;
		temp = temp->next;
		free(temp2);
	}
	buf->head = buf->tail = buf->start = buf->end = NULL;
	buf->bufRows = -1;
}

void backBuffer(buffer *buf) {
	node *temp, *temp2;
	int i = 0, count, row, col, midflag = 0;
	getyx(stdscr, cRow, cCol);
	row = cRow;
	col = cCol;
	
	if(righta == 1) {
		destroyBuffer(buf);
		printBuffer(buf);
		righta = 0;
		return;
	}

	temp = buf->start;
	//Condition for backspace pressed as the first character
	if((buf->head == buf->tail) && buf->head == NULL)
		return;	
	while(i != row) {
			//Condition when input reaches end of line, to create a new line
			if(temp->next == NULL) {
				temp2 = (node*)malloc(sizeof(node));
				buf->bufRows++;
				temp2->p = (int*)malloc(sizeof(int)*maxCols);
				temp2->next = NULL;
				temp2->prev = temp;
				temp->next = temp2;
				buf->tail = buf->end = temp2;
				temp = temp2;
			}
			else 
				temp = temp->next;
			i++;
	}

	//For scrolling
	if(row == 0 && col == 0 && temp->prev != NULL) {
		buf->start = buf->start->prev;
		buf->end = buf->end->prev;
		printBuffer(buf);
		return;
	}
		
	//Condition for backspace at the starting of a line
	if((col == 0) && (row != 0)) {
		temp = temp->prev;
		for(count = 0; temp->p[count] != '\0'; count++); //To count the number of characters present in the previous line
		
		//To delete the node if it is empty
		temp2 = temp->next;
		if(temp2->next != NULL) { //Condition for deletion of middle node 
			temp->next = temp2->next;
			temp2->next->prev = temp;
			midflag = 1;
		}
		else { //Condition for deletion of last node
			temp->next = NULL;
			buf->tail = buf->end = temp;
		}
		
		//To copy one line to the end of the previous line
		while(temp2->p[col] != '\0') {
			temp->p[count - 1] = temp2->p[col];
			col++;
			count++;
		}
		temp->p[count-1] = '\0';
		free(temp2);
		buf->bufRows--;
		printBuffer(buf);
		if(midflag == 1)
			move(row - 1, count - 2);
		else
			move(row - 1, count - 1);
	}
	
	else {
		for(i = col-1; temp->p[i] != '\0'; i++) {
			temp->p[i] = temp->p[i+1];
		}
		printBuffer(buf);
		move(row, col - 1);
	}
}

void deleteBuffer(buffer *buf) {
	node *temp, *temp2;
	int i = 0, count, row, col;
	getyx(stdscr, cRow, cCol);
	row = cRow;
	col = cCol;
	
	if(righta == 1) {
		destroyBuffer(buf);
		printBuffer(buf);
		righta = 0;
		return;
	}
	
	temp = buf->start;
	while(i != cRow) {
		temp = temp->next;
		i++;
	}
	temp2 = temp;
	
	//Condition for delete pressed at the end of a line	
	if((temp->p[col] == '\0') || (temp->p[col] == '\n')){
		if(temp->next != NULL) {
			count = col;
			temp = temp->next;		
			for(i = 0; temp->p[i] != '\0'; i++) {
				temp2->p[count] = temp->p[i];
				count++;
			}
			temp2->p[count] = '\0';

			if(temp->next != NULL)
				temp2->next = temp->next;
			else {
				temp2->next = NULL;
				buf->tail = buf->end = temp2;
			}
			free(temp);
			buf->bufRows--;
		
			printBuffer(buf);
			move(row, col);
		}
	}
	//Condition for delete pressed anywhere else
	else {
		for(i = cCol; temp2->p[i] != '\0'; i++) {
			temp2->p[i] = temp2->p[i+1];	
		}
		move(row, col);
	}
	printBuffer(buf);
	move(row, col);
}

void printBuffer(buffer *buf) {
	node *temp;
	int i = 0;
	clear();
	addTilda();
	
	if(buf->head == NULL && buf->tail == NULL)
		return;
	temp = buf->start;
	
	while(temp != buf->end) {
		//Printing a line
		while(temp->p[i] != '\0') {
			printw("%c", temp->p[i]);
			i++;
		}
		temp = temp->next;
		i = 0;
	}
	//Printing the last line
	for(i = 0; temp->p[i] != '\0'; i++)
		printw("%c", temp->p[i]);
}

/*--------------------------- CURSOR MOVEMENTS -----------------------------*/
void arrowLeft(buffer *buf) {
	node *temp;
	int i = 0;
	getyx(stdscr, cRow, cCol);
	
	temp = buf->start;	
	while(i != cRow) {
		temp = temp->next;
		i++;
	}

	//To avoid arrow key movements when no character is entered
	if((cCol == 0) && (cRow == 0) && (buf->head == NULL) && (buf->tail == NULL)) {
		move(cRow, cCol);
		return;
	}

	if(temp == NULL && cRow != 0) {
		move(cRow - 1, cCol);
		return;
	}
			
	
	//Conditon for key pressed in empty editor
	if((temp->prev == NULL) && (cCol==0))
		move(cRow, cCol);
	else {
		//Conditon for key pressed at the starting of a line
		if(cCol == 0) {
			if(temp != buf->start) { //not on first line
				temp = temp->prev;
				for(i = 0; temp->p[i] != '\0'; i++);
				move(cRow - 1, i - 1);
			}
			else if(buf->start->prev != NULL) { //for scrolling
				buf->start = buf->start->prev;
				buf->end = buf->end->prev;
				temp = temp->prev;
				for(i = 0; temp->p[i] != '\0'; i++);
				printBuffer(buf);
				move(0, i - 1);
			}
		}
		else
			move(cRow, cCol - 1);
	}
}

void arrowRight(buffer *buf) {
	node *temp;
	int i = 0;
	getyx(stdscr, cRow, cCol);

	temp = buf->start;	
	while(i != cRow) {
		temp = temp->next;
		i++;
	}
	if((cCol == 0) && (cRow == 0) && (buf->head == NULL) && (buf->tail == NULL)) {
		move(cRow, cCol);
		return;
	}
	if(temp == NULL && cRow != 0) {
		move(cRow - 1, cCol);
		return;
	}
	//Condition for last character of last line
	if((temp->next == NULL) && (temp->p[cCol] == '\0'))
		move(cRow, cCol);
	//Condition for last character of any other line
	else if(temp->p[cCol] == '\0') {
		if(temp != buf->end) //not last line
			move(cRow + 1, 0);
		else if(buf->end->next != NULL) { //for scrolling
			buf->end = buf->end->next;
			buf->start = buf->start->next;
			printBuffer(buf);
			move(maxRows, 0);
		}
	}
	else
		move(cRow, cCol + 1);
}

void arrowUp(buffer *buf) {
	node *temp;
	int i = 0;
	getyx(stdscr, cRow, cCol);
	temp = buf->start;
	
	while(i != cRow) {
		temp = temp->next;
		i++;
	}
	if(temp == NULL && cRow != 0) {
		move(cRow - 1, cCol);
		return;
	}
	
	//Condition for cursor not on the first line
	if(cRow != 0) {
		//Condition for end of a line
		if((temp->p[cCol] == '\0') || (temp->p[cCol] == '\n')) {
			temp = temp->prev;
			for(i = 0; temp->p[i] != '\0'; i++);
			move(cRow - 1, i - 1);
		}
		else
			move(cRow - 1, cCol);
	}
	else { //Cursor on first line
		if(temp == buf->head) 
			move(cRow, cCol);
		else if(buf->start->prev != NULL) {
			buf->start = buf->start->prev;
			buf->end = buf->end->prev;
			temp = temp->prev;
			for(i = 0; temp->p[i] != '\0'; i++);
			printBuffer(buf);
			move(0, i - 1);
		}
	}
}

void arrowDown(buffer *buf) {
	node *temp;
	int i = 0;
	getyx(stdscr, cRow, cCol);

	temp = buf->start;
	while(i != cRow) {
		temp = temp->next;
		i++;
	}
	if((cCol == 0) && (cRow == 0) && (buf->head == NULL) && (buf->tail == NULL)) {
		move(cRow, cCol);
		return;
	}
	if(temp == NULL && cRow != 0) {
		move(cRow, cCol);
		return;
	}
	if(temp->next != NULL) {
		if(temp != buf->end) { //Not the last line on the screen
			//Condition for end of a line
			if((temp->p[cCol] == '\0') || (temp->p[cCol] == '\n')) {
				temp = temp->next;
				for(i = 0; temp->p[i] != '\0'; i++);
				if(temp->p[i - 1] == '\n')
					move(cRow + 1, i - 1);
				else
					move(cRow + 1, i);
			}
			else
				move(cRow + 1, cCol);
		}
		else { //For scrolling
			buf->end = buf->end->next;
			buf->start = buf->start->next;
			temp = temp->next;
			for(i = 0; temp->p[i] != '\0'; i++);
			printBuffer(buf);
			move(maxRows, i);
		}
	}
	//Condtion for last line of the data structure
	else
		move(cRow, cCol);
}
void mouseLeftKey(buffer *buf) {
	MEVENT event;
	node *temp;
	int i = 0;
	temp = buf->start;
	
	if(temp == NULL) {
		move(cRow, cCol);
		return;
	}
			
 	if (getmouse(&event) == OK) {
		if(event.bstate & BUTTON1_PRESSED) {
			while((i != event.y) && (temp->next != NULL)) {
				temp = temp->next;
				i++;
			}
			i = 0;
			for(i = 0; temp->p[i] != '\0'; i++);//To count the number of columns
			if(event.x <= i) {
				move(event.y, event.x);
			}
			else
				move(event.y, i - 1);
		}
	}
}	

/*--------------------------------- FEATURES ------------------------------------*/
void search(buffer *buf) {
	int tRow, tCol;
	int c, i = 0, rowCount = 0, startCol = 0, j = 0, diff = 0, countReplace = 0;
	char *foundWord = NULL;
	char ch, chtemp[300], chtemp2[300], chtemp3[300], word[50];
	node *temp;

	//Refreshing window
	getyx(stdscr, cRow, cCol);
	tRow = cRow;
	tCol = cCol;
	printBuffer(buf);
	move(tRow, tCol);
	
	temp = buf->head;
	
	wclear(win);
	callBox();
	getyx(stdscr, cRow, cCol);
	getmaxyx(stdscr, maxRows, maxCols);

	//Handling user input
	mvwprintw(win, 1, 1, "1. Search only");
	mvwprintw(win, 2, 1, "2. Search and Replace\n");
	wrefresh(win);
	i = 0;
	wmove(win, 3, 1);
	wrefresh(win);

	//Accepting user input
	while((ch = getch()) != '\n') {
		c = (int)ch;
		if(ch == 7) {
			wclear(win);
			callBox();
			mvwprintw(win, 1, 1, "1. Search only");
			mvwprintw(win, 2, 1, "2. Search and Replace\n");
			wmove(win, 3, 1);
		}
		else
			mvwprintw(win, 3, 1, "%c", c);
		wrefresh(win);
	}
	wclear(win);
	callBox();
	
	c = c - 48;
	switch(c) {
		case 1 :
			getyx(stdscr, cRow, cCol);
			tRow = cRow;
			tCol = tCol;
			
			mvwprintw(win, 1, 1, "Enter word to be searched:");
			wrefresh(win);
			i = 0;
			wmove(win, 2, 1);
			wrefresh(win);
			
			//Accepting user input : chtemp2 contains substring to be searched
			chtemp2[0] = '\0';
			while((ch = getch()) != '\n') {
				if(ch == 7) {
					i--;
					chtemp2[i] = '\0';
					wclear(win);
					callBox();
					mvwprintw(win, 1, 1, "Enter word to be searched:");
					wrefresh(win);
				}
				else {
					chtemp2[i] = ch;
					chtemp2[i + 1] = '\0';
					i++;
				}
				mvwprintw(win, 2, 1, "%s", chtemp2);
				wrefresh(win);
			}	
			
			//To dissapear the box window
			insertBuffer(buf, 'f');
			backBuffer(buf);

			while(temp != NULL) {
				//Store every line of the editor in chtemp
				for(i = 0; temp->p[i] != '\0'; i++) {
					chtemp[i] = (char)temp->p[i]; 
				}
				if(chtemp[i - 1] == '\n')
				i = i - 1;
				chtemp[i] = ' ';
				chtemp[i + 1] = '\0';

				//To separate each word of the line and compare it with the substring to be searched
				foundWord = strtok(chtemp, " ");
				startCol = foundWord - chtemp;
				while(foundWord != NULL) {
					strcpy(word, foundWord);
					//If word found, highlight it
					if(strcmp(word, chtemp2) == 0) {
						move(rowCount, startCol);
						highlight(buf, rowCount, rowCount, startCol, startCol + strlen(word) - 1);
					}
					foundWord = strtok(NULL, " ");
					startCol = foundWord - chtemp;
				}
				temp = temp->next;
				rowCount++;
			}
			move(0, 0);
			
			break;
			
		case 2 :
			wclear(win);
			callBox();
			getyx(stdscr, cRow, cCol);
			tRow = cRow;
			tCol = tCol;

			mvwprintw(win, 1, 1, "Enter word to be searched:");
			wrefresh(win);
			i = 0;
			wmove(win, 2, 1);
			wrefresh(win);

			//Accepting string to be searched in chtemp2
			chtemp2[0] = '\0';
			while((ch = getch()) != '\n') {
				if(ch == 7) {
					i--;
					chtemp2[i] = '\0';
					wclear(win);
					callBox();
					mvwprintw(win, 1, 1, "Enter word to be searched:");
					wrefresh(win);
				}
				else {
					chtemp2[i] = ch;
					chtemp2[i + 1] = '\0';
					i++;
				}
				mvwprintw(win, 2, 1, "%s", chtemp2);
				wrefresh(win);
			}

			wclear(win);
			callBox();
			mvwprintw(win, 1, 1, "Enter replacing word:");
			wrefresh(win);
			i = 0;
			wmove(win, 2, 1);
			wrefresh(win);

			//Accepting string to be replaced with in chtemp3			
			chtemp3[0] = '\0';
			while((ch = getch()) != '\n') {
				if(ch == 7) {
					i--;
					chtemp3[i] = '\0';
					wclear(win);
					callBox();
					mvwprintw(win, 1, 1, "Enter replacing word:");
					wrefresh(win);
				}
				else {
					chtemp3[i] = ch;
					chtemp3[i + 1] = '\0';
					i++;
				}
				mvwprintw(win, 2, 1, "%s", chtemp3);
				wrefresh(win);
			}	

			//Difference in length between the original word and the new word
			diff = strlen(chtemp3) - strlen(chtemp2);

			while(temp != NULL) {
				//Search for every word
				for(i = 0; temp->p[i] != '\0'; i++) {
					chtemp[i] = (char)temp->p[i];
				}
				if(chtemp[i - 1] == '\n')
				i = i - 1;
				chtemp[i] = ' ';
				chtemp[i + 1] = '\0';

				foundWord = strtok(chtemp, " ");
				startCol = foundWord - chtemp;
				while(foundWord != NULL) {
					strcpy(word, foundWord);
					if(strcmp(word, chtemp2) == 0) {
						move(rowCount, startCol);
						countReplace++; //To count the number of words replaced
						//To delete the original word
						for(i = startCol; i <= startCol + strlen(word) - 1; i++) {
							deleteBuffer(buf);
						}
						
						j = 0;
						//To insert the new word
						for(i = startCol; i <= startCol + strlen(chtemp3) - 1; i++) {
							insertBuffer(buf, chtemp3[j]);
							j++;
						}	
					}
					foundWord = strtok(NULL, " ");
					startCol = (foundWord - chtemp) + (diff * countReplace);
				}
				temp = temp->next;
				rowCount++; 
				startCol = 0; countReplace = 0; foundWord = NULL;
			}
			printBuffer(buf);
			
			i = 0; rowCount = 0; startCol = 0; j = 0;
			foundWord = NULL;
			
			temp = buf->head;
			
			//To search the replaced words and highlight them
			while(temp != NULL) {
				for(i = 0; temp->p[i] != '\0'; i++) {
					chtemp[i] = (char)temp->p[i];
				}
				if(chtemp[i - 1] == '\n')
				i = i - 1;
				
				chtemp[i] = ' ';
				chtemp[i + 1] = '\0';
				foundWord = strtok(chtemp, " ");
				startCol = foundWord - chtemp;
				while(foundWord != NULL) {
					strcpy(word, foundWord);
					if(strcmp(word, chtemp3) == 0) {
						move(rowCount, startCol);
						highlight(buf, rowCount, rowCount, startCol, startCol + strlen(word) - 1);
					}
					foundWord = strtok(NULL, " ");
					startCol = foundWord - chtemp;
				}
				temp = temp->next;
				rowCount++;
			}
			move(0, 0);
			
			break;
			
			
		default :
			wclear(win);
			callBox(); 
			mvwprintw(win, 1, 1, "Invalid input");
			wrefresh(win);
			break;
	}
	getmaxyx(stdscr, maxRows, maxCols);
	move(tRow, tCol);	
}

void copy(buffer *buf) {
	int i = 0, j = 0, x, y, z1 = initr, z2 = initc;
	node *temp;
	
	getyx(stdscr, cRow, cCol);
	x = cRow;
	y = cCol;
	temp = buf->start;
	
	if(left == 1) {
		while(i != cRow){
			temp = temp->next;	
			i++;
		}
		while(x != z1) {
			while(temp->p[y] != '\0') {
				copy1[j] = temp->p[y];
				y++;
				j++;
			}
			y = 0;
			x++;
			temp = temp->next;
		}
		while(y != z2) {
			copy1[j] = temp->p[y];
			y++;
			j++;
		}
		copy1[j] = '\0';
	}
	else if(right == 1 || righta == 1) {
		while(i != z1) {
			temp = temp->next;
			i++;
		}
		while(z1 != cRow) {	
			while(temp->p[z2] != '\0') {
				copy1[j] = temp->p[z2];
				z2++;
				j++;
			}
			z2 = 0;
			z1++;
			temp = temp->next;
		}
		while(cCol != z2) {
			copy1[j] = temp->p[z2];
			z2++;
			j++;
		}
		copy1[j] = '\0';
	}
	flag = 0;
}

void cut(buffer *buf) {
	int x, y, i = initr, j = initc, count = 0;
	node *temp;
	getyx(stdscr,cRow,cCol);
	
	//Copying selected elements		
	copy(buf);
	
	x = cRow;
	y = cCol;
		
	temp = buf->start;
	
	//For right shift
	if(right == 1 || righta == 1)
	{
		righta = 0;
		while(i != x) {
			while(temp->p[j] != '\0') {
				j++;
				count++;
			}
			temp = temp->next;
			i++;
			j=0;
		}


		while(j != y) {
			count++;
			j++;
		}

		move(initr,initc);
	}

	//For left shift
	else if(left == 1) {
		while(x != i) {
			while(temp->p[y]!='\0') {
				y++;
				count++;
			}
			temp = temp->next;
			x++;
			y = 0;
		}
		while(y != j) {
			count++;
			y++;

		}
		move(cRow,cCol);
	}
	
	//Deleting selected elements
	for(i = 0; i < count; i++) {
		deleteBuffer(buf);
	}
}

void paste(buffer *buf) {
	int j = 0;

	left = 0;
	right = 0;
		
	//Calling insertBuffer to paste
	while(copy1[j] != '\0') {
		insertBuffer(buf, copy1[j]);
		j++;
	}
}

void selectLeft(buffer *buf){
	int x, y, i = 0, count = 0;
	getyx(stdscr, cRow, cCol);
	node *temp;
	
	temp = buf->start;
	while(i != cRow) {
		temp = temp->next;
		i++;
	}
	
	x = cRow;
	y = cCol;

	//To get the starting position from where shift was pressed and record it
	if(flag == 0) {
		initr = cRow;
		initc = cCol;
		flag = 1;
	}

	//To handle cursor movements
	if(cCol == 0 && cRow == 0) {
		highlight(buf, cRow, cRow, cCol, cCol);
		move(0, 0);
	}
	else if(cCol == 0) {
		highlight(buf, cRow, cRow, cCol, cCol);
		temp = temp->prev;
		for(count = 0; temp->p[count] != '\0'; count++);
		move(x - 1, count);
	}
	else {
		highlight(buf, cRow, cRow, cCol, cCol);
		move(x, y - 1);
	}
	left = 1;
}

void selectRight(buffer *buf){
	int x2, y2, i = 0;
	getyx(stdscr,cRow,cCol);
	node *temp;
	
	temp = buf->start;
	
	while(i != cRow) {
		temp = temp->next;
		i++;
	}

	x2 = cRow;
	y2 = cCol;

	//To get the starting position from which shift was pressed	
	if(flag == 0) {
		initr = cRow;
		initc = cCol;
		flag = 1;
	}

	//To handle cursor movements
	if(temp == NULL) {
		move(cRow, cCol);
		return;
	}
	if(temp->p[cCol] == '\0') {
		move(cRow + 1, 0);
	}
	else {
		highlight(buf, cRow, cRow, cCol, cCol);
		move(x2, y2 + 1);
	}
	right = 1;
}

/*------------------------ EXTRA FEATURES ------------------------*/
void deleteRow(buffer *buf) {
	int i = 0, tRow;
	node *temp;

	temp = buf->start;
	getyx(stdscr, cRow, cCol);
	tRow = cRow;
	while(i != cRow) {
		temp = temp->next;
		i++;
	}
	if(temp == NULL)
		return;
	i = 0;
	while(temp->p[i] != '\0') {
		i++;
	}
	move(tRow, i);

	//Calling backBuffer to backspace the characters in that row
	for(; i > 0; i--) {
		backBuffer(buf);	
	}
	move(tRow, 0);
	deleteBuffer(buf);
	move(tRow - 1, 0);
}

void copyRow(buffer *buf) {
	int i = 0, tempi, j = 0;
	node *temp;
	temp = buf->start;
	getyx(stdscr, cRow, cCol);

	//To traverse till the row
	while(i != cRow) {
		temp = temp->next;
		i++;
	}
	if(temp == NULL)
		return;
	i = 0;

	//To count the number of characters in the row
	while(temp->p[i] != '\0') {
		i++;
	}
	tempi = i;
	tempi = tempi + 1; 

	//To store the characters in the copy array
	while(tempi != 0) {
		copy1[j] = (char)temp->p[j];
		tempi--;
		j++;
	}
	
}
void selectAll(buffer *buf) {
	int i = 0, j = 0;
	node *temp;
	
	initr = 0;
	initc = 0;
	
	temp = buf->start;

	//To traverse the entire data structure and highlight it 
	while(temp != NULL) {
		while(temp->p[j] != '\0') {
			j++;
		}
		highlight(buf, i, i, 0, j);
		i++;
		j = 0;
		temp = temp->next;
	}
	righta = 1;
}

void ctrlRight(buffer *buf) {
	int i = 0, tRow;
	node *temp;

	temp = buf->start;
	getyx(stdscr, cRow, cCol);
	tRow = cRow;

	//To traverse till the row
	while(i != cRow) {
		temp = temp->next;
		i++;
	}
	i = cCol;
	
	if(temp == NULL)
		return;
	
	//To make the cursor point to the nearest word
	while(temp->p[i] == ' ')
		i = i + 1;
	
	//To search for the nearest word
	while(temp->p[i] != ' ') {
		if(temp->p[i] == '\0' || temp->p[i] == '\n') {
			break;
		}
		i++;
	}
	
	//To make the cursor point to the next word
	move(tRow, i);
}
void ctrlLeft(buffer *buf) {
	int i = 0, tRow;
	node *temp;

	temp = buf->start;
	getyx(stdscr, cRow, cCol);
	tRow = cRow;
	while(i != cRow) {
		temp = temp->next;
		i++;
	}
	i = cCol;
	
	if(temp == NULL)
		return;
	
	while(temp->p[i] == ' ' && i != 0)
		i = i - 1;
		
	while(temp->p[i] != ' ') {
		if(i == 0)
			break;
		i--;
	}
	
	move(tRow, i);
}

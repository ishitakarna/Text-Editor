#include "func.h"
#include <ncurses.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define CTRL_KEYPRESS(k) ((k)  & 0x1f)

int cRow, cCol;
int maxRows, maxCols;

int main(int argc, char *argv[]) {
	int c, i = 0, e = 0, mode, err, fileflag = 0, firstTab = 0;
	char ch;
	node *temp;
	buffer text;
	FILE *fp, *fp_new;

	//User must enter name of file along with ./project
	if(argc != 2) {
		errno = EINVAL;
		perror("Usage - <./project> <file_name> ");
		return errno;
	}

	e = fileExists(argv[1]); //To check if the file already exists
	
	//Condition when the file does not exist
	if(e == -1) { 
		fp = fopen(argv[1], "w+");
		if(fp == NULL) {
			perror("Could not open file ");
			return errno;
		}
		mode = 0;
		initNcurses(mode);
		initBuffer(&text);
	}
	//Conditon when the file exists
	else {
		fp = fopen(argv[1], "r+");
		if(fp == NULL) {
			perror("Could not open file ");
			return errno;
		}
		mode = 1;
		initNcurses(mode);
		initBuffer(&text);
		readFileToBuffer(&text, fp);
		fclose(fp);
		//To erase the previous contents of the file in order to rewrite
		fp = fopen(argv[1], "w");
		move(0,0);
	}
		
	//Handling user input 
	while((c = getch()) != CTRL_KEYPRESS('q')) {
		if((c > 31 && c < 127) || (c == 10)) {
			insertBuffer(&text, c);
		}
		else if(c == 9) {
			if(firstTab == 0) {
				tabPrompt();
				firstTab = 1;
			}
			insertTab(&text);
		}
		else if(c == KEY_BACKSPACE) {
			backBuffer(&text);
		}
		else if(c == KEY_DC) {
			deleteBuffer(&text);
		}
		else if(c == KEY_RIGHT) {
			arrowRight(&text);
		}
		else if(c == KEY_LEFT) {
			arrowLeft(&text);
		}
		else if(c == KEY_UP) {
			arrowUp(&text);
		}
		else if(c == KEY_DOWN) {
			arrowDown(&text);
		}
		else if(c == KEY_MOUSE) {
			mouseLeftKey(&text);
		}
		else if(c == KEY_SRIGHT) {
			selectRight(&text);
		}
		else if(c == KEY_SLEFT) {
			selectLeft(&text);
		}
		else if(c == KEY_RESIZE) {
			resizeTerminal(&text);
		}
		else if (c == CTRL_KEYPRESS('f')) {
      			search(&text); //Ctrl F to search text
      		}
      		else if (c == CTRL_KEYPRESS('c')) {
      			copy(&text); //Ctrl C to copy text
      		}
      		else if (c == CTRL_KEYPRESS('x')) {
      			cut(&text); //Ctrl X to cut text
      		}
      		else if (c == CTRL_KEYPRESS('v')) {
      			paste(&text); //Ctrl V to paste text
      		}
		else if (c == CTRL_KEYPRESS('a')) {
      			selectAll(&text); //Ctrl A to select all
      		}
		else if (c == CTRL_KEYPRESS('d')) {
      			deleteRow(&text); //Ctrl D to delete row
      		}
		else if (c == CTRL_KEYPRESS('y')) {
      			copyRow(&text); //Ctrl Y to copy row
      		}
		else if (c == CTRL_KEYPRESS('r')) {
			ctrlRight(&text); //Ctrl R to point to the next word
		}
		else if (c == CTRL_KEYPRESS('l')) {
			ctrlLeft(&text); //Ctrl L to point to the previous word
		}
     		else if (c == CTRL_KEYPRESS('s')) {
      			err = saveAs(&text); //Ctrl S to Save As
      			if(err == -1) {
				destroyBuffer(&text);
				clear();
				endwin();	
      				return 0;
			}	
			else
				break; //Stop accepting input after saving and quit
		}
		else if (c == CTRL_KEYPRESS('o')) {
      			fp_new = openNewFile(&text, fp); //Ctrl O to Open new
      			if(fp_new == NULL) {
				perror("Could not open file ");
      				endwin();
				return 0;
			}
			fileflag = 1; //Original file closed, write data in new file on quiting
      		}
	}
	
	//Writing the data structure into the file on quiting - Ctrl Q
	temp = text.head;
	while(temp != NULL) {
		while(temp->p[i] != '\0') {
			ch = (char)temp->p[i];
			if(fileflag == 0)
				fwrite(&ch, 1, 1, fp);
			else
				fwrite(&ch, 1, 1, fp_new);
			i++;
		}
		temp = temp->next;
		i = 0;
	}
	fclose(fp);

	destroyBuffer(&text);
	clear();
	endwin();
}

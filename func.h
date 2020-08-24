#include <ncurses.h>
typedef struct node {
	int *p;
	struct node *next, *prev;
}node;
typedef struct buffer {
	node *head, *tail, *start, *end;
	int bufRows;
}buffer;

//File Handling
int fileExists(char *file);
FILE* openNewFile(buffer *buf, FILE *fp);
int saveAs(buffer *buf);
void readFileToBuffer(buffer *buf, FILE *fp);

//Initialise NCurses
WINDOW* initNcurses(int mode);
void addTilda();
void callBox();
void highlight(buffer *buf, int startRow, int endRow, int startCol, int endCol);
void resizeTerminal(buffer *buf);

//Data structure handling
void initBuffer(buffer *buf);
void insertBuffer(buffer *buf, int c);
void tabPrompt();
void insertTab(buffer *buf);
void destroyBuffer(buffer *buf);
void backBuffer(buffer *buf);
void deleteBuffer(buffer *buf);
void printBuffer(buffer *buf);

//Cursor movements
void arrowLeft(buffer *buf);
void arrowRight(buffer *buf);
void arrowUp(buffer *buf);
void arrowDown(buffer *buf);
void mouseLeftKey(buffer *buf);

//Features
void search(buffer *buf);
void copy(buffer *buf);
void cut(buffer *buf);
void paste(buffer *buf);
void selectLeft(buffer *buf);
void selectRight(buffer *buf);

//Extra features
void deleteRow(buffer *buf);
void copyRow(buffer *buf);
void selectAll(buffer *buf);
void ctrlRight(buffer *buf);
void ctrlLeft(buffer *buf);





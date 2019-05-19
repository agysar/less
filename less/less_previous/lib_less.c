#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


#define start() printf("\E[H")
#define clearscr() printf("\E[2J")
int x = 1;
int y = 2;
int px = 1;
int py = 1;
int opt = 0;
int top = 1;
int bot = 0;
int left = 1;
int right = 0;
int length = 0;
int tmp = 0;
int p_x = 0;
int p_y = 0;
int prev_pos_x = 1;
int prev_pos_y = 0;
char *str_srch = NULL;


struct global_args_t
{
    char *filename; /* print file with numbers before strings*/
    int n;
    int v; /* print version of this program */
    int h; /* print help of this program */
} global_args_;


struct list_node
{
    char *str;
    struct list_node *prev;
    struct list_node *next;
};

typedef struct
{
    size_t num_elements;
    struct list_node *head;
    struct list_node *tail;
} Bidirect_list;

Bidirect_list *getText(char *path)
{
    Bidirect_list *tmp_list;
    struct list_node *tmp_node, *tmp_head, *tmp_node_prev;
    FILE *f;
    char c;
    char *str_temp;
    char *check_ptr = NULL;
    int str_len = 1;
    int cnt_of_nodes = 0;    


    /* making tmp_list */
    tmp_list = (Bidirect_list*)malloc(sizeof(Bidirect_list));
    
    if (tmp_list == NULL)
    {
        printf("\nCan not make tmp_list\n");
        return NULL;
    }

    /* making str_tmp */
    str_temp = (char*)malloc(sizeof(char));
    
    if (str_temp == NULL)
    {
        printf("\nCan not make str_temp\n");
        return NULL;
    }

    /* making tmp_head */
    tmp_head = (struct list_node*)malloc(sizeof(struct list_node));
    
    if (tmp_head == NULL)
    {
        printf("\nCan not make tmp_head\n");
        return NULL;
    }

    /* making tmp_node_prev */
    /*tmp_node_prev = (struct list_node*)malloc(sizeof(struct list_node));
    
    if (tmp_node_prev == NULL)
    {
        printf("\nCan not make tmp_node_prev\n");
        return NULL;
    }*/
    
    
    /* opening */
    f = fopen(path, "r");
    if (f == NULL)
    {
        printf("\nCan not open file %s\n", path);
        return NULL;
    }
    
    /* making head */
    while(((c = fgetc(f)) != '\n') && (c != EOF)) 
    {
        str_temp[str_len - 1] = c;
        ++str_len;
        check_ptr = (char*)realloc(str_temp, str_len);
        if (check_ptr == NULL)
        {
            printf("\nCan not reallocate memory for str_temp\n");
            return NULL;
        }
        str_temp = check_ptr;
    }
        
    str_temp[str_len - 1] = '\0';
    /*str_temp[str_len] = '\0';*/

    /* giving fields for head */
    tmp_head->str = str_temp;
    tmp_head->prev = NULL;
    tmp_head->next = NULL;

    /* saving head to tmp_node_prev */
    tmp_node_prev = tmp_head;
    ++cnt_of_nodes;


    /* reading */
    while (c != EOF)
    {
        str_temp = (char*)malloc(sizeof(char));
    
        if (str_temp == NULL)
        {
            printf("\nCan not make str_temp\n");
            return NULL;
        }
    
        tmp_node = (struct list_node*)malloc(sizeof(struct list_node));
    
        if (tmp_node == NULL)
        {
            printf("\nCan not make tmp_node, because of malloc\n");
            return NULL;
        }

        str_len = 1;

        /* reading from file for one string */
        while(((c = fgetc(f)) != '\n') && (c != EOF)) 
        {
            str_temp[str_len - 1] = c;
            ++str_len;
            check_ptr = (char*)realloc(str_temp, str_len);
            if (check_ptr == NULL)
            {
                printf("\nCan not reallocate memory for str_temp\n");
                return NULL;
            }
            str_temp = check_ptr;
        }
        
        str_temp[str_len - 1] = '\0';

        /* giving ptrs for previous struct */
        tmp_node->str = str_temp;
        tmp_node->prev = tmp_node_prev;
        tmp_node->next = NULL;
        tmp_node_prev->next = tmp_node;
        ++cnt_of_nodes;

        /* replacing tmp_node_prev with temp_node */
        tmp_node_prev = tmp_node;
    }
    
    /* cycling the list */
    /*tmp_head->prev = tmp_node_prev;
    tmp_node_prev->next = tmp_head;*/
    
    /* giving information about list */
    tmp_list->num_elements = cnt_of_nodes;
    tmp_list->head = tmp_head;
    tmp_list->tail = tmp_node_prev;

    
    /*free(tmp_head);
    free(tmp_node_prev);
    free(str_temp);
    free(tmp_node);
    free(check_ptr);*/

    fclose(f);
    return tmp_list;    
}


void freeNode(struct list_node * node)
{
    free(node->prev->str);
    free(node->prev);
}

void getWindowSize(int *row, int *col)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *row = w.ws_row;
    *col = w.ws_col; 
}


int getCount(int i)
{   
    int _tmp = 0;
    while (i != 0)
    {
        ++_tmp;
        i /= 10;
    }        
    return _tmp;
}


void printText(Bidirect_list *l)
{
    if (l == NULL)
    {
        printf("\nIts not a list\n");
        return;
    }
    struct list_node *p;
    int i, j, k, tmp_;
    /*int row, col;*/
    p = l->head;
    /*getWindowSize(&row, &col);*/
    

    /*printed_rows = row;*/

    for (i = 1; i <= (l->num_elements); ++i)
    {
        if ((i >= top) && (i <= bot - 1))
        {
            if (global_args_.n == 1)
            {
                tmp_ = i;
                tmp_ = getCount(tmp_);
                for (k = 0; k < (length - tmp_); ++k)
                {
                    putchar(' ');
                }
                printf("%d", i);
                putchar(':');
            }
            if (left == 1)
            {
                putchar('|');
            }
            else 
            {
                putchar('<');
            }
            
            /*printf("%s\n", p->str);*/
            for (j = 1; j <= strlen(p->str); ++j)
            {
                if ((j >= left) && (j <= right))
                {
                    putchar(p->str[j - 1]);
                }
            }
            putchar('\n');
        }
        p = p->next;    
    }
}


void clearWindow()
{
    /*int row, col, i;
    getWindowSize(&row, &col);
    for (i = 0; i < row; ++i)
    {
        printf("\n");
    }*/
    start();
    clearscr();
    return;    
}

void moveCursor(int p, Bidirect_list *l)
{
    int row, col;
    getWindowSize(&row, &col);
    

    switch (p)
    {
        /* up */
        case 65:
            if (x >= 2)
            {   
                printf("\E[%d;%dH", --x, y);
                --px;
                
            }
            else if (top >= 2)
            {
                --top;
                --bot;
                clearWindow();
                printText(l);
                printf("\E[%d;%dH", x, y);
                --px;
            }
            /*if (x % row == top)
            {
                clearWindow();
                printText(l);
                printf("\E[%d;%dH", --x, y);
            }*/
                /*printf("\033[1A");*/
        break;
        /* down */
        case 66:
            if (x < row - 1)
            {
                printf("\E[%d;%dH", ++x, y);
                ++px;
            }
            else if (bot <= l->num_elements)
            {
                ++top;
                ++bot;
                clearWindow();
                printText(l);
                printf("\E[%d;%dH", x, y);
                ++px;
            }
                /*printf("\033[1B");*/
        break;    
        /* left */
        case 68:
        {    
            if (global_args_.n == 1) 
            {
                if (y > tmp + 1)
                {
                    printf("\E[%d;%dH", x, --y);
                    --py;
                }
                else if (left >= 2)
                {
                    --left;
                    --right; 
                    clearWindow();
                    printText(l);
                    printf("\E[%d;%dH", x, y);
                    --py;
                }
            }
            else
            {
                if (y >= 3)
                {
                    printf("\E[%d;%dH", x, --y);
                    --py;
                }
                else if (left >= 2)
                {
                    --left;
                    --right; 
                    clearWindow();
                    printText(l);
                    printf("\E[%d;%dH", x, y);
                    --py;
                }
            }
        }        /*printf("\033[1D");*/
        break;
        /* right */
        case 67:
            if (y < col)
            {
                printf("\E[%d;%dH", x, ++y);
                ++py;
            }
            else 
            { 
                if (l == NULL)
                {
                    printf("\nIts not a list\n");
                    return;
                }
                struct list_node *p;
                int i;
                p = l->head;

                for (i = 1; i < px; ++i)
                {
                    p = p->next;    
                }   

                if (right < strlen(p->str))
                {
                    ++left;
                    ++right;
                    clearWindow();
                    printText(l);
                    printf("\E[%d;%dH", x, y);
                    ++py;
                }
            }
                /*printf("\033[1С");*/
        break;
    }     
}   


void freeList(Bidirect_list *l)
{
    if (l == NULL)
    {
        printf("\nIts not a list\n");
        return;
    }
    struct list_node *p;
    int i;
    p = l->head;
     
    for (i = 1; i <= l->num_elements; ++i)
    {
        p = p->next;
        freeNode(p);
    }
    free(p->str);
    free(p);
    free(l);
}


void printPosition(Bidirect_list *l)
{
    int row, col, i;
    getWindowSize(&row, &col);
    printf("\E[%d;1H", row);
    for (i = 0; i < col; ++i)
    {
        printf(" ");
    }
    printf("\E[%d;1H", row);
    for (i = 0; i < (getCount(l->num_elements - 1) - getCount(px)); ++i)
    {
        printf(" ");
    }
    printf("%d (%lu) rows %d cols %d top %d bot %d x %d y %d left %d right %d p_x %d p_y %d px %d py %d", px, l->num_elements, row, col, top, bot, x, y, left, right, p_x, p_y, px, py);
    printf("\E[%d;%dH", x, y);
    return;
}

int goToNumber(int t, Bidirect_list *l)
{
    int row, col;
    getWindowSize(&row, &col);

    /* if x is out of range */
    if ((t < 1) || (t > l->num_elements))
    {
        return 1;
    }
    
    clearWindow();
    top = t;
    bot = t + row - 1;
    
    px = t;
    p_x = t;
    prev_pos_x = t;
    x = 1;  
    printText(l);
    /*printf("\E[%d;%dH", x, y);*/
    
    printPosition(l); 
    return 0;
}

void clearLastRow()
{
    int row, col, i;
    getWindowSize(&row, &col);
    printf("\E[%d;1H", row);
    for (i = 0; i < col; ++i)
    {
        printf(" ");
    }
    printf("\E[%d;1H", row);
    return;
}

int searchString(char *str, Bidirect_list *l)
{
    int i, position = -1;
    char *str_temp = NULL;
    struct list_node *p;
    p = l->head;
    for (i = 1; i <= l->num_elements; ++i)
    {
        if (i >= (prev_pos_x > px ? prev_pos_x : px))
        {
            if (i == (prev_pos_x > px ? prev_pos_x : px))
            {
                str_temp = strstr((p->str + prev_pos_y + 1), str);
            }
            else 
            {
                str_temp = strstr(p->str, str);
            }
            if (str_temp != NULL)
            {
                position = str_temp - p->str;
                if (((position > prev_pos_y) && (i == (prev_pos_x > px ? prev_pos_x : px))) || (i != prev_pos_x))
                {
                    p_x = i;
                    p_y = position;
                    prev_pos_x = i;
                    prev_pos_y = position;
                    return position;
                }        
            }
        }
        p = p->next;
    }
    return position;
}

int _atoi(char *str)
{
    int i = 0, value = 0, len = 0;
    len = strlen(str);
    
    while ((i < len) && (str[i] <= '9') && (str[i] >= '0'))
    {
        value *= 10;
        value += (str[i] - '0');
        ++i;            
    }
    
    if (i != len)
    {
        return -1;
    }
    else 
    {
        return value;
    }       
    
}

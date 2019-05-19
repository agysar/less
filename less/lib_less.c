/*#define _POSIX_SOURCE*/

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
char *returned_str = NULL;
char *argv_1;


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

Bidirect_list *l_for_subst;
Bidirect_list *l;


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

    /* opening */
    if ((path != global_args_.filename) && (path != argv_1) && (path != NULL))
    {
        f = fmemopen(path, strlen(path), "r");
    }
    else 
    {
        f = fopen(path, "r");
    }


    if (f == NULL)
    {
        printf("\nCan not open file %s\n", path);
        return NULL;
    }

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
    
        str_len = 1;

        /* reading from file for one string */
        while(((c = fgetc(f)) != '\n') && (c != EOF)) 
        {
            str_temp[str_len - 1] = c;
            ++str_len;
            check_ptr = (char*)realloc(str_temp, str_len);
            if (check_ptr == NULL)
            {
                if (tmp_node != NULL)
                {
                    free(tmp_node);
                }
                printf("\nCan not reallocate memory for str_temp\n");
                return NULL;
            }
            str_temp = check_ptr;
        }
        
        str_temp[str_len - 1] = '\0';

        /* giving ptrs for previous struct */
        tmp_node = (struct list_node*)malloc(sizeof(struct list_node));
    
        if (tmp_node == NULL)
        {
            printf("\nCan not make tmp_node, because of malloc\n");
            return NULL;
        }
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


void freeNode(struct list_node *node)
{
    if (node->prev->str != NULL)
    {
        free(node->prev->str);
    }
    if (node->prev)
    {
        free(node->prev);
    }
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
            int cnt = 0;
            cnt = strlen(p->str); 
            for (j = 1; j <= cnt; ++j)
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
    /*int i;*/
     
    /*for (p = l->head; p->next != NULL; p = p->next)
    {
        freeNode(p);
    }*/
    p = l->head;
    while (p->next != NULL)
    {
        p = p->next;
        free(p->prev->str);
        free(p->prev);
        
        
    }
    /*for (i = 0; i < l->num_elements + 1; ++i)
    {
        p = p->next;
        freeNode(p);
    }*/
    free(p->str);
    free(p);
    /*free(p->str);
    free(p->prev->str);
    free(p->prev);
    free(p);*/
    free(l);
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



void printPosition(Bidirect_list *l)
{
    int row, col, i;
    getWindowSize(&row, &col);
    /*printf("\E[%d;1H", row);
    for (i = 0; i < col; ++i)
    {
        printf(" ");
    }*/
    clearLastRow();
    printf("\E[%d;1H", row);
    for (i = 0; i < (getCount(l->num_elements - 1) - getCount(px)); ++i)
    {
        printf(" ");
    }
    printf("%d (%lu) rows %d cols %d top %d bot %d x %d y %d left %d right %d p_x %d p_y %d px %d py %d prev_pos_x %d prev_pos_y %d", px, l->num_elements, row, col, top, bot, x, y, left, right, p_x, p_y, px, py, prev_pos_x, prev_pos_y);
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

int getRuChar(char *str)
{
    if (str == NULL)
    {
        return 0;
    }
    int cnt, i;
    i = 0;
    cnt = 0;
    for (; i < strlen(str); ++i)
    {
        if ((int)str[i] < 0)
        {
            ++cnt;
        }
        
    }
    return cnt;
}

char *getStringFromTo(char *l, char *r)
{
    int len, i;
    char *str, *p;

    len = r - l + 1;
    p = l;

    if (l > r)
    {
        /*printf("\nl is more than r in getStringFromTo\n");*/
        return NULL;
    }
       
    if ((str = (char*)malloc(sizeof(char) * (len + 1))) == NULL)
    {
        printf("\nCan not allocate memory for str in getStringFromTo\n");
        return NULL;
    }

    for (i = 0; i < len; ++i)
    {
        str[i] = *p;
        ++p;
    }

    str[len] = '\0';
    return str;
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
                if (strlen(p->str) - 1 >= prev_pos_y + 1)
                {
                    str_temp = strstr((p->str + prev_pos_y + 1), str);
                }
                else 
                {
                    str_temp = NULL;
                }
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
                    char *t = NULL;
                    t = getStringFromTo(p->str, str_temp);
                    p_x = i;
                    p_y = position - (getRuChar(t) / 2);
                    free(t);
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

int writeToFile(char *path, Bidirect_list *l)
{
    FILE *f;
    int i;
    struct list_node *p;
    
    f = fopen(path, "w");
    if (f == NULL)
    {
        printf("\nCan not create file for writing int it\n");
        return 1;
    }    

    p = l->head;    

    for (i = 0; i < l->num_elements - 1; ++i)
    {
        fprintf(f, "%s\n", p->str);
        p = p->next;
    }
    
    fprintf(f, "%s", p->str);
    
    fclose(f);
    return 0;
}

/*Bidirect_list *createListFromString(char *str)
{
    Bidirect_list *tmp = NULL;
    struct list_node *p = NULL, *tmp_head = NULL;
    
    long len = 0;
    
    if (str == NULL)
    {
        printf("\nIts not a string for a list\n");
        return NULL;
    }

    tmp_head->prev = NULL;
     
    
    while ()
    {
    }


    free(returned_str);
    return tmp;
}*/

/* this function get string from l to r with begin and end*/

int substInList(char *what, char *to, Bidirect_list *l)
{
    /*int prev_prev_pos_x, prev_prev_pos_y;
    
    prev_prev_pos_x = prev_pos_x;
    prev_prev_pos_y = prev_pos_y;

    prev_pos_x = 1;
    prev_pos_y = 0;*/

    if (what == to)
    {
        return 0;
    }
    
    char *main_str, *tmp_main, *left_pos;
    int i, j = 0, difference = 0;
    long main_len = 0, tmp_len = 1, returned_len = 0;
    struct list_node *p;
    returned_str = NULL;

    if ((main_str = (char*)malloc(sizeof(char))) == NULL)
    {
        printf("\nCan not allocate memory for main_str\n");
        return 1;
    }
    main_str[0] = '\0';
    /* copying list to one string main_str */    
    p = l->head;
    for (i = 0; i < l->num_elements; ++i)
    {
        tmp_len = strlen(p->str);

        if ((main_str = (char*)realloc(main_str, sizeof(char) * (tmp_len + 1 + main_len) )) == NULL)
        {
            printf("\nCan not reallocate memory for main_str\n");
            return 1;
        }

        for (j = 0; j < tmp_len; ++j)
        {
            main_str[j + main_len] = p->str[j];
        }

        main_len += strlen(p->str);
        main_str[main_len] = '\n';
        ++main_len; /* add for \n symbol */
    
        p = p->next;
    }   
    /*if ((main_str = (char*)realloc(main_str, sizeof(char) * (1 + main_len) )) == NULL)
        {
            printf("\nCan not reallocate memory for main_str\n");
            return 1;
        }
    */
    main_str[main_len - 1] = '\0';

    /* get difference between l_subst and r_subst */
    difference = strlen(to) - strlen(what);

    /* looking for matches in the main_str */
    if (strstr(main_str, what) == NULL)
    {
        clearLastRow();
        printf("No matches for this substring");
        return 1;
    }
    
    tmp_main = main_str;  
    left_pos = main_str;  /* pointer to current position in main_str */

    returned_len = main_len;

    if ((returned_str = (char*)calloc(sizeof(char) * returned_len, sizeof(char))) == NULL)
    {
        free(main_str);
        printf("\nCan not allocate memory for returned_str\n");
        return 1;
    }

    while ((tmp_main = strstr(tmp_main, what)) != NULL)
    {
        /* changing length of returned_str */
        if (difference != 0)
        {
            returned_len += difference;
            /* reallocating memory for it */
            if ((returned_str = (char*)realloc(returned_str, sizeof(char) * returned_len)) == NULL)
            {
                printf("\nCan not reallocate memory for returned_str\n");
                free(main_str);
                return 1;
            }
        }   

        /* cat string before founded string in main_str to returned_str*/
        char *tmp_tmp = NULL;
        tmp_tmp = getStringFromTo(left_pos, tmp_main - 1);
        if (tmp_tmp != NULL)
        {
            strcat(returned_str, tmp_tmp);
            if (strlen(to) != 0)
            {
                strcat(returned_str, to);
            }
        }
        free(tmp_tmp);
        
        /* changing left_pos and tmp_main */
        tmp_main += strlen(what) * sizeof(char);
        left_pos = tmp_main;
    }
    
    returned_str[returned_len - 1] = '\0';
   
    if ((left_pos - main_str) <= (main_len - 1))
    {
        char *tmp_tmp = NULL;
        tmp_tmp = getStringFromTo(left_pos, main_str + main_len - 1);
        if (tmp_tmp != NULL)
        {
            strcat(returned_str, tmp_tmp);
        }
        free(tmp_tmp);
    } 

    /* get list from str */
    l_for_subst = getText(returned_str); 

   
    free(returned_str);     
    free(main_str);
    return 0; 
}




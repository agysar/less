/*#define _POSIX_SOURCE*/

#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <wchar.h>
#include <locale.h>

#define start() wprintf(L"\E[H")
#define clearscr() wprintf(L"\E[2J")
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
wchar_t *str_srch = NULL;
wchar_t *returned_str = NULL;
char *argv_1;
int flg_for_delete_int_subst = 0;
int subst = 0;

struct global_args_t
{
    char *filename; /* print file with numbers before strings*/
    int n;
    int v; /* print version of this program */
    int h; /* print help of this program */
} global_args_;


struct list_node
{
    wchar_t *str;
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
    wchar_t c;
    wchar_t *str_temp = NULL;
    wchar_t *check_ptr = NULL;
    int str_len = 1;
    int cnt_of_nodes = 0;
    int check = 0;

    /* opening */
    /*if ((path != global_args_.filename) && (path != argv_1) && (path != NULL))
    {
        f = fmemopen(path, strlen(path), "r");
        check = 1;
    }
    else 
    {
    }*/

    f = fopen(path, "r");

    if (f == NULL)
    {
        wprintf(L"\nCan not open file %s\n", path);
        return NULL;
    }

    /* making tmp_list */
    tmp_list = (Bidirect_list*)malloc(sizeof(Bidirect_list));
    
    if (tmp_list == NULL)
    {
        wprintf(L"\nCan not make tmp_list\n");
        return NULL;
    }

    /* making str_tmp */
    str_temp = (wchar_t*)malloc(sizeof(wchar_t));
    
    if (str_temp == NULL)
    {
        wprintf(L"\nCan not make str_temp\n");
        return NULL;
    }

    /* making tmp_head */
    tmp_head = (struct list_node*)malloc(sizeof(struct list_node));
    
    if (tmp_head == NULL)
    {
        wprintf(L"\nCan not make tmp_head\n");
        return NULL;
    }

   
    /* making head */
    if (!check)
    {
        while(((c = fgetwc(f)) != L'\n') && (c != WEOF)) 
        {
            str_temp[str_len - 1] = c;
            ++str_len;
            str_temp = (wchar_t*)realloc(str_temp, str_len * sizeof(wchar_t));
            if (str_temp == NULL)
            {
                wprintf(L"\nCan not reallocate memory for str_temp\n");
                return NULL;
            }
        }
        
        str_temp[str_len - 1] = L'\0';
        /*str_temp[str_len] = '\0';*/

        /* giving fields for head */
        tmp_head->str = str_temp;
        tmp_head->prev = NULL;
        tmp_head->next = NULL;

        /* saving head to tmp_node_prev */
        tmp_node_prev = tmp_head;
        ++cnt_of_nodes;


        /* reading */
        while (c != WEOF)
        {
            str_temp = (wchar_t*)malloc(sizeof(wchar_t));
        
            if (str_temp == NULL)
            {
                wprintf(L"\nCan not make str_temp\n");
                return NULL;
            }
    
            str_len = 1;

            /* reading from file for one string */
            while(((c = fgetwc(f)) != L'\n') && (c != WEOF)) 
            {
                str_temp[str_len - 1] = c;
                ++str_len;
                check_ptr = (wchar_t*)realloc(str_temp, str_len * sizeof(wchar_t));
                if (check_ptr == NULL)
                {
                    if (tmp_node != NULL)
                    {
                        free(tmp_node);
                    }
                    wprintf(L"\nCan not reallocate memory for str_temp\n");
                    return NULL;
                }
                str_temp = check_ptr;
            }
        
            str_temp[str_len - 1] = L'\0';

            /* giving ptrs for previous struct */
            tmp_node = (struct list_node*)malloc(sizeof(struct list_node));
    
            if (tmp_node == NULL)
            {
                wprintf(L"\nCan not make tmp_node, because of malloc\n");
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
    }
    fclose(f);
    return tmp_list;    
}

Bidirect_list *getTextFromStr(wchar_t *path)
{
    Bidirect_list *tmp_list;
    struct list_node *tmp_node, *tmp_head, *tmp_node_prev;
    wchar_t c;
    wchar_t *str_temp = NULL;
    wchar_t *check_ptr = NULL;
    int str_len = 1;
    int cnt_of_nodes = 0;
    int position = 0;
    int path_len = 0;

    path_len = wcslen(path);
    
    /* opening */
    /*f = fmemopen((wchar_t*)path, wcslen(path), "r");*/
    /*size_t path_len = 0;
    path_len = wcslen(path);
    f = open_wmemstream(&path, &path_len);*/
    /*fwide(f, 1);*/
    /*check = 1;*/


    /*if (f == NULL)
    {
        wprintf(L"\nCan not open str %ls\n", path);
        return NULL;
    }*/

    /* making tmp_list */
    tmp_list = (Bidirect_list*)malloc(sizeof(Bidirect_list));
    
    if (tmp_list == NULL)
    {
        wprintf(L"\nCan not make tmp_list\n");
        return NULL;
    }

    /* making str_tmp */
    str_temp = (wchar_t*)malloc(sizeof(wchar_t));
    
    if (str_temp == NULL)
    {
        wprintf(L"\nCan not make str_temp\n");
        return NULL;
    }

    /* making tmp_head */
    tmp_head = (struct list_node*)malloc(sizeof(struct list_node));
    
    if (tmp_head == NULL)
    {
        wprintf(L"\nCan not make tmp_head\n");
        return NULL;
    }

   
    /* making head */
    while((position < path_len) && ((c = path[position]) != L'\n') && (c != L'\0'))  
    {
        str_temp[str_len - 1] = c;
        ++str_len;
        ++position;
        str_temp = (wchar_t*)realloc(str_temp, str_len * sizeof(wchar_t));
        if (str_temp == NULL)
        {
            wprintf(L"\nCan not reallocate memory for str_temp\n");
            return NULL;
        }
    }
        
    str_temp[str_len - 1] = L'\0';
    ++position;
    /*str_temp[str_len] = '\0';*/

    /* giving fields for head */
    tmp_head->str = str_temp;
    tmp_head->prev = NULL;
    tmp_head->next = NULL;

    /* saving head to tmp_node_prev */
    tmp_node_prev = tmp_head;
    ++cnt_of_nodes;


        /* reading */
        while (position < path_len)
        {
            str_temp = (wchar_t*)malloc(sizeof(wchar_t));
        
            if (str_temp == NULL)
            {
                wprintf(L"\nCan not make str_temp\n");
                return NULL;
            }
    
            str_len = 1;

            /* reading from file for one string */
            /*while(((c = fgetwc(f)) != L'\n') && (c != WEOF)) */
            while ((position < path_len) && ((c = path[position]) != L'\n') && (c != L'\0'))  
            {
                str_temp[str_len - 1] = c;
                ++str_len;
                ++position;
                check_ptr = (wchar_t*)realloc(str_temp, str_len * sizeof(wchar_t));
                if (check_ptr == NULL)
                {
                    if (tmp_node != NULL)
                    {
                        free(tmp_node);
                    }
                    wprintf(L"\nCan not reallocate memory for str_temp\n");
                    return NULL;
                }
                str_temp = check_ptr;
            }
        
            str_temp[str_len - 1] = L'\0';
            ++position;

            /* giving ptrs for previous struct */
            tmp_node = (struct list_node*)malloc(sizeof(struct list_node));
    
            if (tmp_node == NULL)
            {
                wprintf(L"\nCan not make tmp_node, because of malloc\n");
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
        wprintf(L"\nIts not a list\n");
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
                    putwc(L' ', stdout);
                }
                wprintf(L"%d", i);
                putwc(L':', stdout);
            }
            if (left == 1)
            {
                putwc(L'|', stdout);
            }
            else 
            {
                putwc(L'<', stdout);
            }
            
            /*printf("%s\n", p->str);*/
            int cnt = 0;
            cnt = wcslen(p->str); 
            for (j = 1; j <= cnt; ++j)
            {
                if ((j >= left) && (j <= right))
                {
                    if (p->str[j - 1] != L'\n')
                    {
                        putwc(p->str[j - 1], stdout);
                    }
                }
            }
            putwc(L'\n', stdout);
            /*if (!flg_for_delete_int_subst)
            {
                putwc(L'\n', stdout);
            }*/
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
                wprintf(L"\E[%d;%dH", --x, y);
                --px;
                
            }
            else if (top >= 2)
            {
                --top;
                --bot;
                clearWindow();
                printText(l);
                wprintf(L"\E[%d;%dH", x, y);
                --px;
            }
        break;
        /* down */
        case 66:
            if ((x < row - 1) && (x < l->num_elements))
            {
                wprintf(L"\E[%d;%dH", ++x, y);
                ++px;
            }
            else if (bot <= l->num_elements)
            {
                ++top;
                ++bot;
                clearWindow();
                printText(l);
                wprintf(L"\E[%d;%dH", x, y);
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
                    wprintf(L"\E[%d;%dH", x, --y);
                    --py;
                }
                else if (left >= 2)
                {
                    --left;
                    --right; 
                    clearWindow();
                    printText(l);
                    wprintf(L"\E[%d;%dH", x, y);
                    --py;
                }
            }
            else
            {
                if (y >= 3)
                {
                    wprintf(L"\E[%d;%dH", x, --y);
                    --py;
                }
                else if (left >= 2)
                {
                    --left;
                    --right; 
                    clearWindow();
                    printText(l);
                    wprintf(L"\E[%d;%dH", x, y);
                    --py;
                }
            }
        }        /*printf("\033[1D");*/
        break;
        /* right */
        case 67:
            if (y < col)
            {
                struct list_node *p;
                int i, len = 0;
                p = l->head;

                for (i = 1; i < px; ++i)
                {
                    p = p->next;    
                }

                len = wcslen(p->str);
                if (global_args_.n)
                {
                    len += tmp + 1;
                }
                else
                {
                    len += tmp + 2;
                }
                if (y < len - 1)
                { 
                    wprintf(L"\E[%d;%dH", x, ++y);
                    ++py;
                }
            }
            else 
            { 
                if (l == NULL)
                {
                    wprintf(L"\nIts not a list\n");
                    return;
                }
                struct list_node *p;
                int i;
                p = l->head;

                for (i = 1; i < px; ++i)
                {
                    p = p->next;    
                }   

                if (right < wcslen(p->str))
                {
                    ++left;
                    ++right;
                    clearWindow();
                    printText(l);
                    wprintf(L"\E[%d;%dH", x, y);
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
        wprintf(L"\nIts not a list\n");
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
    wprintf(L"\E[%d;1H", row);
    for (i = 0; i < col; ++i)
    {
        wprintf(L" ");
    }
    wprintf(L"\E[%d;1H", row);
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
    wprintf(L"\E[%d;1H", row);
    for (i = 0; i < (getCount(l->num_elements - 1) - getCount(px)); ++i)
    {
        wprintf(L" ");
    }
    /*wprintf(L"%d (%lu) rows %d cols %d top %d bot %d x %d y %d left %d right %d p_x %d p_y %d px %d py %d prev_pos_x %d prev_pos_y %d", px, l->num_elements, row, col, top, bot, x, y, left, right, p_x, p_y, px, py, prev_pos_x, prev_pos_y);*/
    wprintf(L"%d (%lu)", px, l->num_elements);
    wprintf(L"\E[%d;%dH", x, y);
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

/*int getRuChar(char *str)
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
}*/

wchar_t *getStringFromTo(wchar_t *l, wchar_t *r)
{
    int len, i;
    i = 0;
    wchar_t *str, *p;

    len = r - l + 1;
    p = l;

    if ((l > r) && (subst != 1))
    {
        /*printf("\nl is more than r in getStringFromTo\n");*/
        return NULL;
    }
       
    if ((str = (wchar_t*)malloc(sizeof(wchar_t) * (len + 1))) == NULL)
    {
        wprintf(L"\nCan not allocate memory for str in getStringFromTo\n");
        return NULL;
    }
    
    if ((r - l == -1) && (subst == 1))
    {
        str[0] = L'\0';
        return str;
    }

    while ((r - p) >= 0)
    {
        /*if ((*p == '\\') || (*p == '/'))
        {
            if (i < len - 1)
            {
                str[i] = *(p + 1);
                ++p;
            }
        }
        else 
        {
            str[i] = *p;
        }*/
        if (*p == L'\\')
        {
            if ((r - p) >= 1)
            {
                switch (*(p + 1))
                {
                    case L'\\':
                        str[i] = L'\\';
                        ++i;
                        ++p;
                    break;
                    case L'n':
                        str[i] = L'\n';
                        ++i;
                        ++p;
                    break;
                    case L'/':
                        str[i] = L'/';
                        ++i;
                        ++p;
                    break;
                }    
            }
        }
        else 
        {
            str[i] = *p;
            ++i;
        }
        if (r - p == 0)
        {
            break;
        }
        if ((r - p) > 0)
        {
            ++p;
        }

        /*
        str[i] = *p;
        ++i;
        if (i < len - 1)
        {
            ++p;
        }*/
    }

    str[i] = L'\0';
    return str;
}


int searchString(wchar_t *str, Bidirect_list *l)
{
    int i, position = -1;
    wchar_t *str_temp = NULL;
    struct list_node *p;
    p = l->head;
    for (i = 1; i <= l->num_elements; ++i)
    {
        if (i >= (prev_pos_x > px ? prev_pos_x : px))
        {
            if (i == (prev_pos_x > px ? prev_pos_x : px))
            {
                if (wcslen(p->str) >= prev_pos_y + 1)
                {
                    str_temp = wcsstr((p->str + prev_pos_y + 1), str);
                }
                else 
                {
                    str_temp = NULL;
                }
            }
            else 
            {
                str_temp = wcsstr(p->str, str);
            }
            if (str_temp != NULL)
            {
                position = str_temp - p->str;
                if (((position > prev_pos_y) && (i == (prev_pos_x > px ? prev_pos_x : px))) || (i != prev_pos_x))
                {
                    wchar_t *t = NULL;
                    t = getStringFromTo(p->str, str_temp);
                    p_x = i;
                    p_y = position;
                    /*p_y = position - (getRuChar(t) / 2);*/
                    free(t);
                    prev_pos_x = i;
                    /*if (global_args_.n)
                    {
                        p_y += length;
                    }*/

                    prev_pos_y = position;
                    return position;
                }        
            }
        }
        p = p->next;
    }
    return position;
}

int _atoi(wchar_t *str)
{
    int i = 0, value = 0, len = 0;
    len = wcslen(str);
    
    while ((i < len) && (str[i] <= L'9') && (str[i] >= L'0'))
    {
        value *= 10;
        value += (str[i] - L'0');
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

char *w2c(wchar_t *str)
{
    char *path_char = NULL;
    int len = 0, i;

    len = wcslen(str);
    if ((path_char = (char*)malloc(sizeof(wchar_t) * (len + 1))) == NULL)
    {
        wprintf(L"\nCan not allocate memory for path_char\n");
        return NULL;
    }

    for (i = 0; i < len; ++i)
    {
        path_char[i] = str[i];
    }
    path_char[len] = '\0';
    
    return path_char;
}

int writeToFile(wchar_t *path, Bidirect_list *l)
{
    FILE *f;
    int i;
    struct list_node *p;
    char *path_char = NULL;

    
    /*wcstombs(path_char, path, sizeof(wchar_t) * (wcslen(path) + 1));    

    path_char[wcslen(path)] = '\0';*/
    path_char = w2c(path);
    
    f = fopen(path_char, "w");
    if (f == NULL)
    {
        free(path_char);
        wprintf(L"\nCan not create file for writing int it\n");
        return 1;
    }    

    p = l->head;    

    for (i = 0; i < l->num_elements - 1; ++i)
    {
        fwprintf(f, L"%ls\n", p->str);
        p = p->next;
    }
    
    fwprintf(f, L"%ls", p->str);
    
    free(path_char);
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

int substInList(wchar_t *what, wchar_t *to, Bidirect_list *l)
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
   
    wchar_t *main_str, *tmp_main, *left_pos;
    int i, j = 0, difference = 0;
    long main_len = 0, tmp_len = 1, returned_len = 0;
    struct list_node *p, *tmp_p;
    returned_str = NULL;
    
    if ((!wcscmp(what,L"\n")) && (!wcscmp(to, L"")))
    {
        p = l->head;
        while ((p != NULL) && (wcslen(p->str) == 0))
        {
            tmp_p = p->next;
            l->head = p->next;
            l->head->prev = NULL;
            free(p->str);
            free(p);
            --l->num_elements;
            p = tmp_p;
            /*p = l->head;*/
        }

        /*p = p->next;*/

        while (p != NULL)
        {
            if (wcslen(p->str) == 0)
            {
                if ((p->next != NULL) && (p->prev != NULL))
                {
                    tmp_p = p->next;
                    p->prev->next = p->next;
                    p->next->prev = p->prev;
                    free(p->str);
                    free(p);
                    p = tmp_p;
                    --l->num_elements;
                    continue;
                }
            }
            p = p->next;
        }
        
        l->tail = p;
        /*p = l->tail;
        
        
        if (wcslen(p->str) == 0)
        {
            l->tail = p->prev;
            l->tail->next = NULL;
            free(p->str);
            free(p);
            --l->num_elements;
        }*/
        
        flg_for_delete_int_subst = 1;

        return 0; 
    }
        /*if ((what = (wchar_t*)realloc(what, 3 * sizeof(wchar_t))) == NULL)
        {
            wprintf(L"\nCan not reallocate memory for what\n");
            return 1;
        }
        if ((to = (wchar_t*)realloc(to, 2 * sizeof(wchar_t))) == NULL)
        {
            wprintf(L"\nCan not reallocate memory for to\n");
            return 1;
        }
        wcscat(what, L"\n");
        wcscat(to, L"\n");*/
          /*what = "\n\n";
         to = "\n";*/
  

    if ((main_str = (wchar_t*)malloc(sizeof(wchar_t))) == NULL)
    {
        wprintf(L"\nCan not allocate memory for main_str\n");
        return 1;
    }
    main_str[0] = L'\0';
    /* copying list to one string main_str */    
    p = l->head;
    for (i = 0; i < l->num_elements; ++i)
    {
        tmp_len = wcslen(p->str);

        if ((main_str = (wchar_t*)realloc(main_str, sizeof(wchar_t) * (tmp_len + 1 + main_len) )) == NULL)
        {
            wprintf(L"\nCan not reallocate memory for main_str\n");
            return 1;
        }

        for (j = 0; j < tmp_len; ++j)
        {
            main_str[j + main_len] = p->str[j];
        }

        main_len += wcslen(p->str);
        main_str[main_len] = L'\n';
        ++main_len; /* add for \n symbol */
    
        p = p->next;
    }   
    /*if ((main_str = (char*)realloc(main_str, sizeof(char) * (1 + main_len) )) == NULL)
        {
            printf("\nCan not reallocate memory for main_str\n");
            return 1;
        }
    */
    main_str[main_len - 1] = L'\0';

    /* get difference between l_subst and r_subst */
    difference = wcslen(to) - wcslen(what);

    /* looking for matches in the main_str */
    if (wcsstr(main_str, what) == NULL)
    {
        clearLastRow();
        wprintf(L"No matches for this substring");
        free(main_str);
        return 1;
    }
    
    tmp_main = main_str;  
    left_pos = main_str;  /* pointer to current position in main_str */

    returned_len = main_len;

    if ((returned_str = (wchar_t*)calloc(returned_len, sizeof(wchar_t))) == NULL)
    {
        free(main_str);
        wprintf(L"\nCan not allocate memory for returned_str\n");
        return 1;
    }

    while ((tmp_main = wcsstr(tmp_main, what)) != NULL)
    {
        /* changing length of returned_str */
        if (difference != 0)
        {
            returned_len += difference;
            /* reallocating memory for it */
            if ((returned_str = (wchar_t*)realloc(returned_str, sizeof(wchar_t) * returned_len)) == NULL)
            {
                wprintf(L"\nCan not reallocate memory for returned_str\n");
                free(main_str);
                return 1;
            }
        }   

        /* cat string before founded string in main_str to returned_str*/
        wchar_t *tmp_tmp = NULL;
        tmp_tmp = getStringFromTo(left_pos, tmp_main - 1);
        if (tmp_tmp != NULL)
        {
            wcscat(returned_str, tmp_tmp);
            wcscat(returned_str, to);
            /*if (strlen(to) != 0)
            {
                strcat(returned_str, to);
            }*/
        }
        free(tmp_tmp);
        
        /* changing left_pos and tmp_main */
        tmp_main += wcslen(what);
        left_pos = tmp_main;
    }
    
    returned_str[returned_len - 1] = L'\0';
   
    if ((left_pos - main_str) <= (main_len - 1))
    {
        wchar_t *tmp_tmp = NULL;
        tmp_tmp = getStringFromTo(left_pos, main_str + main_len - 1);
        if (tmp_tmp != NULL)
        {
            wcscat(returned_str, tmp_tmp);
        }
        free(tmp_tmp);
    } 

    /* get list from str */
    l_for_subst = getTextFromStr(returned_str); 

   
    free(returned_str);     
    free(main_str);
    return 0; 
}




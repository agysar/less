#include "lib_less.c"
#define HELP "Use -n for printing numbers before lines\nUse -v for version of this program\nYou must use ':' for using commands below\nUse write *file* for writing to file\nUse /string for searching in this file\nUse subst /original/new/ for subsitude original with new\nUse *number* to go to *number* line\n"


struct termios old_attr, new_attr;
static const char *optString = "n:vh";

void changeTerminal()
{
    /* changing terminal mode */
    tcgetattr(0, &old_attr);
    memcpy(&new_attr, &old_attr, sizeof(struct termios));
    new_attr.c_lflag &= ~ECHO;
    new_attr.c_lflag &= ~ICANON;
    new_attr.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_attr);
    return;
}

void returnTerminal()
{
    tcsetattr(0,TCSANOW,&old_attr);
    return;
}

int main(int argc, char **argv)
{
    Bidirect_list *l;
    char c;
    int temp = 0, i = 1;
    char *str_tmp = NULL;
    getWindowSize(&bot, &right);
    /*if ((str_srch = (char*)malloc(sizeof(char))) == NULL)
    {
        printf("\nCan not create str_srch\n");
        return 11;
    }*/


    if (!isatty(0))
    {
        printf("Input is not from terminal\n");        
        return 1;
    }

    if (argc < 2)
    {
        printf("\nNot enough arguments, please use -h for help\n");
        return 10;
    }
 

    global_args_.filename = NULL;
    global_args_.v = 0;
    global_args_.h = 0;
    global_args_.n = 0;


    opt = getopt(argc, argv, optString);
    while (opt != - 1)
    {
        switch (opt)
        {
            case 'n': 
                global_args_.filename = optarg;
                global_args_.n = 1;
            break;
            case 'v':
                global_args_.v = 1;
            break;
            case 'h':
                global_args_.h = 1;
            break;
        }
        opt = getopt(argc, argv, optString);
    }

    clearWindow();

    if (global_args_.v == 1)
    {
        /*printf("\E[%d;%dH", (top + bot) / 8, (left - 1 + right) / 4);*/
        printf("Version 1.01\n");
        /*printf("\E[%d;%dH", ((top + bot) / 8) + 1, (left - 1 + right) / 4);*/
        printf("Powered by Arsenyi Margasov\n");
        /*tcsetattr(0,TCSANOW,&old_attr);*/
        /*clearWindow();*/
        return 0;    
    }

    if (global_args_.h == 1)
    {
        printf(HELP);
        /*tcsetattr(0,TCSANOW,&old_attr);*/
        return 0;
    }
    

    if (global_args_.n == 1)
    {
        l = getText(global_args_.filename);
        if (l == NULL)
        {
            printf("\nList wasn`t created correctly\n");
            return 2;
        }
        temp = l->num_elements;
        length = getCount(temp);
        y += length;
        ++y;
        tmp = length + 2;
        right -= tmp;
    } 
    else 
    {
        l = getText(argv[1]);
        if (l == NULL)
        {
            printf("\nList wasn`t created correctly\n");
            return 2;
        }
        --right;
    }
    
    --(l->num_elements);
    changeTerminal();   

    printText(l);
    printPosition(l);
    if (global_args_.n)
    {
        printf("\E[1;%dH", length + 3);
    }
    else 
    {
        printf("\E[1;2H");
        
    }

    printPosition(l);

    while((c = getchar()) != EOF)
    {
        if ((c == '\004') || (c == 'q'))
        {
            break;
        }
        
        moveCursor(c, l);
        printPosition(l);
             
        if ((c =='/') || ((c >= '0') && (c <= '9')) || (c == 'w') || (c == 's'))
        {
            if ((str_tmp = (char*)malloc(sizeof(char))) == NULL)
            {
                printf("\nCan not create str_tmp\n");
                return 11;
            }

            clearLastRow();
            printf("%c", c);
            str_tmp[i - 1] = c;
            ++i;

            if ((str_tmp = (char*)realloc(str_tmp, sizeof(char) * i)) == NULL)
            {
                clearLastRow();
                printf("\nCan not reallocate memory for str_tmp\n");
                return 12;
            }

            /* reading string from stdin */
            while ((c = getchar()) != '\n')
            {   
                if (((c < 65) || (c > 68)) && (c != '\033') && (c != '['))
                {
                    printf("%c", c);
                    str_tmp[i - 1] = c;
                    ++i;
                    if ((str_tmp = (char*)realloc(str_tmp, sizeof(char) * i)) == NULL)
                    {
                        clearLastRow();
                        printf("\nCan not reallocate memory for str_tmp\n");
                        return 12;
                    }
                }
            }
            str_tmp[i - 1] = '\0';

        }
        else 
        {
            i = 1;
            printPosition(l);
            continue;
        }
    
               
        /* go to line */    
        if ((str_tmp[0] >= '0') && (str_tmp[0] <= '9'))
        {
            if (goToNumber(_atoi(str_tmp), l))
            {
                clearLastRow();
                printf("Wrong position for cursor");
                printf("\E[%d;%dH", x, y);
                i = 1;
            }
        }
        
        /* searching for substr */
        if (str_tmp[0] == '/')
        {
             
            char *tmp_str = NULL;
            if (str_tmp[1] == '\0')
            {
                tmp_str = str_srch;
            }
            else
            {
                tmp_str = str_tmp + 1;
            }
            if ((tmp_str == NULL) || (searchString((tmp_str), l) == -1))
            {
                char symbol;
                clearLastRow();
                printf("No such string in this file");
                while ((symbol = getchar()) != '\n')
                {
                    clearLastRow();
                    printf("Press ENTER to return to :");
                }
                clearLastRow();
                printf(":");
            } 
            else
            {   
                goToNumber(p_x, l);
                
                if (global_args_.n)
                {
                    y = tmp + 1;
                }
                else
                {
                    y = tmp + 2;
                }
                while (left > 1)
                {
                    moveCursor(68, l);
                }
                int j;
                for (j = 0; j < p_y; ++j)
                {
                    moveCursor(67, l);
                }
                
                if ((str_srch != tmp_str) || (str_tmp[1] != '\0'))
                {
                    /*prev_pos_x = px;
                    prev_pos_y = 0;*/
                    free(str_srch);
                    if ((str_srch = (char*)malloc(sizeof(char) * (strlen(tmp_str) + 1))) == NULL)
                    {
                        printf("\nCan not allocate memory for str_srch\n");
                        return 112;
                    }
                    /*if (tmp_str != )*/
                    strcpy(str_srch, tmp_str);
                }
            }
        }
        free(str_tmp); 
        i = 1;
    }    

    free(str_srch);
    /* returning terminal to initial state */
    returnTerminal();    

    clearWindow();
    freeList(l);
    return 0;
}

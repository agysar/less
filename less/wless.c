#include "wlib_less.c"
#define HELP L"Use -n for printing numbers before lines\nUse -v for version of this program\nYou must use ':' for using commands below\nUse write *file* for writing to file\nUse /string for searching in this file\nUse subst /original/new/ for subsitude original with new\nUse *number* to go to *number* line\n"


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
    /*fwide(stdout, 1);*/
    setlocale(LC_ALL, "ru_RU.utf8");
    argv_1 = argv[1];
    wchar_t c;
    int temp = 0, i = 1;
    wchar_t *str_tmp = NULL;
    wchar_t *l_subst, *r_subst;
    getWindowSize(&bot, &right);

    if (!isatty(1))
    {
        /*wprintf(L"\nLooks like input is not from terminal\n");*/
        if (argc >= 2)
        {
            l = getText(argv[1]);
            
            struct list_node *p = NULL;
            p = l->head;    

            for (i = 0; i < l->num_elements - 1; ++i)
            {
                wprintf(L"%ls\n", p->str);
                p = p->next;
            }
            wprintf(L"%ls\n", p->str);
            freeList(l);

        }
        return 0; 
    }

    if (!isatty(0))
    {
        wchar_t c;
        fwide(stdin, 1);
        while ((c = getwchar()) != WEOF)
        {
            putwchar(c);
        }        
        return 0;
    }
 
    if (argc < 2)
    {
        wprintf(L"\nNot enough arguments, please use -h for help\n");
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
        wprintf(L"Version 1.01\n");
        /*printf("\E[%d;%dH", ((top + bot) / 8) + 1, (left - 1 + right) / 4);*/
        wprintf(L"Powered by Arsenyi Margasov\n");
        /*tcsetattr(0,TCSANOW,&old_attr);*/
        /*clearWindow();*/
        return 0;    
    }

    if (global_args_.h == 1)
    {
        wprintf(HELP);
        /*tcsetattr(0,TCSANOW,&old_attr);*/
        return 0;
    }
    

    if (global_args_.n == 1)
    {
        l = getText(global_args_.filename);
        if (l == NULL)
        {
            wprintf(L"\nList wasn`t created correctly\n");
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
            wprintf(L"\nList wasn`t created correctly\n");
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
        wprintf(L"\E[1;%dH", length + 3);
    }
    else 
    {
        wprintf(L"\E[1;2H");
        
    }

    printPosition(l);

    while((c = fgetwc(stdin)) != WEOF)
    {
        if ((c == L'\004') || (c == L'q'))
        {
            break;
        }
        
        moveCursor(c, l);
        printPosition(l);
             
        if ((c ==L'/') || ((c >= L'0') && (c <= L'9')) || (c == L'w') || (c == L's'))
        {
            if ((str_tmp = (wchar_t*)malloc(sizeof(wchar_t))) == NULL)
            {
                wprintf(L"\nCan not create str_tmp\n");
                return 11;
            }

            clearLastRow();
            wprintf(L"%lc", c);
            str_tmp[i - 1] = c;
            ++i;

            if ((str_tmp = (wchar_t*)realloc(str_tmp, sizeof(wchar_t) * i)) == NULL)
            {
                clearLastRow();
                wprintf(L"\nCan not reallocate memory for str_tmp\n");
                return 12;
            }

            /* reading string from stdin */
            while ((c = fgetwc(stdin)) != L'\n')
            {   
                /*if (((c < 65) || (c > 68)) && (c != L'\033') && (c != L'['))*/
               /* {*/
                if (c == 27)
                {
                    fgetwc(stdin);
                    fgetwc(stdin);
                    continue;
                }
                    wprintf(L"%lc", c);
                    str_tmp[i - 1] = c;
                    ++i;
                    if ((str_tmp = (wchar_t*)realloc(str_tmp, sizeof(wchar_t) * i)) == NULL)
                    {
                        clearLastRow();
                        wprintf(L"\nCan not reallocate memory for str_tmp\n");
                        return 12;
                    }
               /* }*/
            }
            str_tmp[i - 1] = L'\0';

        }
        else 
        {
            i = 1;
            printPosition(l);
            continue;
        }
    
               
        /* go to line */    
        if ((str_tmp[0] >= L'0') && (str_tmp[0] <= L'9'))
        {
            if (goToNumber(_atoi(str_tmp), l))
            {
                clearLastRow();
                wprintf(L"Wrong position for cursor");
                wprintf(L"\E[%d;%dH", x, y);
                i = 1;
            }
        }
        
        /* searching for substr */
        if (str_tmp[0] == L'/')
        {
             
            wchar_t *tmp_str = NULL;
            if (wcslen(str_tmp) == 1)
            {
                tmp_str = str_srch;
            }
            else
            {
                tmp_str = str_tmp + 1;
            }
            if ((tmp_str == NULL) || (searchString((tmp_str), l) == -1))
            {
                wchar_t symbol;
                clearLastRow();
                wprintf(L"No such string in this file");
                while ((symbol = fgetwc(stdin)) != L'\n')
                {
                    clearLastRow();
                    wprintf(L"Press ENTER to return to :");
                }
                clearLastRow();
                wprintf(L":");
            } 
            else
            {   
                goToNumber(prev_pos_x, l);
                
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
                
                if (str_srch != tmp_str)
                {
                    /*prev_pos_x = px;
                    prev_pos_y = 0;*/
                    free(str_srch);
                    if ((str_srch = (wchar_t*)malloc(sizeof(wchar_t) * (wcslen(tmp_str) + 1))) == NULL)
                    {
                        wprintf(L"\nCan not allocate memory for str_srch\n");
                        return 112;
                    }
                    /*if (tmp_str != )*/
                    wcscpy(str_srch, tmp_str);
                }
            }
        }

        /* write to file */
        if (str_tmp[0] == L'w')
        {
            wchar_t a[7];
            int i;
            
            for (i = 0; i < 6; ++i)
            {
                a[i] = str_tmp[i];
            }    
            a[6] = L'\0';
            
            if (!wcscmp(a, L"write "))
            {
                if (str_tmp[6] != L'\0')
                {
                    if (!writeToFile((str_tmp + 6), l)) 
                    {
                        clearLastRow();
                        wprintf(L"File was created and printed successfully");
                    }   
                }
                else
                {
                    clearLastRow();
                    wprintf(L"File name is empty");
                }
            }
            else
            {
                clearLastRow();
                wprintf(L"Incorrect command");
            }
        }

        /* subst in list */
        if (str_tmp[0] == L's')
        {
            wchar_t *a = NULL;
            /*int i;
    
            for (i = 0; i < 6; ++i)
            {
                a[i] = str_tmp[i];
            }    
            a[6] = '\0';*/
            if (wcslen(str_tmp) >= 6)
            {
                a = getStringFromTo(str_tmp, str_tmp + 5);
            }
            if ((a != NULL) && (!wcscmp(a, L"subst ")))
            {
                wchar_t *_str = NULL;
                wchar_t *__str = NULL;
                if (str_tmp[6] == L'/')
                {
                    if ((wcslen(str_tmp) > 7) && ((_str = wcschr(str_tmp + 7, L'/')) != NULL))
                    {
						wchar_t *middle_slash = _str;
						while (*middle_slash != L'\0')
						{
							if ((*middle_slash == L'/') && (*(middle_slash - 1) != L'\\'))
							{
								_str = middle_slash;
								break;
							}
							++middle_slash;
						}
                        if ((l_subst = getStringFromTo(str_tmp + 7, _str - 1)) != NULL) 
                        {
                            if ((wcslen(str_tmp) > (_str - str_tmp + 1)) && ((__str = wcschr(_str + 1, L'/')) != NULL))
                            {
                                if (__str - 1 - _str - 1 == -1)
                                {
                                    subst = 1;
                                }
								wchar_t *right_slash = __str;
								while (*right_slash != L'\0')
								{
									if ((*right_slash == L'/') && (*(right_slash - 1) != L'\\'))
									{
										__str = right_slash;
										break;
									}
									++right_slash;
								}

                                if ((r_subst = getStringFromTo(_str + 1, __str - 1)) != NULL)
                                {
                                    if (!substInList(l_subst, r_subst, l))
                                    {
                                        /*if (!wcslen(r_subst))
                                        {
                                            flg_for_delete_int_subst = 1;
                                        }
                                        else 
                                        {
                                            flg_for_delete_int_subst = 0;
                                        }*/
                                        free(l_subst);
                                        free(r_subst);
                                        free(a);
                                        if (!flg_for_delete_int_subst)
                                        {
                                            --(l->num_elements); 
                                            freeList(l);
                                            l = l_for_subst;
                                        }
                                        flg_for_delete_int_subst = 0;
                                        top = 1;
                                        left = 1;
                                        x = 1;
                                        y = 2;
                                        px = 1;
                                        py = 1;
                                        length = 0;
                                        p_x = 0;
                                        p_y = 0;
                                        temp = 0;
                                        i = 1;
                                        prev_pos_x = 1;
                                        prev_pos_y = 0;
                                        getWindowSize(&bot, &right);
                                        if (global_args_.n == 1)
                                        {
                                            temp = l->num_elements;
                                            length = getCount(temp);
                                            y += length;
                                            ++y;
                                            tmp = length + 2;
                                            right -= tmp;
                                        } 
                                        else 
                                        {
                                            --right;
                                        }
                                        /*--(l->num_elements);*/

                                        clearWindow();
                                        printText(l);
                                        if (global_args_.n)
                                        {
                                            wprintf(L"\E[1;%dH", length + 3);
                                        }
                                        else 
                                        {
                                            wprintf(L"\E[1;2H");
        
                                        }
                                        clearLastRow();
                                        wprintf(L"Subst has been done correctly");
                                    }         
                                    else 
                                    {
                                        free(a);
                                        free(l_subst);
                                        free(r_subst);
                                        clearLastRow();
                                        wprintf(L"Subst has not been done correctly");
                                    }
                                }
                                else 
                                {
                                    free(a);
                                    free(l_subst);
                                    clearLastRow();
                                    wprintf(L"Incorrect string for subst");
                                }
                                subst = 0;
                            }
                            else
                            {
                                free(a);
                                free(l_subst);
                                wprintf(L"Incorrect input");
                            }     
                        }
                        else 
                        {
                            free(a);
                            clearLastRow();
                            wprintf(L"Incorrect original string");
                        } 
                    }
                    else
                    {
                        free(a);
                        clearLastRow();
                        wprintf(L"Incorrect original string");
                    }
                }
                else
                {
                    free(a);
                    clearLastRow();
                    wprintf(L"Incorrect agrs for subst");
                }
            }
            else if (a != NULL)
            {
                free(a);
                clearLastRow();
                wprintf(L"Incorrect strng for subst");
            } 
        }

        free(str_tmp); 
        i = 1;
    }    

    free(str_srch);
    /* returning terminal to initial state */
    returnTerminal();    
    clearWindow();
    --(l->num_elements);
    freeList(l);
    return 0;
}

/*
 * Copyright (c) 2011 Rémi "Remram" Rampin <remirampin@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "libmenu.h"

#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static WINDOW *_libmenu_window = NULL;
static int _libmenu_invalid = 1;

struct _libmenu_menu {
    int win_w, win_h, win_x, win_y;
    int scroll_pos;
    char *title;
    size_t nb_items;
    menu_item_t *items_f;
    menu_item_t *items_l;
    int item;
    menu_t *previous;
};

struct _libmenu_menu_item {
    unsigned int type;
    char *caption;
    menu_item_t *next;
};

static menu_t *_libmenu_menus_l = NULL;

menu_t *menu_push(const char *title)
{
    menu_t *menu = malloc(sizeof(menu_t));

    /* Pas de menu en cours -- ncurses désactivée */
    if(_libmenu_menus_l == NULL)
    {
        _libmenu_menus_l = menu;
        menu->previous = NULL;

        /* Activation ! */
        initscr();
        start_color();
        cbreak();
        noecho();
        curs_set(0);
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
        init_pair(2, COLOR_BLACK, COLOR_WHITE);
        init_pair(3, COLOR_RED, COLOR_WHITE);

        /* Création d'une fenêtre avec la taille par défaut (tout l'écran) */
        menu->win_w = COLS;
        menu->win_h = LINES;
        menu->win_x = 0;
        menu->win_y = 0;
    }
    /* Déjà des menus -- ncurses activée */
    else
    {
        menu->previous = _libmenu_menus_l;
        _libmenu_menus_l = menu;

        /* Nouvelle fenêtre, même taille que la précédente */
        menu->win_w = menu->previous->win_w;
        menu->win_h = menu->previous->win_h;
        menu->win_x = menu->previous->win_x;
        menu->win_y = menu->previous->win_y;
    }

    menu->title = strdup(title);

    /* Pas d'entrée de menu */
    menu->item = -1;
    menu->nb_items = 0;
    menu->items_f = menu->items_l = NULL;
    menu->scroll_pos = 0;

    _libmenu_invalid = 1;

    return menu;
}

void menu_setwindow(menu_t *menu, int width, int height,
        int start_x, int start_y)
{
    if(menu->win_w != width || menu->win_h != height
     || menu->win_x != start_x || menu->win_y != start_y)
    {
        menu->win_w = width;
        menu->win_h = height;
        menu->win_x = start_x;
        menu->win_y = start_y;
        if(_libmenu_menus_l == menu && _libmenu_window)
        {
            /* On détruit la fenêtre ; elle sera recréée aux bonnes
             * dimensions. */
            delwin(_libmenu_window);
            _libmenu_window = NULL;
        }
    }
}

void menu_pop(menu_t *menu)
{
    assert(menu && menu == _libmenu_menus_l);

    /* Il reste encore des menus, tant mieux */
    if(_libmenu_menus_l->previous != NULL)
    {
        /* Si le menu précédent n'a pas la même taille, on supprime la fenêtre
         * qui sera recréée avec les bonnes dimensions */
        if(_libmenu_window
         && ( (menu->win_w != menu->previous->win_w)
           || (menu->win_h != menu->previous->win_h)
           || (menu->win_x != menu->previous->win_x)
           || (menu->win_y != menu->previous->win_y) ) )
        {
            delwin(_libmenu_window);
            _libmenu_window = NULL;
        }

        menu_t *tmp = _libmenu_menus_l;
        _libmenu_menus_l = tmp->previous;
        free(tmp);

        _libmenu_invalid = 1;
    }
    /* Il n'y a plus de menus -- désactivation de ncurses */
    else
    {
        /* Suppression de la fenêtre */
        if(_libmenu_window)
        {
            delwin(_libmenu_window);
            _libmenu_window = NULL;
        }

        free(_libmenu_menus_l);
        _libmenu_menus_l = NULL;

        /* Désactivation ! */
        erase();
        refresh();
        endwin();
        clrtoeol();
    }

    /* Supprime les éléments de menu */
    {
        menu_item_t *item = menu->items_f;
        while(item)
        {
            menu_item_t *tmp = item;
            item = item->next;
            if(tmp->caption)
                free(tmp->caption);
            free(tmp);
        }
    }
}

menu_item_t *menu_newitem(menu_t *menu, unsigned int type)
{
    menu_item_t *item = malloc(sizeof(menu_item_t));
    item->caption = NULL;
    item->type = type;
    item->next = NULL;

    if(menu->items_l == NULL)
    {
        menu->items_f = menu->items_l = item;
        menu->item = 0;
        menu->nb_items = 1;
    }
    else
    {
        menu->items_l->next = item;
        menu->items_l = menu->items_l->next;
        menu->nb_items++;
    }

    _libmenu_invalid = 1;

    return item;
}

void menu_setitem(menu_item_t *item, const char *format, ...)
{
    va_list args;
    int n = 16;
    char *msg = malloc(n);
    for(;;)
    {
        int s;
        va_start(args, format);
        s = vsnprintf(msg, n, format, args);
        va_end(args);
        if(s == -1 || s >= n)
        {
            n *= 2;
            free(msg);
            msg = malloc(n);
        }
        else
            break;
    }

    if(item->caption != NULL)
        free(item->caption);

    item->caption = msg;

    _libmenu_invalid = 1;
}

static void _libmenu_drawmenu()
{
    int i;
    int x, y;
    const menu_t *menu = _libmenu_menus_l;

    /* Scroll dans les éléments jusqu'à la position voulue */
    const int scroll = menu->scroll_pos;
    menu_item_t *item = menu->items_f;
    for(i = 0; i < scroll; i++)
        item = item->next;

    /* Efface la fenêtre */
    werase(_libmenu_window);

    /* Affiche le titre */
    mvwprintw(_libmenu_window,
            menu->win_y, menu->win_x + (menu->win_w - strlen(menu->title))/2,
            menu->title);

    /* Affiche le contour de la fenêtre */
    {
        WINDOW *border_subwin = derwin(
                _libmenu_window,
                menu->win_h-1, menu->win_w,
                menu->win_y+1, menu->win_x);
        box(border_subwin, 0, 0);
        delwin(border_subwin);
    }

    /* Affiche les éléments, en surlignant celui qui est sélectionné */
    x = menu->win_x+1;
    y = menu->win_y+2;
    for(i = scroll; i < menu->win_h-3 && item; i++)
    {
        if(menu->item == i)
            wattron(_libmenu_window, A_REVERSE);
        if(item->caption != NULL)
            mvwprintw(_libmenu_window, y, x, "%s", item->caption);
        else
            mvwprintw(_libmenu_window, y, x, "");
        if(menu->item == i)
            wattroff(_libmenu_window, A_REVERSE);
        item = item->next;
        y++;
    }
    wrefresh(_libmenu_window);
}

static menu_item_t *_libmenu_item_n(menu_item_t *items_f, int n)
{
    for(; n > 0; n--)
        items_f = items_f->next;
    return items_f;
}

/* Trouve un item sélectionnable, avant celui spécifié si possible
 * Si current_item est -1, retourne -1 */
static int _libmenu_selectable_item_before(menu_t *menu, int current_item)
{
    int pos = 0;
    menu_item_t *item = menu->items_f;
    int selected = -1;
    if(current_item == -1 || !item)
        return -1;
    while(item && pos < current_item)
    {
        if(item->type != MENU_ITEM_LABEL)
            selected = pos;
        item = item->next;
        pos++;
    }

    if(selected != -1)
        return selected;
    else
        return current_item;
}

/* Trouve un item sélectionnable, après celui spécifié si possible
 * Attention : current_item peut être -1 !*/
static int _libmenu_selectable_item_after(menu_t *menu, int current_item)
{
    int pos = current_item + 1;
    menu_item_t *item;
    if(current_item == -1)
        item = menu->items_f;
    else
        item = _libmenu_item_n(menu->items_f, current_item)->next;

    while(item)
    {
        if(item->type != MENU_ITEM_LABEL)
            return pos;
        item = item->next;
        pos++;
    }

    return current_item;
}

menu_item_t *menu_wait(menu_t *menu, int timeout)
{
    int c;
    menu_item_t *ret = NULL;
    assert(menu && menu == _libmenu_menus_l);

    /* Création de la fenêtre si nécessaire */
    if(!_libmenu_window)
    {
        _libmenu_window = newwin(menu->win_h, menu->win_w,
                menu->win_y, menu->win_x);
        keypad(_libmenu_window, TRUE);
        /* Va au premier élément sélectionnable */
        menu->item = _libmenu_selectable_item_after(menu, -1);
        _libmenu_invalid = 1;
    }

    if(_libmenu_invalid)
    {
        _libmenu_drawmenu();
        _libmenu_invalid = 0;
    }

    if(timeout >= 0)
        halfdelay(timeout/100);
    else
        cbreak();
    c = wgetch(_libmenu_window);
    switch(c)
    {
    case KEY_UP:
        menu->item = _libmenu_selectable_item_before(menu, menu->item);
        /* Scrolling */
        if(menu->item != -1 && menu->item < menu->scroll_pos)
            menu->scroll_pos = menu->item;
        _libmenu_drawmenu();
        break;
    case KEY_DOWN:
        menu->item = _libmenu_selectable_item_after(menu, menu->item);
        /* Scrolling */
        if(menu->item >= menu->scroll_pos + menu->win_h-2)
            menu->item = menu->scroll_pos + menu->win_h - 3;
        _libmenu_drawmenu();
        break;
    /* TODO : saut des items non-sélectionnables */
    case 10:
        {
            menu_item_t *i = _libmenu_item_n(menu->items_f, menu->item);
            if(i->type != MENU_ITEM_LABEL)
                return i;
        }
        break;
    case 'q':
    case 'Q':
        ret = MENU_QUIT;
        break;
    }

    nocbreak();
    return ret;
}

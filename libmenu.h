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

#ifndef _LIBMENU_H
#define _LIBMENU_H

/** Elément textuel (label immuable). */
#define MENU_ITEM_LABEL 0x1

/** Bouton (peut être activé). */
#define MENU_ITEM_BUTTON 0x2

/**
 * Pointeur opaque vers un menu.
 */
typedef struct _libmenu_menu menu_t;

/**
 * Pointeur opaque vers un élément d'un menu.
 */
typedef struct _libmenu_menu_item menu_item_t;

/**
 * Fonction créant un nouveau menu sur la pile de menus.
 */
menu_t *menu_push(const char *title);

/**
 * Fonction changeant la géométrie de la fenêtre associée à un menu.
 */
void menu_setwindow(menu_t *menu, int width, int height, int start_x, int start_y);

/**
 * Fonction supprimant le menu au sommet de la pile.
 */
void menu_pop(menu_t *menu);

/**
 * Fonction créant un nouvel élément dans un menu.
 */
menu_item_t *menu_newitem(menu_t *menu, unsigned int type);

/**
 * Fonction changeant le libellé d'un élément dans un menu.
 */
void menu_setitem(menu_item_t *item, const char *format, ...);

/**
 * Fonction attendant et gérant les actions sur un menu.
 *
 * @return L'entrée de menu qui a été choisie, ou NULL si rien
 * n'a été choisi dans le laps de temps imparti. La valeur retournée peut
 * aussi être la constante MENU_QUIT, indiquant qu'on a appuyé sur la touche de
 * retour.
 * @param timeout La durée maximale en ms avant le retour de la fonction
 * (négatif = infini).
 */
menu_item_t *menu_wait(menu_t *menu, int timeout);

/*
 * Ceci est moche. Ceci est vraiment moche. Mais il ne peut vraiment pas y
 * avoir un menu_item_t à cette adresse...
 */
/**
 * Constante retournée par menu_wait() pour indiquer que l'on a utilisé le
 * bouton de retour plutôt qu'une entrée du menu.
 */
#define MENU_QUIT ((menu_item_t*)0x01)

#endif

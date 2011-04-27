//#include "../card.h"
#include "libmenu.h"

#include <stdlib.h>

void menu_positionnement()
{
    menu_t *menu = menu_push("Position des roues codeuses et du GPS");
    menu_item_t *i_rc1 = menu_newitem(menu,
            MENU_ITEM_LABEL);
    menu_item_t *i_rc2 = menu_newitem(menu,
            MENU_ITEM_LABEL);
    menu_newitem(menu,
            MENU_ITEM_LABEL);
    menu_item_t *i_gps_x = menu_newitem(menu,
            MENU_ITEM_LABEL);
    menu_item_t *i_gps_y = menu_newitem(menu,
            MENU_ITEM_LABEL);
    menu_item_t *i_gps_th = menu_newitem(menu,
            MENU_ITEM_LABEL);
    menu_newitem(menu,
            MENU_ITEM_LABEL);
    menu_item_t *i_reset = menu_newitem(menu,
            MENU_ITEM_BUTTON);
    menu_setitem(i_reset, "Reset du GPS");
    menu_item_t *i_back = menu_newitem(menu,
            MENU_ITEM_BUTTON);
    menu_setitem(i_back, "Retour");

    menu_item_t *ret = NULL;
    do {
        int rc1, rc2, gps_x, gps_y, gps_th;
        /*es_fpga_read(5, &rc1);*/ rc1 = 4;
        /*es_fpga_read(6, &rc2);*/ rc2 = 9;
        menu_setitem(i_rc1, "Roue codeuse 1 : %5d", rc1);
        menu_setitem(i_rc2, "Roue codeuse 2 : %5d", rc2);

        /*es_fpga_read(2, &gps_x);*/ gps_x = 1012;
        /*es_fpga_read(3, &gps_y);*/ gps_y = 432;
        /*es_fpga_read(4, &gps_th);*/ gps_th = 70;
        menu_setitem(i_gps_x, "x     = %4d", gps_x);
        menu_setitem(i_gps_y, "y     = %4d", gps_y);
        menu_setitem(i_gps_th, "theta = %4d", gps_th);

        ret = menu_wait(menu, 500);

        if(ret == i_reset)
        {
            /*es_fpga_write(2, 0);
            es_fpga_write(3, 0);
            es_fpga_write(4, 0);*/
        }
    }
    while(ret != i_back && ret != MENU_QUIT);

    menu_pop(menu);
}

int main(void)
{
    menu_t *menu = menu_push("Tests et diagnostics de la carte FPGA");

    menu_item_t *i_pos = menu_newitem(menu, MENU_ITEM_BUTTON);
    menu_setitem(i_pos,
            "Lecture de la position des roues codeuses et du GPS");
    menu_item_t *i_comm = menu_newitem(menu, MENU_ITEM_BUTTON);
    menu_setitem(i_comm, "Commande directe des moteurs");
    menu_newitem(menu, MENU_ITEM_LABEL);
    menu_item_t *i_quit = menu_newitem(menu, MENU_ITEM_BUTTON);
    menu_setitem(i_quit, "Quitter");

    menu_item_t *ret = NULL;
    do {
        if(ret == i_pos)
            menu_positionnement();

        ret = menu_wait(menu, -1);
    }
    while(ret != i_quit && ret != MENU_QUIT);

    menu_pop(menu);

    return 0;
}

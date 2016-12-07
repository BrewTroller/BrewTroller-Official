#ifndef _MENU_H
#define _MENU_H

#include <Arduino.h>

#define MAX_MENU_ITEM_NAME_LEN 21

typedef struct {
  char name[MAX_MENU_ITEM_NAME_LEN+1];
  byte value;
} menuItem;


class menu
{
public:
    /* Constructor: pagesize (rows), maximum menu item count */
    menu(byte, byte);

    /* Frees menuItems memory */
    ~menu(void);

    /* Adds or updates a menu item (based on unique value) */
    void setItem(char[], byte);
    void setItem_P(const char *, byte);

    /* Appends text to an existing menu item */
    void appendItem(const char*, byte);
    void appendItem_P(const char *, byte);

    /* Set selected by specifying index */
    void setSelected(byte);

    /* Select by menu item value */
    void setSelectedByValue(byte);

    /* Get selected menu item index */
    byte getSelected(void);

    /* Update _topItem based on selection and pageSize */
    boolean refreshDisp(void);

    /* Get specified row's menu item text based on _topItem and _pageSize */
    void getVisibleRow(byte, char[]);

    /* Get menu item text for currently selected item */
    char* getSelectedRow(char[]);

    /* Get the value for the currently selected menu item */
    byte getValue(void);

    /* Get the cursor position based on current selection, _topItem and _pageSize */
    byte getCursor(void);

    /* Get total number of defined menu items */
    byte getItemCount(void);

    /* Get menu item index based on specified menu item value */
    byte getIndexByValue(byte);
private:
    byte 	_pageSize,
    _maxOpts,
    _itemCount,
    _selected,
    _topItem;

    menuItem *_menuItems;
};

#endif

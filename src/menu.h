/* 
  Farso: a simple GUI.
  Copyright (C) DNTeam <dnt@dnteam.org>
 
  This file is part of Farso.
 
  Farso is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Farso is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with Farso.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _farso_menu_h
#define _farso_menu_h

#include "widget.h"
#include "picture.h"
#include "label.h"
#include "grid.h"

#include <kobold/list.h>
#include <kobold/kstring.h>

namespace Farso
{

/*! Menu widget implementation.
 * \note The menu is created with .beginCreate() .insertItem() [.insertItem()]
 *       .endCreate() calls.
 * \note After created the menu itens are immutable (ie: could not be added or 
 *       removed). But they can be hidden or disabled anytime.
 * \note Menu will use its own WidgetRenderer, so it can be outside parent's */
class Menu : public Widget
{
   public:
      /*! Menu constructor
       * \param minWidth minimum width for menu (0 for no minimum size).
       * \param parent Pointer to menu's parent. */
      Menu(int minWidth = 0, Widget* parent = NULL);
      /*! Menu destructor */
      ~Menu();

      /*! A single menu item */
      class MenuItem : public Kobold::ListElement
      {
         public:
            /*! Constructor for normal items. */
            MenuItem(const Kobold::String& id, const Kobold::String& caption, 
                  const Kobold::String& rightText, const Kobold::String& icon, 
                  Widget* owner);

            /*! Constructor for separators */
            MenuItem();

            /*! Destructor */
            ~MenuItem();

            /*! Set current position of the item */
            void setPosition(int x, int y, int width);

            /*! \return current item X position */
            const int getX() const;

            /*! \return current item X position */
            const int getY() const;

            /*! \return the needed height for current item */
            const int getNeededHeight() const;

            /*! \return the needed width for current item */
            const int getNeededWidth() const;

            /*! Disable the item (but still visible) */
            void disable();
            /*! Enable the item */
            void enable();

            /*! \return if item is enabled or disabled */
            const bool isEnabled() const;

            /*! \return if item is of special separator type */
            const bool isSeparator() const;

            /*! Hide current menu item */
            void hide();
            /*! Display current menu item */
            void show();

            /*! Reset item's text.  
             * \note this won't change menu width, so be carefull to it be
             * enough to contain the new string, otherwise it will be
             * truncated. */
            void setCaption(const Kobold::String& str);
            /*! \return item's text */
            const Kobold::String getCaption() const;

            /*! \return if item is visible or not */
            const bool isVisible() const;

            /*! \return id */
            const Kobold::String& getId() const;

         private:
            /* Calculate item needed size */
            void calculateNeededSize(const Kobold::String& str, 
                  const Kobold::String& rightText, Farso::Picture* pic);

            bool enabled; /**< if enabled or disabled */
            bool visible; /**< if visible or hidden */

            bool separator; /**< if element is a separtor */

            Label* label;  /**< item's label */
            Label* rightLabel; /**< item's right text label */
            Picture* icon; /**< item's icon */
            Kobold::String id; /**< Identifier */

            Widget* owner; /**< pointer to the owner (a grid). */

            int width;   /**< Needed width */
            int height;  /**< Needed height */

            int pX;      /**< X coordinate on parent */ 
            int pY;      /**< Y coordinate on parent */
      };

      /*! Init the process of inserting menu items.
       * \note should only be called once per menu */
      void beginCreate();

      /*! Insert a new menu item.
       * \note should be called between beginCreate() endCreate() calls.
       * \param text item's caption 
       * \param rightText item's right aligned text (usually used for
       *        displaying shortcut alias).
       * \param icon path to icon's image. 
       * \param id optional string identifier. */
      MenuItem* insertItem(const Kobold::String& text, 
            const Kobold::String& rightText="", 
            const Kobold::String& icon="", const Kobold::String& id="");

      /*! Insert a separator item. A separator is just a decorative
       * item that couldn't be selected */
      MenuItem* insertSeparator();
      
      /*! Finish the process of inserting menu items. */
      void endCreate();

      /*! Open the menu at x,y absolute screen position */
      void open(int x, int y, Widget* caller = NULL);

      /*! Close the menu, if its opened, without generating any event */
      void close();

      /*! \return pointer to MenuItem of index */
      MenuItem* getItem(int index);

      /*! \return pointer to MenuItem with specific caption */
      MenuItem* getItem(const Kobold::String& caption);

      /*! \return pointer to current on-selection (or selected, depending on
       * current event state) item */
      const MenuItem* getCurrentItem();

      /* From Widget */
      const Rect& getBody();
      /*! \note should not be called for Menu */
      void setSize(int width, int height);

   protected:

      /*! Get item bellow position
       * \param rX x coordinate in menu coordinate system
       * \param rY y coordinate in menu coordinate system
       * \return item got (note that it must be enabled) */
      MenuItem* getItem(int rX, int rY);

      /* From Widget */
      void doDraw(const Rect& pBody);
      bool doTreat(bool leftButtonPressed, bool rightButtonPressed, 
            int mouseX, int mouseY, int mrX, int mrY);
      void doAfterChildTreat();

   private:
      MenuItem* current; /**< Current on selection item */
      Kobold::List items; /**< Current items list */
      Grid* grid; /**< Grid for selecting items on menu */
      Rect body; /**< Menu's body */
      bool creating; /**< While creating flag */
      bool pressStarted; /**< if press started or no */
      bool hasSeparator; /**< If menu has a separator element */

      int curWidth;  /**< current width of the menu */
      int curHeight; /**< current height of the menu */

      int minWidth; /**< current minimum menu width to accept */

      Widget* caller; /**< Pointer to the current Widget that opened the menu,
                           if any */
      WidgetRenderer* menuRenderer; /**< The renderer used for menu */
};

}

#endif


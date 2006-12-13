/**
 * Copyright (c) 2005 PCMan <pcman.tw@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef VIEW_H
#define VIEW_H

#ifdef __GNUG__
  #pragma interface "view.h"
#endif

#include "widget.h"

/**
@author PCMan
*/
class CView : public CWidget
{
public:
    CView();

    virtual void OnPaint(GdkEventExpose *evt) = 0;
    virtual void OnSize(GdkEventConfigure* evt);
    virtual void OnSetFocus(GdkEventFocus *evt);
    virtual void OnKillFocus(GdkEventFocus *evt);

    void SetContextMenu(GtkWidget* menu)
    {	m_ContextMenu = (GtkMenu*)menu;	}
protected:
    GtkMenu* m_ContextMenu;;
};

#endif

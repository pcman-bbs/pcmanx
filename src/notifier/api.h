#ifndef NOTIFIER_API_H
#define NOTIFIER_API_H

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

void popup_notifier_init(GdkPixbuf *icon);

GtkWidget* popup_notifier_notify(const gchar *caption, const gchar *text, GtkWidget* parent, GCallback click_cb, gpointer click_cb_data);

G_END_DECLS

#endif


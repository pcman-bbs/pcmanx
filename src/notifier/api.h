#ifndef NOTIFIER_API_H
#define NOTIFIER_API_H

#include <glib.h>
#include <gdk/gdk.h>

G_BEGIN_DECLS

void popup_notifier_init(GdkPixbuf *icon);

void popup_notifier_notify(const gchar *caption, const gchar *text);

G_END_DECLS

#endif


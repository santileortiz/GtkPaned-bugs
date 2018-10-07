//gcc size_allocation_warning.c -o size_allocation_warning `pkg-config --cflags --libs gtk+-3.0`
/*
 * Copiright (C) 2018 Santiago León O.
 */
#include <gtk/gtk.h>

gboolean delete_callback (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    gtk_main_quit ();
    return FALSE;
}

void add_custom_css (GtkWidget *widget, gchar *css_data)
{
    GtkStyleContext *style_context = gtk_widget_get_style_context (widget);
    GtkCssProvider *css_provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_data (css_provider, css_data, -1, NULL);
    gtk_style_context_add_provider (style_context,
                                    GTK_STYLE_PROVIDER(css_provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

// This fix replicates the size properties along the other orientation to work
// around GtkPaned's internal bug.
GtkWidget *fix_gtk_paned_new (GtkOrientation orientation)
{
    GtkWidget *res = gtk_paned_new (orientation);
    add_custom_css (res, "paned > separator {"
                    "    margin-top: -7px;"
                    "    min-height: 8px;"
                    "}");
    return res;
}

int main (int argc, char *argv[])
{
    GtkWidget *window;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_resize (GTK_WINDOW(window), 900, 600);
    g_signal_connect (G_OBJECT(window), "delete-event", G_CALLBACK (delete_callback), NULL);

    GtkWidget *list = gtk_list_box_new ();
    GtkWidget *row = gtk_label_new ("test label");
    gtk_container_add (GTK_CONTAINER(list), row);

    GtkWidget *button = gtk_button_new_with_label ("A TEST BUTTON");

    GtkWidget *sidebar = gtk_grid_new ();
    gtk_grid_attach (GTK_GRID(sidebar), list, 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID(sidebar), button, 0, 1, 1, 1);

    GtkWidget *draw_area = gtk_drawing_area_new ();

#if 1
    GtkWidget *paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
#else
    GtkWidget *paned = fix_gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
#endif

    gtk_paned_pack1 (GTK_PANED(paned), sidebar, FALSE, FALSE);
    gtk_paned_pack2 (GTK_PANED(paned), draw_area, TRUE, TRUE);
    gtk_container_add(GTK_CONTAINER(window), paned);

    gtk_widget_show_all(window);
    gtk_main();
}

//gcc useless_set_pos_percentage.c -o useless_set_pos_percentage `pkg-config --cflags --libs gtk+-3.0`
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

void g_object_set_property_bool (GObject *object, const char *property_name, gboolean value)
{
    GValue val = G_VALUE_INIT;
    g_value_init (&val, G_TYPE_BOOLEAN);
    g_value_set_boolean (&val, value);
    g_object_set_property (object, property_name, &val);
}

GtkWidget *fix_gtk_paned_new (GtkOrientation orientation)
{
    GtkWidget *res = gtk_paned_new (orientation);
    add_custom_css (res, "paned > separator {"
                    "    margin-right: 0;"
                    "    min-width: 2px;"
                    "    min-height: 2px;"
                    "}");
    return res;
}

// A simpler approach to this is: After creating the GtkPaned get the size of
// the widget by calling gtk_widget_get_allocated_height() or
// gtk_widget_get_allocated_width() and cal gtk_paned_set_position(). This does
// not work, because at creation time things have not been allocated and these
// fuctions return 1.
struct paned_set_percent_closure_t {
    int percent;
    gulong handler_id;
};

void _paned_set_percent_position_callback (GtkWidget *widget,
                                           GdkRectangle *allocation,
                                           gpointer user_data)
{
    struct paned_set_percent_closure_t *clsr =
        (struct paned_set_percent_closure_t*)user_data;
    GtkPaned *paned = GTK_PANED(widget);

    int size;
    switch (gtk_orientable_get_orientation (GTK_ORIENTABLE(paned))) {
        case GTK_ORIENTATION_HORIZONTAL:
            size = allocation->width;
            break;
        default: // GTK_ORIENTATION_VERTICAL:
            size = allocation->height;
            break;
    }

    int child_1_size = (clsr->percent*size)/100;
    gtk_paned_set_position (paned, child_1_size);
    printf ("Tried to set position to: %d\n", child_1_size);

    // These do nothing:
    gtk_widget_queue_resize (widget);
    gtk_widget_queue_draw (widget);

    g_signal_handler_disconnect (widget, clsr->handler_id);
    free (clsr);
}

void paned_set_percent_position (GtkWidget *paned, int percent)
{
    g_assert (GTK_IS_PANED(paned));
    struct paned_set_percent_closure_t *clsr =
        malloc(sizeof(struct paned_set_percent_closure_t));
    clsr->percent = percent;

    clsr->handler_id = g_signal_connect (G_OBJECT(paned), "size-allocate",
                                         G_CALLBACK (_paned_set_percent_position_callback), clsr);
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

    GtkWidget *top_pane = gtk_image_new_from_file ("./test_image.png");
    GtkWidget *bottom_pane = gtk_image_new_from_file ("./test_image.png");

    GtkWidget *right_pane = fix_gtk_paned_new (GTK_ORIENTATION_VERTICAL);
#if 1
    paned_set_percent_position (right_pane, 66);
#else
    gtk_paned_set_position (GTK_PANED(right_pane), 396);
#endif
    gtk_paned_pack1 (GTK_PANED(right_pane), top_pane, FALSE, FALSE);
    gtk_paned_pack2 (GTK_PANED(right_pane), bottom_pane, TRUE, TRUE);

    GtkWidget *paned = fix_gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_pack1 (GTK_PANED(paned), sidebar, FALSE, FALSE);
    gtk_paned_pack2 (GTK_PANED(paned), right_pane, TRUE, TRUE);
    gtk_container_add(GTK_CONTAINER(window), paned);

    gtk_widget_show_all(window);
    gtk_main();
}

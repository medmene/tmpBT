#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

static void on_destroy (GtkWidget * widget, gpointer data){
gtk_main_quit();
}

int main(int argc, char *argv[] ){
GtkWidget *window;
gtk_init (&argc, &argv);
window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (on_destroy), NULL);
gtk_widget_show (window);
gtk_main ();
return 0;
}

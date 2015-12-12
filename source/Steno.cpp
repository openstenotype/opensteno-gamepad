#include <gtk/gtk.h>

void on_button_clicked (GtkButton *button, gpointer data)
{
  g_print ("Button '%s' clicked!\n", gtk_button_get_label (button));
  gtk_main_quit ();  
}

int main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *button;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "OpenSteno");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  button = gtk_button_new_with_label ("Hello OpenSteno");
  gtk_container_add (GTK_CONTAINER (window), button);

  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (button, "clicked", G_CALLBACK (on_button_clicked), NULL);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}

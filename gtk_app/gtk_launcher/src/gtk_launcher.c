#include <gtk/gtk.h>
#include <stdio.h> // Include stdio.h for sprintf

// Structure to pass multiple widgets to the callback function
typedef struct {
    GtkWidget *label;
    int click_count;
} AppWidgets;

// The callback function for the "clicked" signal of the button
static void on_button_clicked(GtkWidget *button, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    widgets->click_count++;
    
    // Allocate a buffer large enough for the message
    char buffer[50]; 
    sprintf(buffer, "Button clicked %d times!", widgets->click_count);
    
    // Update the label's text
    gtk_label_set_label(GTK_LABEL(widgets->label), buffer);
}

// The activate function, called when the application starts
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *box;
    
    // We need to use a pointer for the widgets structure so it persists
    // beyond the scope of this function and can be accessed by the callback.
    AppWidgets *widgets = g_new(AppWidgets, 1);

    // Create a new window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GTK4 Button Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    // Create a vertical box to arrange widgets
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);
    
    // Set the box as the child of the window
    gtk_window_set_child(GTK_WINDOW(window), box);

    // Create a label
    label = gtk_label_new("Click the button below!");
    
    // Create a button with a label
    button = gtk_button_new_with_label("Click Me");

    // Connect the "clicked" signal of the button to the callback function
    // We pass the pointer to the widgets structure as user data
    widgets->label = label;
    widgets->click_count = 0;
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), widgets);

    // Add label and button to the box
    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), button);

    // Show all widgets
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    // Create a new GtkApplication
    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    
    // Connect the "activate" signal to the activate function
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    // Run the application
    status = g_application_run(G_APPLICATION(app), argc, argv);
    
    // Free the application object
    g_object_unref(app);
    
    return status;
}


// app_launcher_gtk4.c
#include <gtk/gtk.h>
#include <stdlib.h>

// Helper function to run external commands
static void run_command(const char *command) {
    int ret = system(command);
    if (ret == -1) {
        g_printerr("Failed to execute command: %s\n", command);
    }
}

// Callback: Launch the Todo List app
static void launch_todo_app(GtkWidget *widget, gpointer data) {
    run_command("./gtk_todo &");  // Assuming your todo app is in the same folder
}

// Callback: Launch the default browser
static void launch_browser(GtkWidget *widget, gpointer data) {
    run_command("xdg-open https://www.google.com &");
}

// Callback: Launch text editor (e.g., gedit)
static void launch_editor(GtkWidget *widget, gpointer data) {
    run_command("gedit &");
}

// Callback: Launch terminal
static void launch_terminal(GtkWidget *widget, gpointer data) {
    run_command("gnome-terminal &");
}

// -------------------------------------------------------------
// GTK App Activate
// -------------------------------------------------------------
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *todo_button;
    GtkWidget *browser_button;
    GtkWidget *editor_button;
    GtkWidget *terminal_button;

    // Create main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "App Launcher");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 260);
    gtk_window_set_margin_top(GTK_WINDOW(window), 15);
    gtk_window_set_margin_bottom(GTK_WINDOW(window), 15);
    gtk_window_set_margin_start(GTK_WINDOW(window), 15);
    gtk_window_set_margin_end(GTK_WINDOW(window), 15);

    // Layout
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    // Create buttons
    todo_button     = gtk_button_new_with_label("📝 Open Todo List");
    browser_button  = gtk_button_new_with_label("🌐 Open Browser");
    editor_button   = gtk_button_new_with_label("🗒️ Open Text Editor");
    terminal_button = gtk_button_new_with_label("🖥️ Open Terminal");

    // Connect signals
    g_signal_connect(todo_button, "clicked", G_CALLBACK(launch_todo_app), NULL);
    g_signal_connect(browser_button, "clicked", G_CALLBACK(launch_browser), NULL);
    g_signal_connect(editor_button, "clicked", G_CALLBACK(launch_editor), NULL);
    g_signal_connect(terminal_button, "clicked", G_CALLBACK(launch_terminal), NULL);

    // Add buttons to the layout
    gtk_box_append(GTK_BOX(vbox), todo_button);
    gtk_box_append(GTK_BOX(vbox), browser_button);
    gtk_box_append(GTK_BOX(vbox), editor_button);
    gtk_box_append(GTK_BOX(vbox), terminal_button);

    // Make all buttons expand evenly
    gtk_widget_set_hexpand(todo_button, TRUE);
    gtk_widget_set_hexpand(browser_button, TRUE);
    gtk_widget_set_hexpand(editor_button, TRUE);
    gtk_widget_set_hexpand(terminal_button, TRUE);

    gtk_widget_set_vexpand(todo_button, TRUE);
    gtk_widget_set_vexpand(browser_button, TRUE);
    gtk_widget_set_vexpand(editor_button, TRUE);
    gtk_widget_set_vexpand(terminal_button, TRUE);

    // Show the window
    gtk_window_present(GTK_WINDOW(window));
}

// -------------------------------------------------------------
// Main
// -------------------------------------------------------------
int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.launcher", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}


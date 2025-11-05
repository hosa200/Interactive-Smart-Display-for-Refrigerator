// todo_gtk4.c
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *task_entry;
GtkWidget *task_list_box;
const char *TASK_FILE = "tasks.txt";  // File to store tasks

// Forward declarations
static void save_tasks_to_file(void);
static void load_tasks_from_file(void);
static void add_task_from_data(const char *task_text, gboolean completed);

// -------------------------------------------------------------
// Delete a task row
// -------------------------------------------------------------
static void
delete_task_row(GtkWidget *widget, gpointer data)
{
    GtkWidget *row = GTK_WIDGET(data);
    gtk_widget_destroy(row);
}

// -------------------------------------------------------------
// Toggle checkbox (mark task completed)
// -------------------------------------------------------------
static void
toggle_task_status(GtkCheckButton *check_button, gpointer data)
{
    GtkWidget *label = GTK_WIDGET(data);
    gboolean is_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button));

    if (is_active) {
        gtk_style_context_add_class(gtk_widget_get_style_context(label), "completed");
    } else {
        gtk_style_context_remove_class(gtk_widget_get_style_context(label), "completed");
    }
}

// -------------------------------------------------------------
// Add a new task (from entry field)
// -------------------------------------------------------------
static void
add_task(GtkWidget *widget, gpointer data)
{
    const char *task_text = gtk_entry_get_text(GTK_ENTRY(task_entry));

    if (task_text != NULL && strlen(task_text) > 0) {
        add_task_from_data(task_text, FALSE);
        gtk_entry_set_text(GTK_ENTRY(task_entry), "");  // Clear entry
        save_tasks_to_file();  // Save immediately after adding
    }
}

// -------------------------------------------------------------
// Create a new task row (used for both add and load)
// -------------------------------------------------------------
static void
add_task_from_data(const char *task_text, gboolean completed)
{
    GtkWidget *row;
    GtkWidget *hbox;
    GtkWidget *label;
    GtkWidget *check_button;
    GtkWidget *delete_btn;

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    // Checkbox
    check_button = gtk_check_button_new();
    gtk_box_append(GTK_BOX(hbox), check_button);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), completed);

    // Label
    label = gtk_label_new(task_text);
    // Align left
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    // Allow label to expand in box
    gtk_widget_set_hexpand(label, TRUE);
    gtk_box_append(GTK_BOX(hbox), label);

    // Apply completed style if needed
    if (completed)
        gtk_style_context_add_class(gtk_widget_get_style_context(label), "completed");

    // Delete button (symbolic icon)
    delete_btn = gtk_button_new_from_icon_name("list-remove-symbolic");
    gtk_box_append(GTK_BOX(hbox), delete_btn);

    // Row
    row = gtk_list_box_row_new();
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(row), FALSE);
    gtk_widget_set_margin_top(row, 4);
    gtk_widget_set_margin_bottom(row, 4);
    gtk_widget_set_margin_start(hbox, 4);
    gtk_widget_set_margin_end(hbox, 4);

    // Set the hbox as the child of the row
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), hbox);

    // Signals
    g_signal_connect(check_button, "toggled", G_CALLBACK(toggle_task_status), label);
    g_signal_connect(delete_btn, "clicked", G_CALLBACK(delete_task_row), row);

    // Append to list box
    gtk_list_box_append(GTK_LIST_BOX(task_list_box), row);
}

// -------------------------------------------------------------
// Save all tasks to file
// -------------------------------------------------------------
static void
save_tasks_to_file(void)
{
    FILE *fp = fopen(TASK_FILE, "w");
    if (!fp) return;

    // Iterate over rows in list box
    for (GtkWidget *row = gtk_widget_get_first_child(task_list_box);
         row != NULL;
         row = gtk_widget_get_next_sibling(row)) {

        // Each row's child is the hbox we created
        GtkWidget *hbox = gtk_widget_get_first_child(row);
        if (!hbox) continue;

        GtkWidget *check_button = gtk_widget_get_first_child(hbox);
        if (!check_button) continue;

        GtkWidget *label = gtk_widget_get_next_sibling(check_button);
        if (!label) continue;

        gboolean completed = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button));
        const char *text = gtk_label_get_text(GTK_LABEL(label));
        if (!text) text = "";

        // Write: completed|text\n
        fprintf(fp, "%d|%s\n", completed ? 1 : 0, text);
    }

    fclose(fp);
    g_print("Tasks saved to %s\n", TASK_FILE);
}

// -------------------------------------------------------------
// Load tasks from file (on startup)
// -------------------------------------------------------------
static void
load_tasks_from_file(void)
{
    FILE *fp = fopen(TASK_FILE, "r");
    if (!fp) return;

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = 0;  // Strip newline
        char *sep = strchr(line, '|');
        if (!sep) continue;

        *sep = '\0';
        gboolean completed = atoi(line) ? TRUE : FALSE;
        const char *task_text = sep + 1;

        add_task_from_data(task_text, completed);
    }

    fclose(fp);
    g_print("Tasks loaded from %s\n", TASK_FILE);
}

// -------------------------------------------------------------
// Auto-save every 60 seconds
// -------------------------------------------------------------
static gboolean
autosave_callback(gpointer data)
{
    save_tasks_to_file();
    return G_SOURCE_CONTINUE;  // Keep repeating
}

// -------------------------------------------------------------
// Apply CSS for completed tasks
// -------------------------------------------------------------
static void
apply_css(void)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css_data = "label.completed { text-decoration-line: line-through; opacity: 0.6; }";

    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
}

// -------------------------------------------------------------
// GTK App Activate
// -------------------------------------------------------------
static void
activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox_top;
    GtkWidget *add_button;
    GtkWidget *scroll_window;

    apply_css();

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Persistent Todo List (GTK4)");
    gtk_window_set_default_size(GTK_WINDOW(window), 420, 480);
    gtk_window_set_margin_start(GTK_WINDOW(window), 10);
    gtk_window_set_margin_end(GTK_WINDOW(window), 10);
    gtk_window_set_margin_top(GTK_WINDOW(window), 10);
    gtk_window_set_margin_bottom(GTK_WINDOW(window), 10);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    // Top row: entry + add button
    hbox_top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_append(GTK_BOX(vbox), hbox_top);

    task_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(task_entry), "Enter a new task");
    gtk_box_append(GTK_BOX(hbox_top), task_entry);

    add_button = gtk_button_new_with_label("Add Task");
    gtk_box_append(GTK_BOX(hbox_top), add_button);

    g_signal_connect(add_button, "clicked", G_CALLBACK(add_task), NULL);

    // Scrolled list
    scroll_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_append(GTK_BOX(vbox), scroll_window);

    task_list_box = gtk_list_box_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_window), task_list_box);

    // Load saved tasks on startup
    load_tasks_from_file();

    // Set up auto-save every 60 seconds
    g_timeout_add_seconds(60, autosave_callback, NULL);

    // Show the window (children are shown automatically in GTK4)
    gtk_window_present(GTK_WINDOW(window));
}

// -------------------------------------------------------------
// Main
// -------------------------------------------------------------
int
main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.persistent_todolist", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}


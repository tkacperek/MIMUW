/** @defgroup gtk-editor Moduł gtk-editor
    Prosty edytor graficzny.
 */
/**
  @file
  Plik z "pluginami".
  @ingroup gtk-editor
  @author Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
  @date 2015-06-18
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "word_list.h"
#include "dictionary.h"

/// Domyślna nazwa słownika.
#define DEFAULT_DICT_NAME "default"
/// Nazwa tagu do kolorowania błędów.
#define TAG_NAME "not_found"

/// Aktualnie używany słownik.
static
struct dictionary *dict = NULL;
/// Nazwa aktualnie otwartego języka.
static
char *dict_name = NULL;
/// Informacja o zmianach w słowniku.
static
int dict_changed = 0;

/** @name Funkcje pomocnicze
  @{
 */

/**
  Zamienia słowo w utf8 na wchar_t.
  @param[in] in Zamieniane słowo.
  @return Zamienione słowo. Należy zwolnić pamięć.
 */
wchar_t *convert_to_wchar(const char *in)
{
    return (wchar_t *)g_utf8_to_ucs4_fast(in, -1, NULL);
}

/**
  Wyświetla komunikat z podaną wiadomością.
  @param[in] msg Treść komunikatu.
  @param[in] err 1 jeżeli to komunikat o błędzie, 0 w p.p.
 */
static
void show_msg(const char *msg, int err)
{
    GtkWidget *dialog = 
        gtk_message_dialog_new(NULL, 0, 
                ((err == 1) ? GTK_MESSAGE_ERROR : GTK_MESSAGE_INFO),
                GTK_BUTTONS_OK, "%s", msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/**
  Zlicza słowa w wektorze argz.
  @param[in] argz Wektor.
  @param[in] len Długość wektora.
  @return Liczba słów.
 */
static
size_t argz_count(const char *argz, size_t len)
{
  size_t count = 0;
  while (len > 0)
    {
      size_t part_len = strlen(argz);
      argz += part_len + 1;
      len -= part_len + 1;
      count++;
    }
  return count;
}

/**
  Konwertuje argz do argv.
  @param[in] argz Wektor argz.
  @param[in] len Długość argz.
  @param[out] argv Wektor argv.
 */
void argz_extract(const char *argz, size_t len, char **argv)
{
  while (len > 0)
    {
      size_t part_len = strlen (argz);
      *argv++ = (char *) argz;
      argz += part_len + 1;
      len -= part_len + 1;
    }
}

/**
  Zapisuje (lub nie) zmiany w aktualnym słowniku.
  @return 1 gdy się udało lub można porzucić zmiany, 0 w p.p.
 */
int save_lang()
{
    int ret = 1;
    if (dict_changed != 0)
        if (dictionary_save_lang(dict, dict_name) < 0)
        {
            GtkWidget *dialogx, *vboxx, *labelx;
            dialogx = 
                gtk_dialog_new_with_buttons("Error", NULL, 0, 
                                                 GTK_STOCK_OK,
                                                 GTK_RESPONSE_ACCEPT,
                                                 GTK_STOCK_CANCEL,
                                                 GTK_RESPONSE_REJECT,
                                                 NULL);
            vboxx = gtk_dialog_get_content_area(GTK_DIALOG(dialogx));
            labelx = gtk_label_new(
                    "Couldn't save changes in current dictionary. "
                    "Continue? (changes will be lost)");
            gtk_widget_show(labelx);
            gtk_box_pack_start(GTK_BOX(vboxx), labelx, FALSE, FALSE, 1);
            if (gtk_dialog_run(GTK_DIALOG(dialogx)) == GTK_RESPONSE_REJECT)
                ret = 0;
            gtk_widget_destroy(dialogx);
        }
    return ret;
}

/**@}*/

/** @name Funkcje główne 
  @{
 */

/// Pokazuje informacje o programie.
void show_about()
{
    GtkWidget *dialog = gtk_about_dialog_new();

    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Text Editor");
    //gtk_window_set_title(GTK_WINDOW(dialog), "About Text Editor");
  
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
        "Text Editor for IPP exercises\n");
    
    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
}

/// Pokazuje informacje o korzystaniu z programu.
void show_help(void)
{
    GtkWidget *help_window;
    GtkWidget *label;
    char help[5000];

    help_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (help_window), "Help - Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(help_window), 300, 300);
 
    strcpy(help,
        "\nAby podłączyć usługę spell-checkera do programu trzeba:\n\n"
        "Dołączyć ją do menu 'Spell' w menubar.\n\n"
        "Pobrać zawartość bufora tekstu z edytora: całą lub fragment,\n"
        "  zapamiętując pozycję.\n\n");
    strcat(help, "\0");

    label = gtk_label_new(help);
    
    gtk_container_add(GTK_CONTAINER(help_window), label); 

    gtk_widget_show_all(help_window);
}

/**
  Obsługuje wybór słownika.
  @param[in] item Element wołający funkcję.
  @param[in] data Dane przekazywane przez item.
 */
static
void change_lang(GtkMenuItem *item, gpointer data)
{
    if (dict_name == NULL)
    {
        dict_name = calloc(5, sizeof(char));
        strcpy(dict_name, "none");
    }
    char *argz;
    size_t len;
    if (dictionary_lang_list(&argz, &len))
        show_msg("Failed to read language list", 1);
    else if (len == 0)
    {
        // Nie ma słowników, więc tworzy pusty.
        struct dictionary *ddict = dictionary_new();
        if (dictionary_save_lang(ddict, DEFAULT_DICT_NAME))
        {
            show_msg("Failed to create default dictionary", 1);
            dictionary_done(ddict);
        }
        else
        {
            dictionary_done(ddict);
            change_lang(item, data);
        }
    }
    else
    { 
        // Wybór słowników z listy.
        size_t n = argz_count(argz, len);
        char **argv = calloc(n, sizeof(char *));
        argz_extract(argz, len, argv);

        GtkWidget *dialog, *vbox, *label1, *label2, *combo;

        dialog = gtk_dialog_new_with_buttons("Change dictionary", NULL, 0, 
                                             GTK_STOCK_OK,
                                             GTK_RESPONSE_ACCEPT,
                                             GTK_STOCK_CANCEL,
                                             GTK_RESPONSE_REJECT,
                                             NULL);

        vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        char msg[20 + strlen(dict_name) + 1];
        strcpy(msg, "Current dictionary: ");
        strcat(msg, dict_name);
        label1 = gtk_label_new(msg);
        label2 = gtk_label_new("Available dictionaries:");
        gtk_widget_show(label1);
        gtk_widget_show(label2);
        gtk_box_pack_start(GTK_BOX(vbox), label1, FALSE, FALSE, 1);
        gtk_box_pack_start(GTK_BOX(vbox), label2, FALSE, FALSE, 1);

        combo = gtk_combo_box_text_new();
        for (int i = 0; i < n; ++i)
        {
            char *uword = g_ucs4_to_utf8((gunichar *)argv[i], -1, NULL, NULL, NULL);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), argv[i]);
            g_free(uword);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
        gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 1);
        gtk_widget_show(combo);

        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
        {
            struct dictionary *new_dict;
            char *pick=
                gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
            if (!(new_dict = dictionary_load_lang(pick)))
                show_msg("Failed to load dictionary", 1);
            else if (strcmp(dict_name, pick) != 0)
                if (save_lang())
                {
                    if (dict != NULL)
                        dictionary_done(dict);
                    dict = new_dict;

                    free(dict_name);
                    dict_name = calloc(strlen(pick) + 1, sizeof(char));
                    strcpy(dict_name, pick);

                    dict_changed = 0;
                    show_msg("Dictionary loaded successfully", 0);
                }
            g_free(pick);
        }
        gtk_widget_destroy(dialog);
        free(argv);
        free(argz);
    }
}

/**
  Sprawdzanie poprawności jednego słowa.
  @param[in] item Element wołający funkcję.
  @param[in] data Dane przekazywane przez item.
 */
static
void check_word(GtkMenuItem *item, gpointer data)
{
    if (dict == NULL)
        change_lang(NULL, NULL);
    if (dict == NULL)
    {
        show_msg("You have to choose a dictionary to use the spellcheck", 1);
        return;
    }
    GtkWidget *dialog;
    GtkTextIter start, end;
    char *word;
    gunichar *wword;
 
    // Znajdujemy pozycję kursora
    gtk_text_buffer_get_iter_at_mark(editor_buf, &start,
                                     gtk_text_buffer_get_insert(editor_buf));

    // Jeśli nie wewnątrz słowa, kończymy
    if (!gtk_text_iter_inside_word(&start))
    {
        dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_OK,
                                        "Place the cursor in the middle of a word");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Znajdujemy początek i koniec słowa, a potem samo słowo
    end = start;
    gtk_text_iter_backward_word_start(&start);
    gtk_text_iter_forward_word_end(&end);
    word = gtk_text_iter_get_text(&start, &end);

    // Zamieniamy na wide char (no prawie)
    wword = g_utf8_to_ucs4_fast(word, -1, NULL);

    // Sprawdzamy
    if (dictionary_find(dict, (wchar_t *)wword))
    {
        dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                        "Correct");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    else
    {
        // Czas korekty
        GtkWidget *vbox, *label, *combo;
        struct word_list hints;
        int i;

        dictionary_hints(dict, (wchar_t *)wword, &hints);
        const wchar_t * const *words = word_list_get(&hints);
        dialog = gtk_dialog_new_with_buttons("Add / replace", NULL, 0,
                                             GTK_STOCK_OK,
                                             GTK_RESPONSE_ACCEPT,
                                             GTK_STOCK_CANCEL,
                                             GTK_RESPONSE_REJECT,
                                             NULL);
        vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        label = gtk_label_new("Not found. " 
                              "Replace with a similar word " 
                              "or add to dictionary:");
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 1);

        combo = gtk_combo_box_text_new();
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "<add to dictionary>");
        for (i = 0; i < word_list_size(&hints); i++)
        {
            // Combo box lubi mieć Gtk
            char *uword = g_ucs4_to_utf8((gunichar *)words[i], -1, NULL, NULL, NULL);

            // Dodajemy kolejny element
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), uword);
            g_free(uword);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
        gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 1);
        gtk_widget_show(combo);

        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
        {
            char *pick =
                gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
            if (pick[0] == '<')
            {
                // Dodaje słowo do słownika.
                if (!dictionary_insert(dict, (wchar_t *)wword))
                    show_msg("Couldn't add word. Try again", 1);
                else
                    dict_changed = 1;
            }
            else
            {
                // Usuwamy stare
                gtk_text_buffer_delete(editor_buf, &start, &end);
                // Wstawiamy nowe
                gtk_text_buffer_insert(editor_buf, &start, pick, -1);
                g_free(pick);
            }
        }
        gtk_widget_destroy(dialog);
    }
    g_free(word);
    g_free(wword);
}

/**
  Sprawdzanie poprawności całego dokumentu i kolorowanie błędów.
  @param[in] item Element wołający funkcję.
  @param[in] data Dane przekazywane przez item.
 */
static
void check_document(GtkMenuItem *item, gpointer data)
{
    if (dict == NULL)
        change_lang(NULL, NULL);
    if (dict == NULL)
    {
        show_msg("You have to choose a dictionary to use the spellcheck", 1);
        return;
    }
    GtkTextIter start, end, prev;
    if (!gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(editor_buf),
                                   TAG_NAME))
        gtk_text_buffer_create_tag(editor_buf, TAG_NAME,
                                "foreground", "red",
                                "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_get_bounds(editor_buf, &start, &end);
    gtk_text_buffer_remove_tag_by_name (editor_buf,
                                        TAG_NAME,
                                        &start,
                                        &end);
    end = start;

    gtk_text_iter_forward_word_end(&end);
    prev = end;
    gtk_text_iter_backward_char(&prev);
    // Pęla po wszystkich słowach.
    while (gtk_text_iter_ends_word(&end) 
           && gtk_text_iter_compare(&prev, &end) != 0)
    {
        char *word;
        wchar_t *wword;
        start = end;
        gtk_text_iter_backward_word_start(&start);
        word = gtk_text_iter_get_text(&start, &end);
        wword = convert_to_wchar(word);
        if (! dictionary_find(dict, wword))
            gtk_text_buffer_apply_tag_by_name(editor_buf, TAG_NAME, &start, &end);
        g_free(wword);
        g_free(word);
        prev = end;
        gtk_text_iter_forward_word_end(&end);
    }
}

/**
  Zapisuje słownik przy zamykaniu okna.
  @return 0 gdy się udało lub można porzucić zmiany, 1 w p.p.
 */
gboolean save_on_delete_window(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    int ret = save_lang();
    if (ret)
    {
        if (dict != NULL)
            dictionary_done(dict);
        if (dict_name != NULL)
            free(dict_name);
    }
    return !ret;
}

/**
  Rozszerza podane menu o opcje spell-check'owe.
  @param[in,out] menubar Rozszerzane menu.
 */
void extend_menu(GtkWidget *menubar)
{
    GtkWidget *spell_menu_item, *spell_menu, *check_item, *lang_item, *doc_item;

    spell_menu_item = gtk_menu_item_new_with_label("Spell");
    spell_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(spell_menu_item), spell_menu);
    gtk_widget_show(spell_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), spell_menu_item);
   
    check_item = gtk_menu_item_new_with_label("Check word");
    g_signal_connect(G_OBJECT(check_item), "activate", 
                     G_CALLBACK(check_word), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), check_item);
    gtk_widget_show(check_item);

    doc_item = gtk_menu_item_new_with_label("Check document");
    g_signal_connect(G_OBJECT(doc_item), "activate", 
                     G_CALLBACK(check_document), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), doc_item);
    gtk_widget_show(doc_item);

    lang_item = gtk_menu_item_new_with_label("Change language/dictionary");
    g_signal_connect(G_OBJECT(lang_item), "activate", 
                     G_CALLBACK(change_lang), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(spell_menu), lang_item);
    gtk_widget_show(lang_item);

    // To nie jest element menu, ale nie można edytować innych plików,
    // więc jest to jedyna możliwość obsługi zamknięcia programu.
    g_signal_connect(editor_window, "delete-event",
                     G_CALLBACK(save_on_delete_window), NULL);
}

/**@}*/

/*EOF*/

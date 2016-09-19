# deklarujemy plik wykonywalny tworzony na podstawie odpowiedniego pliku źródłowego
add_executable (multi-editor multi-editor.c)

# przy kompilacji programu należy dołączyć bibliotekę
target_link_libraries (multi-editor dictionary)

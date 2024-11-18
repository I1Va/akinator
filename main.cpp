#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include "general.h"
#include "akinator_err.h"
#include "akinator_funcs.h"
#include "bin_tree_proc.h"
#include "bin_tree_loger.h"
#include "bin_tree_err_proc.h"
#include "error_processing.h"

#include "stack_funcs.h"
#include "string_funcs.h"

#include <stdlib.h>

const char logs_dir[] = "./logs";
const char LOGS_FILE[] = "./logs/log.html";
const char TREE_DUMP_FILE[] = "./tree.txt";
const char EMPTY_LEAF_NAME[] = "ХЗ_ЧТО";

int test_mode() {
     create_logs_dir(logs_dir);

    bin_tree_err_t last_err = BT_ERR_OK;

    bin_tree_t tree = {};
    bin_tree_ctor(&tree, "./logs/log.html");

    str_storage_t *string_storage = str_storage_t_ctor(CHUNK_SIZE);


    bin_tree_log_file_start(tree.log_file_ptr);

    bin_tree_elem_value_t node = {1, (char *) EMPTY_LEAF_NAME};
    tree.root = bin_tree_create_node(&tree, NULL, false, NULL, NULL, node);

    for (int i = 0; i < 5; i++) {
        akinator_play(&tree, tree.root, string_storage);
    }
    TreeLogDump(&tree);

    FILE *tree_file_ptr = fopen(TREE_DUMP_FILE, "wb");
    if (tree_file_ptr == NULL) {
        debug("file '%s' open failed", TREE_DUMP_FILE);
        CLEAR_MEMORY(exit_mark)
    }

    akinator_tree_file_dump(tree_file_ptr, tree.root);

    if (fclose(tree_file_ptr)) {
        debug("file '%s' close failed", TREE_DUMP_FILE);
        CLEAR_MEMORY(exit_mark)
    }

    if (1) {
        str_t text = read_text_from_file(TREE_DUMP_FILE);
        clear_text(&text, " \n");

        printf("%s\n\n", text);
        bin_tree_t new_tree = {};
        bin_tree_ctor(&new_tree, LOGS_FILE);
        akinator_load_tree(&new_tree, NULL, false, &text, string_storage);
        TreeLogDump(&new_tree);
    }

    bin_tree_dtor(&tree);
    str_storage_t_dtor(string_storage);
    return EXIT_SUCCESS;

    exit_mark:
    bin_tree_dtor(&tree);
    str_storage_t_dtor(string_storage);
    return EXIT_FAILURE;
}

int main() {
    create_logs_dir(logs_dir);

    bin_tree_err_t last_err = BT_ERR_OK;

    bin_tree_t tree = {};
    bin_tree_ctor(&tree, "./logs/log.html");

    str_storage_t *string_storage = str_storage_t_ctor(CHUNK_SIZE);
    bin_tree_log_file_start(tree.log_file_ptr);


    str_t text = read_text_from_file(TREE_DUMP_FILE);
    str_t temp_text = text;

    clear_text(&text, " \n");

    akinator_load_tree(&tree, NULL, false, &temp_text, string_storage);

    akinator_give_definition(&tree, "kapibara");
    akinator_compare(&tree, "kapibara", "CHINAA");





    // akinator_play(&tree, tree.root, string_storage);

    // FILE *tree_file_ptr = fopen(TREE_DUMP_FILE, "wb");
    // if (tree_file_ptr == NULL) {
    //     debug("file '%s' open failed", TREE_DUMP_FILE);
    //     CLEAR_MEMORY(exit_mark)
    // }

    // akinator_tree_file_dump(tree_file_ptr, tree.root);

    // if (fclose(tree_file_ptr)) {
    //     debug("file '%s' close failed", TREE_DUMP_FILE);
    //     CLEAR_MEMORY(exit_mark)
    // }

    TreeLogDump(&tree);

    bin_tree_dtor(&tree);
    str_storage_t_dtor(string_storage);
    FREE(text.str_ptr)

    return EXIT_SUCCESS;

    exit_mark:
    bin_tree_dtor(&tree);
    str_storage_t_dtor(string_storage);
    FREE(text.str_ptr)
    // if (tree_file_ptr != NULL) {
    //     fclose(tree_file_ptr);
    // }

    return EXIT_FAILURE;

}

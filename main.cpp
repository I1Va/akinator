#include <algorithm>
#include <cassert>
#include <cmath>
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


akinator_err_t load_tree_from_description(bin_tree_t *tree, str_storage_t **string_storage) {;
    descr_arr_t *arr = get_descriptions_from_file("./descriptions.txt", string_storage);
    if (arr == NULL) {
        return AR_ERR_GET_DESCR;
    }

    build_tree_from_descr_arr(tree, arr, string_storage);

    descr_arr_t_dtor(arr);

    return AR_ERR_OK;
}

int main() {
    create_logs_dir(logs_dir);
    bin_tree_err_t last_err = BT_ERR_OK;

    bin_tree_t tree = {};
    bin_tree_ctor(&tree, "./logs/log.html");
    str_storage_t *string_storage = str_storage_t_ctor(CHUNK_SIZE);
    bin_tree_log_file_start(tree.log_file_ptr);


    printf("Choose mode : \nPlay[P], Compare[C], Definition[D], Load (from description file)[L]?\n");
    char mode = 0;
    scanf("%c", &mode);
    if (mode == 'P') {
        printf_grn("PLAY MODE\n");

        str_t text = read_text_from_file(TREE_DUMP_FILE);
        str_t temp_text = text;
        remove_chars_from_text(&text, " \n");
        akinator_load_tree(&tree, NULL, false, &temp_text, &string_storage);

        akinator_play(&tree, tree.root, &string_storage);

        printf("Save tree: [0/1]?\n");
        int ans = 0;
        scanf("%d", &ans);
        if (ans == 1) {
            char tree_dump_name[BUFSIZ] = {};
            printf("enter tree file name: \n");
            scanf("%s", tree_dump_name);

            akinator_tree_file_dump(tree_dump_name, &tree);
        }

        FREE(text.str_ptr)

        return EXIT_SUCCESS;
    } else if (mode == 'C') {
        printf_grn("COMPARE MODE\n");

        str_t text = read_text_from_file(TREE_DUMP_FILE);
        str_t temp_text = text;
        remove_chars_from_text(&text, " \n");
        akinator_load_tree(&tree, NULL, false, &temp_text, &string_storage);

        char name1[BUFSIZ] = {};
        char name2[BUFSIZ] = {};

        const size_t attemps = 5;
        for (size_t i = 0; i < attemps; i++) {
            scanf("%s %s", name1, name2);
            printf("name1 : '%s', name2 : '%s'\n", name1, name2);
            if (akinator_compare(&tree, name1, name2)) {
                break;
            }
        }

        FREE(text.str_ptr)
    } else if (mode == 'D') {
        printf_grn("DEFINITION MODE\n");

        str_t text = read_text_from_file(TREE_DUMP_FILE);
        str_t temp_text = text;
        remove_chars_from_text(&text, " \n");
        akinator_load_tree(&tree, NULL, false, &temp_text, &string_storage);

        char name[BUFSIZ] = {};

        const size_t attemps = 5;
        for (size_t i = 0; i < attemps; i++) {
            scanf("%s", name);
            printf("name : '%s'\n", name);
            if (akinator_give_definition(&tree, name)) {
                break;
            }
        }

        FREE(text.str_ptr)
    } else if (mode == 'L'){
        akinator_err_t ret_err = load_tree_from_description(&tree, &string_storage);

        if (ret_err != AR_ERR_OK) {
            DEBUG_AR_LIST_ERROR(ret_err, )
            bin_tree_dtor(&tree);
            return EXIT_FAILURE;
        }
    }

    printf("Dump database? [0/1]?\n");
    int answer = 0;
    scanf("%d", &answer);
    if (answer == 1) {
        akinator_tree_file_dump(TREE_DUMP_FILE, &tree);
        TreeLogDump(&tree);
        // system("")
    }

    bin_tree_dtor(&tree);
    if (string_storage != NULL) {
        str_storage_t_dtor(string_storage);
    }

    return EXIT_SUCCESS;
}

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

struct description_t {
    char *name;
    size_t name_len;

    feature_t *features;
    size_t features_cnt;
};

struct descr_arr_t {
    description_t *data;
    size_t len;
};

bool description_t_ctor(description_t *description, char *name, const size_t features_cnt, str_storage_t **storage) {
    assert(description != NULL);
    assert(name != NULL);
    assert(storage != NULL);

    description->name_len = strlen(name);
    description->name = get_new_str_ptr(storage, description->name_len);
    strcpy(description->name, name);

    description->features_cnt = features_cnt;
    description->features = (feature_t *) calloc(description->features_cnt, sizeof(feature_t));

    if (description->features == NULL) {
        DEBUG_AR_LIST_ERROR(AR_ERR_ALLOC, "description->features")
        return false;
    }

    return true;
}

bool description_t_dtor(description_t *description) {
    if (!description) {
        return false;
    }
    if (!description->features) {
        return false;

    }

    FREE(description->features);
    return true;
}

bool descr_arr_t_dtor(descr_arr_t *arr) {
    if (!arr) {
        return false;
    }
    if (!arr->data) {
        return false;
    }

    bool state = true;

    for (size_t i = 0; i < arr->len; i++) {
        if (!description_t_dtor(&(*arr).data[i])) {
            state = false;
        }
    }
    if (arr->data) {
        FREE(arr->data)
    }
    FREE(arr)

    return state;
}

void fprintf_description_t(FILE *stream, description_t *description) {
    fprintf(stream, "object_name: '%s', features_cnt: {%lu}\n", description->name, description->features_cnt);
    for (size_t i = 0; i < description->features_cnt; i++) {
        fprintf(stream, "{%d}'%s'; ", description->features[i].positive, description->features[i].data);
    }
    fprintf(stream, "\n");
}

void fprintf_arr_descr_t(FILE *stream, descr_arr_t *arr) {
    bin_tree_fprintf_title(stream, "DESCR_ARRAY", '-', BORDER_SZ);

    for (size_t i = 0; i < arr->len; i++) {
        fprintf_description_t(stream, &arr->data[i]);
        fprintf(stream, "\n");
    }
    fprintf(stream, "\n");

    bin_tree_fprintf_border(stream, '-', BORDER_SZ, true);
}

descr_arr_t *get_descriptions_from_file(const char path[], str_storage_t **storage) {
    FILE *inp_file = fopen(path, "r");
    size_t descriptions_cnt = 0;
    char bufer[BUFSIZ] = {};

    descr_arr_t *descr_arr = (descr_arr_t *) calloc(1, sizeof(descr_arr_t));
    if (!descr_arr) {
        DEBUG_AR_LIST_ERROR(AR_ERR_ALLOC, "*descr_arr")
        CLEAR_MEMORY(exit_mark)
    }

    if (inp_file == NULL) {
        DEBUG_AR_LIST_ERROR(AR_ERR_FILE_OPEN, "path : '%s'", path)
        CLEAR_MEMORY(exit_mark)
    }

    fscanf(inp_file, "%lu", &descriptions_cnt);

    descr_arr->data = (description_t *) calloc(descriptions_cnt, sizeof(description_t));
    descr_arr->len = descriptions_cnt;

    if (descr_arr->data == NULL) {
        DEBUG_AR_LIST_ERROR(AR_ERR_ALLOC, "descr_arr->data")
        CLEAR_MEMORY(exit_mark)
    }


    for (size_t descr_idx = 0; descr_idx < descriptions_cnt; descr_idx++) {
        size_t features_cnt = 0;
        fscanf(inp_file, "%s %lu", bufer, &features_cnt);

        description_t *description = &descr_arr->data[descr_idx];

        if (!description_t_ctor(description, bufer, features_cnt, storage)) {
            DEBUG_AR_LIST_ERROR(AR_ERR_ALLOC, "description_t_ctor")
            CLEAR_MEMORY(exit_mark)
        }


        // printf("object_name: '%s', features_cnt: {%lu}\n", descr_arr->data[descr_idx].name, features_cnt);

        for (size_t feature_idx = 0; feature_idx < features_cnt; feature_idx++) {
            int positive_state = 0;
            fscanf(inp_file, "%d %s", &positive_state, bufer);
            printf("bufer_sz: %lu\n", strlen(bufer));
            description->features[feature_idx].data = get_new_str_ptr(storage, strlen(bufer));
            description->features[feature_idx].positive = positive_state;
            strcpy(description->features[feature_idx].data, bufer);

        }
    }

    return descr_arr;

    exit_mark:
    descr_arr_t_dtor(descr_arr);

    return NULL;
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

    // akinator_load_tree(&tree, NULL, false, &temp_text, string_storage);

    // akinator_give_definition(&tree, "kapibara");
    // akinator_compare(&tree, "kapibara", "CHINAA");

    descr_arr_t *arr = get_descriptions_from_file("./descriptions.txt", &string_storage);

    

    // fprintf_arr_descr_t(stdout, arr);

    descr_arr_t_dtor(arr);





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

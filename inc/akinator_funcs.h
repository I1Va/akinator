#ifndef AKINATOR_FUNCS_H
#define AKINATOR_FUNCS_H

#include <string.h>
#include "bin_tree_proc.h"
#include "string_funcs.h"

const size_t MAX_NAME_SZ = 128;
const size_t CHUNK_SIZE = 128;

struct feature_t {
    bool positive;
    bool used;

    char *data;
};

struct description_t {
    char *name;
    size_t name_len;

    size_t uniq_feature_idx = 0;

    feature_t *features;
    size_t features_cnt;
};

struct descr_arr_t {
    description_t *data;
    size_t len;
};

bin_tree_elem_t *akinator_load_tree(bin_tree_t *tree, bin_tree_elem_t *prev, bool prev_left, str_t *text, str_storage_t **storage);
void akinator_play(bin_tree_t *tree, bin_tree_elem_t *cur_node, str_storage_t **string_storage);
void fprintf_n_chars(FILE *stream, const char c, const size_t n);
void akinator_tree_file_dump(FILE* stream, bin_tree_elem_t *node, size_t indent=4);
void akinator_print_node_path(bin_tree_elem_t *node, bool left_son_state);
void akinator_write_node_path(stack_t *feature_stack, bin_tree_elem_t *node, bool left_son_state);
bin_tree_elem_t *akinator_get_node_ptr(bin_tree_t *tree, const char name[]);
void akinator_fprintf_feature_stack(FILE* stream, stack_t *feature_stack);
void akinator_give_definition(bin_tree_t *tree, const char name[]);
bool feature_t_cmp(feature_t *feature_1, feature_t *feature_2);
void akinator_compare(bin_tree_t *tree, const char name1[], const char name2[]);
bool description_t_ctor(description_t *description, char *name, const size_t features_cnt, str_storage_t **storage);
bool description_t_dtor(description_t *description);
bool descr_arr_t_dtor(descr_arr_t *arr);
void fprintf_description_t(FILE *stream, description_t *description);
void fprintf_arr_descr_t(FILE *stream, descr_arr_t *arr);
descr_arr_t *get_descriptions_from_file(const char path[], str_storage_t **storage);
feature_t *get_uniq_feature(description_t *descr);
void change_uniq_feature_status(feature_t *feature, descr_arr_t *arr);
bool feature_in_description(description_t *descr, char *name, descr_arr_t *arr);
void akinator_add_description_to_tree(bin_tree_t *tree, bin_tree_elem_t *cur_node, str_storage_t **string_storage,
    description_t *descr, descr_arr_t *arr);
void build_tree_from_descr_arr(bin_tree_t *tree, descr_arr_t *arr, str_storage_t **storage);

#endif // AKINATOR_FUNCS_H
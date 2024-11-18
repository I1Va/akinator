#ifndef AKINATOR_FUNCS_H
#define AKINATOR_FUNCS_H

#include <string.h>
#include "bin_tree_proc.h"
#include "string_funcs.h"

const size_t MAX_NAME_SZ = 128;
const size_t CHUNK_SIZE = 128;

struct feature_t {
    bool positive;

    char *data;
};

bin_tree_elem_t *akinator_load_tree(bin_tree_t *tree, bin_tree_elem_t *prev, bool prev_left, str_t *text, str_storage_t *storage);
void akinator_play(bin_tree_t *tree, bin_tree_elem_t *cur_node, str_storage_t *string_storage);
void fprintf_n_chars(FILE *stream, const char c, const size_t n);
void akinator_tree_file_dump(FILE* stream, bin_tree_elem_t *node, size_t indent=4);
void akinator_print_node_path(bin_tree_elem_t *node, bool left_son_state);
void akinator_write_node_path(stack_t *feature_stack, bin_tree_elem_t *node, bool left_son_state);
bin_tree_elem_t *akinator_get_node_ptr(bin_tree_t *tree, const char name[]);
void akinator_fprintf_feature_stack(FILE* stream, stack_t *feature_stack);
void akinator_give_definition(bin_tree_t *tree, const char name[]);
bool feature_t_cmp(feature_t *feature_1, feature_t *feature_2);
void akinator_compare(bin_tree_t *tree, const char name1[], const char name2[]);

#endif // AKINATOR_FUNCS_H
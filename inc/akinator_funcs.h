#ifndef AKINATOR_FUNCS_H
#define AKINATOR_FUNCS_H

#include <string.h>
#include "bin_tree_proc.h"
#include "string_funcs.h"

const size_t MAX_NAME_SZ = 128;
const size_t CHUNK_SIZE = 128;

void akinator_play(bin_tree_t *tree, bin_tree_elem_t *cur_node, str_storage_t *string_storage);

#endif // AKINATOR_FUNCS_H
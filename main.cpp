#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include "akinator_funcs.h"
#include "bin_tree_proc.h"
#include "bin_tree_loger.h"
#include "bin_tree_err_proc.h"
#include "general.h"
#include "string_funcs.h"

#include <stdlib.h>

const char logs_dir[] = "./logs";
const char TREE_DUMP_FILE[] = "./tree.txt";

int test_mode() {
    create_logs_dir(logs_dir);

    bin_tree_err_t last_err = BT_ERR_OK;

    bin_tree_t tree = {};
    bin_tree_ctor(&tree, "./logs/log.html");

    bin_tree_log_file_start(tree.log_file_ptr);

    // bin_tree_elem_value_t node = {52, "fwef"};

    bin_tree_elem_t *root = bin_tree_create_node(&tree, NULL, false, NULL, NULL, {52, "sfsdfds"});
    tree.root = root;
    for (int i = 0; i < 20; i++) {
        int val = rand() % 128;
        printf("added!!! {%d}\n", val);
        bin_tree_push_val(&tree, tree.root, {val, "3423"}, node_t_cmp);
    }

    bin_tree_print(tree.root, node_t_get_outp); printf("\n");

    // stk_err last_stk_err = STK_ERR_OK;

    // bin_tree_elem_t *node1 = *(bin_tree_elem_t **)stack_get_elem(&tree.node_stack, 3, &last_stk_err);
    // bin_tree_elem_t *node2 = *(bin_tree_elem_t **)stack_get_elem(&tree.node_stack, 5, &last_stk_err);

    // node1->left = node2;
    // node2->right = node1;

    // bin_tree_verify(tree, &last_err);

    // DUMP(&tree.node_stack, stdout, tree_node_fprintf);

    TreeLogDump(&tree);


    // bin_tree_dtor(&tree);
    // return 0;

    return 0;
}

const char EMPTY_LEAF_NAME[] = "ХЗ ЧТО";

int main() {
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
        char *text = read_text_from_file(TREE_DUMP_FILE);
        printf("%s", text);
    }

    bin_tree_dtor(&tree);
    str_storage_t_dtor(string_storage);
    return EXIT_SUCCESS;

    exit_mark:
    bin_tree_dtor(&tree);
    str_storage_t_dtor(string_storage);
    return EXIT_FAILURE;
}

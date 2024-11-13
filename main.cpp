#include <cassert>
#include <cstring>
#include <stdio.h>

#include "akinator_funcs.h"
#include "bin_tree_proc.h"
#include "bin_tree_loger.h"
#include "bin_tree_err_proc.h"
#include "general.h"

#include <stdlib.h>

const char logs_dir[] = "./logs";

void akinator_tree_fprintf(FILE *stream, bin_tree_elem_t *node) {
    if (!node) {
        return;
    }
    printf("{ ");

    if (node->left) {
        akinator_tree_fprintf(stream, node->left);
    }
    fprintf(stream, "\"%s\" %d %d\n", node->data.name, node->left != NULL, node->right != NULL);

    if (node->right) {
        akinator_tree_fprintf(stream, node->right);
    }

    printf("}");
}

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

    akinator_tree_fprintf(stdout, tree.root);
    bin_tree_dtor(&tree);

    return 0;
}
#include <stdio.h>

#include "bin_tree_proc.h"
#include "bin_tree_loger.h"
#include "bin_tree_err_proc.h"

#include <stdlib.h>

const char logs_dir[] = "./logs";

const size_t MAX_NAME_SZ = 128;

// void play(bin_tree_t *tree, bin_tree_elem_t *cur_node, bin_tree_elem_value_t val) {
//     printf("%s?", cur_node->data.name);
//     int answer = 0;
//     scanf("%d", &answer);

//     if (!answer) {
//         if (!cur_node->left) {
//             printf("ОГО! новый персонаж! Введите имя: \n");
//             char name[MAX_NAME_SZ];
//             scanf("%s", name);
//             bin_tree_elem_value_t node = {0, "fef"};
//             bin_tree_create_node(tree, cur_node, true, NULL, NULL, node);
//             return;
//         } else {
//             play(tree, cur_node->left, val);
//         }
//     } else {
//         if (!cur_node->right) {
//             play(tree, cur_node, false, NULL, NULL, val);
//         } else {
//             play(tree, cur_node->right, val, compare_func);
//         }
//     }
// }

int main() {
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
        // printf("added!!! {%d}\n", val);
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
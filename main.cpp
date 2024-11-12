#include <cassert>
#include <stdio.h>

#include "bin_tree_proc.h"
#include "bin_tree_loger.h"
#include "bin_tree_err_proc.h"
#include "general.h"

#include <stdlib.h>

const char logs_dir[] = "./logs";

const size_t MAX_NAME_SZ = 128;


void play(bin_tree_t *tree, bin_tree_elem_t *cur_node) {
    assert(tree != NULL);
    assert(cur_node != NULL);

    // У каждой вершины, у которой leaf == 0 обязательно есть левый и правый сыновья

    if (cur_node->data.value) { // leaf
        printf("Ваш персонаж: %s? [0/1]\n", cur_node->data.name);
        int answer = 0;
        scanf("%d", &answer);

        if (answer) {
            printf("Изи, я угадал ^_^!\n");
            return;
        } else {
            printf("Хм... Не могу угадать. Кто ваш персонаж?\n");
            char *name = (char *) calloc(MAX_NAME_SZ, sizeof(char));
            scanf("%s", name);
            printf("Чем ваш персонаж отличается от '%s'?\n", cur_node->data.name);
            char *feature = (char *) calloc(MAX_NAME_SZ, sizeof(char));
            scanf("%s", feature);

            bin_tree_elem_t *new_node = bin_tree_create_node(tree, NULL, false, NULL, NULL, {1, name});
            bin_tree_elem_t *feature_node = bin_tree_create_node(tree, cur_node->prev, cur_node->left_son, cur_node, NULL, {0, feature});

            new_node->prev = feature_node;
            feature_node->right = new_node;

            if (tree->root == cur_node) {
                tree->root = feature_node;
            }
            cur_node->left_son = true;
            cur_node->prev = feature_node;
            cur_node->data.value = true;

            return;
        }
    } else {
        printf("Ваш персонаж: '%s'? [0/1]\n", cur_node->data.name);
        int answer = 0;
        scanf("%d", &answer);
        if (!answer) {
            play(tree, cur_node->left);
        } else {
            play(tree, cur_node->right);
        }
    }
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

    bin_tree_log_file_start(tree.log_file_ptr);

    bin_tree_elem_value_t node = {1, (char *) EMPTY_LEAF_NAME};

    tree.root = bin_tree_create_node(&tree, NULL, false, NULL, NULL, node);

    for (int i = 0; i < 10; i++) {
        play(&tree, tree.root);
        TreeLogDump(&tree);
    }



    bin_tree_dtor(&tree);

    return 0;
}
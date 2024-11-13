#include <stdio.h>
#include <assert.h>
#include "bin_tree_proc.h"
#include "akinator_funcs.h"

#include "string_funcs.h"

const size_t STR_BUFER_SZ = 128;

char str_bufer[STR_BUFER_SZ] = {};

void clear_str_bufer(char *bufer_ptr, const size_t len) {
    for (size_t i = 0; i < len; i++) {
        bufer_ptr[i] = '\0';
    }
}

void akinator_play(bin_tree_t *tree, bin_tree_elem_t *cur_node, str_storage_t *string_storage) {
    assert(tree != NULL);
    assert(cur_node != NULL);

    // У каждой вершины, у которой leaf == 0 обязательно есть левый и правый сыновья

    if (cur_node->data.value) { // leaf
        printf("Ваш персонаж: '%s'? [0/1]\n", cur_node->data.name);
        int answer = 0;
        scanf("%d", &answer);

        if (answer) {
            printf("Изи, я угадал ^_^!\n");
            return;
        } else {
            printf("Хм... Не могу угадать. Кто ваш персонаж?\n");

            scanf("%s", str_bufer);
            size_t name_bufer_len = strlen(str_bufer) + 1;
            char *name = get_new_str_ptr(&string_storage, name_bufer_len);
            strncpy(name, str_bufer, name_bufer_len);
            clear_str_bufer(str_bufer, STR_BUFER_SZ);

            printf("Чем ваш персонаж отличается от '%s'?\n", cur_node->data.name);

            scanf("%s", str_bufer);
            size_t feature_bufer_len = strlen(str_bufer) + 1;
            char *feature = get_new_str_ptr(&string_storage, feature_bufer_len);
            strncpy(feature, str_bufer, feature_bufer_len);
            clear_str_bufer(str_bufer, STR_BUFER_SZ);

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
        printf("У вашего персонажа есть признак: %s? [0/1]\n", cur_node->data.name);
        int answer = 0;
        scanf("%d", &answer);
        if (!answer) {
            akinator_play(tree, cur_node->left, string_storage);
        } else {
            akinator_play(tree, cur_node->right, string_storage);
        }
    }
}

// void akinator_save_tree(bin_tree_t *tree) {
//     bin_tree_print(bin_tree_elem_t *node, void (*outp_func)(char *, const size_t, const bin_tree_elem_t *))
// }
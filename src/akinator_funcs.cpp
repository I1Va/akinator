#include <stdio.h>
#include <assert.h>

#include "general.h"
#include "akinator_err.h"
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

bin_tree_elem_t *akinator_load_tree(bin_tree_t *tree, bin_tree_elem_t *prev, bool prev_left, str_t *text, str_storage_t *storage) {
    assert(tree != NULL);
    assert(text != NULL);

    text->str_ptr = strchr(text->str_ptr, '{');
    if (text->str_ptr == NULL) {
        debug("'{' lexem hasn't been found");
        return NULL;
    }
    text->str_ptr++;

    text->str_ptr = strchr(text->str_ptr, '"');
    if (text->str_ptr == NULL) {
        debug("'\"' lexem hasn't been found");
        return NULL;
    }
    text->str_ptr++;

    char *word_end = strchr(text->str_ptr, '"');
    if (word_end == NULL) {
        debug("'\"' lexem hasn't been found");
        return NULL;
    }
    size_t word_len = (size_t) (word_end - text->str_ptr);

    char *name = get_new_str_ptr(&storage, word_len);
    strncpy(name, text->str_ptr, word_len);

    text->str_ptr = word_end + 1;

    char left = *text->str_ptr;
    text->str_ptr++;
    char right = *text->str_ptr;
    text->str_ptr++;

    bool leaf_state = true;
    bin_tree_elem_t *node = bin_tree_create_node(tree, prev, prev_left, NULL, NULL, {0, name});

    if (left == '1') {
        leaf_state = false;
        node->left = akinator_load_tree(tree, node, true, text, storage);
    }
    if (right == '1') {
        leaf_state = false;
        node->right = akinator_load_tree(tree, node, false, text, storage);
    }
    if (prev == NULL) {
        tree->root = node;
    }
    node->data.value = leaf_state;
    node->is_node_left_son = prev_left;

    return node;
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
            bin_tree_elem_t *feature_node = bin_tree_create_node(tree, cur_node->prev, cur_node->is_node_left_son, cur_node, NULL, {0, feature});

            new_node->prev = feature_node;
            feature_node->right = new_node;

            if (tree->root == cur_node) {
                tree->root = feature_node;
            }
            cur_node->is_node_left_son = true;
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

void fprintf_n_chars(FILE *stream, const char c, const size_t n) {
    for (size_t i = 0; i < n; i++) {
        fprintf(stream, "%c", c);
    }
}

void akinator_tree_file_dump(FILE* stream, bin_tree_elem_t *node, size_t indent) {
    if (!node) {
        return;
    }
    fprintf_n_chars(stream, ' ', indent);
    fprintf(stream, "{");
    fprintf(stream, "\"%s\" %d %d\n", node->data.name, node->left != NULL, node->right != NULL);
    if (node->left) {
        akinator_tree_file_dump(stream, node->left, indent + 4);
    }
    // else {
    //     fprintf_n_chars(stream, ' ', indent);
    //     fprintf(stream, "{}\n");
    // }
    if (node->right) {
        akinator_tree_file_dump(stream, node->right, indent + 4);
    }
    // else {
    //     fprintf_n_chars(stream, ' ', indent);
    //     fprintf(stream, "{}\n");
    // }
    fprintf_n_chars(stream, ' ', indent);
    fprintf(stream, "}\n");
}

void akinator_print_node_path(bin_tree_elem_t *node, bool left_son_state) {
    if (node == NULL) {
        return;
    }

    akinator_print_node_path(node->prev, node->is_node_left_son);

    if (left_son_state) {
        printf("НЕ ");
    }
    printf("'%s'; ", node->data.name);
}

void akinator_write_node_path(stack_t *feature_stack, bin_tree_elem_t *node, bool left_son_state) {
    if (node == NULL) {
        return;
    }

    assert(feature_stack != NULL);


    akinator_write_node_path(feature_stack, node->prev, node->is_node_left_son);

    feature_t feature = {};
    feature.positive = !left_son_state;
    feature.data = node->data.name;

    stk_err stack_err = STK_ERR_OK;

    stack_push(feature_stack, &feature, &stack_err);
    if (stack_err != STK_ERR_OK) {
        DEBUG_AR_LIST_ERROR(AR_ERR_STACK, "feature_stack")
        CLEAR_MEMORY(exit_mark)
    }

    return;

    exit_mark:

    // if (feature != NULL) {
    //     FREE(feature)
    // }

    return;
}

bin_tree_elem_t *akinator_get_node_ptr(bin_tree_t *tree, const char name[]) {
    bool not_leaf_state = false;

    for (size_t i = 0; i < tree->node_stack.size; i++) {
        stk_err err = STK_ERR_OK;

        bin_tree_elem_t *node = *(bin_tree_elem_t **) stack_get_elem(&tree->node_stack, i, &err);
        if (err != STK_ERR_OK) {
            DEBUG_AR_LIST_ERROR(AR_ERR_STACK, "stack elem[%lu] get failed")
            return NULL;
        }
        // printf("node[%lu] : '%s' left_son?: {%d}\n", i, node->data.name, node->is_node_left_son);

        if (strcmp(node->data.name, name) == 0) {
            if (node->data.value == 0) { // not leaf
                not_leaf_state = true;
            } else {
                return node;
            }
        }
    }
    if (not_leaf_state) {
        printf("'%s' - не персонаж, а признак!\n", name);
    }

    return NULL;
}

void akinator_fprintf_feature_stack(FILE* stream, stack_t *feature_stack) {
    assert(feature_stack != NULL);

    stk_err stack_err = STK_ERR_OK;

    for (size_t i = 0; i < feature_stack->size; i++) {
        feature_t feature = *(feature_t *) stack_get_elem(feature_stack, i, &stack_err);
        if (stack_err != STK_ERR_OK) {
            DEBUG_AR_LIST_ERROR(AR_ERR_STACK, "stack_get_elem [%lu] falied", i)
            return;
        }
        fprintf(stream, "feature[%lu]: {pos: '%d', data: '%s'}\n", i, feature.positive, feature.data);
    }
}

void akinator_give_definition(bin_tree_t *tree, const char name[]) {
    bin_tree_elem_t *start_node = akinator_get_node_ptr(tree, name);

    if (start_node == NULL) {
        printf("Персонажа '%s' нет в базе данных\n", name);
        return;
    }

    printf("Описание персонажа '%s': \n", name);
    akinator_print_node_path(start_node->prev, start_node->is_node_left_son);
    printf("\n");


    printf("stack_contaiment: \n");
    stk_err stack_err = STK_ERR_OK;

    stack_t feature_stack = {};
    STACK_INIT(&feature_stack, tree->n_nodes, sizeof(feature_t), tree->log_file_ptr, &stack_err)

    akinator_write_node_path(&feature_stack, start_node->prev, start_node->is_node_left_son);

    akinator_fprintf_feature_stack(stdout, &feature_stack);

    printf("\n");

    stack_destroy(&feature_stack);
}

bool feature_t_cmp(feature_t *feature_1, feature_t *feature_2) {
    return (strcmp(feature_1->data, feature_2->data) == 0) && feature_1->positive == feature_2->positive;
}

void akinator_compare(bin_tree_t *tree, const char name1[], const char name2[]) {
    bin_tree_elem_t *node1 = akinator_get_node_ptr(tree, name1);
    bin_tree_elem_t *node2 = akinator_get_node_ptr(tree, name2);

    stk_err stack_err = STK_ERR_OK;

    if (node1 == NULL) {
        printf("Персонажа '%s' нет в базе данных\n", name1);
        return;
    }
    if (node2 == NULL) {
        printf("Персонажа '%s' нет в базе данных\n", name2);
        return;
    }

    stack_t path1_stack = {};
    stack_t path2_stack = {};
    size_t eq_prefix_idx = 0;
    STACK_INIT(&path1_stack, tree->n_nodes, sizeof(feature_t), NULL, &stack_err);
    if (stack_err != STK_ERR_OK) {
        DEBUG_AR_LIST_ERROR(AR_ERR_STACK, "node stack 1 init failed")
        CLEAR_MEMORY(exit_mark)
    }
    akinator_write_node_path(&path1_stack, node1->prev, node1->is_node_left_son);


    STACK_INIT(&path2_stack, tree->n_nodes, sizeof(feature_t), NULL, &stack_err);
    if (stack_err != STK_ERR_OK) {
        DEBUG_AR_LIST_ERROR(AR_ERR_STACK, "node stack 1 init failed")
        CLEAR_MEMORY(exit_mark)
    }
    akinator_write_node_path(&path2_stack, node2->prev, node2->is_node_left_son);


    printf("stack1:\n");
    akinator_fprintf_feature_stack(stdout, &path1_stack);
    printf("stack2:\n");
    akinator_fprintf_feature_stack(stdout, &path2_stack);


    printf("Персонажи '%s' и '%s' похожи тем, что они оба: \n", name1, name2);

    for (; eq_prefix_idx < MIN(path1_stack.size, path2_stack.size); eq_prefix_idx++) {
        feature_t feature_1 = *(feature_t *) stack_get_elem(&path1_stack, eq_prefix_idx, &stack_err);
        if (stack_err != STK_ERR_OK) {
            DEBUG_AR_LIST_ERROR(AR_ERR_STACK, "feature_1")
            CLEAR_MEMORY(exit_mark)
        }

        feature_t feature_2 = *(feature_t *) stack_get_elem(&path2_stack, eq_prefix_idx, &stack_err);
        if (stack_err != STK_ERR_OK) {
            DEBUG_AR_LIST_ERROR(AR_ERR_STACK, "feature_1")
            CLEAR_MEMORY(exit_mark)
        }

        if (feature_t_cmp(&feature_1, &feature_2)) {
            if (!feature_1.positive) {
                printf("НЕ ");
            }
            printf("'%s'; ", feature_1.data);
        } else {
            break;
        }
    }
    printf("\nОтличительные черты '%s':\n", name1);
    for (size_t idx = eq_prefix_idx; idx < path1_stack.size; idx++) {
        feature_t feature_1 = *(feature_t *) stack_get_elem(&path1_stack, idx, &stack_err);
        if (stack_err != STK_ERR_OK) {
            DEBUG_AR_LIST_ERROR(AR_ERR_STACK, "feature_1")
            CLEAR_MEMORY(exit_mark)
        }
        if (!feature_1.positive) {
            printf("НЕ ");
        }
        printf("'%s'; ", feature_1.data);
    }

    printf("\nОтличительные черты '%s':\n", name2);
    for (size_t idx = eq_prefix_idx; idx < path1_stack.size; idx++) {
        feature_t feature_2 = *(feature_t *) stack_get_elem(&path2_stack, idx, &stack_err);
        if (stack_err != STK_ERR_OK) {
            DEBUG_AR_LIST_ERROR(AR_ERR_STACK, "feature_1")
            CLEAR_MEMORY(exit_mark)
        }
        if (!feature_2.positive) {
            printf("НЕ ");
        }
        printf("'%s'; ", feature_2.data);
    }


    exit_mark:
    stack_destroy(&path1_stack);
    stack_destroy(&path2_stack);

}

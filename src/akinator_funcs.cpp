#include <stdio.h>
#include <assert.h>
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


#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "general.h"
#include "akinator_err.h"
#include "bin_tree_proc.h"
#include "akinator_funcs.h"
#include "bin_tree_loger.h"
#include "string_funcs.h"

const size_t bufer_SZ = 128;

char bufer[bufer_SZ] = {};

void clear_bufer(char *bufer_ptr, const size_t len) {
    for (size_t i = 0; i < len; i++) {
        bufer_ptr[i] = '\0';
    }
}

bin_tree_elem_t *akinator_load_tree(bin_tree_t *tree, bin_tree_elem_t *prev, bool prev_left, str_t *text, str_storage_t **storage) {
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

    char *name = get_new_str_ptr(storage, word_len);
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

void akinator_play(bin_tree_t *tree, bin_tree_elem_t *cur_node, str_storage_t **string_storage) {
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

            scanf("%s", bufer);
            size_t name_bufer_len = strlen(bufer) + 1;
            char *name = get_new_str_ptr(string_storage, name_bufer_len);
            strncpy(name, bufer, name_bufer_len);

            printf("Чем ваш персонаж отличается от '%s'?\n", cur_node->data.name);

            scanf("%s", bufer);
            size_t feature_bufer_len = strlen(bufer) + 1;
            char *feature = get_new_str_ptr(string_storage, feature_bufer_len);
            strncpy(feature, bufer, feature_bufer_len);

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

    // akinator_fprintf_feature_stack(stdout, &feature_stack);

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


    // printf("stack1:\n");
    // akinator_fprintf_feature_stack(stdout, &path1_stack);
    // printf("stack2:\n");
    // akinator_fprintf_feature_stack(stdout, &path2_stack);


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
    for (size_t idx = eq_prefix_idx; idx < path2_stack.size; idx++) {
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

bool description_t_ctor(description_t *description, char *name, const size_t features_cnt, str_storage_t **storage) {
    assert(description != NULL);
    assert(name != NULL);
    assert(storage != NULL);

    description->name_len = strlen(name);
    description->name = get_new_str_ptr(storage, description->name_len);
    strcpy(description->name, name);

    description->features_cnt = features_cnt;
    description->features = (feature_t *) calloc(description->features_cnt, sizeof(feature_t));

    if (description->features == NULL) {
        DEBUG_AR_LIST_ERROR(AR_ERR_ALLOC, "description->features")
        return false;
    }

    return true;
}

bool description_t_dtor(description_t *description) {
    if (!description) {
        return false;
    }
    if (!description->features) {
        return false;

    }

    FREE(description->features);
    return true;
}

bool descr_arr_t_dtor(descr_arr_t *arr) {
    if (!arr) {
        return false;
    }
    if (!arr->data) {
        return false;
    }

    bool state = true;

    for (size_t i = 0; i < arr->len; i++) {
        if (!description_t_dtor(&(*arr).data[i])) {
            state = false;
        }
    }
    if (arr->data) {
        FREE(arr->data)
    }
    FREE(arr)

    return state;
}

void fprintf_description_t(FILE *stream, description_t *description) {
    fprintf(stream, "object_name: '%s', features_cnt: {%lu}\n", description->name, description->features_cnt);
    for (size_t i = 0; i < description->features_cnt; i++) {
        fprintf(stream, "p%d u%d '%s'; ", description->features[i].positive, description->features[i].used, description->features[i].data);
    }
    fprintf(stream, "\n");
}

void fprintf_arr_descr_t(FILE *stream, descr_arr_t *arr) {
    bin_tree_fprintf_title(stream, "DESCR_ARRAY", '-', BORDER_SZ);

    for (size_t i = 0; i < arr->len; i++) {
        fprintf_description_t(stream, &arr->data[i]);
        fprintf(stream, "\n");
    }
    fprintf(stream, "\n");

    bin_tree_fprintf_border(stream, '-', BORDER_SZ, true);
}

descr_arr_t *get_descriptions_from_file(const char path[], str_storage_t **storage) {
    FILE *inp_file = fopen(path, "r");
    size_t descriptions_cnt = 0;

    descr_arr_t *descr_arr = (descr_arr_t *) calloc(1, sizeof(descr_arr_t));
    if (!descr_arr) {
        DEBUG_AR_LIST_ERROR(AR_ERR_ALLOC, "*descr_arr")
        CLEAR_MEMORY(exit_mark)
    }

    if (inp_file == NULL) {
        DEBUG_AR_LIST_ERROR(AR_ERR_FILE_OPEN, "path : '%s'", path)
        CLEAR_MEMORY(exit_mark)
    }

    fscanf(inp_file, "%lu", &descriptions_cnt);

    descr_arr->data = (description_t *) calloc(descriptions_cnt, sizeof(description_t));
    descr_arr->len = descriptions_cnt;

    if (descr_arr->data == NULL) {
        DEBUG_AR_LIST_ERROR(AR_ERR_ALLOC, "descr_arr->data")
        CLEAR_MEMORY(exit_mark)
    }


    for (size_t descr_idx = 0; descr_idx < descriptions_cnt; descr_idx++) {
        size_t features_cnt = 0;
        fscanf(inp_file, "%s %lu", bufer, &features_cnt);

        description_t *description = &descr_arr->data[descr_idx];

        if (!description_t_ctor(description, bufer, features_cnt, storage)) {
            DEBUG_AR_LIST_ERROR(AR_ERR_ALLOC, "description_t_ctor")
            CLEAR_MEMORY(exit_mark)
        }


        // printf("object_name: '%s', features_cnt: {%lu}\n", descr_arr->data[descr_idx].name, features_cnt);

        for (size_t feature_idx = 0; feature_idx < features_cnt; feature_idx++) {
            int positive_state = 0;
            fscanf(inp_file, "%d %s", &positive_state, bufer);
            printf("bufer_sz: %lu\n", strlen(bufer));
            description->features[feature_idx].data = get_new_str_ptr(storage, strlen(bufer));
            description->features[feature_idx].positive = positive_state;
            strcpy(description->features[feature_idx].data, bufer);

        }
    }

    return descr_arr;

    exit_mark:
    descr_arr_t_dtor(descr_arr);

    return NULL;
}

feature_t *get_uniq_feature(description_t *descr) {
    for (; descr->uniq_feature_idx < descr->features_cnt; descr->uniq_feature_idx++) {
        if (!descr->features[descr->uniq_feature_idx].used) {
            descr->features[descr->uniq_feature_idx++].used = true;
            return &descr->features[descr->uniq_feature_idx - 1];
        }
    }
    return NULL;
}

void change_uniq_feature_status(feature_t *feature, descr_arr_t *arr) {
    for (size_t descr_idx = 0; descr_idx < arr->len; descr_idx++) {
        for (size_t feature_idx = 0; feature_idx < arr->data[descr_idx].features_cnt; feature_idx++) {
            if (strcmp(arr->data[descr_idx].features[feature_idx].data, feature->data) == 0) {
                arr->data[descr_idx].features[feature_idx].used = true;
            }
        }
    }
}

bool feature_in_description(description_t *descr, char *name, descr_arr_t *arr) { // O(n^2)
    for (size_t i = 0; i < descr->features_cnt; i++) {
        if (strcmp(descr->features[i].data, name) == 0 && descr->features[i].positive) {
            change_uniq_feature_status(&descr->features[i], arr);
            return true;
        }
    }
    return false;
}

void akinator_add_description_to_tree(bin_tree_t *tree, bin_tree_elem_t *cur_node, str_storage_t **string_storage,
    description_t *descr, descr_arr_t *arr)
{
    assert(tree != NULL);
    assert(cur_node != NULL);

    // У каждой вершины, у которой leaf == 0 обязательно есть левый и правый сыновья

    if (cur_node->data.value) { // leaf
        printf("Ваш персонаж: '%s'? [0/1]\n", cur_node->data.name);
        int answer = (strcmp(descr->name, cur_node->data.name) == 0);

        printf("-> %d\n", answer);
        if (answer) {
            printf("Изи, я угадал ^_^!\n");
            return;
        } else {
            printf("Хм... Не могу угадать. Кто ваш персонаж?\n");

            char *name = get_new_str_ptr(string_storage, descr->name_len);
            strncpy(name, descr->name, descr->name_len);

            printf("-> '%s'\n", name);

            printf("Чем ваш персонаж отличается от '%s'?\n", cur_node->data.name);

            feature_t *uniq_feature = get_uniq_feature(descr);
            if (uniq_feature == NULL) {
                DEBUG_AR_LIST_ERROR(AR_ERR_NOT_ENOUGH_FEATURES, "description : '%s'")
                return;
            }

            printf("-> '%s'\n", uniq_feature->data);

            bin_tree_elem_t *new_node = bin_tree_create_node(tree, NULL, false, NULL, NULL, {1, descr->name});

            if (uniq_feature->positive) {
                bin_tree_elem_t *feature_node = bin_tree_create_node(tree, cur_node->prev, cur_node->is_node_left_son, cur_node, NULL, {0, uniq_feature->data});
                printf_grn("cur_node : '%s', left? : [%d]\n", cur_node->data.name, cur_node->is_node_left_son);
                printf_grn("feature : '%s', left? : [%d]\n", feature_node->data.name, cur_node->is_node_left_son);
                new_node->prev = feature_node;
                feature_node->right = new_node;
                cur_node->is_node_left_son = true;
                new_node->is_node_left_son = false;

                if (tree->root == cur_node) {
                    tree->root = feature_node;
                }
                cur_node->prev = feature_node;
                cur_node->data.value = true;
            } else {
                bin_tree_elem_t *feature_node = bin_tree_create_node(tree, cur_node->prev, cur_node->is_node_left_son, NULL, cur_node, {0, uniq_feature->data});
                new_node->prev = feature_node;
                feature_node->left = new_node;
                cur_node->is_node_left_son = false;
                new_node->is_node_left_son = true;
                if (tree->root == cur_node) {
                    tree->root = feature_node;
                }
                cur_node->prev = feature_node;
                cur_node->data.value = true;
            }


            return;
        }
    } else {
        printf("У вашего персонажа есть признак: %s? [0/1]\n", cur_node->data.name);

        int answer = feature_in_description(descr, cur_node->data.name, arr);
        printf("-> %d\n", answer);

        if (!answer) {
            akinator_add_description_to_tree(tree, cur_node->left, string_storage, descr, arr);
        } else {
            akinator_add_description_to_tree(tree, cur_node->right, string_storage, descr, arr);
        }
    }
}

void build_tree_from_descr_arr(bin_tree_t *tree, descr_arr_t *arr, str_storage_t **storage) {
    assert(tree != NULL);
    assert(arr != NULL);

    if (arr->len == 0) {
       return;
    }

    tree->root = bin_tree_create_node(tree, NULL, false, NULL, NULL, {1, arr->data[0].name});
    for (size_t i = 1; i < arr->len; i++) {
        akinator_add_description_to_tree(tree, tree->root, storage, &arr->data[i], arr);
    }

    // fprintf_arr_descr_t(stdout, arr);
}

//
// Created by gilas on 11/14/23.
//

#ifndef LINKEDLIST_S_H
#define LINKEDLIST_S_H

struct linkedlist_structure{
    struct linkedlist_node * first;
    struct linkedlist_node * last;
    int length;
};

struct linkedlist_node{
    struct linkedlist_node * next;
    struct linkedlist_node * prev;
    void * data;
};

typedef struct linkedlist_structure linkedlist_structure;

void linkedlist_push(linkedlist_structure * list, void * data);

void * linkedlist_pop(linkedlist_structure * list);

void * linkedlist_remove_first(linkedlist_structure * list);

void * linkedlist_add_last(linkedlist_structure * list, void * data);

void * linkedlist_get_first(linkedlist_structure * list);

int linkedlist_length(linkedlist_structure * list);

int linkedlist_not_empty(linkedlist_structure * list);

void * linkedlist_pop_first_match_from_rear(linkedlist_structure * list, int (*match)(void * data, void * params), void * args);

void * linkedlist_get_first_match_from_rear(linkedlist_structure * list, int (*match)(void * data, void * params), void * args);

void node_iterator(linkedlist_structure * list, void (*action)(void * data, void * args), void * args);

void linkedlist_init(linkedlist_structure * list);

struct linkedlist_node *sorted_merge(struct linkedlist_node *a, struct linkedlist_node *b, int (*cmp)(void * a, void * b));

void front_back_split(struct linkedlist_node *source, struct linkedlist_node **frontRef, struct linkedlist_node **backRef);

void merge_sort(linkedlist_structure * list, int (*cmp)(void * a, void * b));

void __merge_sort(struct linkedlist_node **headRef, int (*cmp)(void * a, void * b));

int unlink_procedure_of_current(linkedlist_structure * list, struct linkedlist_node *prev, struct linkedlist_node * current);

#endif

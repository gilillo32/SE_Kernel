#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linkedlist_s.h"


void merge_sort(linkedlist_structure *list, int (*compare)(void *, void *)){
    __merge_sort(&(list->first), compare);
    struct linkedlist_node *new_last = list->first;
    for(; new_last && new_last->next; new_last = new_last->next);
    list->last = new_last;
}

void __merge_sort(struct linkedlist_node **headRef, int (*compare)(void *a_data, void *b_data)){
    struct linkedlist_node *head = *headRef;
    struct linkedlist_node *a;
    struct linkedlist_node *b;
    if((head == NULL) || (head->next == NULL)){
        return;
    }
    front_back_split(head, &a, &b);
    __merge_sort(&a, compare);
    __merge_sort(&b, compare);
    *headRef = sorted_merge(a, b, compare);
}

struct linkedlist_node *sorted_merge(struct linkedlist_node *a, struct linkedlist_node *b, int (*compare)(void *a_data, void *b_data)){
    struct linkedlist_node *result = NULL;
    if(a == NULL){
        return b;
    }
    else if(b == NULL){
        return a;
    }
    if(compare(a->data, b->data) <= 0){
        result = a;
        result->next = sorted_merge(a->next, b, compare);
    }
    else{
        result = b;
        result->next = sorted_merge(a, b->next, compare);
    }
    return result;
}

void front_back_split(struct linkedlist_node *source, struct linkedlist_node **frontRef, struct linkedlist_node **backRef){
    struct linkedlist_node *fast;
    struct linkedlist_node *slow;
    if(source == NULL || source->next == NULL){
        *frontRef = source;
        *backRef = NULL;
    }
    else{
        slow = source;
        fast = source->next;
        while(fast != NULL){
            fast = fast->next;
            if(fast != NULL){
                slow = slow->next;
                fast = fast->next;
            }
        }
        *frontRef = source;
        *backRef = slow->next;
        slow->next = NULL;
    }
}

int __unlink_procedure_of_current(linkedlist_structure *list, struct linkedlist_node *prev, struct linkedlist_node *current);
int __search_procedure(linkedlist_structure *list, struct linkedlist_node **prev, struct linkedlist_node **current, int (*match)(void *data, void *params), void *args);

void *linkedlist_pop(linkedlist_structure *list){
    if(list->length == 0){
        return NULL;
    }
    struct linkedlist_node *last = list->last;
    void *data = last->data;
    if(list->length == 1){
        list->first = NULL;
        list->last = NULL;
    }
    else{
        list->last = last->prev;
        list->last->next = NULL;
    }
    free(last);
    list->length--;
    return data;
}

void *linkedlist_peek(linkedlist_structure *list){
    if(list->length == 0){
        return NULL;
    }
    return list->last->data;
}

int linkedlist_length(linkedlist_structure *list){
    return list->length;
}

int linkedlist_not_empty(linkedlist_structure *list){
    return list->length > 0;
}

void linkedlist_push(linkedlist_structure *list, void *data){
    struct linkedlist_node *new_node = malloc(sizeof(struct linkedlist_node));
    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = NULL;
    if(list->length == 0){
        list->first = new_node;
        list->last = new_node;
    }
    else{
        list->last->next = new_node;
        new_node->prev = list->last;
        list->last = new_node;
    }
    list->length++;
}

void linkedlist_init(linkedlist_structure *list){
    list->first = NULL;
    list->last = NULL;
    list->length = 0;
}

void *linkedlist_pop_first_match_from_rear(linkedlist_structure *list, int (*match)(void *data, void *params), void *args){
    struct linkedlist_node *prev;
    struct linkedlist_node *current;
    int result = __search_procedure(list, &prev, &current, match, args);
    if(result == 0){
        return NULL;
    }
    return __unlink_procedure_of_current(list, prev, current);
}

void *linkedlist_peek_first_match_from_rear(linkedlist_structure *list, int (*match)(void *data, void *params), void *args){
    struct linkedlist_node *prev;
    struct linkedlist_node *current;
    int result = __search_procedure(list, &prev, &current, match, args);
    if(result == 0){
        return NULL;
    }
    return current->data;
}

void node_iterator(linkedlist_structure *list, void (*action)(void *data, void *args), void *args){
    struct linkedlist_node *current = list->first;
    while(current != NULL){
        action(current->data, args);
        current = current->next;
    }
}

int __unlink_procedure_of_current(linkedlist_structure *list, struct linkedlist_node *prev, struct linkedlist_node *current){
    void *data = current->data;
    if(prev == NULL){
        list->first = current->next;
    }
    else{
        prev->next = current->next;
    }
    if(current->next == NULL){
        list->last = prev;
    }
    else{
        current->next->prev = prev;
    }
    free(current);
    list->length--;
    return (int) data;
}

int __search_procedure(linkedlist_structure *list, struct linkedlist_node **prev, struct linkedlist_node **current, int (*match)(void *data, void *params), void *args){
    *prev = NULL;
    *current = list->first;
    while(*current != NULL){
        if(match((*current)->data, args)){
            return 1;
        }
        *prev = *current;
        *current = (*current)->next;
    }
    return 0;
}
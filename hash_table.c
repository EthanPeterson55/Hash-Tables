/*
 * In this file, you will write the structures and functions needed to
 * implement a hash table.  Feel free to implement any helper functions
 * you need in this file to implement a hash table.  Make sure to add your
 * name and @oregonstate.edu email address below:
 *
 * Name: Ethan Peterson  
 * Email: petereth@oregonstate.edu
 */

#include <stdlib.h>
#include <stdio.h>

#include "dynarray.h"
#include "list.h"
#include "hash_table.h"


/*
 * This is the structure that represents a hash table.  You must define
 * this struct to contain the data needed to implement a hash table.
 */
struct ht{
    struct dynarray* hash;
    int num_of_elements;
};

struct obj{
    void* key;
    void* value;
};


/*
 * This function should allocate and initialize an empty hash table and
 * return a pointer to it.
 */
struct ht* ht_create(){
    struct ht* hasht = malloc(sizeof(struct ht));
    hasht->hash = dynarray_create();
    hasht->num_of_elements = 0; // initializes num of elements in hash table to 0.

    return hasht;
}

/*
 * This function should free the memory allocated to a given hash table.
 * Note that this function SHOULD NOT free the individual elements stored in
 * the hash table.  That is the responsibility of the caller.
 *
 * Params:
 *   ht - the hash table to be destroyed.  May not be NULL.
 */
void ht_free(struct ht* ht){

    for (int i = 0; i < dynarray_capacity(ht->hash); i++)
    {
        struct list* list = dynarray_get(ht->hash, i);
        struct node* curr = get_head(list); // creates pointer to the head of the linked list at the index generated form hash function.
        while(curr != NULL){
            struct obj* temp = get_val(curr); // creates obj pointer to the value at the node in the linked list.
            free(temp); // frees the obj
            curr = get_next(curr); // increments through the list.
        }
        list_free(list); // frees memory for the linked list.
        
    }

    dynarray_free(ht->hash); // frees memeory for dynarray
    free(ht); // frees memory for hashtable.
    
    return;
}

/*
 * This function should return 1 if the specified hash table is empty and
 * 0 otherwise.
 *
 * Params:
 *   ht - the hash table whose emptiness is to be checked.  May not be
 *     NULL.
 *
 * Return:
 *   Should return 1 if ht is empty and 0 otherwise.
 */
int ht_isempty(struct ht* ht){
    if(ht->num_of_elements == 0){ // chekcs if num of elemtns equals 06
        return 1;
    }else{
        return 0;
    }
}


/*
 * This function returns the size of a given hash table (i.e. the number of
 * elements stored in it, not the capacity).
 */
int ht_size(struct ht* ht){
    return ht->num_of_elements; // returns num of elemtns in hash table.
}


/*
 * This function takes a key, maps it to an integer index value in the hash table,
 * and returns it. The hash algorithm is totally up to you. Make sure to consider
 * Determinism, Uniformity, and Speed when design the hash algorithm
 *
 * Params:
 *   ht - the hash table into which to store the element.  May not be NULL.
 *   key - the key of the element to be stored
 *   convert - pointer to a function that can be passed the void* key from
 *     to convert it to a unique integer hashcode
 */
int ht_hash_func(struct ht* ht, void* key, int (*convert)(void*)){
    int hash_idx = convert(key) % dynarray_capacity(ht->hash); // converts void* ket to int value and then mods with capacity of dynarray to get an index.
    return hash_idx;
}

 /*********************************************************************
** Function: create_obj()
** Description: creates a new obj and sets value and key to value and key passed into function then returns pointer to obj
** Parameters: void* key, void* value
** Pre-Conditions: key and value
** Post-Conditions: returns pointer to new obj.
*********************************************************************/

struct obj* create_obj(void* key, void* value){
    struct obj* newobj = malloc(sizeof(struct obj)); // allocates memory for new obj
    newobj->key = key; // sets key value of new obj to key
    newobj->value = value;// sets value of new obj to value
    return newobj;
}

 /*********************************************************************
** Function: rehash()
** Description: resizes the hash tables capacity to double its original capacity and rehashes all node in hash table % new capacity.
** Parameters: struct ht* ht, int (*convert)(void*)
** Pre-Conditions: hash table created.
** Post-Conditions: ht is resized and nodes are rehashed.
*********************************************************************/

void rehash(struct ht* ht, int (*convert)(void*)){
    
    int new_capacity = 2 * dynarray_capacity(ht->hash);

    set_capacity(ht->hash, new_capacity); // sets new_capacity to 

    void** newd = malloc(new_capacity * sizeof(void*)); // alloactes memory for new data with new capacity.

    for (int i = 0; i < dynarray_capacity(ht->hash); i++)
    {
        newd[i] = NULL; // sets each index of new data to NULL
    }

    void** old = get_data(ht->hash); // creates void** to the old data before resize/rehash.

    set_data(ht->hash, newd); // sets hash tables data to the new data created.

    for (int i = 0; i < new_capacity / 2; i++)
    {
        struct list* list = old[i];
        if(list != NULL){
            struct node* node = get_head(list);

            while(node != NULL){
                struct obj* curr = get_val(node);

                int new_idx = ht_hash_func(ht, curr->key, convert); // rehashes the index for each obj in ht.

                dynarray_insert(ht->hash, curr, new_idx); // inserts the new obj into the new data.

                node = get_next(node); // increments through linked list.
            }
        }
    }

    for (int i = 0; i < new_capacity/2; i++)
    {
        if (old[i] != NULL)
        {
            list_free(old[i]); // frees the old_datas memory allocated
        }
    }
    free(old); // frees old_data.
    
}

 /*********************************************************************
** Function: get_load()
** Description: calculates and returns the load factor for a given hashtable
** Parameters: struct ht* ht
** Pre-Conditions: hash table created.
** Post-Conditions: laod factor of hashtable returned.
*********************************************************************/

int get_loadf(struct ht* ht){
    int load = ht->num_of_elements / dynarray_capacity(ht->hash); // calculates the load factor for a hash table.

    return load;
}


/*
 * This function should insert a given element into a hash table with a
 * specified key.  Note that you cannot have two same keys in one hash table.
 * If the key already exists, update the value associated with the key.  
 * This function is passed a function pointer that is used to convert the key (void*) 
 * to a unique hashcode (int). 
 * Resolution of collisions is requried, use either chaining or open addressing.
 * If using chaining, double the number of buckets when the load factor is >= 4
 * If using open addressing, double the array capacity when the load factor is >= 0.75
 * load factor = (number of elements) / (hash table capacity)
 *
 * Params:
 *   ht - the hash table into which to insert an element.  May not be NULL.
 *   key - the key of the element
 *   value - the value to be inserted into ht.
 *   convert - pointer to a function that can be passed the void* key from
 *     to convert it to a unique integer hashcode
 */


void ht_insert(struct ht* ht, void* key, void* value, int (*convert)(void*)){
    struct dynarray* hash = ht->hash;

    int loadfactor = get_loadf(ht);

    if(loadfactor >= 4){ // checks if load factor is >= 4
        rehash(ht, convert); // rehashes ht.
    } 

    int hash_idx = ht_hash_func(ht, key, convert); // gets the hash index for the new obj

    if(dynarray_get(ht->hash, hash_idx) == NULL){ // checks if ht is empty at the hash_idx
        dynarray_insert(hash, create_obj(key, value), hash_idx); // inserts at that index.
        ht->num_of_elements++; // increments num of elements in ht.
        return;
    }

    struct node* curr = get_head(dynarray_get(ht->hash, hash_idx));

    while(curr != NULL){
        struct obj* node = get_val(curr);
        if(convert(node->key) == convert(key)){ // checks if key already exists in hash table 
            node->value = value; // repalces the value at the key with the new value.
            return;
        }else{
            curr = get_next(curr); // icnrements through linked list.
        }
    }

    dynarray_insert(hash, create_obj(key, value), hash_idx);
    ht->num_of_elements++;

    return;
}


/*
 * This function should search for a given element in a hash table with a
 * specified key provided.   
 * This function is passed a function pointer that is used to convert the key (void*) 
 * to a unique hashcode (int). 
 * If the key is found, return the corresponding value (void*) of the element,
 * otherwise, return NULL
 *
 * Params:
 *   ht - the hash table into which to loop up for an element.  May not be NULL.
 *   key - the key of the element to search for
 *   convert - pointer to a function that can be passed the void* key from
 *     to convert it to a unique integer hashcode
 */
void* ht_lookup(struct ht* ht, void* key, int (*convert)(void*)){
    int hash_idx = ht_hash_func(ht, key, convert);

    struct list* list = dynarray_get(ht->hash, hash_idx);

    if(list == NULL){ // if list at hash_idx is empty then return NULL
        return NULL;
    }else{

        struct node* head = get_head(dynarray_get(ht->hash, hash_idx));

        while(head != NULL){
            struct obj* curr = get_val(head);

            if(convert(curr->key) == convert(key)){ // searches for the same key.
                return curr->value; // returns value of obj.

            }else{
                head = get_next(head); // increments linked list.
            }
        }
    }
    return NULL;
}


/*
 * This function should remove a given element in a hash table with a
 * specified key provided.   
 * This function is passed a function pointer that is used to convert the key (void*) 
 * to a unique hashcode (int). 
 * If the key is found, remove the element and return, otherwise, do nothing and return 
 *
 * Params:
 *   ht - the hash table into which to remove an element.  May not be NULL.
 *   key - the key of the element to remove
 *   convert - pointer to a function that can be passed the void* key from
 *     to convert it to a unique integer hashcode
 */
void ht_remove(struct ht* ht, void* key, int (*convert)(void*)){
    int hash_idx = ht_hash_func(ht, key, convert);

    if(ht_lookup(ht, key, convert) == NULL){ // if key does not exist in list
        return; // return and dont remove.
    }

    struct list* bucket = dynarray_get(ht->hash, hash_idx); // 
    struct node* curr = get_head(bucket);

    while(curr != NULL){
        struct obj* node = get_val(curr);
        if(convert(node->key) == convert(key)){ // checks for the key we want to remove.
            list_remove(dynarray_get(ht->hash, hash_idx), node, convert); // removes node from list.
            free(node); // frees memory allocated for node.
            ht->num_of_elements--; // decrements num of elements in ht.
            return;
        }else{
            curr = get_next(curr); // increments through linked list.
        }
    }
    
    
    return;
} 

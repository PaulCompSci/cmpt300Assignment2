#include "list.h"
#include <stddef.h>
#include <stdio.h>

static Node nodePool[LIST_MAX_NUM_NODES];
static List listHeadArray[LIST_MAX_NUM_HEADS];
static Node *freeNodes[LIST_MAX_NUM_NODES];
static int initializerFlag = 0;
static int nextFreeListIndex = 0;
int freeNodeCount = 0;
int listCount = 0;

void pushToFreeNodeStack(Node *node)
{
    if (freeNodeCount < LIST_MAX_NUM_NODES)
    {
        node->item = NULL;
        node->next = NULL;
        node->previous = NULL;
        freeNodes[freeNodeCount++] = node;
    }
}
int listHeadCount()
{
    int freeCount = 0;
    for (int i = 0; i < LIST_MAX_NUM_HEADS; i++)
        if (listHeadArray[i].flag == 0)
             freeCount++;
        
    return freeCount;
}

// Makes a new, empty list, and returns its reference on success.
// Returns a NULL pointer on failure.
// Makes a new, empty list, and returns its reference on success.
// Returns a NULL pointer on failure.
List *List_create()
{
    if (!initializerFlag)
    {
        for (int i = 0; i < LIST_MAX_NUM_HEADS; i++)
            listHeadArray[i].flag = 0; // Mark as unused
        
        for (int i = 0; i < LIST_MAX_NUM_NODES; i++)
            freeNodes[i] = &nodePool
        [i];
        
        freeNodeCount = LIST_MAX_NUM_NODES;
        initializerFlag = 1;
    }

   
    if (nextFreeListIndex < LIST_MAX_NUM_HEADS)
    {
        List *newList = &listHeadArray[nextFreeListIndex];
        newList->flag = 1; 
        newList->size = 0;
        newList->head = NULL;
        newList->tail = NULL;
        newList->current = NULL;
        newList->outOfBounds = LIST_OOB_START;
        listCount++;
        nextFreeListIndex++; 
        return newList;
    }

    return NULL;
}

// Returns the number of items in pList.
int List_count(List *pList)
{
    if (pList == NULL)
        return -1;

    return pList->size;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void *List_first(List *pList)
{
    if (pList == NULL)
        return NULL;
    
    if (pList->head == NULL)
    {
        pList->current = NULL;
        pList->outOfBounds = LIST_OOB_START;
        return NULL;
    }

    pList->current = pList->head;
    pList->outOfBounds = LIST_OOB_END;


    return pList->head->item;
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void *List_last(List *pList)
{
    if (pList == NULL)
        return NULL; 
    

    if (pList->tail == NULL)
    {
        pList->current = NULL;
        pList->outOfBounds = LIST_OOB_END;
        return NULL; 
    }
    pList->current = pList->tail;
    pList->outOfBounds = LIST_OOB_END; 

    return pList->tail->item; 
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer
// is returned and the current item is set to be beyond end of pList.
void *List_next(List *pList)
{
    if (pList == NULL)
        return NULL; 

    if (pList->current == NULL)
    {
        if (pList->outOfBounds == LIST_OOB_START)
            pList->current = pList->head; 
        
        else
            return NULL; 
    }
    else
        pList->current = pList->current->next;
    

    if (pList->current == NULL)
    {
        pList->outOfBounds = LIST_OOB_END;
        return NULL;
    }
    else
    {
        pList->outOfBounds = LIST_OOB_END; 
        return pList->current->item;       
    }
}

// Backs up pList's current item by one, and returns a pointer to the new current item.
// If this operation backs up the current item beyond the start of the pList, a NULL pointer
// is returned and the current item is set to be before the start of pList.
void *List_prev(List *pList)
{
    if (pList == NULL)
        return NULL; 
    

    if (pList->current == NULL)
    {
        if (pList->outOfBounds == LIST_OOB_END)
             pList->current = pList->tail; 
        else
            return NULL;    
    }
    else
        pList->current = pList->current->previous;
    


    if (pList->current == NULL)
    {
        pList->outOfBounds = LIST_OOB_START; 
        return NULL;
    }
    else
    {
        pList->outOfBounds = LIST_OOB_END; 
        return pList->current->item;       
    }
}


// Returns a pointer to the current item in pList.
void *List_curr(List *pList)
{
    // Ensure list is not NULL
    if (pList == NULL)
    {
        return NULL;
    }

    // Ensure current is not NULL and is within bounds
    if (pList->current == NULL || pList->outOfBounds != LIST_OOB_END)
    {
        return NULL;
    }

    return pList->current->item;
}

// Adds the new item to pList directly after the current item, and makes item the current item.
// If the current pointer is before the start of the pList, the item is added at the start. If
// the current pointer is beyond the end of the pList, the item is added at the end.
// Returns 0 on success, -1 on failure.
int List_insert_after(List *pList, void *pItem)
{

    if (pList == NULL || pItem == NULL)
        return LIST_FAIL;


    if (freeNodeCount <= 0)
        return LIST_FAIL;
    
    Node *newNode = freeNodes[--freeNodeCount];
    newNode->item = pItem;


    if (pList->current == NULL)
    { 
        if (pList->head == NULL)
        {
            newNode->next = NULL;
            newNode->previous = NULL;
            pList->head = newNode;
            pList->tail = newNode;
        }
    
        else if (pList->outOfBounds == LIST_OOB_END)
        {
            newNode->next = NULL;
            newNode->previous = pList->tail;
            pList->tail->next = newNode;
            pList->tail = newNode;
        }

        else if (pList->outOfBounds == LIST_OOB_START)
        {
            newNode->next = pList->head;
            newNode->previous = NULL;
            if (pList->head != NULL)
                pList->head->previous = newNode;
            
            pList->head = newNode;
            if (pList->tail == NULL)
                pList->tail = newNode;
            
        }
    }

    else
    {
        newNode->next = pList->current->next;
        newNode->previous = pList->current;
        if (pList->current->next != NULL)
            pList->current->next->previous = newNode;
    
        else
            pList->tail = newNode;
        
        pList->current->next = newNode;
    }


    pList->current = newNode;
    pList->outOfBounds = LIST_OOB_END; 
    pList->size++;

    return LIST_SUCCESS;
}

// Adds item to pList directly before the current item, and makes the new item the current one.
// If the current pointer is before the start of the pList, the item is added at the start.
// If the current pointer is beyond the end of the pList, the item is added at the end.
// Returns 0 on success, -1 on failure.
int List_insert_before(List *pList, void *pItem)
{
    if (pList == NULL || freeNodeCount <= 0)
        return LIST_FAIL;
    

    Node *newNode = freeNodes[--freeNodeCount];
    newNode->item = pItem;


    if (pList->head == NULL)
    {
        pList->head = newNode;
        pList->tail = newNode;
        newNode->next = NULL;
        newNode->previous = NULL;
    }
    else if (pList->outOfBounds == LIST_OOB_START || pList->current == NULL || pList->current == pList->head)
    {
        newNode->next = pList->head;
        newNode->previous = NULL;
        if (pList->head != NULL)
            pList->head->previous = newNode;
        
        pList->head = newNode;
        if (pList->tail == NULL)
            pList->tail = newNode;
        
    }
    else
    {
        newNode->next = pList->current;
        newNode->previous = pList->current->previous;
        pList->current->previous = newNode;
        if (newNode->previous != NULL)
            newNode->previous->next = newNode;
        
    }


    pList->current = newNode;
    pList->size++;
    pList->outOfBounds = LIST_OOB_END;

    return LIST_SUCCESS;
}

// Adds item to the end of pList, and makes the new item the current one.
// Returns 0 on success, -1 on failure.
// Adds item to the end of pList, and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int List_append(List *pList, void *pItem)
{
    if (pList == NULL || pItem == NULL)
        return LIST_FAIL;
    

    if (freeNodeCount <= 0)
        return LIST_FAIL;
    

    Node *newNode = freeNodes[--freeNodeCount];
    newNode->item = pItem;
    newNode->next = NULL; 
    if (pList->head == NULL)
    {
        pList->head = newNode;
        pList->tail = newNode;
        newNode->previous = NULL; 
    }
    else
    {
        newNode->previous = pList->tail;
        pList->tail->next = newNode;
        pList->tail = newNode;
    }


    pList->current = newNode;
    pList->outOfBounds = LIST_OOB_END;
    pList->size++; 

    return LIST_SUCCESS;
}

// Adds item to the front of pList, and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int List_prepend(List *pList, void *pItem)
{
    if (pList == NULL || freeNodeCount <= 0)
        return LIST_FAIL;
    

    Node *newNode = freeNodes[--freeNodeCount];
    newNode->item = pItem;
    newNode->next = pList->head;
    newNode->previous = NULL;

    if (pList->head != NULL)
        pList->head->previous = newNode;
    
    else
    
        pList->tail = newNode;
    

    pList->head = newNode;
    pList->current = newNode;
    pList->size++;
    pList->outOfBounds = LIST_OOB_END;

    return LIST_SUCCESS;
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void *List_remove(List *pList)
{
    if (pList == NULL || pList->current == NULL || pList->outOfBounds != LIST_OOB_END)
        return NULL;
    

    void *itemToRemove = pList->current->item;
    Node *nodeToRemove = pList->current;

    if (pList->head == pList->current && pList->tail == pList->current)
    {
        // Removing the only node in the list
        pList->head = NULL;
        pList->tail = NULL;
        pList->current = NULL;
        pList->outOfBounds = LIST_OOB_START;
    }
    else if (pList->head == pList->current)
    {
        pList->head = pList->current->next;
        if (pList->head)
            pList->head->previous = NULL;
        
        pList->current = pList->head;
    }
    else if (pList->tail == pList->current)
    {
        pList->tail = pList->current->previous;
        if (pList->tail)
            pList->tail->next = NULL;
        
        pList->current = pList->tail;
    }
    else
    {
        pList->current->previous->next = pList->current->next;
        pList->current->next->previous = pList->current->previous;
        pList->current = pList->current->next;
    }

    nodeToRemove->item = NULL;
    nodeToRemove->next = NULL;
    nodeToRemove->previous = NULL;


    pushToFreeNodeStack(nodeToRemove);


    pList->size--;

    return itemToRemove;
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void *List_trim(List *pList)
{
    if (pList == NULL || pList->head == NULL)
        return NULL;
    

    void *itemToRemove = pList->tail->item;

    if (pList->head == pList->tail)
    {
        pushToFreeNodeStack(pList->tail);
        pList->head = NULL;
        pList->tail = NULL;
        pList->current = NULL; 
    }
    else
    {
        Node *newTail = pList->tail->previous;
        newTail->next = NULL;


        pushToFreeNodeStack(pList->tail);

        pList->tail = newTail;
        pList->current = newTail;
    }

    pList->size--;

    return itemToRemove;
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1.
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List *pList1, List *pList2)
{
    if (pList1 == NULL || pList2 == NULL)
        return;

    if (pList2->head == NULL)
        return; 

    if (pList1->head == NULL)
    {
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
    }
    else
    {
        pList1->tail->next = pList2->head;
        pList2->head->previous = pList1->tail;
        pList1->tail = pList2->tail;
    }

    pList1->size += pList2->size;

    pList2->head = NULL;
    pList2->tail = NULL;
    pList2->current = NULL;
    pList2->size = 0;
    pList2->outOfBounds = LIST_OOB_START;


    listCount--;

}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item.
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are
// available for future operations.
typedef void (*FREE_FN)(void *pItem);
void List_free(List *pList, FREE_FN pItemFreeFn)
{
    if (pList == NULL)
        return;

    Node *currentNode = pList->head;
    while (currentNode != NULL)
    {
        Node *nextNode = currentNode->next;

        if (pItemFreeFn != NULL && currentNode->item != NULL)
            pItemFreeFn(currentNode->item);
        

        currentNode->item = NULL;
        currentNode->next = NULL;
        currentNode->previous = NULL;
        pushToFreeNodeStack(currentNode);

        currentNode = nextNode;
    }

    pList->size = 0;
    pList->head = NULL;
    pList->tail = NULL;
    pList->current = NULL;
    pList->outOfBounds = LIST_OOB_START;

    int listHeadIndex = pList - listHeadArray;


    if (listHeadIndex >= 0 && listHeadIndex < LIST_MAX_NUM_HEADS)
    {
        pList->flag = 0;
        if (listHeadIndex < nextFreeListIndex)
            nextFreeListIndex = listHeadIndex;
        
    }

    listCount--; 
}

// Search pList, starting at the current item, until the end is reached or a match is found.
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match,
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator.
//
// If a match is found, the current pointer is left at the matched item and the pointer to
// that item is returned. If no match is found, the current pointer is left beyond the end of
// the list and a NULL pointer is returned.
//
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
typedef bool (*COMPARATOR_FN)(void *pItem, void *pComparisonArg);
void *List_search(List *pList, COMPARATOR_FN pComparator, void *pComparisonArg)
{
    if (pList == NULL || pComparator == NULL)
    {
        printf("I am here");
        return NULL;
    }


    if (pList->outOfBounds == LIST_OOB_START || pList->current == NULL)
    {
        pList->current = pList->head;
        pList->outOfBounds = LIST_OOB_END; 
    }

    while (pList->current != NULL)
    {
        if (pComparator(pList->current->item, pComparisonArg))
            return pList->current->item; 

        pList->current = pList->current->next;
    }

    pList->outOfBounds = LIST_OOB_END;
    return NULL;
}

// Function to print all elements in the list
void List_print(List *pList)
{

    if (pList == NULL)
    {
        printf("List is NULL.\n");
        return;
    }

    Node *node = pList->head; 

    if (node == NULL)
    {
        printf("List is empty.\n");
    }
    else
    {
        printf("List elements: ");
        while (node != NULL)
        {
            printf("%d ", *((int *)node->item)); 
            node = node->next;                   
        }
        printf("\n");
    }

}

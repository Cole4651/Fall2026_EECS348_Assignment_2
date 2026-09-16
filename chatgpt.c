#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMAILS 1000

typedef struct {
    char sender[30];
    char subject[200];
    char date[11];
    int priority;
} Email;

typedef struct {
    Email heap[MAX_EMAILS];
    int size;
} MaxHeap;

/* Get priority based on sender category */
int getPriority(char sender[]) {
    if (strcmp(sender, "Boss") == 0)
        return 5;
    else if (strcmp(sender, "Subordinate") == 0)
        return 4;
    else if (strcmp(sender, "Peer") == 0)
        return 3;
    else if (strcmp(sender, "ImportantPerson") == 0)
        return 2;
    else
        return 1;
}

/* Convert date to a number so newer dates are larger */
int dateValue(char date[]) {
    int month, day, year;

    sscanf(date, "%d-%d-%d", &month, &day, &year);

    return year * 10000 + month * 100 + day;
}

/* Determine if email a has higher priority than email b */
int higherPriority(Email a, Email b) {
    if (a.priority != b.priority)
        return a.priority > b.priority;

    return dateValue(a.date) > dateValue(b.date);
}

/* Swap two emails */
void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

/* Move an email upward in the heap */
void heapifyUp(MaxHeap *heap, int index) {
    int parent;

    while (index > 0) {
        parent = (index - 1) / 2;

        if (higherPriority(heap->heap[index], heap->heap[parent])) {
            swap(&heap->heap[index], &heap->heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

/* Move an email downward in the heap */
void heapifyDown(MaxHeap *heap, int index) {
    int left, right, largest;

    while (1) {
        left = 2 * index + 1;
        right = 2 * index + 2;
        largest = index;

        if (left < heap->size &&
            higherPriority(heap->heap[left], heap->heap[largest])) {
            largest = left;
        }

        if (right < heap->size &&
            higherPriority(heap->heap[right], heap->heap[largest])) {
            largest = right;
        }

        if (largest != index) {
            swap(&heap->heap[index], &heap->heap[largest]);
            index = largest;
        } else {
            break;
        }
    }
}

/* Add an email to the MaxHeap */
void insert(MaxHeap *heap, Email email) {
    if (heap->size >= MAX_EMAILS)
        return;

    heap->heap[heap->size] = email;
    heapifyUp(heap, heap->size);
    heap->size++;
}

/* Remove the highest-priority email */
Email removeMax(MaxHeap *heap) {
    Email removed = heap->heap[0];

    heap->heap[0] = heap->heap[heap->size - 1];
    heap->size--;

    if (heap->size > 0)
        heapifyDown(heap, 0);

    return removed;
}

/* Display the highest-priority email without removing it */
void nextEmail(MaxHeap *heap) {
    if (heap->size == 0)
        return;

    printf("Next email:\n");
    printf("Sender: %s\n", heap->heap[0].sender);
    printf("Subject: %s\n", heap->heap[0].subject);
    printf("Date: %s\n", heap->heap[0].date);
}

int main(int argc, char *argv[]) {
    MaxHeap heap;
    heap.size = 0;

    FILE *file;

    /* Open the test file */
    if (argc > 1)
        file = fopen(argv[1], "r");
    else
        file = stdin;

    if (file == NULL) {
        printf("Could not open file.\n");
        return 1;
    }

    char line[300];

    while (fgets(line, sizeof(line), file) != NULL) {

        /* Remove newline */
        line[strcspn(line, "\n")] = '\0';

        /* EMAIL command */
        if (strncmp(line, "EMAIL ", 6) == 0) {
            Email email;

            char *data = line + 6;

            char *sender = strtok(data, ",");
            char *subject = strtok(NULL, ",");
            char *date = strtok(NULL, ",");

            if (sender != NULL && subject != NULL && date != NULL) {
                strcpy(email.sender, sender);
                strcpy(email.subject, subject);
                strcpy(email.date, date);

                email.priority = getPriority(email.sender);

                insert(&heap, email);
            }
        }

        /* NEXT command */
        else if (strcmp(line, "NEXT") == 0) {
            nextEmail(&heap);
        }

        /* READ command */
        else if (strcmp(line, "READ") == 0) {
            if (heap.size > 0)
                removeMax(&heap);
        }

        /* COUNT command */
        else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap.size);
        }
    }

    if (file != stdin)
        fclose(file);

    return 0;
}
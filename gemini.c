#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

typedef struct {
    char sender[32];
    char subject[128];
    char date_str[11];
    int priority_score;
    int date_key;
} Email;

typedef struct {
    Email *data;
    int size;
    int capacity;
} MaxHeap;

// Helper: Convert sender category string to priority integer score
int get_sender_priority(const char *sender) {
    if (strcmp(sender, "Boss") == 0) return 5;
    if (strcmp(sender, "Subordinate") == 0) return 4;
    if (strcmp(sender, "Peer") == 0) return 3;
    if (strcmp(sender, "ImportantPerson") == 0) return 2;
    if (strcmp(sender, "OtherPerson") == 0) return 1;
    return 0;
}

// Helper: Convert MM-DD-YYYY to YYYYMMDD integer for chronological comparison
int parse_date_key(const char *date_str) {
    int month, day, year;
    sscanf(date_str, "%d-%d-%d", &month, &day, &year);
    return year * 10000 + month * 100 + day;
}

// Compare two emails: returns >0 if 'a' has higher priority than 'b'
int compare_emails(Email a, Email b) {
    if (a.priority_score != b.priority_score) {
        return a.priority_score - b.priority_score;
    }
    // Newer date takes precedence
    return a.date_key - b.date_key;
}

// Heap initialization
MaxHeap* create_heap(int initial_capacity) {
    MaxHeap *heap = (MaxHeap*)malloc(sizeof(MaxHeap));
    heap->capacity = initial_capacity;
    heap->size = 0;
    heap->data = (Email*)malloc(sizeof(Email) * heap->capacity);
    return heap;
}

void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(MaxHeap *heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (compare_emails(heap->data[index], heap->data[parent]) > 0) {
            swap(&heap->data[index], &heap->data[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

void heapify_down(MaxHeap *heap, int index) {
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < heap->size && compare_emails(heap->data[left], heap->data[largest]) > 0) {
            largest = left;
        }
        if (right < heap->size && compare_emails(heap->data[right], heap->data[largest]) > 0) {
            largest = right;
        }

        if (largest != index) {
            swap(&heap->data[index], &heap->data[largest]);
            index = largest;
        } else {
            break;
        }
    }
}

void insert_email(MaxHeap *heap, Email email) {
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;
        heap->data = (Email*)realloc(heap->data, sizeof(Email) * heap->capacity);
    }
    heap->data[heap->size] = email;
    heapify_up(heap, heap->size);
    heap->size++;
}

void pop_max(MaxHeap *heap) {
    if (heap->size == 0) return;
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    heapify_down(heap, 0);
}

void free_heap(MaxHeap *heap) {
    free(heap->data);
    free(heap);
}

int main() {
    MaxHeap *heap = create_heap(10);
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), stdin)) {
        // Strip trailing newline characters
        line[strcspn(line, "\r\n")] = 0;

        if (strncmp(line, "EMAIL ", 6) == 0) {
            Email e;
            char *payload = line + 6;
            char *token = strtok(payload, ",");

            if (token) strncpy(e.sender, token, sizeof(e.sender));
            token = strtok(NULL, ",");
            if (token) strncpy(e.subject, token, sizeof(e.subject));
            token = strtok(NULL, ",");
            if (token) strncpy(e.date_str, token, sizeof(e.date_str));

            e.priority_score = get_sender_priority(e.sender);
            e.date_key = parse_date_key(e.date_str);

            insert_email(heap, e);
        } 
        else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap->size);
        } 
        else if (strcmp(line, "NEXT") == 0) {
            if (heap->size > 0) {
                Email top = heap->data[0];
                printf("Next email:\n");
                printf("Sender: %s\n", top.sender);
                printf("Subject: %s\n", top.subject);
                printf("Date: %s\n", top.date_str);
            }
        } 
        else if (strcmp(line, "READ") == 0) {
            if (heap->size > 0) {
                pop_max(heap);
            }
        }
    }

    free_heap(heap);
    return 0;
}
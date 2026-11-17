
#include <stdio.h>
#include <string.h>
#include "huffman.h"

#define MAX_TEXT 200
#define MAX_SYMBOLS 256
#define MAX_NODES (2 * MAX_SYMBOLS - 1)
#define MAX_CODE_LENGTH 256

typedef struct {
    unsigned char symbol;
    unsigned int frequency;
    int left;
    int right;
} HuffmanNode;

typedef struct {
    int heap[MAX_SYMBOLS];
    int size;
} MinHeap;

static HuffmanNode nodes[MAX_NODES];
static char codes[MAX_SYMBOLS][MAX_CODE_LENGTH];

/* Swap two heap elements */
static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* Maintain minimum heap property */
static void heapify(MinHeap *heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size &&
        nodes[heap->heap[left]].frequency <
        nodes[heap->heap[smallest]].frequency) {
        smallest = left;
    }

    if (right < heap->size &&
        nodes[heap->heap[right]].frequency <
        nodes[heap->heap[smallest]].frequency) {
        smallest = right;
    }

    if (smallest != index) {
        swap(&heap->heap[index], &heap->heap[smallest]);
        heapify(heap, smallest);
    }
}

/* Insert node into min heap */
static void insertHeap(MinHeap *heap, int nodeIndex) {
    int i = heap->size;
    heap->heap[heap->size++] = nodeIndex;

    while (i > 0) {
        int parent = (i - 1) / 2;

        if (nodes[heap->heap[parent]].frequency <=
            nodes[heap->heap[i]].frequency) {
            break;
        }

        swap(&heap->heap[parent], &heap->heap[i]);
        i = parent;
    }
}

/* Remove minimum frequency node */
static int extractMin(MinHeap *heap) {
    if (heap->size <= 0) {
        return -1;
    }

    int minimum = heap->heap[0];

    heap->heap[0] = heap->heap[heap->size - 1];
    heap->size--;

    if (heap->size > 0) {
        heapify(heap, 0);
    }

    return minimum;
}

/* Check whether node is a leaf */
static int isLeaf(int index) {
    return nodes[index].left == -1 &&
           nodes[index].right == -1;
}

/* Generate Huffman codes recursively */
static void generateCodes(int root, char code[],
                          int depth) {

    if (isLeaf(root)) {
        if (depth == 0) {
            code[depth++] = '0';
        }

        code[depth] = '\0';

        strcpy(codes[nodes[root].symbol], code);
        return;
    }

    code[depth] = '0';
    generateCodes(nodes[root].left, code, depth + 1);

    code[depth] = '1';
    generateCodes(nodes[root].right, code, depth + 1);
}

/* Build Huffman tree */
static int buildHuffmanTree(unsigned int frequency[]) {
    MinHeap heap;
    heap.size = 0;

    int nodeCount = 0;

    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (frequency[i] > 0) {

            nodes[nodeCount].symbol = (unsigned char)i;
            nodes[nodeCount].frequency = frequency[i];
            nodes[nodeCount].left = -1;
            nodes[nodeCount].right = -1;

            insertHeap(&heap, nodeCount);
            nodeCount++;
        }
    }

    if (heap.size == 0) {
        return -1;
    }

    while (heap.size > 1) {
        int left = extractMin(&heap);
        int right = extractMin(&heap);

        nodes[nodeCount].symbol = 0;
        nodes[nodeCount].frequency =
            nodes[left].frequency + nodes[right].frequency;

        nodes[nodeCount].left = left;
        nodes[nodeCount].right = right;

        insertHeap(&heap, nodeCount);
        nodeCount++;
    }

    return extractMin(&heap);
}

/* Main Huffman Coding function */
void runHuffmanCoding(void) {
    char text[MAX_TEXT];
    char code[MAX_CODE_LENGTH];

    unsigned int frequency[MAX_SYMBOLS] = {0};

    memset(codes, 0, sizeof(codes));

    printf("\n===== HUFFMAN CODING =====\n");

    printf("Enter a message (maximum %d characters): ",
           MAX_TEXT - 1);

    if (fgets(text, sizeof(text), stdin) == NULL) {
        printf("Input error!\n");
        return;
    }

    text[strcspn(text, "\n")] = '\0';

    int length = (int)strlen(text);

    if (length == 0) {
        printf("Empty message! Please enter some text.\n");
        return;
    }

    /* Calculate character frequencies */
    for (int i = 0; i < length; i++) {
        frequency[(unsigned char)text[i]]++;
    }

    printf("\n===== CHARACTER FREQUENCIES =====\n");

    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (frequency[i] > 0) {
            if (i == ' ') {
                printf("SPACE : %u\n", frequency[i]);
            } else {
                printf("%c : %u\n", i, frequency[i]);
            }
        }
    }

    /* Build Huffman tree */
    int root = buildHuffmanTree(frequency);

    if (root == -1) {
        printf("Unable to build Huffman tree!\n");
        return;
    }

    generateCodes(root, code, 0);

    printf("\n===== HUFFMAN CODES =====\n");

    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (frequency[i] > 0) {
            if (i == ' ') {
                printf("SPACE : %s\n", codes[i]);
            } else {
                printf("%c : %s\n", i, codes[i]);
            }
        }
    }

    /* Encode original message */
    char encoded[MAX_TEXT * MAX_CODE_LENGTH];
    encoded[0] = '\0';

    for (int i = 0; i < length; i++) {
        strcat(encoded, codes[(unsigned char)text[i]]);
    }

    printf("\n===== ENCODED MESSAGE =====\n");
    printf("%s\n", encoded);

    int originalBits = length * 8;
    int compressedBits = (int)strlen(encoded);

    printf("\n===== COMPRESSION RESULT =====\n");
    printf("Original message size : %d bits\n", originalBits);
    printf("Encoded message size  : %d bits\n", compressedBits);

    printf("Bits saved            : %d bits\n",
           originalBits - compressedBits);

    printf("Compression ratio     : %.2f%%\n",
           (double)compressedBits * 100 / originalBits);
}

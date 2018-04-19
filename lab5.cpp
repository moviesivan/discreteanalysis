#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

const int alphabet = 27;

struct SuffixTreeNode {
    struct SuffixTreeNode *children[alphabet];

    struct SuffixTreeNode *suffixLink;

    int start;
    int *end;
    
    int suffixIndex;
};

typedef struct SuffixTreeNode Node;

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

int *p;
int counter = 0;

char *text; 
Node *root = NULL; 

Node *lastNewNode = NULL;
Node *activeNode = NULL;

int activeEdge = -1;
int activeLength = 0;

int remainingSuffixCount = 0;
int leafEnd = -1;
int *rootEnd = NULL;
int *splitEnd = NULL;
int size = -1; 

Node *newNode(int start, int *end)
{
    Node *node =(Node*) malloc(sizeof(Node));
    for (int i = 0; i < alphabet; i++)
        node->children[i] = NULL;

    node->suffixLink = root;
    node->start = start;
    node->end = end;

    node->suffixIndex = -1;
    return node;
}

int edgeLength(Node *n) {
    return *(n->end) - (n->start) + 1;
}

int walkDown(Node *currNode)
{
    if (activeLength >= edgeLength(currNode)) {
        activeEdge += edgeLength(currNode);
        activeLength -= edgeLength(currNode);
        activeNode = currNode;
        return 1;
    }
    return 0;
}

void extendSuffixTree(int pos)
{
    leafEnd = pos;
    remainingSuffixCount++;
    lastNewNode = NULL;

    while(remainingSuffixCount > 0) {
        if (activeLength == 0)
            activeEdge = pos; 

        if (activeNode->children[( text[activeEdge]- 'a' + 1) > 0 ? (text[activeEdge] - 'a' + 1) : 0] == NULL) {
            activeNode->children[( text[activeEdge]- 'a' + 1) > 0 ? (text[activeEdge] - 'a' + 1) : 0] = newNode(pos, &leafEnd);

            if (lastNewNode != NULL) {
                lastNewNode->suffixLink = activeNode;
                lastNewNode = NULL;
            }
        } else {
            Node *next = activeNode->children[( text[activeEdge]- 'a' + 1) > 0 ? (text[activeEdge] - 'a' + 1) : 0];
            if (walkDown(next))
                continue;
            if (text[next->start + activeLength] == text[pos]) {
                if(lastNewNode != NULL && activeNode != root) {
                    lastNewNode->suffixLink = activeNode;
                    lastNewNode = NULL;
                }

                activeLength++;
                break;
            }

            splitEnd = (int*) malloc(sizeof(int));
            *splitEnd = next->start + activeLength - 1;
            Node *split = newNode(next->start, splitEnd);
            activeNode->children[( text[activeEdge]- 'a' + 1) > 0 ? (text[activeEdge] - 'a' + 1) : 0] = split;
            split->children[( text[pos]- 'a' + 1) > 0 ? (text[pos] - 'a' + 1) : 0] = newNode(pos, &leafEnd);
            next->start += activeLength;
            split->children[( text[next->start]- 'a' + 1) > 0 ? (text[next->start] - 'a' + 1) : 0] = next;

            if (lastNewNode != NULL) {
                lastNewNode->suffixLink = split;
            }
            lastNewNode = split;
        }

        remainingSuffixCount--;
        if (activeNode == root && activeLength > 0) {
            activeLength--;
            activeEdge = pos - remainingSuffixCount + 1;
        } else if (activeNode != root) {
            activeNode = activeNode->suffixLink;
        }
    }
}



void buildSuffixArrayByDFS(Node *n, int labelHeight)
{
    if (n == NULL) return;

    int leaf = 1;
    for (int i = 0; i < alphabet; i++) {
        if (n->children[i] != NULL) {
            leaf = 0;
            buildSuffixArrayByDFS(n->children[i], labelHeight + edgeLength(n->children[i]));
        }
    }
    if (leaf == 1) {
        n->suffixIndex = size - labelHeight;
        p[counter++] = n->suffixIndex;
    }
}

void freeSuffixTreeByPostOrder(Node *n)
{
    if (n == NULL)
        return;
    for (int i = 0; i < alphabet; i++) {
        if (n->children[i] != NULL) {
            freeSuffixTreeByPostOrder(n->children[i]);
        }
    }
    if (n->suffixIndex == -1)
        free(n->end);
    free(n);
}

void buildSuffixTree()
{
    size = strlen(text) + 1;
    rootEnd = (int*) malloc(sizeof(int));
    *rootEnd = - 1;

    root = newNode(-1, rootEnd);

    activeNode = root; 
    for (int i=0; i<size; i++)
        extendSuffixTree(i);

}

int main(int argc, char *argv[])
{
// strcpy(text, "abc"); buildSuffixTree();
// strcpy(text, "xabxac#"); buildSuffixTree();
// strcpy(text, "xabxa"); buildSuffixTree();
// strcpy(text, "xabxa$"); buildSuffixTree();
    std::string line;
    getline(std::cin, line);
    text = new char[line.length() + 1];
    strcpy(text, line.c_str());
    buildSuffixTree();
    p = new int[size];
    int labelHeight = 0;
    buildSuffixArrayByDFS(root, labelHeight);
    freeSuffixTreeByPostOrder(root);

    char *pat;
    int m, l_bound, r_bound, l, r, mid, res;
    for (int i = 0; getline(std::cin, line);i++) {
        pat = new char[line.length() + 1];
        strcpy(pat, line.c_str());
        m = strlen(pat);
        l_bound = 0; 
        r = size-1; 
        while (l_bound <= r) {
            mid = l_bound + (r - l_bound)/2;
            res = strncmp(pat, text+p[mid], m);
            if (res <= 0) 
                r = mid - 1;
            else 
                l_bound = mid + 1;
        }
     
        l = 0;
        r_bound = size-1; 
        while (l <= r_bound) {
            mid = l + (r_bound - l)/2;
            res = strncmp(pat, text+p[mid], m);
            if (res < 0) 
                r_bound = mid - 1;
            else l = mid + 1;
        }
        if (l_bound <= r_bound && m > 0) {
            int sizebound = r_bound - l_bound + 1;
            int *answers = new int[sizebound];
            for (int j = 0; l_bound + j <= r_bound; j++)
                answers[j] = p[l_bound + j];
            qsort (answers, sizebound, sizeof(int), compare);
            printf ("%d: %d", i+1, answers[0]+1);
            for (int n=1; n < sizebound; n++)
                printf (", %d", answers[n]+1);
            printf("\n");
            delete[] answers;
        }
        delete[] pat;
    }

    delete[] text;
    delete[] p;
    
    
    return 0;
}
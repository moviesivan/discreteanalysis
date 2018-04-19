#ifndef TRIE_DOT_H
#define TRIE_DOT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct trie_node{
	int suffix; 
	int prefix; 
	int code; 

	int appearances; 
	
	int kid_count; 
	int *children; 
} *trie;


void init(trie *table)
{
	(*table) = (trie) malloc(512*sizeof(struct trie_node));      

	////////// NULL
	(*table)[0].code = 0;
	(*table)[0].suffix = 0;
	(*table)[0].prefix = 0;
	(*table)[0].appearances = 0;
	(*table)[0].kid_count = 0;
	(*table)[0].children = NULL;
	(*table)[0].children = (int*)malloc(2 * sizeof(int));
	
	////////// ESC
	(*table)[1].prefix = 0;
	(*table)[1].code = 1;
	(*table)[1].suffix = 0;
	(*table)[1].appearances = 0;
	(*table)[1].kid_count = 0;
	(*table)[1].children = (int*) malloc(2 * sizeof(int));
	
	////////// EOF
	(*table)[2].prefix = 0;
	(*table)[2].code = 2;
	(*table)[2].suffix = 0;
	(*table)[2].appearances = 0;
	(*table)[2].kid_count = 0;
	(*table)[2].children = (int*) malloc(2 * sizeof(int));
	
	for(int i = 0; i < 256; i++){
		(*table)[i+3].suffix = i;
		(*table)[i+3].prefix = 0;
		(*table)[i+3].code = i+3;

		(*table)[i+3].appearances = 0;
		(*table)[i+3].kid_count = 0;
		(*table)[i+3].children = (int*) malloc(2 * sizeof(int));
	}
}

int findNumBits(int curr_code){
	int num_bits = 0;
	int code_bits = curr_code;
	while(code_bits > 0){
		code_bits = code_bits / 2;
		num_bits++;
	}
	if (num_bits <= 8) 
		return 9;
	return num_bits;
}

void kill_children(trie *table, int total_codes){
	for (int i = 0; i < total_codes; i++){
		if ((*table)[i].children != NULL){
			free((*table)[i].children);
		}
	}
}

int reinit(trie *table, int total_codes, int maxbits)
{
	if (findNumBits(total_codes) > findNumBits(total_codes - 1)){
		if (findNumBits(total_codes) <= maxbits){
			(*table) = (trie) realloc(*table,2*(total_codes)*sizeof(struct trie_node));
			return findNumBits(total_codes);
		}
		return maxbits;
	}
	return findNumBits(total_codes);
	
}
                                    
void insert(trie *table, int pre, int suffix, int code)
{
	(*table)[code].code = code;
	(*table)[code].suffix = suffix;
	(*table)[code].prefix = (*table)[pre].code;
 
	(*table)[code].appearances = 1;
	(*table)[code].kid_count = 0;
	(*table)[code].children = NULL;

	(*table)[pre].children = (int*) realloc((*table)[pre].children, 
		((*table)[pre].kid_count + 1)* sizeof(int));
	(*table)[pre].children[(*table)[pre].kid_count] = code;
	(*table)[pre].kid_count++;

	int i;
	for(i = (*table)[pre].kid_count - 2; i >= 0; i--){
		if ((*table)[(*table)[pre].children[i]].suffix > suffix){
			int temp = (*table)[pre].children[i];
			(*table)[pre].children[i] = code;
			(*table)[pre].children[i+1] = temp;
		} else{
			break;
		}
	}

}

int search(trie *table, int pre, int suffix, int start, int end)
{
	if (start > end)
		return -1;

	int middle = (start + end) / 2;
	int middle_code = (*table)[pre].children[middle];
	int middle_suffix = (*table)[middle_code].suffix;
	if (middle_suffix == suffix)
		return middle_code;
	else if (suffix > middle_suffix){
		return search(table, pre, suffix, middle + 1, end);
	} else{
		return search(table, pre, suffix, start, middle - 1);
	}

}

void print(trie *table, int total_codes){
	printf("Code\tPref\tSuff\tAppearances\n"); 
	for (int i = 0; i < total_codes; i++){
		printf("%i\t%i\t%i\t%i\n", (*table)[i].code, (*table)[i].prefix, (*table)[i].prefix, (*table)[i].appearances);
	}

}

int prune(trie *start, int total_codes){
	trie end;		
	init(&end);
	int new_count = 259;

	int start_count = new_count;

	for (int i = 0; i < new_count; i++){
		end[i].appearances = (*start)[i].appearances/2;
	}
	while(start_count < total_codes){
		(*start)[start_count].appearances /= 2; 
		if ((*start)[start_count].appearances > 0){
			
			insert(&end, (*start)[start_count].prefix, 
				(*start)[start_count].suffix, new_count);

			for (int i = 0; i < (*start)[start_count].kid_count; i++){
				(*start)[(*start)[start_count].children[i]].prefix = new_count;
			}
			new_count++;
			reinit(&end, new_count, 20);
		}
		start_count++;
	}

	kill_children(start, total_codes);
	free(*start);
	*start = end;
	return new_count;
}


#endif

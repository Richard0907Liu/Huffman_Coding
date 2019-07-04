#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#define code_max 32 //
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <map>
#include <cassert>
#include<iostream>
#include <cstring>
#include<fstream>
using namespace std;

typedef struct node
{
	char ch;
	int freq;
	char node_code[code_max] = { 0 };  // can add code[] digit for bigger char
	struct node *next;
	struct node *left;
	struct node *right;
}node;

typedef struct freq_node
{
	char ch;
	int freq;
}freq_node;

node *root = NULL;
node *LL_for_encoding = NULL;
node *Link_node = NULL;
node *f_node;

int char_total = 0;
unsigned int char_freq = 0;
unsigned int total = 0;  // header
int heap_num = 0;
map<unsigned char, unsigned int> sorting_ch;  // do not use map										 
int code_pos = -2;
char code[code_max];  // can add code[] digit for bigger char
char ch_code[code_max];
static char byte;
int padding;

int decode_count = 0;  // for correct char number
node *decode_ptr;

//void count_char_freq(FILE *input, FILE *output);
int encode(const char *input, const char *output);
void Min_heap();
node Min_heap_remove();
void Min_heap_insert(node insert);
void huffman_heap();
void Tree_sorting(node *root);
node *getNode();
void wordCode(node *root, char a);
//void write_bit(FILE *input, FILE *output);
//void search_node(node *root, char ch, FILE *output);
//void bit_writing(char *ch, FILE *output);
/////////// decode ///////////////
int decode(const char *input, const char *output);
void read_header(FILE *input);
void write_byte(FILE *de_input, FILE *de_output);
/*
void count_char_freq(FILE *input, FILE *output)
{
	char ch;


	while ((fread(&ch, sizeof(char), 1, input)) != 0)
	{
		sorting_ch[ch]++;  // do not use map
		total++;
	}

	fwrite(&padding, sizeof(int), 1, output);
	char_total = (int)sorting_ch.size();
	fwrite(&char_total, sizeof(int), 1, output);

	f_node = new node[char_total + 1];
	assert(f_node);
	map<unsigned char, unsigned int>::iterator f;
	int i;
	for (f = sorting_ch.begin(), i = 1; f != sorting_ch.end(); ++f, ++i)
	{
		f_node[i].ch = (*f).first;
		f_node[i].freq = (*f).second;
	}

	heap_num = char_total;
	Min_heap();

	for (i = 1; i < char_total + 1; i++)
	{
		fwrite(&f_node[i].ch, sizeof(char), 1, output);
		fwrite(&f_node[i].freq, sizeof(unsigned int), 1, output);
	}
}
*/
void Min_heap()
{
	node temp;
	for (int i = heap_num; i > 1; i--)  // from bottom right to top
	{
		if (f_node[i].freq < f_node[(int)i / 2].freq)
		{
			temp = f_node[i];
			f_node[i] = f_node[(int)i / 2];
			f_node[(int)i / 2] = temp;
		}
	}

	for (int j = 0; j < 2; j++)  // do twice, make sure sort correctly
	{
		for (int i = 1; i < (int)(heap_num / 2); i++) // sort again from top to bottom 
		{
			if (f_node[i].freq > f_node[i * 2].freq && f_node[i * 2].freq < f_node[(i * 2) + 1].freq)
			{
				temp = f_node[i];
				f_node[i] = f_node[i * 2];
				f_node[i * 2] = temp;
			}
			else if (f_node[i].freq > f_node[(i * 2) + 1].freq && f_node[(i * 2) + 1].freq < f_node[i * 2].freq)
			{
				temp = f_node[i];
				f_node[i] = f_node[(i * 2) + 1];
				f_node[(i * 2) + 1] = temp;
			}
		}
	}
}

node Min_heap_remove()
{
	int i = 1;
	node extract;
	node temp;

	extract = f_node[1];
	f_node[1] = f_node[heap_num];
	heap_num--;

	while (i < (heap_num / 2) + 1)
	{
		if (heap_num == 2 && f_node[i].freq > f_node[i * 2].freq)
		{
			temp = f_node[i];
			f_node[i] = f_node[i * 2];
			f_node[i * 2] = temp;
		}
		if (f_node[i].freq > f_node[i * 2].freq && (f_node[i * 2].freq < f_node[(i * 2) + 1].freq
			|| f_node[i * 2].freq == f_node[(i * 2) + 1].freq))  // i*2 samller
		{
			temp = f_node[i];
			f_node[i] = f_node[i * 2];
			f_node[i * 2] = temp;
			i = (i * 2);
		}
		else if (f_node[i].freq > f_node[(i * 2) + 1].freq && f_node[(i * 2) + 1].freq < f_node[i * 2].freq) // (i*2+1) smaller
		{
			temp = f_node[i];
			f_node[i] = f_node[(i * 2) + 1];
			f_node[(i * 2) + 1] = temp;
			i = (i * 2 + 1);
		}
		else
			i = i * 2;
	}
	return extract; // freq_node
}

void Min_heap_insert(node insert) // have to upheap
{
	int i;
	node temp;

	heap_num++;
	i = heap_num; // last node;
	f_node[heap_num] = insert;

	while (i != 1)
	{
		if (f_node[heap_num].freq < f_node[(int)(heap_num / 2)].freq)
		{
			temp = f_node[heap_num];
			f_node[heap_num] = f_node[(int)(heap_num / 2)];
			f_node[(heap_num) / 2] = temp;
			i = heap_num / 2;
		}
	}
}

void huffman_heap()
{
	node *new_node, *new_node2;
	node *ptr;
	node temp;
	int node_total = char_total;
	int i = 0;
	ptr = root;
	while (i < node_total)
	{
		if (root == NULL)
		{
			new_node = getNode();
			new_node2 = getNode();

			new_node->left = new_node2;
			root = new_node;
		}
		else
		{
			new_node = getNode();
			new_node2 = getNode();

			new_node->left = new_node2;
			ptr = root;
			while (ptr->next != NULL)
				ptr = ptr->next;
			ptr->next = new_node;
		}
		i++;
	}
	ptr = root;

	for (i = 0; i < char_total; i++)  // try to print LL
	{
		temp = Min_heap_remove();
		ptr->freq = temp.freq;
		ptr->left->ch = temp.ch;
		ptr->left->freq = temp.freq;  // add
		ptr = ptr->next;
	}
	Tree_sorting(root);
}

node *getNode()
{
	node *new_node;
	new_node = (node*)malloc(sizeof(node));
	if (new_node == NULL)
	{
		printf("Memory is not enough!!");
		exit(1);
	}
	new_node->freq = 0;
	new_node->next = NULL, new_node->left = NULL;
	new_node->right = NULL;
	return new_node;
}

void Tree_sorting(node *head)
{
	node *ptr, *new_node;
	node *second;
	node *third = NULL;

	ptr = head;  // head
	second = head->next;

	if (second->next != NULL)
		third = head->next->next;
	head = third;
	if (ptr->right == NULL)
	{
		if (second->right == NULL)
		{
			ptr->freq += second->freq;
			ptr->right = second->left;
			ptr->next = NULL;
		}
		else
		{
			ptr->freq += second->freq;
			ptr->right = second;
			ptr->next = NULL;
		}
	}
	else if (second->right == NULL)
	{
		second->freq += ptr->freq;
		second->right = ptr;
		ptr = second;
	}
	else // each has two nodes
	{
		new_node = getNode();
		new_node->freq = ptr->freq + second->freq;
		new_node->right = ptr;
		new_node->left = second;
		ptr = new_node;
	}

	if (head->next == NULL) // last two node, create a node to combine;
	{
		new_node = getNode();
		new_node->freq = ptr->freq + head->freq;  // head
		new_node->right = head;  // head
		new_node->left = ptr;
		root = new_node;

		return;
	}

	if (ptr->freq < third->freq)
	{
		ptr->next = third;
		head = ptr;
		Tree_sorting(head);
	}
	else
	{
		while (ptr->freq == third->freq || ptr->freq > third->freq)
		{
			third = third->next;
			if (third->next == NULL)
			{
				third->next = ptr;
				ptr->next = NULL;
				Tree_sorting(head);
				return;
			}
			if (ptr->freq < third->next->freq) // middle
			{
				ptr->next = third->next;
				third->next = ptr;
				Tree_sorting(head);
				return;
			}
		}
	}
	return;
}

void wordCode(node *ptr, char a)  // left = '0', right = '1'
{
	if (ptr != NULL)
	{
		code_pos++;
		code[code_pos] = a;
		code[(code_pos)+1] = '\0';
		if (code_pos != -1)
			strcpy(ptr->node_code, code);  // solved 
		if (ptr->right == NULL && ptr->left == NULL)
		{
			static int i;// ### LL_for_encoding ###
			if (i == 0)
			{
				LL_for_encoding = ptr;
				Link_node = LL_for_encoding;
				i++;
			}
			else
			{
				Link_node->next = ptr;
				Link_node = ptr;
			}
		}
		wordCode(ptr->left, '0');
		if (code_pos == -1) // run right side, has to reset code
		{
			for (int i = 0; i < code_max; i++) // reset code[];
				code[i] = 0;
		}
		wordCode(ptr->right, '1');
		code_pos--;
	}
	return;
}

void read_header(FILE *input)
{
	char ch_file = '0';
	int freq_file = 0;

	fread(&padding, sizeof(int), 1, input);
	//printf("padding: %d\n", padding);

	//  ## read char_total ###
	fread(&char_total, sizeof(int), 1, input);
	//printf("char_total: %d\n", char_total);
	heap_num = char_total;

	f_node = new node[char_total + 1];
	assert(f_node);

	//  ## read char and frequenct ###
	for (int i = 0; i < char_total; i++)
	{
		fread(&ch_file, sizeof(char), 1, input);
		f_node[i + 1].ch = ch_file;
		fread(&freq_file, sizeof(int), 1, input);
		f_node[i + 1].freq = freq_file;
		total += freq_file;
	}
	//printf("\ntotal frequency: %d", total);

	Min_heap();
}
void write_byte(FILE *de_input, FILE *de_output)
{
	unsigned char temp = 1, get_code;  // temp=1;
	unsigned char ch;
	char external_ch;

	decode_ptr = root;

	while (fread(&ch, sizeof(unsigned char), 1, de_input) != 0)
	{
		if (decode_count + 1 == total)  // last byte 
		{
			for (int i = 7; i > -1 + padding;)
			{
				get_code = ch;
				get_code = (get_code >> i) & temp;
				if (get_code == 1) // go right;
				{
					decode_ptr = decode_ptr->right;
					if (decode_ptr->left == NULL && decode_ptr->right == NULL)
					{
						fwrite(&decode_ptr->ch, sizeof(char), 1, de_output);
						decode_count++;
						decode_ptr = root;
					}
				}
				else if (get_code == 0)  // go left;
				{
					decode_ptr = decode_ptr->left;
					if (decode_ptr->left == NULL && decode_ptr->right == NULL)
					{
						fwrite(&decode_ptr->ch, sizeof(char), 1, de_output);
						decode_count++;
						decode_ptr = root;
					}
				}
				i--;
			}
			return;
		}
		for (int i = 7; i > -1;) // a byte has to be shifted 
		{
			get_code = ch;
			get_code = (get_code >> i) & temp;
			if (get_code == 1) // go right;
			{
				decode_ptr = decode_ptr->right;
				if (decode_ptr->left == NULL && decode_ptr->right == NULL)
				{
					fwrite(&decode_ptr->ch, sizeof(char), 1, de_output);
					decode_count++;
					decode_ptr = root;
				}
			}
			else if (get_code == 0)  // go left;
			{

				decode_ptr = decode_ptr->left;
				if (decode_ptr->left == NULL && decode_ptr->right == NULL)
				{
					fwrite(&decode_ptr->ch, sizeof(char), 1, de_output);
					decode_count++;
					decode_ptr = root;
				}
			}
			i--;
		}
	}
}

int encode(const char *inputfile, const char *outputfile)
{
	FILE *fin, *fout;
	if ((fin = fopen(inputfile, "rb")) == NULL) {
		printf("Cannot open input file");
		return -1;
	}
	if ((fout = fopen(outputfile, "wb")) == NULL) {
		printf("Cannot open output file");
		fclose(fin);
		return -1;
	}
	//count_char_freq(fin, fout);
	fclose(fin);
	huffman_heap();
	free(f_node);
	wordCode(root, '0');

	if ((fin = fopen(inputfile, "rb")) == NULL) {
		printf("Cannot to open input file");
		return -1;
	}
	//write_bit(fin, fout);  // has to open "inputfile" again

	fclose(fin);
	fclose(fout);

	return 0;
}

int decode(const char*input, const char *output)
{
	FILE *de_input, *de_output;

	if ((de_input = fopen(input, "rb")) == NULL) {
		printf("Cannot open input file");
		return -1;
	}

	if ((de_output = fopen(output, "wb")) == NULL) {
		printf("Cannot open output file");
		return -1;
	}
	printf("Decoding...\n");

	read_header(de_input);
	huffman_heap();
	write_byte(de_input, de_output);

	fclose(de_input);
	fclose(de_output);
	return 0;
}

int main(int argc, char **argv) 
{
	//FILE *input, *output;
	char filename[80]; // = "test_unhuff.txt";

	/////	decode	///////////
	if ((strcmp(argv[1], " ") != 0))
	{
		strncpy(filename, argv[1], (strlen(argv[1]) - 4));
		filename[(strlen(argv[1]) - 4)] = '\0';

		decode(argv[1], filename);
		if (remove(argv[1]) == 0)
			printf("\nThe original file deleted successfully");
		else
			printf("\nUnable to delete the file");
	
	}
	else 
	{
		printf("Please key in-\"./hdec <finlename>\"");
	}

	////////////////////////////////

	return 0;
}

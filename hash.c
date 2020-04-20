#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHSIZE 101

//-----------------------------------------------------------------------
typedef struct hash_node hash_node;
struct hash_node {
    struct hash_node* next;
    const char* name;
    char* defn;
};

//-----------------------------------------------------------------------
static hash_node* hashtab[HASHSIZE];    /* pointer table */

//-----------------------------------------------------------------------
unsigned hash(const char* s)
{
    unsigned hashval;

    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

//-----------------------------------------------------------------------
hash_node* hash_lookup(const char* s)
{
    hash_node* np;

    for (np = hashtab[hash(s)]; np != NULL; np = np->next) {
      if (strcmp(s, np->name) == 0)
          return np;  /* found */
    }

    return NULL;        /* not found */
}

//-----------------------------------------------------------------------
hash_node* hash_install(char* name, char* defn)
{
    hash_node* np;
    unsigned hashval;

    if ((np = hash_lookup(name)) == NULL) {  /* not found */
        np = (hash_node*)malloc(sizeof(*np));

        if (np == NULL || (np->name = strdup(name)) == NULL) {
          return NULL;
        }

        hashval = hash(name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;

    } else {      /* already there */
        free((void*)np->defn);  /* free previous defn */
      }

    if ((np->defn = strdup(defn)) == NULL) { return NULL; }

    return np;
}

//-----------------------------------------------------------------------
void print_defn(const char* s) {
  hash_node* p = hash_lookup(s);
  printf("%s: ", s);
  printf("%s\n", p ? p->defn : "Not found");
}

//-----------------------------------------------------------------------
//Exercise 6-5 in K & R
int undef(char * name) {
    hash_node * np1, * np2;

    if ((np1 = hash_lookup(name)) == NULL)  /*  name not found  */
        return 1;

    for ( np1 = np2 = hashtab[hash(name)]; np1 != NULL;
          np2 = np1, np1 = np1->next ) {
        if ( strcmp(name, np1->name) == 0 ) {  /*  name found  */

            /*  Remove node from list  */

            if ( np1 == np2 )
                hashtab[hash(name)] = np1->next;
            else
                np2->next = np1->next;

            /*  Free memory  */

            free((void*)np1->name);
            free((void*)np1->defn);
            free(np1);

            return 0;
        }
    }

    return 1;  /*  name not found  */
}

//-----------------------------------------------------------------------
void hash_print() {

  for (int i = 0; i < HASHSIZE; i++) {
    printf("%3d -- \n", i);
    hash_node* p = hashtab[i];

    while (p != NULL) {
      print_defn(p->name);
      p = p->next;
    }
  }

}

//-----------------------------------------------------------------------
static void node_clear(hash_node* p) {
  if (p == NULL) { return; }
  free((void*)p->name);
  free(p->defn);
  free(p);
}

//-----------------------------------------------------------------------
void hash_clear() {

  for (int i = 0; i < HASHSIZE; i++) {
    hash_node* p = hashtab[i];
    while (p != NULL) {
      hash_node* q = p;
      p = p->next;
      node_clear(q);
    }
    hashtab[i] = NULL;
  }
  printf("Hash table cleared\n");
}

//-----------------------------------------------------------------------
void test_hash_table() {
    printf("=======================TESTING HASH TABLE======================\n");

    hash_install("cat", "animal that likes fish");
    hash_install("dog", "animal that hates cats");
    hash_install("mouse", "animal eaten by cats");
    hash_install("virus", "pain in the neck");

    hash_print();

    print_defn("cat");
    print_defn("pig");
    print_defn("dog");
    print_defn("pig");
    print_defn("mouse");
    print_defn("bacteria");
    print_defn("amoeba");
    print_defn("paramecium");
    print_defn("virus");

    printf("\nredefining cat...\n");
    printf("redefining virus...\n");
    hash_install("cat", "animal that likes fish and mice and birds");
    hash_install("virus", "VERY EXPENSIVE pain in the neck");

    print_defn("cat");
    print_defn("virus");

    undef("virus");
    printf("\nremoving virus...\n");
    print_defn("virus");

    hash_print();

    hash_clear();
    hash_print();
    printf("=========================END OF TESTING========================\n");
  }

//-----------------------------------------------------------------------
int main(int argc, const char* argv[]) {

  test_hash_table();

  return 0;
}

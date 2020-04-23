#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define COMP_LIMIT 6

//-------------------------------------------------------------------------
typedef struct tnode tnode;
struct tnode {
  const char* word;
  int count;
  tnode* left;
  tnode* right;
};

//-------------------------------------------------------------------------
typedef struct tree tree;
struct tree {
  tnode* root;
  size_t size;
};

//-------------------------------------------------------------------------
tnode* tnode_create(const char* word) {
  tnode* p = (tnode*)malloc(sizeof(tnode));
  p->word = strdup(word);   //copy of word allocated on heap
  p->count = 1;
  p->left = NULL;
  p->right = NULL;
  return p;
}

//-------------------------------------------------------------------------
void tnode_delete(tnode* t) {
  free((void*)t->word);
  free(t);
}

//-------------------------------------------------------------------------
tree* tree_create() {
  tree* p = (tree*)malloc(sizeof(tree));
  p->root = NULL;
  p->size = 0;
  return p;
}

//-------------------------------------------------------------------------
static void tree_deletenodes(tree* t, tnode* p) {
  if ( p == NULL) { return; }

  tree_deletenodes(t, p->left);
  tree_deletenodes(t, p->right);
  tnode_delete(p);
  t->size--;
}

//-------------------------------------------------------------------------
void tree_delete(tree* t) { tree_deletenodes(t, t->root); }

//-------------------------------------------------------------------------
bool tree_empty(tree* t) { return t->size == 0; }

//-------------------------------------------------------------------------
size_t tree_size(tree* t) { return t->size; }

//-------------------------------------------------------------------------
static tnode* tree_addnode(tree* t, tnode** p, const char* w) {
  int compare;

  if (*p == NULL) {
    *p = tnode_create(w);
    t->size++;
  } else if ((compare = strcmp(w, (*p)->word)) == 0) {
    (*p)->count++;
  } else if (compare < 0) { tree_addnode(t, &(*p)->left, w);
  } else {
    tree_addnode(t, &(*p)->right, w);
  }

  return *p;
}

//-------------------------------------------------------------------------
tnode* tree_add(tree* t, const char* word) {
  tnode* p = tree_addnode(t, &(t->root), word);
  return p;
}

//-------------------------------------------------------------------------
tree* console_input() {
  tree* t = tree_create();

  char line[BUFSIZ];
  memset(line, 0, BUFSIZ);

  int c, i = 0;
  for ( ; i < BUFSIZ - 1 && ((c = getchar()) != EOF && c != '\n'); ++i) {
    line[i] = c;
  }
  line[i] = '\0';

  char* p = strtok(line, ",. !");
  tree_add(t, p);

  while (p != NULL) {
    p = strtok(NULL, ", .!");
    if (p == NULL) { continue; }
    tree_add(t, p);
  }

  return t;
}

//-------------------------------------------------------------------------
tree* file_input(const char* filename) {
  tree* t = tree_create();

  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    fclose(f);
    exit(1);
  }

  char line[BUFSIZ];
  memset(line, 0, BUFSIZ);

  while (fgets(line, BUFSIZ, f) != NULL) {
    if (*line == '\n') { continue; }
    char* p = strtok(line, ",. !\n");
    tree_add(t, p);

    while (p != NULL) {
      p = strtok(NULL, ",. !\n");
      if (p == NULL) { continue; }
      tree_add(t, p);
    }
  }

  fclose(f);
  return t;
}

//-------------------------------------------------------------------------
void tree_clear(tree* t) {
  tree_delete(t);
  t->root = NULL;
  t->size = 0;
}

//-------------------------------------------------------------------------
void tree_print(tnode* p) {
  printf("%s -- %d  (%p, %p)\n", p->word, p->count, p->left, p->right);
}

//-------------------------------------------------------------------------
static void tree_printnodes_inorder(tree* t, tnode* p) {
  if (p == NULL) { return; }

  tree_printnodes_inorder(t, p->left);
  tree_print(p);
  tree_printnodes_inorder(t, p->right);
}

//-------------------------------------------------------------------------
void tree_print_inorder(tree* t) {
  tree_printnodes_inorder(t, t->root);
}

//-------------------------------------------------------------------------
static void tree_printnodes_preorder(tree* t, tnode* p) {
  if (p == NULL) { return; }

  tree_print(p);
  tree_printnodes_preorder(t, p->left);
  tree_printnodes_preorder(t, p->right);
}

//-------------------------------------------------------------------------
void tree_print_preorder(tree* t) {
  tree_printnodes_preorder(t, t->root);
}

//-------------------------------------------------------------------------
static void tree_printnodes_postorder(tree* t, tnode* p) {
  if (p == NULL) { return; }

  tree_printnodes_postorder(t, p->left);
  tree_printnodes_postorder(t, p->right);
  tree_print(p);
}

//-------------------------------------------------------------------------
void tree_print_postorder(tree* t) {
  tree_printnodes_postorder(t, t->root);
}

//-------------------------------------------------------------------------
void tree_printnodes_n(tnode* p, int n) {
  static tnode* prev = NULL;

  if (p != NULL) {
    tree_printnodes_n(p->left, n);
    if (prev == NULL) { printf("%s ", p->word); }

    if (prev != NULL) {
      if (strncmp(prev->word, p->word, n) == 0) {
        printf("%s ", p->word);
      }
      else { printf("\n%s ", p->word); }
    }
    prev = p;
  }
  else { return; }
  tree_printnodes_n(p->right, n);
}

//-------------------------------------------------------------------------
void tree_print_n(tree* t, int n) {
  if (n == 0) { tree_print_inorder(t); }
  else { tree_printnodes_n(t->root, n); }
}

//-------------------------------------------------------------------------
int main(int argc, const char* argv[]) {
  int lim = COMP_LIMIT;
  tree* t = NULL;

  if (argc == 1) {
    printf("Waiting for tree data input:\n");
    t = console_input();
  }

  if (argc == 2) {
    lim = atoi(argv[1]);
    printf("Waiting for tree data input:\n");
    t = console_input();
  }

  if (argc == 3) {
    lim = atoi(argv[1]);
    const char* filename = argv[2];
    t = file_input(filename);
  }

  tree_print_inorder(t);

  printf("\nPrinting tree with %d letter comparisons\n\n", lim);
  tree_print_n(t, lim);
  printf("\nIs my tree empty? %s\n", tree_empty(t) ? "Yes" : "No");

  tree_clear(t);
  printf("\nClearing tree...\n");
  printf("Is my tree empty? %s\n", tree_empty(t) ? "Yes" : "No");

  free(t);

  return 0;
}

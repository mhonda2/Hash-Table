#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

//-------------------------------------------------------------------------
typedef struct pnode pnode;
struct pnode {
  int line;
  pnode* next;
};

//-------------------------------------------------------------------------
typedef struct tnode tnode;
struct tnode {
  const char* word;
  int count;
  pnode* lines;
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
pnode* pnode_create(int num) {
  pnode* p = (pnode*)malloc(sizeof(pnode));
  p->line = num;
  p->next = NULL;
  return p;
}

//-------------------------------------------------------------------------
void pnode_append(pnode* t, int num) {
  pnode* p = t;
  while (p->next != NULL) {
    p = p->next;
  }
  if (p->line != num) {
    pnode* q = pnode_create(num);
    p->next = q;
  }
}

//-------------------------------------------------------------------------
void pnode_delete(pnode* p) {
  while (p != NULL) {
    pnode* q = p;
    p = p->next;
    free(q);
  }
}

//-------------------------------------------------------------------------
tnode* tnode_create(const char* word, int line_num) {
  tnode* p = (tnode*)malloc(sizeof(tnode));
  p->word = strdup(word);   //copy of word allocated on heap
  p->count = 1;
  p->lines = pnode_create(line_num);
  p->left = NULL;
  p->right = NULL;
  return p;
}

//-------------------------------------------------------------------------
void tnode_delete(tnode* t) {
  free((void*)t->word);
  pnode_delete(t->lines);
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
bool noise_word(const char* w) {
  const char* noise[] = {"a","an","and","be","but","by","he","I","is"
                       ,"it","of","off","on","she","so","the","they","you"};
                       
  int size = sizeof(noise)/sizeof(noise[0]);
  for (int i = 0; i < size; ++i) {
    if (strcmp(w, noise[i]) == 0)
      return true;
  }

  return false;
}

//-------------------------------------------------------------------------
static tnode* tree_addnode(tree* t, tnode** p, const char* w, int lineNum) {
  int compare;

  if (*p == NULL) {
    *p = tnode_create(w, lineNum);
    t->size++;
  } else if ((compare = strcmp(w, (*p)->word)) == 0) {
    (*p)->count++;
    pnode_append((*p)->lines, lineNum);
  } else if (compare < 0) { tree_addnode(t, &(*p)->left, w, lineNum);
  } else {
    tree_addnode(t, &(*p)->right, w, lineNum);
  }

  return *p;
}

//-------------------------------------------------------------------------
tnode* tree_add(tree* t, const char* word, int cur_line) {
  tnode* p = tree_addnode(t, &(t->root), word, cur_line);
  return p;
}

//-------------------------------------------------------------------------
void console_input(tree* t, int argc, const char* argv[]) {
  int i = 1;
  char* p = strtok((char*)argv[i++], ",. !");
  if (!noise_word(p))
    tree_add(t, p, 1);

  while (p != NULL && i < argc) {
    p = strtok((char*)argv[i], ", .!");
    if (!noise_word(p))
      tree_add(t, p, 1);
    ++i;
  }
}

//-------------------------------------------------------------------------
void file_input(tree* t, const char* filename) {
  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    fclose(f);
    exit(1);
  }

  char line[BUFSIZ];
  memset(line, 0, BUFSIZ);
  int lineCount = 0;

  while (fgets(line, BUFSIZ, f) != NULL) {
    ++lineCount;
    if (*line == '\n') { continue; }
    char* p = strtok(line, ",. !\n");
    if (!noise_word(p))
      tree_add(t, p, lineCount);

    while (p != NULL) {
      p = strtok(NULL, ",. !\n");
      if (p == NULL) { continue; }
      if (!noise_word(p))
        tree_add(t, p, lineCount);
    }
  }

  fclose(f);
}

//-------------------------------------------------------------------------
tree* get_input(int argc, const char* argv[]) {
  tree* t = tree_create();

  if (argc == 1) {
    fprintf(stderr, "Usage: ./program file/keyboard input\n");
    exit(1);
  }

  if (argc == 2) {
    const char* filename = argv[1];
    file_input(t, filename);
  }

  if (argc > 2) {
    console_input(t, argc, argv);
  }
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
  printf("%d -- %s  ", p->count, p->word);
  pnode* q = p->lines;
  if (q != NULL) {
    printf("[");
    while (q != NULL) {
      if(q->next == NULL) printf("%d]\n", q->line);
      else printf("%d, ", q->line);
      q = q->next;
    }
  } else printf("Rogue word\n");
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
int main(int argc, const char* argv[]) {

  tree* t = get_input(argc, argv);

  tree_print_inorder(t);

  tree_clear(t);

  free(t);

  return 0;
}

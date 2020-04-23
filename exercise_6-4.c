#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_FREQ 200

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
void tree_freq_delete(tree* tr[]) {
  for (int i = 1; i < MAX_FREQ; ++i) {
    tree* t = tr[i];

    if (t != NULL) {
      tree_delete(t);
      t = NULL;
    }
  }

}

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
static tnode* tree_freq_addnode(tree* t, tnode** p, tnode* base_p) {
  int compare;
  if(*p == NULL) {
    *p = tnode_create(base_p->word);
    (*p)->count = base_p->count;
    (*p)->left = NULL;
    (*p)->right = NULL;
  }
  else if ((compare = strcmp(base_p->word, (*p)->word)) < 0) {
    tree_freq_addnode(t, &(*p)->left, base_p);
  }
  else if (compare > 0) {
    tree_freq_addnode(t, &(*p)->right, base_p);
  }

  return *p;
}

//-------------------------------------------------------------------------
static void tree_freq_node_fromtree(tree* tr[], tnode* base_node) {
  if (base_node == NULL) { return; }
  tree_freq_node_fromtree(tr, base_node->left);

  int i = base_node->count;
  tree** t = &tr[i];

  if (*t == NULL && i != 0) {
    *t = tree_create();
  }
  tree_freq_addnode(*t, &(*t)->root, base_node);
  tree_freq_node_fromtree(tr, base_node->right);
}

//-------------------------------------------------------------------------
void tree_freq_fromtree(tree* tr[], tree* base_tree) {
  tree_freq_node_fromtree(tr, base_tree->root);
}

//-------------------------------------------------------------------------
void console_input(tree* t, int argc, const char* argv[]) {
  int i = 1;
  char* p = strtok((char*)argv[i++], ",. !");
  tree_add(t, p);

  while (p != NULL && i < argc) {
    p = strtok((char*)argv[i], ", .!");
    tree_add(t, p);
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
  printf("%s -- %d\n", p->word, p->count);
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
void tree_freq_print(tree* tr[]) {
  for (int i = MAX_FREQ - 1; i >= 1; --i) {
    tree* t = tr[i];
    if (t != NULL && t->root != NULL) {
      tree_print_inorder(t);
    }
  }
}

//-------------------------------------------------------------------------
int main(int argc, const char* argv[]) {

  tree* t = get_input(argc, argv);

  tree* trees[MAX_FREQ];
  memset(trees, 0, MAX_FREQ * sizeof(tree*));

  tree_freq_fromtree(trees, t);

  tree_freq_print(trees);
  tree_freq_delete(trees);

  tree_clear(t);
  free(t);
  free(*trees);

  return 0;
}

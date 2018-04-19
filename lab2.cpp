#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

class PatriciaTrie;

class PatriciaTrieNode {
    private:
        friend class PatriciaTrie;
        int bit_index;
        char* key;
        unsigned long long data;
        PatriciaTrieNode* left;
        PatriciaTrieNode* right;

    public:

        PatriciaTrieNode();
        PatriciaTrieNode(char*,unsigned long long , int, PatriciaTrieNode*, PatriciaTrieNode*);
        virtual ~PatriciaTrieNode();

        void Initialize(char*,unsigned long long , int, PatriciaTrieNode*, PatriciaTrieNode*);

        unsigned long long  GetData();
        bool SetData(unsigned long long);

        char* GetKey();

        PatriciaTrieNode* GetLeft();
        PatriciaTrieNode* GetRight();

};

class PatriciaTrie {
    private:
        void recursive_remove(PatriciaTrieNode*);
        void recursive_save(PatriciaTrieNode*, ofstream*);
        int bit_get(char*, int);
        int bit_first_different(char*, char*);
        bool key_compare(char*, char*);
        void key_copy(PatriciaTrieNode*, PatriciaTrieNode*);
        PatriciaTrieNode* head;

    public:

        PatriciaTrie();
        virtual ~PatriciaTrie();

        PatriciaTrieNode* Insert(char*,unsigned long long );
        unsigned long long Lookup(char*);
        PatriciaTrieNode* LookupNode(char*);
        bool Delete(char*);
        bool Save(char*);

};

PatriciaTrieNode::PatriciaTrieNode() {
	Initialize((char*)NULL, (unsigned long long)NULL, -1, this, this);
}

PatriciaTrieNode::PatriciaTrieNode(char* k,
                                        unsigned long long d,
                                        int bi,
                                        PatriciaTrieNode* l,
                                        PatriciaTrieNode* r) {
    Initialize(k, d, bi, l, r);
}

void PatriciaTrieNode::Initialize(char* k,
                                      unsigned long long d,
                                      int bi,
                                      PatriciaTrieNode* l,
                                      PatriciaTrieNode* r) {
    if (k)
        key = (char*)strdup(k);
    else
        key = k;
    data = d;
    left = l;
    right = r;
    bit_index = bi;
}

PatriciaTrieNode::~PatriciaTrieNode() {
    if (key) {
        free(key);
        key = NULL;
    }
}

unsigned long long PatriciaTrieNode::GetData() {
    return data;
}

bool PatriciaTrieNode::SetData(unsigned long long d) {
    memcpy(&data, &d, sizeof(unsigned long long));
    return true;
}

char* PatriciaTrieNode::GetKey() {
    return key;
}

PatriciaTrieNode* PatriciaTrieNode::GetLeft() {
    return left;
}


PatriciaTrieNode* PatriciaTrieNode::GetRight() {
    return right;
}


PatriciaTrie::PatriciaTrie() {
	head = new PatriciaTrieNode();
	head->key = (char*)calloc(1, 1);
}

PatriciaTrie::~PatriciaTrie() {
	recursive_remove(head);
}

PatriciaTrieNode* PatriciaTrie::Insert(char* k, unsigned long long d) {

    PatriciaTrieNode *p, *t, *x;

    p = head;
    t = (PatriciaTrieNode*)(p->right);

    while ( (p->bit_index < t->bit_index) ){
        p = t;
        t = (PatriciaTrieNode*)(bit_get(k, t->bit_index) ? t->right : t->left);
    }

    if (key_compare(k, t->key))
        return NULL;

    int i = bit_first_different(k, t->key);

    p = head;
    x = (PatriciaTrieNode*)(p->right);
    while ( ( p->bit_index < x->bit_index ) &&
            ( x->bit_index < i) ) {
        p = x;
        x = (PatriciaTrieNode*)(bit_get(k, x->bit_index) ? x->right : x->left);
    }

    t = new PatriciaTrieNode();
    t->Initialize(k, d, i, (bit_get(k, i) ? x : t), (bit_get(k, i) ? t : x));

    if (bit_get(k, p->bit_index))
        p->right = t;
    else
        p->left = t;
    return t;

}

unsigned long long PatriciaTrie::Lookup(char* k) {

    PatriciaTrieNode* node = LookupNode(k);

    if (!node)
        return (unsigned long long)NULL;

    return node->data;

}

PatriciaTrieNode* PatriciaTrie::LookupNode(char* k) {

    PatriciaTrieNode* p;
    PatriciaTrieNode* x;

    p = head;
    x = (PatriciaTrieNode*)(head->right);

    while (p->bit_index < x->bit_index) {
        p = x;
        x = (PatriciaTrieNode*)(bit_get(k, x->bit_index) ? x->right : x->left);
    }

    if (!key_compare(k, x->key))
        return NULL;

    return x;

}

bool PatriciaTrie::Delete(char* k) {

    PatriciaTrieNode *p, *t, *x, *pp, *lp;
    int bp, bl, br;
    char* key = NULL;

    p  = head;
    t  = (PatriciaTrieNode*)(p->right);

    while (p->bit_index < t->bit_index) {
        pp = p;
        p = t;
        t = (PatriciaTrieNode*)(bit_get(k, t->bit_index) ? t->right : t->left);
    }

    if (!key_compare(k, t->key))
        return false; 

    if (t != p)
        key_copy(p, t);

    bp = p->bit_index;
    bl = ((PatriciaTrieNode*)(p->left))->bit_index;
    br = ((PatriciaTrieNode*)(p->right))->bit_index;

    if ((bl > bp) || (br > bp)) {
        if (p != t) {
            key = strdup(p->key);
            lp = p;
            x = (PatriciaTrieNode*)(bit_get(key, p->bit_index) ? p->right : p->left);
            while (lp->bit_index < x->bit_index) {
                lp = x;
                x = (PatriciaTrieNode*)(bit_get(key, x->bit_index) ? x->right : x->left);
            }
            if (!key_compare(key, x->key)) {
                free(key);
                return false; 
            }
            if (bit_get(key, lp->bit_index))
                lp->right = t;
            else
                lp->left = t;
        }

        if (pp != p) {
            PatriciaTrieNode* ch = (PatriciaTrieNode*)(bit_get(k, p->bit_index) ? p->left : p->right);
            if (bit_get(k, pp->bit_index))
                pp->right = ch;
            else
                pp->left = ch;
        }

        free(key);
        key = NULL;
    } else {
        if (pp != p) {
            PatriciaTrieNode* blx = (PatriciaTrieNode*)(p->left);
            PatriciaTrieNode* brx = (PatriciaTrieNode*)(p->right);
            if (bit_get(k, pp->bit_index))
                pp->right = (((blx == brx) && (blx == p)) ? pp : ((blx==p)?brx:blx));
            else
                pp->left  = (((blx == brx) && (blx == p)) ? pp : ((blx==p)?brx:blx));
        }
    }

    delete p;
    return true;

}

PatriciaTrie* Load(char* filename) {
    
    ifstream file (filename, ios::in | ios::binary);
    if (!file.is_open())
        return NULL;
    file.seekg(0, ios::beg);
    
    if (!file.is_open())
        return NULL;
    
    PatriciaTrie* temp = new PatriciaTrie();
    
    while(true) {
        short key_length;
        unsigned long long data;
        file.read((char*)&key_length, sizeof(short));
        if (file.eof()) break;
        char *temp_key = (char*)calloc(key_length,sizeof(char)+1);
        file.read(temp_key, sizeof(char)*key_length);
        file.read((char*)&data, sizeof(unsigned long long));
        temp->Insert(temp_key, data);
        free(temp_key);
    }
    
    return temp;
}

bool PatriciaTrie::Save(char* filename) {

    
    ofstream file;
    file.open(filename, ofstream::binary | ofstream::out | ofstream::trunc);
    if (!file.is_open())
        return false;
    
    recursive_save(head->right, &file);
    
    file.close();
    
    return true;
}

void PatriciaTrie::recursive_save(PatriciaTrieNode* root, ofstream* file) {
    
    PatriciaTrieNode* l = (PatriciaTrieNode*)root->left;
    PatriciaTrieNode* r = (PatriciaTrieNode*)root->right;
    
    
    file->seekp(0, ios::end);
    short key_length = strlen(root->key);
    file->write((char*)&key_length, sizeof(short));
    file->write(root->key, sizeof(char)*key_length);
    file->write((char*)&root->data, sizeof(unsigned long long));

    if ( (l->bit_index >= root->bit_index) && (l != root) && (l != head) )
        recursive_save(l, file);

    if ( (r->bit_index >= root->bit_index) && (r != root) && (r != head) )
        recursive_save(r, file);

}

void PatriciaTrie::recursive_remove(PatriciaTrieNode* root) {

    PatriciaTrieNode* l = (PatriciaTrieNode*)root->left;
    PatriciaTrieNode* r = (PatriciaTrieNode*)root->right;

    if ( (l->bit_index >= root->bit_index) && (l != root) && (l != head) )
        recursive_remove(l);

    if ( (r->bit_index >= root->bit_index) && (r != root) && (r != head) )
        recursive_remove(r);

    delete root;

}

int PatriciaTrie::bit_get(char* bit_stream, int n) {
  if (n < 0) return 2;
  int k = 7 - (n & 7);
  return ( (*(bit_stream + (n >> 3))) >> k) & 1;
}

bool PatriciaTrie::key_compare(char* k1, char* k2) {
    if (!k1 || !k2)
        return false;
    return (strcmp((char*)k1, (char*)k2) == 0);
}

int PatriciaTrie::bit_first_different(char* k1, char* k2) {
    if (!k1 || !k2)
        return 0;
    int n = 0;
    int d = 0; 
    while (	(k1[n] == k2[n]) &&
            (k1[n] != 0) &&
            (k2[n] != 0) )
        n++; 
    while (bit_get(&k1[n], d) == bit_get(&k2[n], d))
        d++;
    return ((n << 3) + d);
}

void PatriciaTrie::key_copy(PatriciaTrieNode* src, PatriciaTrieNode* dest) {

    if (src == dest)
        return;

    if (strlen(dest->key) < strlen(src->key))
        dest->key = (char*)realloc(dest->key, 1 + strlen(src->key));
    strcpy(dest->key, src->key);
    dest->data = src->data;
}


int main(int argc, char* argv[]) {

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);


    PatriciaTrie* p = new PatriciaTrie();
    char one[256];

    
    while (cin >> one) {
        char two[256];
        char three[256];
        unsigned long long num;
        PatriciaTrie* temp;
        
        switch (one[0]){
            case '+':
                cin >> two >> num;
                for(char *c = two;*c;++c) *c=*c>0x40&&*c<0x5b?*c|0x60:*c;
                if (p->Insert(two, num)) 
                    printf("OK\n");
                else
                    printf("Exist\n");
                break;
            case '-':
                cin >> two;
                for(char *c = two;*c;++c) *c=*c>0x40&&*c<0x5b?*c|0x60:*c;
                if (p->Delete(two)) 
                    printf("OK\n");
                else
                    printf("NoSuchWord\n");
                break;
            case '!':
                cin >> two >> three;
                if (two[0] == 'S') {
                    if (p->Save(three)) {
                        printf("OK\n");
                    } else {
                        printf("ERROR: Couldn't save file\n");
                    }
                } else {
                    temp = Load(three);
                    if (temp){
                        delete p;
                        p = temp;
                        printf("OK\n");
                    } else {
                        printf("ERROR: Couldn't load file\n");
                    }
                }
                break;
            default:
                for(char *c = one;*c;++c) *c=*c>0x40&&*c<0x5b?*c|0x60:*c;
                num = p->Lookup(one);
                if (num)
                    printf("OK: %llu\n", num);
                else
                    printf("NoSuchWord\n");
                break;
        } 
    }
      
    delete p;

    return 0;
}
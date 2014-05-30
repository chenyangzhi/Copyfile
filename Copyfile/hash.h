#ifndef HASHMAP_H
#define HASHMAP_H

#define HMAP_PRESET_SIZE 	2 <<23
#define HMAP_GROWTH_RATE 	2 
#define HMAP_DESTRUCTORS 
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

typedef  ino_t inode_key;  
typedef  dev_t dev_key;
typedef  int int32;

typedef struct key_val_pair key_val_pair;
typedef struct hashmap hashmap;

// 建造hash函数
hashmap* mk_hmap(int32 (*hash_fn)(inode_key), bool (*compare_fn)(inode_key a, inode_key b, dev_key c,dev_key d));
void free_hmap(hashmap*);
bool hmap_add(hashmap* hmap, inode_key ik, dev_key dk,const char* fp);
char* hmap_find(hashmap* hmap, inode_key ik, dev_key dk);
int32 int_hash_fn(inode_key);
bool int_compare_fn(inode_key, inode_key, dev_key, dev_key);

#endif

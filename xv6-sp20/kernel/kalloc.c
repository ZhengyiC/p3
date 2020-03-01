// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "spinlock.h"

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;


typedef struct allolist{
    struct allolist* next;  //a ptr to the head of an allocated list
    int addr;
}
int allo_sz; //size of the allocated list
allolist* head;
allolist* new;

extern char end[]; // first address after kernel loaded from ELF file

// Initialize free list of physical pages.
void
kinit(void)
{
  char *p;

  initlock(&kmem.lock, "kmem");
  p = (char*)PGROUNDUP((uint)end);
  for(; p + PGSIZE <= (char*)PHYSTOP; p += PGSIZE){
    kfree(p);}

  head->addr  = NULL;
  head-> next = NULL;
  allo_sz = 0 ;


}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;
  allolist * head_st = head;//head store

  if((uint)v % PGSIZE || v < end || (uint)v >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  //remove r from allocated list
  if(head->addr == (int)r){
      head = head ->next;
      allo_sz --;
      goto release;
  }

  while(head -> next != NULL){
      if(head -> next->addr == (int)r){
          head -> next = head -> next -> next;
          allo_sz --;
          goto restore;
      }
      head = head -> next;

  }
  restore:
    head = head_st;
  release:
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;


  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
    if(allo_sz != 0){
        new-> addr = (int) r;
        head -> next = head;
        head = new;
    }else{
        head-> addr = (int) r;
    }
    allo_sz ++;
  release(&kmem.lock);

  return (char*)r;
}

//This function return an array of most recently allocated pages with size numframes
//The array will be filled to *frames
//Return -1 if numframes > total allocated pages
int dump_allocated(int *frames, int numframes) {
    if( numframes > allo_sz) return -1;

    allolist * curr = head;
    for( int i=0; i< numframes; i++){
        *(frames+i) = curr->addr;
        curr= curr -> next;
    }


    return 0 ;
}

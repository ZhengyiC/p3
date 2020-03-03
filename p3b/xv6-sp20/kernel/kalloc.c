// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "spinlock.h"
#include "rand.h"

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;


int allolist[512];

int allo_sz; //size of the allocated list

int free_sz;

extern char end[]; // first address after kernel loaded from ELF file

// Initialize free list of physical pages.
void
kinit(void)
{
  char *p;

  initlock(&kmem.lock, "kmem");
  p = (char*)PGROUNDUP((uint)end);
  free_sz = 0;
  for(; p + PGSIZE <= (char*)PHYSTOP; p += PGSIZE){
    kfree(p);
    free_sz++;
  }

  memset(allolist, 0, sizeof allolist);
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

  if((uint)v % PGSIZE || v < end || (uint)v >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  free_sz++;

  //remove r from allocated list
  int which;
  for(int i = 0; i < allo_sz; i++) {
    if(allolist[i] == (int) r){
      which = i;
      break;
    }
  }
  for(int i = which; i < allo_sz; i++){
    allolist[i] = allolist[i+1];
  }
  allo_sz--;
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
  // r = kmem.freelist;
  int rand = ((unsigned int)xv6_rand()) % free_sz;
  struct run* now = kmem.freelist;
  struct run* last = NULL;
  for(int i = 0; i < rand; i++){
    last = now;
    now = now->next;
  }

  r = now;
  if(!r) {
    release(&kmem.lock); 
    return (char*)r;
  }

  if(last) {
    last->next = now->next;
  } else {
    kmem.freelist = r->next;
  }

  if(r){
    for(int i = allo_sz-1; i >= 0; i--){
      allolist[i+1] = allolist[i];
    }
    allolist[0] = (int)r;
    allo_sz++;
  }
  release(&kmem.lock);

  return (char*)r;
}

//This function return an array of most recently allocated pages with size numframes
//The array will be filled to *frames
//Return -1 if numframes > total allocated pages
int dump_allocated(int *frames, int numframes) {
    if( numframes > allo_sz) return -1;

    for(int i = 0; i < numframes; i++){
        *(frames+i) = allolist[i];
    }

    return 0 ;
}

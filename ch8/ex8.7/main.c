/*
 * Exercise 8-7. malloc accepts a size request without checking its
 * plausibility; free believes that the block it is asked to free contains a
 * valid size field. Improve these routines so they make more pains with error
 * checking.
 */
#include <stdio.h>
#include <stdlib.h>

#define NALLOC 1024    /* minimum #units to request */
#define MAXBYTES 10240 /* maximum bytes to allocate */

typedef long Align; /* for alignment to long boundary */
union header {      /* block header */
  struct {
    union header *ptr; /* next block if on free list */
    unsigned size;     /* size of this block */
  } s;
  Align x; /* force alignment of blocks */
};

typedef union header Header;

static Header base;          /* empty list to get started */
static Header *freep = NULL; /* start of free list */
static unsigned maxalloc;    /* max number of units allocated */

void *malloc2(unsigned nbytes);
static Header *morecore(unsigned);
void free2(void *);
void *calloc2(unsigned, unsigned);

/* malloc2: general-purpose storage allocator */
void *malloc2(unsigned nbytes) {
  Header *p, *prevp;
  Header *moreroce(unsigned);
  unsigned nunits;
  if (nbytes > MAXBYTES) {
    fprintf(stderr, "alloc: can't allocate more than %u bytes\n", MAXBYTES);
    return NULL;
  }
  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  if ((prevp = freep) == NULL) {
    /* no free list yet */
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    if (p->s.size >= nunits) { /* big enough */
      if (p->s.size == nunits) /* exactly */
        prevp->s.ptr = p->s.ptr;
      else {
        /* allocate tail end */
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void *)(p + 1);
    }
    if (p == freep) /* wrapped around free list */
      if ((p = morecore(nunits)) == NULL)
        return NULL; /* none left */
  }
}

/* morecore: ask system for more memory */
static Header *morecore(unsigned nu) {
  char *cp, *sbrk(int);
  Header *up;
  if (nu < NALLOC)
    nu = NALLOC;
  cp = sbrk(nu * sizeof(Header));
  if (cp == (char *)-1) /* no space at all */
    return NULL;
  up = (Header *)cp;
  up->s.size = nu;
  maxalloc = (up->s.size > maxalloc) ? up->s.size : maxalloc;
  free2((void *)(up + 1));
  return freep;
}

/* free: put block ap in free list */
void free2(void *ap) {
  Header *bp, *p;
  bp = (Header *)ap - 1; /* point to block header */
  if (bp->s.size == 0 || bp->s.size > maxalloc) {
    fprintf(stderr, "free: can't free %u units\n", bp->s.size);
    return;
  }
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;                         /* freed block at start or end of arena */
  if (bp + bp->s.size == p->s.ptr) { /* join to upper nbr */
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if (p + p->s.size == bp) { /* join to lower nbr */
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

int main(int argc, char *argv[]) { return 0; }

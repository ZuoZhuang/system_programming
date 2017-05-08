/**
 * Ideal Indirection Lab
 * CS 241 - Spring 2017
 */
#include "kernel.h"
#include "mmu.h"
#include <assert.h>
#include <stdio.h>

MMU *MMU_create() {
  MMU *mmu = calloc(1, sizeof(MMU));
  mmu->tlb = TLB_create();
  mmu->curr_pid = 0;
  return mmu;
}

void *get_physical_address(long long *page, long long offset) {
    return (void *) ((long long)page + offset);
}

void *MMU_get_physical_address(MMU *mmu, void *virtual_address, size_t pid) {
    assert(pid < MAX_PROCESS_ID);

    // Check whether pid is current pid
    if (mmu->curr_pid != pid) {
        TLB_flush(&mmu->tlb);
        mmu->curr_pid = pid;
    }

    long long v_addr_l = (long long)virtual_address;
    long long *page = NULL;

    // get offset
    long long offset = v_addr_l & 0x7fff;

    // Look up TLB first
    long long key = (v_addr_l >> 15) & 0xfffffffff;

    if (mmu->tlb == NULL) {
        MMU_tlb_miss(mmu, virtual_address, pid);
        mmu->tlb = TLB_create();
    }

    page = TLB_get_physical_address(&mmu->tlb, (void *)key);

    // If entry exists in TLB, then assemble the address and return
    if (page != NULL) {
        return get_physical_address(page, offset);
    }

    // Print tlb miss
    MMU_tlb_miss(mmu, virtual_address, pid);

    // Look up page tables
    // check wheter exists a page table
    if (mmu->base_pts[pid] == NULL) {
        MMU_raise_page_fault(mmu, virtual_address, pid);
        MMU_add_process(mmu, pid);
        mmu->base_pts[pid] = PageTable_create();
    }

    long long vpn3 = key & 0xfff;
    long long vpn2 = (key >> 12) & 0xfff;
    long long vpn1 = (key >> 24) & 0xfff;

    PageTable *pt2 = (PageTable *)PageTable_get_entry(mmu->base_pts[pid], vpn1);
    if (pt2 == NULL) {
        MMU_raise_page_fault(mmu, virtual_address, pid);
        pt2 = PageTable_create();
        PageTable_set_entry(mmu->base_pts[pid], vpn1, pt2);
    }

    PageTable *pt3 = (PageTable *) PageTable_get_entry(pt2, vpn2);
    if (pt3 == NULL) {
        MMU_raise_page_fault(mmu, virtual_address, pid);
        pt3 = PageTable_create();
        PageTable_set_entry(pt2, vpn2, pt3);
    }

    page = PageTable_get_entry(pt3, vpn3);
    if (page == NULL) {
        MMU_raise_page_fault(mmu, virtual_address, pid);
        page = ask_kernel_for_frame();
        PageTable_set_entry(pt3, vpn3, page);
    }

    if (page == NULL) {
        return NULL;
    }

    // When there is no record in TLB, then add the entry to it
    TLB_add_physical_address(&mmu->tlb, (void *)key, (void *)page);

    return get_physical_address(page, offset);
}

void MMU_tlb_miss(MMU *mmu, void *address, size_t pid) {
  assert(pid < MAX_PROCESS_ID);
  mmu->num_tlb_misses++;
  printf("Process [%lu] tried to access [%p] and it couldn't find it in the "
         "TLB so the MMU has to check the PAGE TABLES\n",
         pid, address);
}

void MMU_raise_page_fault(MMU *mmu, void *address, size_t pid) {
  assert(pid < MAX_PROCESS_ID);
  mmu->num_page_faults++;
  printf(
      "Process [%lu] tried to access [%p] and the MMU got an invalid entry\n",
      pid, address);
}

void MMU_add_process(MMU *mmu, size_t pid) {
  assert(pid < MAX_PROCESS_ID);
  mmu->base_pts[pid] = PageTable_create();
}

void MMU_free_process_tables(MMU *mmu, size_t pid) {
  assert(pid < MAX_PROCESS_ID);
  PageTable *base_pt = mmu->base_pts[pid];
  Pagetable_delete_tree(base_pt);
}

void MMU_delete(MMU *mmu) {
  for (size_t i = 0; i < MAX_PROCESS_ID; i++) {
    MMU_free_process_tables(mmu, i);
  }
  TLB_delete(mmu->tlb);
  free(mmu);
}

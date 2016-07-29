#ifndef COS_KERNEL_API_H
#define COS_KERNEL_API_H

/*
 * Copyright 2015, Qi Wang and Gabriel Parmer, GWU, gparmer@gwu.edu.
 *
 * This uses a two clause BSD License.
 */

#include <cos_component.h>
#include <cos_debug.h>

/* Memory source information */
struct cos_meminfo {
	vaddr_t untyped_ptr,      umem_ptr,      kmem_ptr;
	vaddr_t untyped_frontier, umem_frontier, kmem_frontier;
};

/* Component captbl/pgtbl allocation information */
struct cos_compinfo {
	/* capabilities to higher-order capability tables (or -1) */
	capid_t pgtbl_cap, captbl_cap, comp_cap;
	/* the frontier of unallocated caps, and the allocated captbl range */
	capid_t cap_frontier, caprange_frontier;
	/* the frontier for each of the various sizes of capability */
	capid_t cap16_frontier, cap32_frontier, cap64_frontier;
	/* heap pointer equivalent, and range of allocated PTEs */
	vaddr_t vas_frontier, vasrange_frontier;
	/* shared memory pointer and range of allocated PTEs */
	vaddr_t shm_frontier, shmrange_frontier;
	vaddr_t shmmap_bump_ptr;
	/* the source of memory */
	struct cos_compinfo *memsrc; /* might be self-referential */
	struct cos_meminfo mi;	     /* only populated for the component with real memory */
};

struct cos_shminfo {
	vaddr_t shm_ptr, shm_offset, shm_size;
};
/*
struct cos_shm_rb {
	unsigned int head, tail; 
	int vmid;
	unsigned int size;
	unsigned int mask;
	char buf[0];
};

struct cos_shm_rb * vk_shmem_addr_send(int vmid);

struct cos_shm_rb * vk_shmem_addr_recv(int vmid);

int vk_ringbuf_create(struct cos_compinfo *ci, struct cos_shm_rb * sm_rb, size_t tsize, int vmid);

int vk_ringbuf_enqueue(struct cos_shm_rb * rb, void * buff, size_t size);

int vk_ringbuf_dequeue(struct cos_shm_rb *rb, void * buff, size_t size);
*/
int cos_mem_partition(struct cos_compinfo *ci, vaddr_t start_addr, unsigned long untyped_sz);

void cos_meminfo_alloc(struct cos_compinfo *ci, vaddr_t untyped_ptr, unsigned long untyped_sz);
void cos_compinfo_init(struct cos_compinfo *ci, captblcap_t pgtbl_cap, pgtblcap_t captbl_cap, compcap_t comp_cap, vaddr_t heap_ptr, capid_t cap_frontier, vaddr_t shm_ptr, struct cos_compinfo *ci_resources);
/*
 * This only needs be called on compinfos that are managing resources
 * (i.e. likely only one).  All of the capabilities will be relative
 * to this component's captbls.
 */
void cos_meminfo_init(struct cos_meminfo *mi, vaddr_t untyped_ptr, unsigned long untyped_sz);

/*
 * This uses the next three functions to allocate a new component and
 * correctly populate ci (allocating all resources from ci_resources).
 */
int cos_compinfo_alloc(struct cos_compinfo *ci, vaddr_t heap_ptr, vaddr_t entry, vaddr_t shm_ptr, struct cos_compinfo *ci_resources);
captblcap_t cos_captbl_alloc(struct cos_compinfo *ci);
pgtblcap_t  cos_pgtbl_alloc(struct cos_compinfo *ci);
compcap_t   cos_comp_alloc(struct cos_compinfo *ci, captblcap_t ctc, pgtblcap_t ptc, vaddr_t entry);

typedef void (*cos_thd_fn_t)(void *);
thdcap_t  cos_thd_alloc(struct cos_compinfo *ci, compcap_t comp, cos_thd_fn_t fn, void *data);
/* Create the initial (cos_init) thread */
thdcap_t  cos_initthd_alloc(struct cos_compinfo *ci, compcap_t comp);
sinvcap_t cos_sinv_alloc(struct cos_compinfo *srcci, compcap_t dstcomp, vaddr_t entry);
arcvcap_t cos_arcv_alloc(struct cos_compinfo *ci, thdcap_t thdcap, tcap_t tcapcap, compcap_t compcap, arcvcap_t enotif);
asndcap_t cos_asnd_alloc(struct cos_compinfo *ci, arcvcap_t arcvcap, captblcap_t ctcap);

void *cos_page_bump_alloc(struct cos_compinfo *ci);
void cos_shmem_alloc(struct cos_compinfo *ci, unsigned long sz);
void cos_shmem_map(struct cos_compinfo *ci, unsigned long sz);

capid_t cos_cap_cpy(struct cos_compinfo *dstci, struct cos_compinfo *srcci, cap_t srcctype, capid_t srccap);
int cos_cap_cpy_at(struct cos_compinfo *dstci, capid_t dstcap, struct cos_compinfo *srcci, capid_t srccap);

int cos_thd_switch(thdcap_t c);
#define CAP_NULL 0
sched_tok_t cos_sched_sync(void);
int cos_switch(thdcap_t c, tcap_t t, tcap_prio_t p, tcap_res_t r, arcvcap_t rcv, sched_tok_t stok);
int cos_thd_mod(struct cos_compinfo *ci, thdcap_t c, void *tls_addr); /* set tls addr of thd in captbl */

int cos_asnd(asndcap_t snd);
/* returns non-zero if there are still pending events (i.e. there have been pending snds) */
int cos_rcv(arcvcap_t rcv);
/* returns the same value as cos_rcv, but also information about scheduling events */
int cos_sched_rcv(arcvcap_t rcv, thdid_t *thdid, int *rcving, cycles_t *cycles);

int cos_introspect(struct cos_compinfo *ci, capid_t cap, unsigned long op);

vaddr_t cos_mem_alias(struct cos_compinfo *dstci, struct cos_compinfo *srcci, vaddr_t src);
int cos_mem_alias_at(struct cos_compinfo *dstci, vaddr_t dst, struct cos_compinfo *srcci, vaddr_t src);
vaddr_t cos_mem_move(struct cos_compinfo *dstci, struct cos_compinfo *srcci, vaddr_t src);
int cos_mem_move_at(struct cos_compinfo *dstci, vaddr_t dst, struct cos_compinfo *srcci, vaddr_t src);
int cos_mem_remove(pgtblcap_t pt, vaddr_t addr);

/* Tcap operations */
tcap_t cos_tcap_alloc(struct cos_compinfo *ci, tcap_prio_t prio);
int cos_tcap_transfer(tcap_t src, arcvcap_t dst, tcap_res_t res, tcap_prio_t prio);
int cos_tcap_delegate(asndcap_t dst, tcap_t src, tcap_res_t res, tcap_prio_t prio, tcap_deleg_flags_t flags);
int cos_tcap_merge(tcap_t dst, tcap_t rm);

/* Hardware (interrupts) operations */
hwcap_t cos_hw_alloc(struct cos_compinfo *ci, u32_t bitmap);
int cos_hw_attach(hwcap_t hwc, hwid_t hwid, arcvcap_t rcvcap);
int cos_hw_detach(hwcap_t hwc, hwid_t hwid);
void *cos_hw_map(struct cos_compinfo *ci, hwcap_t hwc, paddr_t pa, unsigned int len);
int cos_hw_cycles_per_usec(hwcap_t hwc);

void *cos_va2pa(struct cos_compinfo *ci, void * vaddr);


#endif /* COS_KERNEL_API_H */

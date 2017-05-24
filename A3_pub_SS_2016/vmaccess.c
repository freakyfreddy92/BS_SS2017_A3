/**
 * @file vmaccess.c
 * @author Prof. Dr. Wolfgang Fohl, HAW Hamburg
 * @date 2010
 * @brief The access functions to virtual memory.
 */

#include "vmem.h"
#include "debug.h"
#include <limits.h>


/*
 * static variables
 */

static struct vmem_struct *vmem = NULL; //!< Reference to virtual memory
static sem_t *local_sem = NULL;

/**
 *****************************************************************************************
 *  @brief      This function setup the connection to virtual memory.
 *              The virtual memory has to be created by mmanage.c module.
 *
 *  @return     void
 ****************************************************************************************/
static void vmem_init(void) {
	void* shmdata = NULL;
	key_t key = ftok(SHMKEY,SHMPROCID);
	int shmid = shmget(key,SHMSIZE,IPC_CREAT|SHM_R|SHM_W);
	if(shmid == -1){
		printf("Fehler bei shm erstellung");
	}
	shmdata = shmat(shmid,NULL,0);
	if(shmdata == (void*)-1){
		printf("Fehler bei shmd");
	}

	vmem = (struct vmem_struct*)shmdata;
	local_sem = sem_open(NAMED_SEM,0);
}

/**
 *****************************************************************************************
 *  @brief      This function does aging for aging page replacement algorithm.
 *              It will be called periodic based on g_count.
 *              This function must be used only when aging page replacement algorithm is activ.
 *              Otherwise update_age_reset_ref may interfere with other page replacement 
 *              alogrithms that base on PTF_REF bit.
 *
 *  @return     void
 ****************************************************************************************/
static void update_age_reset_ref(void) {
	if((vmem->adm.g_count % UPDATE_AGE_COUNT) == 0){
		int i = 0;
		for(i = 0 ; i <VMEM_NFRAMES; i++ ){
			int page_number = vmem->pt.framepage[i];
			if(page_number != VOID_IDX){
			vmem->pt.entries[page_number].age;
			int r = vmem->pt.entries[page_number].flags & PTF_REF;
			vmem->pt.entries[page_number].age = vmem->pt.entries[page_number].age/2; //right shift.
			if(r == PTF_REF){
				vmem->pt.entries[page_number].age = vmem->pt.entries[page_number].age|(~CHAR_MAX);
				vmem->pt.entries[page_number].flags  = vmem->pt.entries[page_number].flags ^ PTF_REF;
			}}
		 }
		}
}

/**
 *****************************************************************************************
 *  @brief      This function puts a page into memory (if required).
 *              It must be called by vmem_read and vmem_write
 *
 *  @param      address The page that stores the contents of this address will be put in (if required).
 * 
 *  @return     The int value read from virtual memory.
 ****************************************************************************************/
static void vmem_put_page_into_mem(int address) {
}

int vmem_read(int address) {
	if(vmem == NULL){
		vmem_init();
	}


	// bit mask offset
	int offset = address & (VMEM_PAGESIZE -1);
	//bit mask page
	int page_index = (address&(~(VMEM_PAGESIZE -1))) / (VMEM_PAGESIZE);
	int frame = vmem->pt.entries[page_index].frame;
	vmem->adm.req_pageno= page_index;
    TEST_AND_EXIT(page_index <  0,           (stderr, "page_index out of range\n"));
    TEST_AND_EXIT(page_index >= VMEM_NPAGES, (stderr, "page_index out of range\n"));
	vmem->pt.entries[page_index].flags = PTF_REF |vmem->pt.entries[page_index].flags ;
	if(frame == VOID_IDX ){
		kill(vmem->adm.mmanage_pid,SIGUSR1);
		sem_wait(local_sem);
	}
	int page = (vmem->pt.entries[page_index].frame*(VMEM_PAGESIZE))|offset;
    int holder =vmem->data[page];
	TEST_AND_EXIT( page <  0,           (stderr, "page_index out of range\n"));
	TEST_AND_EXIT( page >= 128 ,         (stderr, "page_index %i out of range\n",page));
	vmem->adm.g_count++;
	if(vmem->adm.page_rep_algo == VMEM_ALGO_AGING){
		update_age_reset_ref();
	}
	return holder;
}

void vmem_write(int address, int data) {
	if(vmem == NULL){
		vmem_init();
	}


	// bit mask offset
	int offset = address & (VMEM_PAGESIZE -1);
	//bit mask page
	int page_index = (address&(~(VMEM_PAGESIZE -1))) / (VMEM_PAGESIZE);
    TEST_AND_EXIT(page_index <  0,           (stderr, "page_index out of range\n"));
    TEST_AND_EXIT(page_index >= VMEM_NPAGES, (stderr, "page_index out of range\n"));
	vmem->adm.req_pageno= page_index;
	int frame = vmem->pt.entries[page_index].frame;
	vmem->pt.entries[page_index].flags = PTF_DIRTY| PTF_REF | vmem->pt.entries[page_index].flags;
	if(frame == VOID_IDX ){
		kill(vmem->adm.mmanage_pid,SIGUSR1);
		sem_wait(local_sem);
	}
	int page = (vmem->pt.entries[page_index].frame*(VMEM_PAGESIZE))|offset;
	TEST_AND_EXIT( page <  0,           (stderr, "page_index out of range\n"));
	TEST_AND_EXIT( page >= 128 ,         (stderr, "page_index %i out of range\n",page));
	vmem->data[page] = data;
	vmem->adm.g_count++;
	if(vmem->adm.page_rep_algo == VMEM_ALGO_AGING){
		update_age_reset_ref();
	}
}

// EOF

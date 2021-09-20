/*******************************************************************************
* Copyright (c) 2019 Cadence Design Systems, Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and
* not with any other processors and platforms, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
#if 1 > 1

#include "cnnrt.h"
//#include <cnnrtsupport.h>

xmem_mgr_t *_cnnrt_mem_mgr;
xmem_status_t _cnnrt_mem_error_code;

#ifdef __XCC__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <xtensa/hal.h>
#include <xtensa/xtensa-versions.h>
#include <xtensa/xipc/xipc_sys.h>
#include <xtensa/system/xmp_subsystem.h>

#if defined(XTENSA_SWVERSION_RI_2018_0) && (XTENSA_SWVERSION >= XTENSA_SWVERSION_RI_2018_0)
/* RI release deprecated xipc_get_ipi_mmio_addr function */
static inline uintptr_t xipc_get_ipi_mmio_addr(xipc_pid_t pid)
{
  return xmp_mmio_addrs_xipc_intr[pid];
}
#endif

static xipc_cqueue_t *cq_remote;                /* concurrent queue pointer (for slave) */

/*
 * Print log info with timestamp and core name
 * e.g.,
 * (     12504) XMP_LOG: core0: Proc ID: 0
 * (     12508)           XMP_LOG: core1: Proc ID: 1
 */
static const char *xmp_proc_names[] = { XMP_PROC_ARRAY_P(NAME_STR) };
void xmp_log(const char *fmt, ...)
{
    int l = 0;
    char _lbuf[1024];

    l += sprintf(&_lbuf[l], "(%10d) ", (int)_time_stamp());

    char _tbuf[256];
    int i;
    for (i = 0; i < cnnrt_core_id()*10; i++)
        _tbuf[i] = ' ';
    _tbuf[i] = '\0';
    l += sprintf(&_lbuf[l], "%s", _tbuf);

    l += sprintf(&_lbuf[l], "XMP_LOG: %s: ",
                  xmp_proc_names[cnnrt_core_id()]);

    va_list argp;
    va_start(argp, fmt);
    vsprintf(&_lbuf[l], fmt, argp);
    va_end(argp);
    printf("%s", _lbuf);
}

static int is_mpu_entry_ok(struct xthal_MPU_entry mpu_entry)
{
    unsigned v = XTHAL_MPU_ENTRY_GET_MEMORY_TYPE(mpu_entry) & 0x1fe;
    // See Table 4-129 (Memory Protection Unit Option Memory Type)
    // of ISA reference manual.
    if (v == 0x1e || // non-cacheable, sharable
        v == 0x6  || // Device shareable, non-interruptible
        v == 0xe)    // Device shareable, interruptible
        return 1;

    return 0;
}

void xmp_set_mpu_attrs_region_uncached(void *addr, size_t size)
{
  int err;
  err = xthal_mpu_set_region_attribute(addr, size,
                                       XTHAL_MPU_USE_EXISTING_ACCESS_RIGHTS,
                                        (XTHAL_MEM_NON_CACHEABLE    |
                                         XTHAL_MEM_SYSTEM_SHAREABLE |
                                         XTHAL_MEM_BUFFERABLE),
                                       0);
  if (err != XTHAL_SUCCESS) {
    xmp_log("Error setting region attribute on [%x,%x], err %d\n",
            (void *)addr, (void *)(addr+size), err);
    exit(-1);
  }
}

static void xmp_set_mpu_attrs()
{
    int i, err;

    /* Set the shared system ram as shared and non-cachable */
    xmp_set_mpu_attrs_region_uncached((void *)(XMP_SYSTEM_RAM_ADDR + SHARED_SYSTEM_RAM_OFFSET),
                                      XMP_SYSTEM_RAM_SIZE - SHARED_SYSTEM_RAM_OFFSET);

#ifdef XCHAL_HAVE_DATARAM0
    // Marks the global address space for dataram0 for all cores as shared
    // non-cached
    uintptr_t global_base_addr_dataram0[] =
                             {XMP_PROC_ARRAY_P(GLOBAL_BASE_ADDR_DATARAM0)};
    uint32_t dataram0_size[] = {XMP_PROC_ARRAY_P(DATARAM0_SIZE)};
    for (i = 0; i < XMP_NUM_PROCS; i++) {
        int is_fg;
        struct xthal_MPU_entry mpu_entry =
          xthal_get_entry_for_address((void *)global_base_addr_dataram0[i], &is_fg);
        if (is_mpu_entry_ok(mpu_entry))
            continue;
        xmp_set_mpu_attrs_region_uncached((void *)global_base_addr_dataram0[i], dataram0_size[i]);
    }
#endif

#ifdef XCHAL_HAVE_DATARAM1
    // Marks the global address space for dataram1 for all cores as shared
    // non-cached
    uintptr_t global_base_addr_dataram1[] =
                             {XMP_PROC_ARRAY_P(GLOBAL_BASE_ADDR_DATARAM1)};
    uint32_t dataram1_size[] = {XMP_PROC_ARRAY_P(DATARAM1_SIZE)};
    for (i = 0; i < XMP_NUM_PROCS; i++) {
        int is_fg;
        struct xthal_MPU_entry mpu_entry =
                xthal_get_entry_for_address((void *)global_base_addr_dataram1[i], &is_fg);
        if (is_mpu_entry_ok(mpu_entry))
            continue;
        xmp_set_mpu_attrs_region_uncached((void *)global_base_addr_dataram0[1], dataram1_size[i]);
    }
#endif

    // Marks the global address space for mmio registers for all cores as shared
    // non-cached
    uint32_t has_xipc_interrupts[] = {XMP_PROC_ARRAY_P(HAS_XIPC_INTR)};
    for (i = 0; i < XMP_NUM_PROCS; i++) {
        if (!has_xipc_interrupts[i])
            continue;
        int is_fg;
        struct xthal_MPU_entry mpu_entry =
            xthal_get_entry_for_address((void *)xipc_get_ipi_mmio_addr(i), &is_fg);
        if (is_mpu_entry_ok(mpu_entry))
            continue;
        uint32_t mmio_size = 64;
        xmp_set_mpu_attrs_region_uncached((void *)xipc_get_ipi_mmio_addr(i), mmio_size);
    }
}

static int xmp_initialization(unsigned int num_cores)
{
    xipc_status_t err;
    int my_procid = XT_RSR_PRID();
    xmp_log("Proc ID: %d\n", my_procid);

    #if (!(XCHAL_HAVE_EXCLUSIVE || XCHAL_HAVE_S32C1I))
      xmp_log("This test expects requires either conditional store or exclusive load/stores \n");
      exit(-1);
    #endif

    #if XCHAL_HAVE_MPU
      xmp_set_mpu_attrs();
    #endif

    // Master proc initializes the barrier
    if (my_procid == XMP_MASTER_PID) {
      xipc_pid_t proc_ids[] = {XMP_PROC_ARRAY_P(PID)};
      if ((err = xipc_barrier_init(&shared_barrier,
                                   XIPC_SPIN_WAIT,
                                   XMP_NUM_PROCS,
                                   proc_ids,
                                   XMP_NUM_PROCS)) != XIPC_OK) {
        xmp_log("Error initializing xipc_barrier, err: %d\n", err);
        exit(-1);
      }
    }

    // Master proc initializes the mutex
    if (my_procid == XMP_MASTER_PID) {
        if ((err = xipc_mutex_init(&shared_mutex, XIPC_SPIN_WAIT)) != XIPC_OK) {
        xmp_log("Error initializing mutex: err: %d\n", err);
        exit(-1);
        }
    }

    uint32_t has_xipc_interrupts[] = {XMP_PROC_ARRAY_P(HAS_XIPC_INTR)};
    if (has_xipc_interrupts[0] == 0) {
        xmp_log("Concurrent queues are blocking and requires XIPC interrupts to be configured in the subsystem\n");
        exit(-1);
    }

    // Master proc initializes the barrier
    if (my_procid == XMP_MASTER_PID) {
        // Master proc is the producer, rest are consumers
        xipc_pid_t prod_proc_ids[XMP_NUM_PROCS];
        xipc_pid_t cons_proc_ids[XMP_NUM_PROCS];
        prod_proc_ids[0] = XMP_MASTER_PID;
        int num_cons = 0;
        int i;
        for (i = 0; i < XMP_NUM_PROCS; i++) {
            if (XMP_MASTER_PID != i)
                cons_proc_ids[num_cons++] = i;
        }

        // Initialize the queue
        err = xipc_cqueue_initialize(&cq_local, 0, PKT_SIZE, NUM_PKTS,
                                 cq_buffer, XMP_MASTER_PID,
                                 prod_proc_ids, 1,
                                 cons_proc_ids, num_cons, 0);
        if (err != XIPC_OK) {
            xmp_log("Error initializing xipc_cqueue, err: %d\n", err);
            exit(-1);
        }
    }

    // Initialize the XIPC library. Note, all procs implicitly synchronize
    // at the end of this call.
    xipc_initialize();

    // Setup the inter-processor message channels
    xipc_setup_channels();

    // The master proc sends the address of the local cqueue to the other
    // cores using the message channel. The address is first translated to
    // the global address space before sending to the other cores
    if (XT_RSR_PRID() == XMP_MASTER_PID) {
        int i;
        for (i = 0; i < XMP_NUM_PROCS; i++) {
            if (i == XMP_MASTER_PID)
                continue;
            xipc_msg_channel_output_port_t *p = xipc_msg_channel_get_output_port(i);
            if (p == NULL) {
                xmp_log("Error, could not get output port to proc %d\n", i);
                exit(-1);
            }
            //void *cq_local_addr = xipc_get_sys_addr(&cq_local, XMP_MASTER_PID);
            void *cq_local_addr = &cq_local;
            xipc_msg_channel_send(p, (int *)&cq_local_addr, XIPC_SPIN_WAIT);
        }
    } else {
        // Receive the address of the master core's cqueue
        xipc_msg_channel_input_port_t *p =
                           xipc_msg_channel_get_input_port(XMP_MASTER_PID);
        if (p == NULL) {
            xmp_log("Error, could not get input port to proc %d\n", XMP_MASTER_PID);
            exit(-1);
        }
        xipc_msg_channel_recv(p, (int *)&cq_remote, XIPC_SPIN_WAIT);
    }

    return 0;
}

void master_broadcast_task(const mail_packet_t *t)
{
    xipc_pkt_t pkt;
    for (int i = 1; i < cnnrt_core_num(); i++) {
        xipc_cqueue_allocate(&cq_local, &pkt);
        memcpy((void*)xipc_pkt_get_ptr(&pkt), (void*)t, sizeof(mail_packet_t));
        xipc_cqueue_send(&cq_local, &pkt, XIPC_SLEEP_WAIT);
    }
}

void slave_receive_task(mail_packet_t* t)
{
    xipc_pkt_t pkt;
    xipc_cqueue_recv(cq_remote, &pkt);
    memcpy((void*)t, (void*)xipc_pkt_get_ptr(&pkt), sizeof(mail_packet_t));
    xipc_cqueue_release(cq_remote, &pkt, XIPC_SLEEP_WAIT);
}

XI_ERR_TYPE cnnrt_barrier(XI_ERR_TYPE local_status)
{
    // FIXME: hande DCache and memory consistency
    if (local_status != XI_ERR_OK) {
        shared_status = local_status;
    }
    xipc_barrier_wait(&shared_barrier);
    local_status = shared_status;
    xipc_barrier_wait(&shared_barrier);
    return local_status;
}

#else

/* CSTUB */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#ifndef __USE_MISC
#  define __USE_MISC  // needed to use MAP_ANONYMOUS
#endif
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <semaphore.h>

#define CHECK_SEM(op) if (op) ERRX("Semaphore operation failed: %s\n", strerror(errno));

typedef struct {
    sem_t sem;
    pid_t pid;
} child_process_t;

typedef struct {
    /* Number of cores in the system */
    int   num_cores;
    /* Semaphore to protect barrier counter */
    sem_t barrier_sem;
    sem_t barrier_turnstile_sem;
    sem_t barrier_turnstile2_sem;
    /* Number of processes that reached barrier */
    int   barrier_count;
    XI_ERR_TYPE barrier_status;

    /* Semaphores to protect consumers counter and signal
       command availability.
     */
    sem_t command_producer_sem;
    sem_t command_consumer_sem;
    /* How many consumers extracted command. */
    int   num_command_consumed;
    /* Current command */
    mail_packet_t command;
    /* Individual signal semaphore and pid for each core */
    child_process_t core[];
} shared_state_t;

/* Shared state is allocated from shared_mem */
static shared_state_t *shared_state;
static void *shared_mem = MAP_FAILED;
static size_t shared_state_size;

static unsigned int _cnnrt_core_id;

unsigned int cnnrt_core_id()
{
    return _cnnrt_core_id;
}

unsigned int cnnrt_core_num()
{
    return (shared_state) ? shared_state->num_cores : 0;
}

static void free_ipcs()
{
    if (shared_state) {
        if (_cnnrt_core_id == 0) {
            for (unsigned int i = 1; i < shared_state->num_cores; i++) {
                if (shared_state->core[i - 1].pid > 0)
                    kill(shared_state->core[i - 1].pid, SIGTERM);
                shared_state->core[i - 1].pid = 0;
            }
        }
        sem_destroy(&shared_state->barrier_sem);
        sem_destroy(&shared_state->barrier_turnstile_sem);
        sem_destroy(&shared_state->barrier_turnstile2_sem);
        sem_destroy(&shared_state->command_producer_sem);
        sem_destroy(&shared_state->command_consumer_sem);
        for (int i = 0; i < shared_state->num_cores - 1; i++) {
            sem_destroy(&shared_state->core[i].sem);
        }
    }
    shared_state = NULL;
    shared_sram_pool = NULL;
    if (shared_mem != MAP_FAILED) {
        munmap(shared_mem, shared_state_size + SHARED_MEM_POOL_SIZE);
        shared_mem = MAP_FAILED;
    }
}

static void sigterm_handler(int signum)
{
    exit(-1);
}

static void register_exit_callbacks()
{
    /* register handler to deallocate IPC resources at normal or abnormal exits*/
    if (atexit(free_ipcs)) {
        ERRX("Failed to register exit function\n");
    }
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigterm_handler;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

static void alloc_ipcs(unsigned int num_cores)
{
    register_exit_callbacks();

    /* Allocate shared memories */
    shared_state_size = sizeof(shared_state_t) + sizeof(child_process_t) * (num_cores - 1);
    shared_mem = mmap(NULL, shared_state_size + SHARED_MEM_POOL_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (shared_mem == MAP_FAILED) {
        ERRX("Failed to allocate shared_sram_pool: %s\n", strerror(errno));
    }

    shared_sram_pool = (uint8_t*)shared_mem + shared_state_size;
    shared_state = (shared_state_t *)shared_mem;

    memset(shared_state, 0, shared_state_size);

    shared_state->num_cores = num_cores;
    shared_state->barrier_status = XI_ERR_OK;

    if (sem_init(&shared_state->barrier_sem, 1, 1)
        || sem_init(&shared_state->barrier_turnstile_sem, 1, 0)
        || sem_init(&shared_state->barrier_turnstile2_sem, 1, 0)
        || sem_init(&shared_state->command_producer_sem, 1, 1)
        || sem_init(&shared_state->command_consumer_sem, 1, 1))
    {
        ERRX("Failed to initialize semaphore: %s\n", strerror(errno));
    }
    for (int i = 0; i < num_cores - 1; i++) {
        if (sem_init(&shared_state->core[i].sem, 1, 0)) {
            ERRX("Failed to initialize semaphore: %s\n", strerror(errno));
        }
    }
}

pid_t _cnnrt_cstub_fork()
{
    pid_t pid = 0;
    for (unsigned int i = 1; i < shared_state->num_cores; i++) {
        pid = fork();
        if (pid < 0) { /* Fork error */
            ERRX("Failed to fork process %d\n", i);
        } else if (pid == 0) { /* Slave exit loop */
            _cnnrt_core_id = i;
            /* Request SIGTERM if parent died */
            prctl(PR_SET_PDEATHSIG, SIGTERM, 0, 0, 0);
            break;
        }
        shared_state->core[i - 1].pid = pid;
    }
    return pid;
}

void _cnnrt_cstub_wait_children()
{
    /* This is the main process. Wait for all children. */
    int status = 0;
    for (unsigned int i = 1; i < shared_state->num_cores; i++) {
        int child_status;
        waitpid(shared_state->core[i - 1].pid, &child_status, 0);
        if (status == 0) {
            if (!WIFEXITED(child_status)) {
                status = -1;
            } else if (WEXITSTATUS(child_status) != 0) {
                /* Propagate first status up */
                status = WEXITSTATUS(child_status);
            }
        }
        shared_state->core[i - 1].pid = 0;
    }
    if (status) {
        ERRX("child process exited with error %d\n", status);
    }
}

XI_ERR_TYPE cnnrt_barrier(XI_ERR_TYPE local_status)
{
    if (shared_state) {
        /* PHASE 1 */
        CHECK_SEM(sem_wait(&shared_state->barrier_sem));

        /* Propagate error */
        if (local_status != XI_ERR_OK) {
            shared_state->barrier_status = local_status;
        }

        shared_state->barrier_count++;
        if (shared_state->barrier_count == shared_state->num_cores) {
            for (unsigned int i=0; i < shared_state->num_cores; i++)
                CHECK_SEM(sem_post(&shared_state->barrier_turnstile_sem));
        }
        CHECK_SEM(sem_post(&shared_state->barrier_sem));
        CHECK_SEM(sem_wait(&shared_state->barrier_turnstile_sem));

        /* PHASE 2 */
        CHECK_SEM(sem_wait(&shared_state->barrier_sem));
        shared_state->barrier_count--;
        if (shared_state->barrier_count == 0) {
            for (unsigned int i=0; i < shared_state->num_cores; i++)
                CHECK_SEM(sem_post(&shared_state->barrier_turnstile2_sem));
        }

        /* Pick up shared status */
        local_status = shared_state->barrier_status;

        CHECK_SEM(sem_post(&shared_state->barrier_sem));
        CHECK_SEM(sem_wait(&shared_state->barrier_turnstile2_sem));

        return local_status;
    } else {
        return XI_ERR_BADARG;
    }
}

/* Master broadcasts task packets to slaves to initiate inference*/
void master_broadcast_task(const mail_packet_t *t)
{
    if (shared_state) {
        CHECK_SEM(sem_wait(&shared_state->command_producer_sem));
        shared_state->command = *t;
        shared_state->num_command_consumed = 0;
        /* Signal to slave cores that command is posted */
        for (unsigned int i = 0; i < shared_state->num_cores - 1; i++)
            CHECK_SEM(sem_post(&shared_state->core[i].sem));
    } else {
        ERRX("Multiprocessing is not initialized\n");
    }
}

/* Slave receives a task packet */
void slave_receive_task(mail_packet_t* t)
{
    if (shared_state) {
        /* Wait for signal from master core */
        CHECK_SEM(sem_wait(&shared_state->core[_cnnrt_core_id - 1].sem));
        CHECK_SEM(sem_wait(&shared_state->command_consumer_sem));
        *t = shared_state->command;
        shared_state->num_command_consumed++;
        if (shared_state->num_command_consumed >= shared_state->num_cores - 1) {
            /* Last consumer signals to producer that the queue is free */
            CHECK_SEM(sem_post(&shared_state->command_producer_sem));
        }
        CHECK_SEM(sem_post(&shared_state->command_consumer_sem));

    } else {
        ERRX("Multiprocessing is not initialized\n");
    }
}

#endif

/* Print packet */
void print_packet(mail_packet_t* t)
{
    int i;
    printf("---- packet -----\n");
    printf("command: %d\n", t->packet_type);

    for (i = 0; i < MAX_PAYLOADS; i++) {
        printf("%08x\n", t->payloads[i]);
    }
}

void cnnrt_init_multicore(unsigned int num_cores)
{
#ifdef __XCC__
    xmp_initialization(num_cores);
#else
    alloc_ipcs(num_cores);
#endif
    if (cnnrt_is_master()) {
        /* Use beginning for memory manager, available only to master core; use a chunk after it for shared memory header */
        _cnnrt_mem_mgr = (xmem_mgr_t *)shared_sram_pool;
        xmem_heap3_init(_cnnrt_mem_mgr, shared_sram_pool + sizeof(xmem_mgr_t) + SHARED_MEM_HEADER_SIZE,
                        SHARED_MEM_POOL_SIZE - sizeof(xmem_mgr_t) - SHARED_MEM_HEADER_SIZE,
                        SHARED_MEM_BLOCK_NUM, shared_sram_pool + sizeof(xmem_mgr_t));
    }
}

#endif /* end of if PARALLEL */



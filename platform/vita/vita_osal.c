/*
 * psp_osal.c
 *
 * Description:
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-embedded (PTE) - POSIX Threads Library for embedded systems
 *      Copyright(C) 2008 Jason Schmidlapp
 *
 *      Contact Email: jschmidlapp@users.sourceforge.net
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "pte_osal.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* For ftime */
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>

// debug
#include <stdarg.h>

#define MAX_PSP_UID 2048 // SWAG

#define DEFAULT_STACK_SIZE_BYTES 0x1000

#define PTHREAD_EVID_CANCEL 0x1

#if 1
#define PSP_DEBUG(x) printf(x)
#else
#define PSP_DEBUG(x)
#endif

#include <sys/psp2cldr_common.h>
#include <sys/psp2cldr_internal.h>

static inline int invert_priority(int priority)
{
    return (pte_osThreadGetMinPriority() - priority) + pte_osThreadGetMaxPriority();
}

/****************************************************************************
 *
 * Initialization
 *
 ***************************************************************************/

pte_osResult PSP2CLDR_STUB pte_osInit(void) // per-process init
{
    UDF_TRAP;
}

/****************************************************************************
 *
 * Threads
 *
 ***************************************************************************/

pte_osResult PSP2CLDR_STUB pte_osThreadCreate(pte_osThreadEntryPoint entryPoint,
                                              int stackSize,
                                              int initialPriority,
                                              void *argv,
                                              pte_osThreadHandle *ppte_osThreadHandle)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osThreadStart(pte_osThreadHandle osThreadHandle)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osThreadDelete(pte_osThreadHandle handle)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osThreadExitAndDelete(pte_osThreadHandle handle)
{
    UDF_TRAP;
}

void PSP2CLDR_STUB pte_osThreadExit()
{
    UDF_TRAP;
}

/*
 * This has to be cancellable, so we can't just call sceKernelWaitThreadEnd.
 * Instead, poll on this in a loop, like we do for a cancellable semaphore.
 */
pte_osResult PSP2CLDR_STUB pte_osThreadWaitForEnd(pte_osThreadHandle threadHandle)
{
    UDF_TRAP;
}

pte_osThreadHandle PSP2CLDR_STUB pte_osThreadGetHandle(void)
{
    UDF_TRAP;
}

int PSP2CLDR_STUB pte_osThreadGetPriority(pte_osThreadHandle threadHandle)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osThreadSetPriority(pte_osThreadHandle threadHandle, int newPriority)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osThreadCancel(pte_osThreadHandle threadHandle)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osThreadCheckCancel(pte_osThreadHandle threadHandle)
{
    UDF_TRAP;
}

void PSP2CLDR_STUB pte_osThreadSleep(unsigned int msecs)
{
    UDF_TRAP;
}

int pte_osThreadGetMinPriority()
{
    return pte_osThreadGetDefaultPriority() - 32;
}

int pte_osThreadGetMaxPriority()
{
    return pte_osThreadGetDefaultPriority() + 31;
}

int pte_osThreadGetDefaultPriority()
{
    return 160;
}

/****************************************************************************
 *
 * Mutexes
 *
 ****************************************************************************/

pte_osResult PSP2CLDR_STUB pte_osMutexCreate(pte_osMutexHandle *pHandle)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osMutexDelete(pte_osMutexHandle handle)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osMutexLock(pte_osMutexHandle handle)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osMutexTimedLock(pte_osMutexHandle handle, unsigned int timeoutMsecs)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osMutexUnlock(pte_osMutexHandle handle)
{
    UDF_TRAP;
}

/****************************************************************************
 *
 * Semaphores
 *
 ***************************************************************************/

pte_osResult PSP2CLDR_STUB pte_osSemaphoreCreate(int initialValue, pte_osSemaphoreHandle *pHandle)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osSemaphoreDelete(pte_osSemaphoreHandle handle)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osSemaphorePost(pte_osSemaphoreHandle handle, int count)
{
    UDF_TRAP;
}

pte_osResult PSP2CLDR_STUB pte_osSemaphorePend(pte_osSemaphoreHandle handle, unsigned int *pTimeoutMsecs)
{
    UDF_TRAP;
}

/*
 * Pend on a semaphore- and allow the pend to be cancelled.
 *
 * PSP OS provides no functionality to asynchronously interrupt a blocked call.  We simulte
 * this by polling on the main semaphore and the cancellation semaphore and sleeping in a loop.
 */
pte_osResult PSP2CLDR_STUB pte_osSemaphoreCancellablePend(pte_osSemaphoreHandle semHandle, unsigned int *pTimeout)
{
    UDF_TRAP;
}

/****************************************************************************
 *
 * Atomic Operations
 *
 ***************************************************************************/

int pte_osAtomicExchange(int *ptarg, int val)
{
    return atomic_exchange(ptarg, val);
}

int pte_osAtomicCompareExchange(int *pdest, int exchange, int comp)
{
    return __extension__(
        {
            (void)(memory_order_seq_cst);
            (void)(memory_order_seq_cst);
            (__sync_val_compare_and_swap(pdest,
                                         comp, exchange));
        });
}

int pte_osAtomicExchangeAdd(int volatile *pAddend, int value)
{
    return atomic_fetch_add(pAddend, value);
}

int pte_osAtomicDecrement(int *pdest)
{
    return __sync_sub_and_fetch(pdest, 1);
}

int pte_osAtomicIncrement(int *pdest)
{
    return __sync_add_and_fetch(pdest, 1);
}

/****************************************************************************
 *
 * Thread Local Storage
 *
 ***************************************************************************/

pte_osResult pte_osTlsSetValue(unsigned int key, void *value)
{
    return __psp2cldr__internal_tls_setvalue(key, value);
}

void *pte_osTlsGetValue(unsigned int index)
{
    return __psp2cldr__internal_tls_getvalue(index);
}

pte_osResult pte_osTlsAlloc(unsigned int *pKey)
{
    *pKey = __psp2cldr__internal_tls_alloc();
    return PTE_OS_OK;
}

pte_osResult pte_osTlsFree(unsigned int index)
{
    __psp2cldr__internal_tls_free(index);
    return PTE_OS_OK;
}

/****************************************************************************
 *
 * Miscellaneous
 *
 ***************************************************************************/

int ftime(struct timeb *tb)
{
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    tb->time = tv.tv_sec;
    tb->millitm = tv.tv_usec / 1000;
    tb->timezone = tz.tz_minuteswest;
    tb->dstflag = tz.tz_dsttime;

    return 0;
}

void __psp2cldr_init_pthread(void)
{
    if (pthread_init() != PTE_TRUE)
        __psp2cldr__internal_panic("pthread_init failed");
}

// pthread does not terminate at a per-thread basis
// void __psp2cldr_fini_pthread(void) {}

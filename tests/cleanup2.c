/*
 * File: cleanup2.c
 *
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-embedded (PTE) - POSIX Threads Library for embedded systems
 *      Copyright(C) 2008 Jason Schmidlapp
 *
 *      Contact Email: jschmidlapp@users.sourceforge.net
 *
 *
 *      Based upon Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2005 Pthreads-win32 contributors
 *
 *      Contact Email: rpj@callisto.canberra.edu.au
 *
 *      The original list of contributors to the Pthreads-win32 project
 *      is contained in the file CONTRIBUTORS.ptw32 included with the
 *      source code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
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
 *
 * --------------------------------------------------------------------------
 *
 * Test Synopsis: Test cleanup handler executes (when thread is not canceled).
 *
 * Test Method (Validation or Falsification):
 * -
 *
 * Requirements Tested:
 * -
 *
 * Features Tested:
 * -
 *
 * Cases Tested:
 * -
 *
 * Description:
 * -
 *
 * Environment:
 * -
 *
 * Input:
 * - None.
 *
 * Output:
 * - File name, Line number, and failed expression on failure.
 * - No output on success.
 *
 * Assumptions:
 * - have working pthread_create, pthread_self, pthread_mutex_lock/unlock
 *   pthread_testcancel, pthread_cancel, pthread_join
 *
 * Pass Criteria:
 * - Process returns zero exit status.
 *
 * Fail Criteria:
 * - Process returns non-zero exit status.
 */


#include "test.h"

#ifdef PTE_SUPPORT_ASYNC_CANCEL

/*
 * Create NUMTHREADS threads in addition to the Main thread.
 */
enum
{
  NUMTHREADS = 10
};

typedef struct bag_t_ bag_t;
struct bag_t_
  {
    int threadnum;
    int started;
    /* Add more per-thread state variables here */
    int count;
  };

static bag_t threadbag[NUMTHREADS + 1];

typedef struct
  {
    int i;
    int null;
    pte_osMutexHandle cs;
  } sharedInt_t;

static sharedInt_t pop_count = {0, 0};

static void
increment_pop_count(void * arg)
{
  sharedInt_t * sI = (sharedInt_t *) arg;

  pte_osMutexLock(sI->cs);
  sI->i++;
  pte_osMutexUnlock(sI->cs);
}

static void *
mythread(void * arg)
{
  int result = 0;
  bag_t * bag = (bag_t *) arg;

  assert(bag == &threadbag[bag->threadnum]);
  assert(bag->started == 0);
  bag->started = 1;

  pthread_cleanup_push(increment_pop_count, (void *) &pop_count);

  sched_yield();

  pthread_cleanup_pop(1);

  return (void *) result;
}

int pthread_test_cleanup2()
{
  int failed = 0;
  int i;
  pthread_t t[NUMTHREADS + 1];

  pop_count.i = 0;
  pop_count.null = 0;
  pte_osMutexCreate(&pop_count.cs);

  assert((t[0] = pthread_self()) != NULL);

  for (i = 1; i <= NUMTHREADS; i++)
    {
      threadbag[i].started = 0;
      threadbag[i].threadnum = i;
      assert(pthread_create(&t[i], NULL, mythread, (void *) &threadbag[i]) == 0);
    }

  /*
   * Code to control or munipulate child threads should probably go here.
   */
  pte_osThreadSleep(1000);

  /*
   * Standard check that all threads started.
   */
  for (i = 1; i <= NUMTHREADS; i++)
    {
      if (!threadbag[i].started)
        {
          failed |= !threadbag[i].started;
        }
    }

  assert(!failed);

  /*
   * Check any results here. Set "failed" and only print output on failure.
   */
  failed = 0;
  for (i = 1; i <= NUMTHREADS; i++)
    {
      int fail = 0;
      int result = 0;

      assert(pthread_join(t[i], (void **) &result) == 0);

      fail = (result != 0);

      failed = (failed || fail);
    }

  assert(!failed);

  assert(pop_count.i == NUMTHREADS);

  pte_osMutexDelete(pop_count.cs);

  /*
   * Success.
   */
  return 0;
}

#else /* PTE_SUPPORT_ASYNC_CANCEL */

int pthread_test_cleanup2()
{
  printf("Test not run - async cancellation not supported\n");

  return 0;
}

#endif /* !PTE_SUPPORT_ASYNC_CANCEL */


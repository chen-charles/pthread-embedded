/*
 * File: exception3.c
 *
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2005 Pthreads-win32 contributors
 * 
 *      Contact Email: rpj@callisto.canberra.edu.au
 * 
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
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
 * Test Synopsis: Test running of user supplied terminate() function.
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

#if defined(__cplusplus)

# if defined(__GNUC__) && __GNUC__ < 3
#   include <new.h>
# else
#   include <new>
    using std::set_terminate;
# endif


/*
 * Create NUMTHREADS threads in addition to the Main thread.
 */
enum {
  NUMTHREADS = 1
};

int caught = 0;
pthread_mutex_t caughtLock;



static void
terminateFunction ()
{
  assert(pthread_mutex_lock(&caughtLock) == 0);
  caught++;
  assert(pthread_mutex_unlock(&caughtLock) == 0);

  pthread_exit((void *) 0);
}

static void *
exceptionedThread(void * arg)
{
  int dummy = 0x1;

  (void) set_terminate(&terminateFunction);

  throw dummy;

  return (void *) 0;
}

int
pthread_test_exception3()
{
  int i;
  pthread_t mt;
  pthread_t et[NUMTHREADS];
  pthread_mutexattr_t ma;

  assert((mt = pthread_self()) != NULL);

  assert(pthread_mutexattr_init(&ma) == 0);
  assert(pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_ERRORCHECK) == 0);
  assert(pthread_mutex_init(&caughtLock, &ma) == 0);
  assert(pthread_mutexattr_destroy(&ma) == 0);

  for (i = 0; i < NUMTHREADS; i++)
    {
      assert(pthread_create(&et[i], NULL, exceptionedThread, NULL) == 0);
    }

  pte_osThreadSleep(1000);

  assert(caught == NUMTHREADS);

  /*
   * Success.
   */
  return 0;
}

#else /* defined(__cplusplus) */

#include <stdio.h>

int
pthread_test_exception3()
{
  printf("Test N/A for this compiler environment.\n");
  return 0;
}

#endif /* defined(__cplusplus) */

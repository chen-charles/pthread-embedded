/*
 * File: condvar3_1.c
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
 * Test Synopsis:
 * - Test timeout of multiple waits on a CV with some signaled.
 *
 * Test Method (Validation or Falsification):
 * - Validation
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
 * - Because some CVs are never signaled, we expect their waits to time out.
 *   Some are signaled, the rest time out. Pthread_cond_destroy() will fail
 *   unless all are accounted for, either signaled or timedout.
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
 * -
 *
 * Pass Criteria:
 * - pthread_cond_timedwait returns ETIMEDOUT.
 * - Process returns zero exit status.
 *
 * Fail Criteria:
 * - pthread_cond_timedwait does not return ETIMEDOUT.
 * - Process returns non-zero exit status.
 */

#define _WIN32_WINNT 0x400

#include "test.h"


static pthread_cond_t cv;
static pthread_cond_t cv1;
static pthread_mutex_t mutex;
static pthread_mutex_t mutex1;
static struct timespec abstime =
  {
    0, 0
  };
static int timedout = 0;
static int signaled = 0;
static int awoken = 0;
static int waiting = 0;

enum
{
  NUMTHREADS = 30
};

static void *
mythread(void * arg)
{
  int result;
  struct _timeb currSysTime;
  const unsigned int NANOSEC_PER_MILLISEC = 1000000;

  assert(pthread_mutex_lock(&mutex1) == 0);
  ++waiting;
  assert(pthread_mutex_unlock(&mutex1) == 0);
  assert(pthread_cond_signal(&cv1) == 0);

  assert(pthread_mutex_lock(&mutex) == 0);

  /* get current system time */
  _ftime(&currSysTime);

  abstime.tv_sec = currSysTime.time;
  abstime.tv_nsec = NANOSEC_PER_MILLISEC * currSysTime.millitm;

  abstime.tv_sec += 5;

  result = pthread_cond_timedwait(&cv, &mutex, &abstime);
  if (result == ETIMEDOUT)
    {
      timedout++;
    }
  else
    {
      awoken++;
    }
  assert(pthread_mutex_unlock(&mutex) == 0);

  return arg;
}

#include "../implement.h"

int pthread_test_condvar3_1()
{
  int i;
  pthread_t t[NUMTHREADS + 1];
  int result = 0;

  timedout = 0;
  signaled = 0;
  awoken = 0;
  waiting = 0;


  assert(pthread_cond_init(&cv, NULL) == 0);
  assert(pthread_cond_init(&cv1, NULL) == 0);

  assert(pthread_mutex_init(&mutex, NULL) == 0);
  assert(pthread_mutex_init(&mutex1, NULL) == 0);

  assert(pthread_mutex_lock(&mutex1) == 0);

  for (i = 1; i <= NUMTHREADS; i++)
    {
      assert(pthread_create(&t[i], NULL, mythread, (void *) i) == 0);
    }

  do
    {
      assert(pthread_cond_wait(&cv1,&mutex1) == 0);
    }
  while ( NUMTHREADS > waiting );

  assert(pthread_mutex_unlock(&mutex1) == 0);

  for (i = NUMTHREADS/3; i <= 2*NUMTHREADS/3; i++)
    {
     assert(pthread_mutex_lock(&mutex) == 0);
      assert(pthread_cond_signal(&cv) == 0);
     assert(pthread_mutex_unlock(&mutex) == 0);

      signaled++;
    }

  for (i = 1; i <= NUMTHREADS; i++)
    {
      assert(pthread_join(t[i], (void **) &result) == 0);
      assert(result == i);
    }

  assert(signaled == awoken);
  assert(timedout == NUMTHREADS - signaled);

  assert(pthread_cond_destroy(&cv1) == 0);

  {
    assert(pthread_cond_destroy(&cv) == 0);
  }

  assert(pthread_mutex_destroy(&mutex1) == 0);
  assert(pthread_mutex_destroy(&mutex) == 0);

  return 0;
}

/*
 * File: condvar9.c
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
 * - Test multiple pthread_cond_broadcasts with thread cancelation.
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
 * - Make NUMTHREADS threads wait on CV, cancel one, broadcast signal them,
 *   and then repeat.
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
 * - Process returns zero exit status.
 *
 * Fail Criteria:
 * - Process returns non-zero exit status.
 */

#include "test.h"


/*
 * Create NUMTHREADS threads in addition to the Main thread.
 */
enum
{
  NUMTHREADS = 9
};

typedef struct bag_t_ bag_t;
struct bag_t_
  {
    int threadnum;
    int started;
    int finished;
    /* Add more per-thread state variables here */
  };

static bag_t threadbag[NUMTHREADS + 1];

typedef struct cvthing_t_ cvthing_t;

struct cvthing_t_
  {
    pthread_cond_t notbusy;
    pthread_mutex_t lock;
    int shared;
  };

static cvthing_t cvthing =
{
  PTHREAD_COND_INITIALIZER,
  PTHREAD_MUTEX_INITIALIZER,
  0
};

static pthread_mutex_t start_flag = PTHREAD_MUTEX_INITIALIZER;

static struct timespec abstime =
  {
    0, 0
  };

static int awoken;

static void *
mythread(void * arg)
{
  bag_t * bag = (bag_t *) arg;

  assert(bag == &threadbag[bag->threadnum]);
  assert(bag->started == 0);
  bag->started = 1;

  /* Wait for the start gun */
  assert(pthread_mutex_lock(&start_flag) == 0);
  assert(pthread_mutex_unlock(&start_flag) == 0);

  assert(pthread_mutex_lock(&cvthing.lock) == 0);

  /*
   * pthread_cond_timedwait is a cancelation point and we're
   * going to cancel some threads deliberately.
   */
  pthread_cleanup_push(pthread_mutex_unlock, (void *) &cvthing.lock);

  while (! (cvthing.shared > 0))
    assert(pthread_cond_timedwait(&cvthing.notbusy, &cvthing.lock, &abstime) == 0);

  pthread_cleanup_pop(0);

  assert(cvthing.shared > 0);

  awoken++;
  bag->finished = 1;

  assert(pthread_mutex_unlock(&cvthing.lock) == 0);

  return (void *) 0;
}

int pthread_test_condvar9()
{
  int failed = 0;
  int i;
  int first, last;
  int canceledThreads = 0;
  pthread_t t[NUMTHREADS + 1];

  struct _timeb currSysTime;
  const unsigned int NANOSEC_PER_MILLISEC = 1000000;

  cvthing.notbusy = PTHREAD_COND_INITIALIZER;
  cvthing.lock = PTHREAD_MUTEX_INITIALIZER;

  assert((t[0] = pthread_self()) != NULL);

  assert(cvthing.notbusy == PTHREAD_COND_INITIALIZER);

  assert(cvthing.lock == PTHREAD_MUTEX_INITIALIZER);

  _ftime(&currSysTime);

  abstime.tv_sec = currSysTime.time;
  abstime.tv_nsec = NANOSEC_PER_MILLISEC * currSysTime.millitm;

  abstime.tv_sec += 5;

  assert((t[0] = pthread_self()) != NULL);

  awoken = 0;

  for (first = 1, last = NUMTHREADS / 2;
       first < NUMTHREADS;
       first = last + 1, last = NUMTHREADS)
    {
      int ct;

      assert(pthread_mutex_lock(&start_flag) == 0);

      for (i = first; i <= last; i++)
        {
          threadbag[i].started = threadbag[i].finished = 0;
          threadbag[i].threadnum = i;
          assert(pthread_create(&t[i], NULL, mythread, (void *) &threadbag[i]) == 0);
        }

      /*
       * Code to control or munipulate child threads should probably go here.
       */
      cvthing.shared = 0;

      assert(pthread_mutex_unlock(&start_flag) == 0);

      /*
       * Give threads time to start.
       */
      pte_osThreadSleep(1000);

      ct = (first + last) / 2;
      assert(pthread_cancel(t[ct]) == 0);
      canceledThreads++;
      assert(pthread_join(t[ct], NULL) == 0);

      assert(pthread_mutex_lock(&cvthing.lock) == 0);
      cvthing.shared++;
      assert(pthread_mutex_unlock(&cvthing.lock) == 0);

      assert(pthread_cond_broadcast(&cvthing.notbusy) == 0);

      /*
       * Standard check that all threads started - and wait for them to finish.
       */
      for (i = first; i <= last; i++)
        {
          failed = !threadbag[i].started;

          if (!failed)
            {
              assert(pthread_join(t[i], NULL) == 0 || threadbag[i].finished == 0);
            }
        }
    }

  /*
   * Cleanup the CV.
   */

  assert(pthread_mutex_destroy(&cvthing.lock) == 0);

  assert(cvthing.lock == NULL);

  assert(pthread_cond_destroy(&cvthing.notbusy) == 0);

  assert(cvthing.notbusy == NULL);

  assert(!failed);

  /*
   * Check any results here.
   */

  assert(awoken == NUMTHREADS - canceledThreads);

  /*
   * Success.
   */
  return 0;
}

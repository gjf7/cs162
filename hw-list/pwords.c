/*
 * Word count application with one thread per input file.
 *
 * You may modify this file in any way you like, and are expected to modify it.
 * Your solution must read each input file from a separate thread. We encourage
 * you to make as few changes as necessary.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#include "word_count.h"
#include "word_helpers.h"

struct thread_params {
  FILE* infile;
  word_count_list_t* word_counts;
};

void* thread_count_word(void* params) {
  struct thread_params* p = (struct thread_params*)params;
  if (p->infile == NULL) {
    fprintf(stderr, "File does not exist.\n");
    pthread_exit(NULL);
  }

  count_words(p->word_counts, p->infile);

  fclose(p->infile);
  pthread_exit(NULL);
}

/*
 * main - handle command line, spawning one thread per file.
 */
int main(int argc, char* argv[]) {
  /* Create the empty data structure. */
  word_count_list_t word_counts;
  init_words(&word_counts);

  if (argc <= 1) {
    /* Process stdin in a single thread. */
    count_words(&word_counts, stdin);
  } else {
    pthread_t threads[argc];
    for (int i = 1; i < argc; i++) {
      FILE* infile = fopen(argv[i], "r");

      struct thread_params* params = (struct thread_params*)malloc(sizeof(struct thread_params));
      if (params == NULL) {
        fprintf(stderr, "Failed to allocate memory for thread parameters.\n");
        fclose(infile);
        return 1;
      }
      params->infile = infile;
      params->word_counts = &word_counts;

      int rc = pthread_create(&threads[i], NULL, thread_count_word, (void*)params);
      if (rc) {
        fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc);
        fclose(infile);
        return 1;
      }
    }

    for (int i = 1; i < argc; i++) {
      pthread_join(threads[i], NULL);
    }
  }

  /* Output final result of all threads' work. */
  wordcount_sort(&word_counts, less_count);
  fprint_words(&word_counts, stdout);
  return 0;
}

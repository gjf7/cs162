/*
 * Implementation of the word_count interface using Pintos lists and pthreads.
 *
 * You may modify this file, and are expected to modify it.
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

#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_lp.c"
#endif

#ifndef PTHREADS
#error "PTHREADS must be #define'd when compiling word_count_lp.c"
#endif

#include "word_count.h"

char* new_string(char* str) {
  char* new_str = (char*)malloc(strlen(str) + 1);
  if (new_str == NULL) {
    return NULL;
  }
  return strcpy(new_str, str);
}

void init_words(word_count_list_t* wclist) { /* TODO */
  list_init(&wclist->lst);
  pthread_mutex_init(&wclist->lock, NULL);
}

size_t len_words(word_count_list_t* wclist) {
  if (wclist == NULL)
    return -1;

  return list_size(&wclist->lst);
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
  struct list_elem* e;
  for (e = list_begin(&wclist->lst); e != list_end(&wclist->lst); e = list_next(e)) {
    word_count_t* wc = list_entry(e, word_count_t, elem);
    if (strcmp(wc->word, word) == 0)
      return wc;
  }

  return NULL;
}

word_count_t* add_word(word_count_list_t* wclist, char* word) {
  if (wclist == NULL || word == NULL)
    return NULL;

  word_count_t* wc;
  pthread_mutex_lock(&wclist->lock);
  if ((wc = find_word(wclist, word))) {
    wc->count++;
    pthread_mutex_unlock(&wclist->lock);
    return wc;
  }

  word_count_t* new_wc = (word_count_t*)malloc(sizeof(word_count_t));
  if (new_wc == NULL) {
    pthread_mutex_unlock(&wclist->lock);
    return NULL;
  }

  new_wc->count = 1;
  new_wc->word = new_string(word);

  list_push_front(&wclist->lst, &new_wc->elem);
  pthread_mutex_unlock(&wclist->lock);

  return new_wc;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) {
  /* TODO */
  /* Please follow this format: fprintf(<file>, "%i\t%s\n", <count>, <word>); */
  struct list_elem* e;
  for (e = list_begin(&wclist->lst); e != list_end(&wclist->lst); e = list_next(e)) {
    word_count_t* wc = list_entry(e, word_count_t, elem);
    fprintf(outfile, "%i\t%s\n", wc->count, wc->word);
  }
}

static bool less_list(const struct list_elem* ewc1, const struct list_elem* ewc2, void* aux) {
  word_count_t* wc1 = list_entry(ewc1, word_count_t, elem);
  word_count_t* wc2 = list_entry(ewc2, word_count_t, elem);

  if (wc1 == NULL || wc2 == NULL) {
    return false; // 或者根据需求处理 NULL 情况
  }

  return ((bool (*)(word_count_t*, word_count_t*))aux)(wc1, wc2);
}

void wordcount_sort(word_count_list_t* wclist,
                    bool less(const word_count_t*, const word_count_t*)) {
  /* TODO */
  pthread_mutex_lock(&wclist->lock);
  list_sort(&wclist->lst, less_list, less);
  pthread_mutex_unlock(&wclist->lock);
}

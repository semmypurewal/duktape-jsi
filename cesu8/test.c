#include "cesu8.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  const char ok_utf8[] = {0xF0, 0x9F, 0x86, 0x97, '\0'};
  const char ok_cesu8[] = {0xED, 0xA0, 0xBC, 0xED, 0xB6, 0x97, '\0'};
  const char mixed_utf8[] = {'t', 'h',  'u',  'm',  'b',  's', 'u',
                             'p', 0xF0, 0x9F, 0x91, 0x8D, '!', '\0'};
  const char mixed_cesu8[] = {'t',  'h',  'u',  'm',  'b',  's',  'u', 'p',
                              0xED, 0xA0, 0xBD, 0xED, 0xB1, 0x8D, '!', '\0'};
  const char thumbs_up_utf8[] = {0xF0, 0x9F, 0x91, 0x8D, '\0'};
  const char *ascii = "hello world";
  char *cesu8_from_ascii;
  char *utf8_from_ascii;
  char *ok_cesu8_copy;
  char *ok_utf8_copy;
  char *mixed_cesu8_copy;
  char *mixed_utf8_copy;

  cesu8_from_ascii = (char *)copy_utf8_as_cesu8(ascii);
  assert(strcmp(ascii, cesu8_from_ascii) == 0);
  assert(strlen(ascii) == strlen(cesu8_from_ascii));
  free(cesu8_from_ascii);

  utf8_from_ascii = (char *)copy_cesu8_as_utf8(ascii);
  assert(strcmp(ascii, utf8_from_ascii) == 0);
  assert(strlen(ascii) == strlen(utf8_from_ascii));
  free(utf8_from_ascii);

  ok_cesu8_copy = (char *)copy_utf8_as_cesu8(ok_utf8);
  assert(strlen(ok_cesu8_copy) == strlen(ok_cesu8));
  assert(strcmp(ok_cesu8_copy, ok_cesu8) == 0);
  free(ok_cesu8_copy);

  ok_utf8_copy = (char *)copy_cesu8_as_utf8(ok_cesu8);
  assert(strlen(ok_utf8_copy) == strlen(ok_utf8));
  assert(strcmp(ok_utf8_copy, ok_utf8) == 0);
  free(ok_utf8_copy);

  mixed_cesu8_copy = (char *)copy_utf8_as_cesu8(mixed_utf8);
  assert(strlen(mixed_cesu8_copy) == strlen(mixed_cesu8));
  assert(strcmp(mixed_cesu8_copy, mixed_cesu8) == 0);
  free(mixed_cesu8_copy);

  mixed_utf8_copy = (char *)copy_cesu8_as_utf8(mixed_utf8);
  assert(strlen(mixed_utf8_copy) == strlen(mixed_utf8));
  assert(strcmp(mixed_utf8_copy, mixed_utf8) == 0);
  free(mixed_utf8_copy);

  printf("%s\n", thumbs_up_utf8);
}

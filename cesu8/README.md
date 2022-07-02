# CESU-8 ANSI C header library

This is an ANSI C header library that implements some basic utility functions that convert between [CESU-8](https://www.unicode.org/reports/tr26/) and [UTF-8](https://en.wikipedia.org/wiki/UTF-8) Unicode representations.

From the [associated technical report](https://www.unicode.org/reports/tr26/):

> CESU-8 documents an obsolete internal-use encoding scheme for Unicode identical to UTF-8 except for its representation of supplementary characters. In CESU-8, supplementary characters are represented as six-byte sequences rather than four-byte sequences.

I've been playing around with the [Duktape](https://duktape.org) JavaScript runtime which uses CESU-8 for its internal representation of unicode characters. I wasn't able to find a small library to convert between UTF-8 and CESU-8, so I wrote this one.

## API

This library allocates new strings when doing the conversion, so you'll want to `free` the strings when you are finished with them. The alternative is to require the caller to pre-allocate a buffer, but that's a bit complex since the resulting length depends on the number of supplementary characters.

To convert a C-style string from cesu-8 to utf-8:

```c
  const char ok_cesu8[] = {0xED, 0xA0, 0xBC, 0xED, 0xB6, 0x97, '\0'};
  const char *ok_utf8_copy = (char *)copy_cesu8_as_utf8(ok_cesu8, strlen(ok_cesu8));
  printf("%s\n", ok_utf8_copy); // prints the :ok: emoji
  free(ok_utf8_copy);
```

To convert a C-style string from utf-8 to cesu-8:

```c
  const char thumbs_up_utf8[] = {0xF0, 0x9F, 0x91, 0x8D, '\0'};
  const char thumbs_up_cesu8_copy = (char*) copy_utf8_as_cesu8(thumbs_up_utf8, strlen(thumbs_up_utf8));
  // do something with the copy
  free(thumbs_up_cesu8_copy);
```

Note that if a string does not contain non-supplementary characters, you might not want to allocate additional memory or make a copy. Currently, the best way to determine that is to check the resulting length and see if its the same as the original string.

```c
  if (strlen(cesu8_str) == utf8_len_from_cesu8(cesu8_str, strlen(cesu8_str))) {
      // no need to copy since the utf8 and cesu8 strings are equivalent
  } else {
      str_utf8_copy = copy_cesu8_as_utf8(cesu8_str, strlen(cesu8_str));  // don't forget to free it later
  }
```

There's a related `cesu8_len_from_utf8` function, along with a handful of other functions that may be useful in other contexts.
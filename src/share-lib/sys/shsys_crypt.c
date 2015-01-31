
/*
 *  Copyright 2015 Neo Natura
 *
 *  This file is part of the Share Library.
 *  (https://github.com/neonatura/share)
 *        
 *  The Share Library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version. 
 *
 *  The Share Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.
*/  

#include "share.h"

char *shcrypt(const char *passwd, const char *salt)
{
  char *cr_pass;

  if (strlen(salt) < 3)
    return (NULL);

  cr_pass = NULL;
  if (0 == strncmp(salt, "$6$", 3)) {
    cr_pass = crypt_sha512(passwd, salt);
  } else if (0 == strncmp(salt, "$5$", 3)) {
    cr_pass = crypt_sha256(passwd, salt);
#if 0
  } else if (0 == strncmp(salt, "$1", 3)) {
    cr_pass = crypt_md5(passwd, salt);
#endif
  }
 
  return (cr_pass);
}

static const struct {
  const char *salt;
  const char *input;
  const char *expected;
} _sha256_test[] =
{
  {
    "$5$saltstring", "Hello world!",
    "$5$saltstring$5B8vYYiY.CVt1RlTTf8KbXBH3hsxY/GNooZaBBGWEc5"
  },
  {
    "$5$rounds=10000$saltstringsaltstring", "Hello world!",
    "$5$rounds=10000$saltstringsaltst$3xv.VbSHBb41AL9AvLeujZkZRBAwqFMz2."
      "opqey6IcA"
  },
  {
    "$5$rounds=5000$toolongsaltstring", "This is just a test",
    "$5$rounds=5000$toolongsaltstrin$Un/5jzAHMgOGZ5.mWJpuVolil07guHPvOW8"
      "mGRcvxa5"
  },
  {
    "$5$rounds=1400$anotherlongsaltstring",
    "a very much longer text to encrypt.  This one even stretches over more"
      "than one line.",
    "$5$rounds=1400$anotherlongsalts$Rx.j8H.h8HjEDGomFU8bDkXm3XIUnzyxf12"
      "oP84Bnq1"
  },
  {
    "$5$rounds=77777$short",
    "we have a short salt string but not a short password",
    "$5$rounds=77777$short$JiO1O3ZpDAxGJeaDIuqCoEFysAe1mZNJRs3pw0KQRd/"
  },
  {
    "$5$rounds=123456$asaltof16chars..", "a short string",
    "$5$rounds=123456$asaltof16chars..$gP3VQ/6X7UUEW3HkBn2w1/Ptq2jxPyzV/"
      "cZKmF/wJvD"
  },
  {
    "$5$rounds=10$roundstoolow", "the minimum number is still observed",
    "$5$rounds=1000$roundstoolow$yfvwcWrQ8l/K0DAWyuPMDNHpIVlTQebY9l/gL97"
      "2bIC"
  },
};
#define n_sha256_test (sizeof (_sha256_test) / sizeof (_sha256_test[0]))

_TEST(shcrypt)
{
  int cnt;

#if 0
  for (cnt = 0; cnt < n_sha256_test; ++cnt) {
    char *cp = shcrypt(_sha256_test[cnt].input, _sha256_test[cnt].salt);

    _TRUE(0 == strcmp(cp, _sha256_test[cnt].expected));

    if (strcmp(cp, _sha256_test[cnt].expected) != 0) {
      printf("test %d: expected \"%s\", got \"%s\"\n",
          cnt, _sha256_test[cnt].expected, cp);
    }
  }
#endif

}


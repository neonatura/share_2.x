
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
    cr_pass = shcrypt_sha512(passwd, salt);
  } else if (0 == strncmp(salt, "$5$", 3)) {
    cr_pass = shcrypt_sha256(passwd, salt);
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

static const struct {
  const char *salt;
  const char *input;
  const char *expected;
} _sha512_test[] =
{
  {
    "$6$saltstring", "Hello world!",
    "$6$saltstring$svn8UoSVapNtMuq1ukKS4tPQd8iKwSMHWjl/O817G3uBnIFNjnQJu"
    "esI68u4OTLiBFdcbYEdFCoEOfaS35inz1"
  },
  {
    "$6$rounds=10000$saltstringsaltstring", "Hello world!",
    "$6$rounds=10000$saltstringsaltst$OW1/O6BYHV6BcXZu8QVeXbDWra3Oeqh0sb"
    "HbbMCVNSnCM/UrjmM0Dp8vOuZeHBy/YTBmSK6H9qs/y3RnOaw5v."
  },
  {
    "$6$rounds=5000$toolongsaltstring", "This is just a test",
    "$6$rounds=5000$toolongsaltstrin$lQ8jolhgVRVhY4b5pZKaysCLi0QBxGoNeKQ"
    "zQ3glMhwllF7oGDZxUhx1yxdYcz/e1JSbq3y6JMxxl8audkUEm0"
  },
  {
    "$6$rounds=1400$anotherlongsaltstring",
    "a very much longer text to encrypt.  This one even stretches over more"
    "than one line.",
    "$6$rounds=1400$anotherlongsalts$POfYwTEok97VWcjxIiSOjiykti.o/pQs.wP"
    "vMxQ6Fm7I6IoYN3CmLs66x9t0oSwbtEW7o7UmJEiDwGqd8p4ur1"
  },
  {
    "$6$rounds=77777$short",
    "we have a short salt string but not a short password",
    "$6$rounds=77777$short$WuQyW2YR.hBNpjjRhpYD/ifIw05xdfeEyQoMxIXbkvr0g"
    "ge1a1x3yRULJ5CCaUeOxFmtlcGZelFl5CxtgfiAc0"
  },
  {
    "$6$rounds=123456$asaltof16chars..", "a short string",
    "$6$rounds=123456$asaltof16chars..$BtCwjqMJGx5hrJhZywWvt0RLE8uZ4oPwc"
    "elCjmw2kSYu.Ec6ycULevoBK25fs2xXgMNrCzIMVcgEJAstJeonj1"
  },
  {
    "$6$rounds=10$roundstoolow", "the minimum number is still observed",
    "$6$rounds=1000$roundstoolow$kUMsbe306n21p9R.FRkW3IGn.S9NPN0x50YhH1x"
    "hLsPuWGsUSklZt58jaTfF4ZEQpyUNGc0dqbpBYYBaHHrsX."
  },
};
#define n_sha512_test (sizeof (_sha512_test) / sizeof (_sha512_test[0]))


_TEST(shcrypt)
{
  int cnt;

  for (cnt = 0; cnt < n_sha256_test; cnt++) {
    char *cp = shcrypt(_sha256_test[cnt].input, _sha256_test[cnt].salt);
    _TRUEPTR(cp);
    _TRUE(0 == strcmp(cp, _sha256_test[cnt].expected));
  }

  for (cnt = 0; cnt < n_sha512_test; ++cnt) {
    char *cp = shcrypt_sha512(_sha512_test[cnt].input, _sha512_test[cnt].salt);

    _TRUE(0 == strcmp(cp, _sha512_test[cnt].expected));
  }

}


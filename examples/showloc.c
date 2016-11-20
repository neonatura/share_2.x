
/*
 * @copyright
 *
 *  Copyright 2016 Neo Natura
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
 *
 *  @endcopyright
*/  

/* gcc -o showloc showloc.c -lshare */

#include <stdio.h>
#include <string.h>
#include <share.h>
#include <unistd.h>

shgeo_t geo;
shloc_t loc;
shnum_t lat, lon;

int main(int argc, char *argv[])
{
  char *place_str;
  unsigned int a, b, c, d;
  int err;
  int n;

  if (argc <= 1 || 0 == strcmp(argv[1], "-h")) {
    printf(
      "showloc [PARAM]\n"
      "\n"
      "Parameters:\n"
      "\t<city>, <state-abrev>\n"
      "\t<zip code>\n"
      "\t<ipv4 address>\n"
      "\tgeo:<latitude>,<longitude>\n"
      "\n"
      "Examples:\n"
      "\tshowloc \"Missoula, MT\"\n"
      "\tshowloc 59801\n"
      "\tshowloc 100.0.0.1\n"
      "\tshowloc geo:46.9,114.2\n");
    exit(1);
  }

  memset(&geo, 0, sizeof(geo));
  n = sscanf(argv[1], "geo:%Lf,%Lf", &lat, &lon);
  if (n == 2) {
    memset(&loc, 0, sizeof(loc));

    err = shgeodb_scan(lat, lon, 0.5, &geo); 
    if (err) {
      printf("ERROR: lat/lon (%Lf,%Lf): %s.\n", lat, lon, sherrstr(err));
      return (1);
    }

    /* reference found location */
    shgeo_loc(&geo, &lat, &lon, NULL);

    err = shgeodb_loc(&geo, &loc);
    if (err) {
      printf("ERROR: lat/lon (%Lf,%Lf): %s.\n", lat, lon, sherrstr(err));
      return (1);
    }

    printf("INFO: lat/lon (%Lf,%Lf) near '%s' (%s)\n", lat, lon, loc.loc_name, shgeo_place_desc(loc.loc_type));
    if (*loc.loc_summary) {
      printf("INFO: Desc: %s (Zone: %s)\n", loc.loc_summary, *loc.loc_zone ? loc.loc_zone : "n/a");
    }
    return (0);
  }

  n = sscanf(argv[1], "%u.%u.%u.%u", &a, &b, &c, &d);
  if (n == 4) {
    char *addr_str = argv[1];

    memset(&geo, 0, sizeof(geo));
    err = shgeodb_host(addr_str, &geo);
    if (err) {
      printf("ERROR: host '%s' not found: %s\n", addr_str, sherrstr(err));
      return (1);
    }

    shgeo_loc(&geo, &lat, &lon, NULL);
    printf("INFO: host '%s': %Lf,%Lf\n", addr_str, lat, lon);

#if 0
memset(&geo, 0, sizeof(geo));
    err = shgeodb_scan(lat, lon, 0.5, &geo); 
    if (!err) {
      err = shgeodb_loc(&geo, &loc);
      if (!err) {
        printf("INFO: host '%s' near '%s' (%s)\n", addr_str, loc.loc_name, shgeo_place_desc(loc.loc_type));
      }
    }
#endif

    return (0);
  } 


  place_str  = argv[1];
  memset(&geo, 0, sizeof(geo));
  err = shgeodb_place(place_str, &geo);
  if (err) {
    printf("ERROR: place '%s' not found: %s\n", place_str, sherrstr(err));
  } else {
    shgeo_loc(&geo, &lat, &lon, NULL);
    printf("INFO: place '%s': %Lf,%Lf\n", place_str, lat, lon);
  }

 
  return (0);
}

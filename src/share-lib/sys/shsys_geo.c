
/*
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
*/  

#include "share.h"
#include <math.h>

/**
 * Upper limit of roughly 300k miles for allowed altitude. 
 */
#define SHGEO_MAX_ALTITUDE 1584000000

void shgeo_set(shgeo_t *geo, shnum_t lat, shnum_t lon, int alt)
{

  alt = MIN(alt, 1584000000);

  geo->geo_stamp = shtime();
  shnum_set((shnum_t)lat, &geo->geo_lat);
  shnum_set((shnum_t)lon, &geo->geo_lon);
  geo->geo_alt = (uint32_t)alt;

}

void shgeo_loc(shgeo_t *geo, shnum_t *lat_p, shnum_t *lon_p, int *alt_p)
{

  if (lat_p)
    *lat_p = (shnum_t)shnum_get(geo->geo_lat);

  if (lon_p)
    *lon_p = (shnum_t)shnum_get(geo->geo_lon);

  if (alt_p)
    *alt_p = (int)geo->geo_alt;

}

time_t shgeo_timestamp(shgeo_t *geo)
{
  return (shutime(geo->geo_stamp));
}

time_t shgeo_lifespan(shgeo_t *geo)
{
  return ((time_t)shtime_diff(shtime(), geo->geo_stamp));
}

shkey_t *shgeo_tag(shgeo_t *geo, int prec)
{
  shnum_t nlat;
  shnum_t nlon;
  double lat;
  double lon;
  char buf[32];

  shgeo_loc(geo, &nlat, &nlon, NULL);

  lat = (double)shnum_prec_dim(nlat, prec);
  lon = (double)shnum_prec_dim(nlon, prec);

  memset(buf, 0, sizeof(buf));
  memcpy(buf, &lat, sizeof(double));
  memcpy(buf + sizeof(double), &lon, sizeof(double));

  return (shkey_bin(buf, sizeof(double) * 2));
}

int shgeo_cmp(shgeo_t *geo, shgeo_t *cmp_geo, int prec)
{
  shnum_t lat;
  shnum_t lon;
  shnum_t c_lat;
  shnum_t c_lon;

  shgeo_loc(geo, &lat, &lon, NULL);
  lat = shnum_prec_dim(lat, prec);
  lon = shnum_prec_dim(lon, prec);

  shgeo_loc(geo, &c_lat, &c_lon, NULL);
  c_lat = shnum_prec_dim(c_lat, prec);
  c_lon = shnum_prec_dim(c_lon, prec);

  return (lat == c_lat && lon == c_lon);
}

int shgeo_cmpf(shgeo_t *geo, double lat, double lon)
{
  shgeo_t t_geo;
  shgeo_set(&t_geo, (shnum_t)lat, (shnum_t)lon, 0);
  return (shgeo_cmp(geo, &t_geo, SHGEO_PREC_POINT)); 
}

static shnum_t _deg2rad(shnum_t deg) 
{
  return (deg * M_PI / 180.0);
}

static shnum_t _rad2deg(shnum_t rad) 
{
  static const shnum_t half_deg = 180.0;
  return (rad * half_deg / (shnum_t)M_PI);
}

/**
 * Calculate the distance between two geodetic location in miles.
 */
double shgeo_dist(shgeo_t *f_geo, shgeo_t *t_geo)
{
  static const shnum_t mile_mod = 90.9;
  shnum_t theta, dist;
  shnum_t lat1, lat2;
  shnum_t lon1, lon2;

  shgeo_loc(f_geo, &lat1, &lon1, NULL);
  shgeo_loc(t_geo, &lat2, &lon2, NULL);

  theta = lon1 - lon2;
  dist = (sinl(_deg2rad(lat1)) * sinl(_deg2rad(lat2))) + 
    (cosl(_deg2rad(lat1)) * cosl(_deg2rad(lat2)) * cosl(_deg2rad(theta)));
  dist = acosl(dist);
  dist = _rad2deg(dist);
  dist = dist * mile_mod;

  return ((double)dist);
}

double shgeo_radius(shgeo_t *f_geo, shgeo_t *t_geo)
{
  return (shgeo_dist(f_geo, t_geo) / 2);
}


_TEST(shgeo_dist)
{
  shgeo_t fl_geo;
  shgeo_t ms_geo;
  shnum_t in_lat, in_lon;
  shnum_t lat, lon;
  double d;
  int ok;

  in_lat = 44.66;
  in_lon = -114.0;
  shgeo_set(&fl_geo, in_lat, in_lon, 1); 
  shgeo_loc(&fl_geo, &lat, &lon, NULL);
  _TRUE((float)lat == (float)in_lat);
  _TRUE((float)lon == (float)(in_lon * -1));

  in_lat = 46.87;
  in_lon = -113.99;
  shgeo_set(&ms_geo, in_lat, in_lon, 0); 
  shgeo_loc(&ms_geo, &lat, &lon, NULL);
  _TRUE((float)lat == (float)in_lat);
  _TRUE((float)lon == (float)(in_lon * -1));

  d = ceil(shgeo_dist(&fl_geo, &ms_geo));
  _TRUE(d == 201);
}



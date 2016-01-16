
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

void shgeo_set(shgeo_t *geo, double lat, double lon, int alt)
{
  double t_lat;
  double t_lon;
  int lat_prec;
  int lon_prec;
  int prec;

  lat = fabs(lat);
  lon = fabs(lon);

  for (lat_prec = 1; lat_prec <= SHGEO_MAX_PRECISION; lat_prec++) {
    double d = lat / pow(10, lat_prec);
    if ((uint64_t)d == 0.0)
      break;
  }
  for (lon_prec = 1; lon_prec <= SHGEO_MAX_PRECISION; lon_prec++) {
    double d = lon / pow(10, lon_prec);
    if ((uint64_t)d == 0.0)
      break;
  }

  prec = MAX(lat_prec, lon_prec);

  memset(geo, 0, sizeof(shgeo_t));
  if (!prec) {
    geo->geo_lat = (uint64_t)lat;
    geo->geo_lon = (uint64_t)lon;
  } else {
    geo->geo_lat = (uint64_t)(lat * (double)(10 * prec));
    geo->geo_lon = (uint64_t)(lon * (double)(10 * prec));
  }
  geo->geo_prec = prec;
  geo->geo_alt = alt;
  geo->geo_stamp = shtime();

}

void shgeo_loc(shgeo_t *geo, double *lat_p, double *lon_p, int *alt_p)
{
  double lat;
  double lon;
  int prec;

  prec = MIN(geo->geo_prec, SHGEO_MAX_PRECISION);

  if (lat_p) {
    *lat_p = (double)geo->geo_lat;
    if (geo->geo_prec)
      *lat_p /= pow(10, geo->geo_prec);
  }

  if (lon_p) {
    *lon_p = (double)geo->geo_lon;
    if (geo->geo_prec)
      *lon_p /= pow(10, geo->geo_prec);
    if (*lon_p > 0)
      *lon_p = *lon_p * -1;
  }

  if (alt_p)
    *alt_p = (int)geo->geo_alt;

}

time_t shgeo_lifespan(shgeo_t *geo)
{
  return ((time_t)shtime_diff(shtime(), geo->geo_stamp));
}

shkey_t *shgeo_tag(shgeo_t *geo, int prec)
{
  double lat;
  double lon;
  char buf[32];

  shgeo_loc(geo, &lat, &lon, NULL);

  lat = floor(lat * (10 * prec)) / prec;
  lon = floor(lon * (10 * prec)) / prec;

  memset(buf, 0, sizeof(buf));
  memcpy(buf, &lat, sizeof(double));
  memcpy(buf + sizeof(double), &lon, sizeof(double));

  return (shkey_bin(buf, sizeof(double) * 2));
}

int shgeo_cmp(shgeo_t *geo, shgeo_t *cmp_geo, int prec)
{
  double lat;
  double lon;
  double c_lat;
  double c_lon;

  shgeo_loc(geo, &lat, &lon, NULL);
  lat = floor(lat * (10 * prec)) / prec;
  lon = floor(lon * (10 * prec)) / prec;

  shgeo_loc(geo, &c_lat, &c_lon, NULL);
  c_lat = floor(c_lat * (10 * prec)) / prec;
  c_lon = floor(c_lon * (10 * prec)) / prec;

  return (lat == c_lat && lon == c_lon);
}

static double _deg2rad(double deg) 
{
  return (deg * M_PI / 180.0);
}

static double _rad2deg(double rad) {
  return (rad * 180.0 / M_PI);
}

/**
 * Calculate the distance between two geodetic location in miles.
 */
double shgeo_dist(shgeo_t *f_geo, shgeo_t *t_geo)
{
  double theta, dist;
  double lat1, lat2;
  double lon1, lon2;

  shgeo_loc(f_geo, &lat1, &lon1, NULL);
  shgeo_loc(t_geo, &lat2, &lon2, NULL);

  theta = lon1 - lon2;
  dist = sin(_deg2rad(lat1)) * sin(_deg2rad(lat2)) + cos(_deg2rad(lat1)) * cos(_deg2rad(lat2)) * cos(_deg2rad(theta));
  dist = acos(dist);
  dist = _rad2deg(dist);
  dist = dist * 60 * 1.1515;

  return (dist);
}


_TEST(shgeo_dist)
{
  shgeo_t fl_geo;
  shgeo_t ms_geo;
  double lat, lon;
  double d;
  int ok;

  shgeo_set(&fl_geo, 44.66, -114, 0); 
  shgeo_set(&ms_geo, 46.87, -113.99, 0); 

  shgeo_loc(&fl_geo, &lat, &lon, NULL);
  shgeo_loc(&ms_geo, &lat, &lon, NULL);

  d = ceil(shgeo_dist(&fl_geo, &ms_geo));
  _TRUE(d == 5);



}



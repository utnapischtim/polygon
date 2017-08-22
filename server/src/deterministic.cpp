
#include "deterministic.h"
#include "CommonSetting.h"
#include "Filter.h"
#include "Point.h"

pl::PointList pl::det::deterministic(pl::CommonSettingList /*common_settings*/, pl::FilterList /*local_filters*/) {
  // there for debugging, to have a point list that is not random!

  pl::PointList point_list = {
    {1, 11},
    {5, 40},
    {10, 20},
    {21, 10},
    {20, 18},
    {20, 41},
    {33, 15},
    {39, 22},
    {48, 9},
    {55, 17},
    {60, 50},
    {68, 19},
    {75, 33}
  };

  // two opt moves
  // pl::PointList point_list = {
  //   {7, 12},
  //   {13, 20},
  //   {21, 7},
  //   {29, 13},
  //   {20, 16},
  //   {25, 22},
  //   {26, 18},
  //   {10, 8}
  // };

  return point_list;
}

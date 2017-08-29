
#include "deterministic.h"
#include "CommonSetting.h"
#include "Filter.h"
#include "Point.h"

pl::PointList pl::det::deterministic(pl::CommonSettingList /*common_settings*/, pl::FilterList /*local_filters*/) {
  // there for debugging, to have a point list that is not random!

  // pl::PointList point_list = {
  //   {1, 11},
  //   {5, 40},
  //   {10, 20},
  //   {21, 10},
  //   {20, 18},
  //   {20, 41},
  //   {33, 15},
  //   {39, 22},
  //   {48, 9},
  //   {55, 17},
  //   {60, 50},
  //   {68, 19},
  //   {75, 33}
  // };

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

  // bouncing vertices
  // pl::PointList point_list = {
  //   {1, 11},
  //   {5, 40},
  //   {10, 20},
  //   {21, 10},
  //   {20, 18},
  //   {20, 41},
  //   {33, 15},
  //   {39, 22},
  //   {48, 9},
  //   {55, 17},
  //   {60, 50},
  //   {68, 19},
  //   {75, 33},
  //   {78, 7},
  //   {10, 1}
  // };

  // convex bottom
  pl::PointList point_list = {
    {459.928, 139.161},
    {658.054, 22.297},
    {975.729, 176.025},
    {900.465, 491.612},
    {1088.5, 583.949},
    {1482.96, 597.586},
    {693.955, 737.121},
    {1122.36, 268.534},
    {419.468, 433.356},
    {514.612, 590.776}
  };

  return point_list;
}

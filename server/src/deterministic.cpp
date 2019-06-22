
#include "deterministic.h"
#include "Point.h"

pl::PointList pl::det::deterministic() {
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
  // pl::PointList point_list = {
  //   {459.928, 139.161},
  //   {658.054, 22.297},
  //   {975.729, 176.025},
  //   {900.465, 491.612},
  //   {1088.5, 583.949},
  //   {1482.96, 597.586},
  //   {693.955, 737.121},
  //   {1122.36, 268.534},
  //   {419.468, 433.356},
  //   {514.612, 590.776}
  // };

  // pl::PointList point_list = {
  //   {1038.73, 379.748},
  //   {305.544, 448.061},
  //   {587.444, 40.6529},
  //   {1306.62, 306.858},
  //   {964.525, 156.442},
  //   {102.284, 412.303},
  //   {225.033, 478.251},
  //   {372.665, 318.17},
  //   {370.44, 170.285},
  //   {1391.37, 19.1067}
  // };

  pl::PointList point_list = {
    {1356.17, 665.814},
    {1415.52, 623.157},
    {1291.81, 582.991},
    {1405.68, 104.452},
    {1275.07, 610.035},
    {1232.36, 577.015},
    {1306.94, 700.561},
    {1299, 687.407},
    {1285.15, 716.948},
    {1356.17, 665.814}
  };

  return point_list;
}

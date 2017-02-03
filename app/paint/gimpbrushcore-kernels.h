/* gimpbrushcore-kernels.h
 *
 *   This file was generated using kernelgen as found in the tools dir.
 *   (threshold = 0.25)
 */

#ifndef __GIMP_BRUSH_CORE_KERNELS_H__
#define __GIMP_BRUSH_CORE_KERNELS_H__

#define KERNEL_WIDTH     3
#define KERNEL_HEIGHT    3
#define KERNEL_SUBSAMPLE 4
#define KERNEL_SUM       256


/*  Brush pixel subsampling kernels  */
static const int subsample[5][5][9] =
{
  {
    {  64,  64,   0,  64,  64,   0,   0,   0,   0, },
    {  25, 103,   0,  25, 103,   0,   0,   0,   0, },
    {   0, 128,   0,   0, 128,   0,   0,   0,   0, },
    {   0, 103,  25,   0, 103,  25,   0,   0,   0, },
    {   0,  64,  64,   0,  64,  64,   0,   0,   0, }
  },
  {
    {  25,  25,   0, 103, 103,   0,   0,   0,   0, },
    {   6,  44,   0,  44, 162,   0,   0,   0,   0, },
    {   0,  50,   0,   0, 206,   0,   0,   0,   0, },
    {   0,  44,   6,   0, 162,  44,   0,   0,   0, },
    {   0,  25,  25,   0, 103, 103,   0,   0,   0, }
  },
  {
    {   0,   0,   0, 128, 128,   0,   0,   0,   0, },
    {   0,   0,   0,  50, 206,   0,   0,   0,   0, },
    {   0,   0,   0,   0, 256,   0,   0,   0,   0, },
    {   0,   0,   0,   0, 206,  50,   0,   0,   0, },
    {   0,   0,   0,   0, 128, 128,   0,   0,   0, }
  },
  {
    {   0,   0,   0, 103, 103,   0,  25,  25,   0, },
    {   0,   0,   0,  44, 162,   0,   6,  44,   0, },
    {   0,   0,   0,   0, 206,   0,   0,  50,   0, },
    {   0,   0,   0,   0, 162,  44,   0,  44,   6, },
    {   0,   0,   0,   0, 103, 103,   0,  25,  25, }
  },
  {
    {   0,   0,   0,  64,  64,   0,  64,  64,   0, },
    {   0,   0,   0,  25, 103,   0,  25, 103,   0, },
    {   0,   0,   0,   0, 128,   0,   0, 128,   0, },
    {   0,   0,   0,   0, 103,  25,   0, 103,  25, },
    {   0,   0,   0,   0,  64,  64,   0,  64,  64, }
  }
};

#endif /* __GIMP_BRUSH_CORE_KERNELS_H__ */

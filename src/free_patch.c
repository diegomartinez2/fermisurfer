/*
The MIT License (MIT)

Copyright (c) 2014 Mitsuaki KAWAMURA

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/**@file
@brief Refresh patch
*/
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "variable.h"
#include "basic_math.h"

/**
 @brief Free variables for patch before new patch is computed

 Free : ::nmlp, ::matp, ::clr, ::kvp, ::nmlp_rot, ::kvp_rot,
        ::kvnl, ::kvnl_rot, ::kv2d, ::clr2d
*/
void free_patch() {
  int ib, i0, i1;
  /*
   Fermi patch
  */
  for (ib = 0; ib < nb; ++ib) {
    for (i0 = 0; i0 < ntri[ib]; ++i0) {
      for (i1 = 0; i1 < 3; ++i1) {
        free(nmlp[ib][i0][i1]);
        free(matp[ib][i0][i1]);
        free(kvp[ib][i0][i1]);
      }
      free(nmlp[ib][i0]);
      free(matp[ib][i0]);
      free(kvp[ib][i0]);
    }
    free(nmlp[ib]);
    free(matp[ib]);
    free(clr[ib]);
    free(kvp[ib]);
    free(nmlp_rot[ib]);
    free(kvp_rot[ib]);
  }
  free(nmlp);
  free(matp);
  free(clr);
  free(kvp);
  free(nmlp_rot);
  free(kvp_rot);
  /*
   Nodal line
  */
  for (ib = 0; ib < nb; ++ib) {
    for (i0 = 0; i0 < nnl[ib]; ++i0) {
      for (i1 = 0; i1 < 2; ++i1) {
        free(kvnl[ib][i0][i1]);
      }/*for (i1 = 0; i1 < 2; ++i1)*/
      free(kvnl[ib][i0]);
    }/*for (i0 = 0; i0 < nnl[ib]; ++i0)*/
    free(kvnl[ib]);
    free(kvnl_rot[ib]);
    free(nmlnl[ib]);
    free(clrnl[ib]);
  }/*for (ib = 0; ib < nb; ++ib)*/
  free(kvnl);
  free(kvnl_rot);
  free(nmlnl);
  free(clrnl);
  /*
   2D Fermi line
  */
  for (ib = 0; ib < nb; ++ib) {
    free(kv2d[ib]);
    free(clr2d[ib]);
    free(nml2d[ib]);
  }/*for (ib = 0; ib < nb; ++ib)*/
  free(kv2d);
  free(clr2d);
  free(nml2d);
  /*
  equator
  */
  for (ib = 0; ib < nb; ++ib) {
    for (i0 = 0; i0 < nequator[ib]; ++i0) {
      for (i1 = 0; i1 < 2; ++i1) {
        free(kveq[ib][i0][i1]);
      }/*for (i1 = 0; i1 < 2; ++i1)*/
      free(kveq[ib][i0]);
    }/*for (i0 = 0; i0 < nequator[ib]; ++i0)*/
    free(kveq[ib]);
    free(kveq_rot[ib]);
    free(nmleq[ib]);
    free(clreq[ib]);
  }/*for (ib = 0; ib < nb; ++ib)*/
  free(kveq);
  free(kveq_rot);
  free(nmleq);
  free(clreq);

}/*void free_patch()*/
/**
 @brief Compute Max. & Min. of matrix elements.
 Compute color of each patch 

 Modify : ::clr
*/
void max_and_min() {
  int itri, ithread;
  GLfloat *max_th, *min_th;
  
  max_th = (GLfloat*)malloc(nthreads * sizeof(GLfloat));
  min_th = (GLfloat*)malloc(nthreads * sizeof(GLfloat));

  printf("\n");
  if (color_scale == 1) printf("  ##  Color Scale as Input Quantity (Real) #############\n");
  else if (color_scale == 2) printf("  ##  Color Scale as Input Quantity (Complex) #############\n");
  else if (color_scale == 3) printf("  ##  Color Scale as Fermi Velocity #############\n");
  else if (color_scale == 4) printf("  ##  Color Scale as Band Index #############\n");
  else if (color_scale == 5) printf("  ##  Gray Scale as Input Quantity (Real) #############\n");
  else if (color_scale == 6) printf("  ##  Gray Scale as Fermi Velocity #############\n");
  printf("\n");
  /*
   Search max and min.
  */
  if (color_scale == 1 || color_scale == 5) {
#pragma omp parallel default(none) \
shared(nb,ntri,matp,max_th,min_th) private(itri,ithread)
    {
      int i, ib;

      ithread = get_thread();
      max_th[ithread] = -1.0e10f;
      min_th[ithread] = 1.0e10f;

      for (ib = 0; ib < nb; ib++) {
#pragma omp for
        for (itri = 0; itri < ntri[ib]; ++itri) {
          for (i = 0; i < 3; ++i) {
            if (matp[ib][itri][i][0] > max_th[ithread]) max_th[ithread] = matp[ib][itri][i][0];
            if (matp[ib][itri][i][0] < min_th[ithread]) min_th[ithread] = matp[ib][itri][i][0];
          }
        }/*for (itri = 0; itri < ntri[ib]; ++itri)*/
      }/*for (ib = 0; ib < nb; ib++)*/
    }/*End of parallel region*/
    /**/
    patch_max = max_th[0];
    patch_min = min_th[0];
    for (ithread = 1; ithread < nthreads; ithread++) {
      if (max_th[ithread] > patch_max) patch_max = max_th[ithread];
      if (min_th[ithread] < patch_min) patch_min = min_th[ithread];
    }
  }/*if (color_scale == 0 || color_scale == 4)*/
  else   if (color_scale == 2) {
#pragma omp parallel default(none) \
shared(nb,ntri,matp,max_th) private(itri,ithread)
    {
      int i, ib;
      GLfloat abs;

      ithread = get_thread();
      max_th[ithread] = -1.0e10f;

      for (ib = 0; ib < nb; ib++) {
#pragma omp for
        for (itri = 0; itri < ntri[ib]; ++itri) {
          for (i = 0; i < 3; ++i) {
            abs = sqrtf(matp[ib][itri][i][0] * matp[ib][itri][i][0]
                      + matp[ib][itri][i][1] * matp[ib][itri][i][1]);
            if (abs > max_th[ithread]) max_th[ithread] = abs;
          }
        }/*for (itri = 0; itri < ntri[ib]; ++itri)*/
      }/*for (ib = 0; ib < nb; ib++)*/
    }/*End of parallel region*/
   /**/
    patch_max = max_th[0];
    for (ithread = 1; ithread < nthreads; ithread++) {
      if (max_th[ithread] > patch_max) patch_max = max_th[ithread];
    }
  }/*if (color_scale == 3)*/
  else if (color_scale == 3 || color_scale == 6) {
#pragma omp parallel default(none) \
shared(nb,ntri,nmlp,max_th,min_th) private(itri,ithread)
    {
      int i, j, ib;
      GLfloat norm;

      ithread = get_thread();
      max_th[ithread] = -1.0e10f;
      min_th[ithread] = 1.0e10f;

      for (ib = 0; ib < nb; ib++) {
#pragma omp for
        for (itri = 0; itri < ntri[ib]; ++itri) {
          for (i = 0; i < 3; ++i) {
            norm = 0.0f;
            for (j = 0; j < 3; ++j) norm += nmlp[ib][itri][i][j]*nmlp[ib][itri][i][j];
            norm = sqrtf(norm);

            if (norm > max_th[ithread]) max_th[ithread] = norm;
            if (norm < min_th[ithread]) min_th[ithread] = norm;
          }
        }/*for (itri = 0; itri < ntri[ib]; ++itri)*/
      }/*for (ib = 0; ib < nb; ib++)*/
    }/*End of parallel region*/
    /**/
    patch_max = max_th[0];
    patch_min = min_th[0];
    for (ithread = 1; ithread < nthreads; ithread++) {
      if (max_th[ithread] > patch_max) patch_max = max_th[ithread];
      if (min_th[ithread] < patch_min) patch_min = min_th[ithread];
    }    
  }/*if (color_scale == 5 || color_scale == 6)*/

  free(max_th);
  free(min_th);

   if (color_scale == 1 || color_scale == 3
    || color_scale == 5 || color_scale == 6) {
     printf("    Min. value : %f\n\n", patch_min);
     printf("    Max. value : %f\n", patch_max);
  }
  else if (color_scale == 2) {
    printf("    Max. absolute value : %f\n", patch_max);
  }
}/* max_and_min */
 /**
 @brief Compute Max. & Min. of matrix elements.
 Compute color of each patch

 Modify : ::clr
 */
void paint()
{
  int itri;

  if (color_scale == 1) {
#pragma omp parallel default(none) \
shared(nb,ntri,matp,clr,cyan,blue,green,yellow,red,patch_max,patch_min) \
private(itri)
    {
      int i, j, ib;
      GLfloat mat2;

      for (ib = 0; ib < nb; ib++) {
#pragma omp for nowait
        for (itri = 0; itri < ntri[ib]; ++itri) {
          for (i = 0; i < 3; ++i) {
            /**/
            mat2 = (matp[ib][itri][i][0] - patch_min) / (patch_max - patch_min);
            mat2 = mat2 * 4.0f;
            /**/
            if (mat2 <= 1.0) {
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = cyan[j] * mat2 + blue[j] * (1.0f - mat2);
            }
            else if (mat2 <= 2.0) {
              mat2 = mat2 - 1.0f;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = green[j] * mat2 + cyan[j] * (1.0f - mat2);
            }
            else if (mat2 <= 3.0) {
              mat2 = mat2 - 2.0f;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = yellow[j] * mat2 + green[j] * (1.0f - mat2);
            }
            else {
              mat2 = mat2 - 3.0f;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = red[j] * mat2 + yellow[j] * (1.0f - mat2);
            }
          }/*for (i = 0; i < 3; ++i)*/
        }/*for (itri = 0; itri < ntri[ib]; ++itri)*/
      }/*for (ib = 0; ib < nb; ib++)*/
    }/*End of parallel region*/
  }/*if (color_scale == 1 || color_scale == 2)*/
  else if (color_scale == 2) {
#pragma omp parallel default(none) \
shared(nb,ntri,matp,clr,cyan,blue,green,yellow,red,magenta,bgray,wgray,blackback,patch_max) \
private(itri)
    {
      int i, j, ib;
      GLfloat theta, abs, theta2;

      for (ib = 0; ib < nb; ib++) {
        for (itri = 0; itri < ntri[ib]; ++itri) {
          for (i = 0; i < 3; ++i) {
            /**/
            abs = sqrtf(matp[ib][itri][i][0] * matp[ib][itri][i][0]
                      + matp[ib][itri][i][1] * matp[ib][itri][i][1]);
            if (abs / patch_max < 0.00001) theta = 0.0f;
            else if (matp[ib][itri][i][1] > 0.0) theta = acosf(matp[ib][itri][i][0] / abs);
            else theta = -acosf(matp[ib][itri][i][0] / abs);
            abs /= patch_max;
            theta = 3.0f * theta / acosf(-1.0f);
            /**/
            if (-3.0f <= theta && theta < -2.0f) {
              theta2 = theta + 3.0f;
              for (j = 0; j < 4; ++j)
                clr[ib][j + 4 * i + 12 * itri] = blue[j] * theta2 + cyan[j] * (1.0f - theta2);
            }
            else if (-2.0f <= theta && theta < -1.0f) {
              theta2 = theta + 2.0f;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = magenta[j] * theta2 + blue[j] * (1.0f - theta2);
            }
            else if (-1.0f <= theta && theta < 0.0f) {
              theta2 = theta;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = red[j] * theta2 + magenta[j] * (1.0f - theta2);
            }
            else if (0.0f <= theta && theta < 1.0f) {
              theta2 = theta - 1.0f;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = yellow[j] * theta2 + red[j] * (1.0f - theta2);
            }
            else if (1.0f <= theta && theta < 2.0f) {
              theta2 = theta - 2.0f;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = green[j] * theta2 + yellow[j] * (1.0f - theta2);
            }
            else {
              theta2 = theta - 3.0f;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = cyan[j] * theta2 + green[j] * (1.0f - theta2);
            }
            if (blackback == 1) {
              for (j = 0; j < 4; ++j)
                clr[ib][j + 4 * i + 12 * itri] = clr[ib][j + 4 * i + 12 * itri] * abs + wgray[j] * (1.0f - abs);
            }
            else {
              for (j = 0; j < 4; ++j)
                clr[ib][j + 4 * i + 12 * itri] = clr[ib][j + 4 * i + 12 * itri] * abs + bgray[j] * (1.0f - abs);
            }
          }/*for (i = 0; i < 3; ++i)*/
        }/*for (itri = 0; itri < ntri[ib]; ++itri)*/
      }/*for (ib = 0; ib < nb; ib++)*/
    }/*End of parallel region*/
  }/*if (color_scale == 4)*/
  else if (color_scale == 3) {
#pragma omp parallel default(none) \
shared(nb,ntri,nmlp,clr,cyan,blue,green,yellow,red,patch_max,patch_min) \
private(itri)
    {
      int i, j, ib;
      GLfloat mat2;

      for (ib = 0; ib < nb; ib++) {
  #pragma omp for nowait
        for (itri = 0; itri < ntri[ib]; ++itri) {
          for (i = 0; i < 3; ++i) {
            /**/
            mat2 = 0.0f;
            for (j = 0; j < 3; ++j) mat2 += nmlp[ib][itri][i][j] * nmlp[ib][itri][i][j];
            mat2 = sqrtf(mat2);
            mat2 = (mat2 - patch_min) / (patch_max - patch_min);
            mat2 = mat2 * 4.0f;
            /**/
            if (mat2 <= 1.0) {
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = cyan[j] * mat2 + blue[j] * (1.0f - mat2);
            }
            else if (mat2 <= 2.0) {
              mat2 = mat2 - 1.0f;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = green[j] * mat2 + cyan[j] * (1.0f - mat2);
            }
            else if (mat2 <= 3.0) {
              mat2 = mat2 - 2.0f;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = yellow[j] * mat2 + green[j] * (1.0f - mat2);
            }
            else {
              mat2 = mat2 - 3.0f;
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = red[j] * mat2 + yellow[j] * (1.0f - mat2);
            }
          }/*for (i = 0; i < 3; ++i)*/
        }/*for (itri = 0; itri < ntri[ib]; ++itri)*/
      }/*for (ib = 0; ib < nb; ib++)*/
    }/*End of parallel region*/
  }/*if (color_scale == 3)*/
  else if (color_scale == 4) {
#pragma omp parallel default(none) \
shared(nb,ntri,matp,clr,cyan,blue,green,yellow,red) \
private(itri)
    {
      int i, j, ib;
      GLfloat mat2;

      for (ib = 0; ib < nb; ib++) {
        /**/
        if (nb == 1) mat2 = 0.5f;
        else mat2 = 1.0f / (GLfloat)(nb - 1) * (GLfloat)ib;
        mat2 *= 4.0f;
        /**/
        if (mat2 <= 1.0) {
#pragma omp for nowait
          for (itri = 0; itri < ntri[ib]; ++itri) {
            for (i = 0; i < 3; ++i) {
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = cyan[j] * mat2 + blue[j] * (1.0f - mat2);
            }
          }
        }
        else if (mat2 <= 2.0) {
          mat2 = mat2 - 1.0f;
#pragma omp for nowait
          for (itri = 0; itri < ntri[ib]; ++itri) {
            for (i = 0; i < 3; ++i) {
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = green[j] * mat2 + cyan[j] * (1.0f - mat2);
            }
          }
        }
        else if (mat2 <= 3.0) {
          mat2 = mat2 - 2.0f;
#pragma omp for nowait
          for (itri = 0; itri < ntri[ib]; ++itri) {
            for (i = 0; i < 3; ++i) {
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = yellow[j] * mat2 + green[j] * (1.0f - mat2);
            }
          }
        }
        else {
          mat2 = mat2 - 3.0f;
#pragma omp for nowait
          for (itri = 0; itri < ntri[ib]; ++itri) {
            for (i = 0; i < 3; ++i) {
              for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = red[j] * mat2 + yellow[j] * (1.0f - mat2);
            }
          }
        }
      }/*for (ib = 0; ib < nb; ib++*/
    }/*End of parallel region*/
  }/*if (color_scale == 4)*/
  else if (color_scale == 5) {
#pragma omp parallel default(none) \
shared(nb,ntri,matp,clr,bgray,wgray,patch_max,patch_min) \
private(itri)
    {
      int i, j, ib;
      GLfloat mat2;

      for (ib = 0; ib < nb; ib++) {
#pragma omp for nowait
        for (itri = 0; itri < ntri[ib]; ++itri) {
          for (i = 0; i < 3; ++i) {
            /**/
            mat2 = (matp[ib][itri][i][0] - patch_min) / (patch_max - patch_min);
            /**/
            for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = wgray[j] * mat2 + bgray[j] * (1.0f - mat2);
          }/*for (i = 0; i < 3; ++i)*/
        }/*for (itri = 0; itri < ntri[ib]; ++itri)*/
      }/*for (ib = 0; ib < nb; ib++)*/
    }/*End of parallel region*/
  }/*if (color_scale == 7 || color_scale == 8)*/
  else if (color_scale == 6) {
#pragma omp parallel default(none) \
shared(nb,ntri,nmlp,clr,bgray,wgray,patch_max,patch_min) \
private(itri)
    {
      int i, j, ib;
      GLfloat mat2;

      for (ib = 0; ib < nb; ib++) {
#pragma omp for nowait
        for (itri = 0; itri < ntri[ib]; ++itri) {
          for (i = 0; i < 3; ++i) {
            /**/
           mat2 = 0.0f;
            for (j = 0; j < 3; ++j) mat2 += nmlp[ib][itri][i][j] * nmlp[ib][itri][i][j];
            mat2 = sqrtf(mat2);
            mat2 = (mat2 - patch_min) / (patch_max - patch_min);
            /**/
            for (j = 0; j < 4; ++j) clr[ib][j + 4 * i + 12 * itri] = wgray[j] * mat2 + bgray[j] * (1.0f - mat2);
          }/*for (i = 0; i < 3; ++i)*/
        }/*for (itri = 0; itri < ntri[ib]; ++itri)*/
      }/*for (ib = 0; ib < nb; ib++)*/
    }/*End of parallel region*/
  }/*if (color_scale == 7 || color_scale == 8)*/

}/* paint */

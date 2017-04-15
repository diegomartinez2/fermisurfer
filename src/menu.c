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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "variable.h"
#include "free_patch.h"
#include "fermi_patch.h"
#include "calc_nodeline.h"
#include "kumo.h"

#if defined(MAC)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/**
 * Main menu
 */
void main_menu(int value /**< [in] Selected menu*/) {
  int i0, i1, ib, ibzl, i;
  /**/
  if (value == 9) {
    /*
    Exit
    */
    printf("\nExit. \n\n");
    free_patch();
    for (ib = 0; ib < nb; ib++) {
      for (i0 = 0; i0 < ng[0]; i0++) {
        for (i1 = 0; i1 < ng[1]; i1++) {
          free(eig[ib][i0][i1]);
          free(mat[ib][i0][i1]);
        }
        free(eig[ib][i0]);
        free(mat[ib][i0]);
      }
      free(eig[ib]);
      free(mat[ib]);
    }
    free(eig);
    free(mat);
    free(ntri);
    free(draw_band);
    for (ibzl = 0; ibzl < nbzl; ++ibzl) {
      for (i = 0; i < 2; ++i) {
        free(bzl[ibzl][i]);
      }
      free(bzl[ibzl]);
    }
    free(bzl);
    free(nnl);
    exit(0);
  }
}
/*
 * Shift Fermi energy
 */
void menu_shiftEF(int value /**< [in] Selected menu*/)
{
  int ib, i0, i1, i2, ierr;
  GLfloat emin, emax;

  if (value == 1) {
    emin = 100000.0;
    emax = -100000.0;
    for (ib = 0; ib < nb; ++ib) {
      for (i0 = 0; i0 < ng[0]; ++i0) {
        for (i1 = 0; i1 < ng[1]; ++i1) {
          for (i2 = 0; i2 < ng[2]; ++i2) {
            if (emin > eig[ib][i0][i1][i2]) emin = eig[ib][i0][i1][i2];
            if (emax < eig[ib][i0][i1][i2]) emax = eig[ib][i0][i1][i2];
          }
        }
      }
    }
    printf("    Min  Max  E_F \n");
    printf("    %f %f %f \n", emin, emax, EF);
    printf("    New Fermi energy : ");
    //
    ierr = scanf("%f", &EF);
    if (ierr != 1) printf("error ! reading ef");
    /**/
    free_patch();
    query = 1;
    fermi_patch();
    query = 0;
    fermi_patch();
    calc_nodeline();
    /**/
    max_and_min();
    /**/
    glutPostRedisplay();
  }
}
/*
 * Shift Fermi energy
 */
void menu_interpol(int value /**< [in] Selected menu*/)
{
  int ib, i0, i1, i2, ierr;
  GLfloat emin, emax;

  if (value == 1) {
    printf("    Old interpolation ratio : %d\n", interpol);
    printf("    New interpolation ratio : ");
    //
    ierr = scanf("%d", &interpol);
    if (ierr != 1) printf("error ! reading interpol");
    /**/
    interpol_energy();
    free_patch();
    query = 1;
    fermi_patch();
    query = 0;
    fermi_patch();
    calc_nodeline();
    /**/
    max_and_min();
    /**/
    glutPostRedisplay();
  }
}
/*
 * Setting of view
 */
void menu_view(int value /**< [in] Selected menu*/)
{
  int ierr;

  if (value == 1) {

    printf("    Current Scale : %f\n", scl);
    printf("        New Scale : ");
    ierr = scanf("%f", &scl);

  }
  else  if (value == 2) {

    printf("    Current Position(x y) : %f %f\n", trans[0], trans[1]);
    printf("        New Position(x y) : ");
    ierr = scanf("%f %f", &trans[0], &trans[1]);

  }
  else  if (value == 3) {

    /**/
    thetay = asin(rot[0][2]);
    if (cosf(thetay) != 0.0) {
      if (-rot[1][2] / cosf(thetay) >= 0.0) thetax = acosf(rot[2][2] / cosf(thetay));
      else thetax = 6.283185307 - acosf(rot[2][2] / cosf(thetay));
      if (-rot[0][1] / cosf(thetay) >= 0.0) thetaz = acosf(rot[0][0] / cosf(thetay));
      else thetaz = 6.283185307 - acosf(rot[0][0] / cosf(thetay));
    }
    else {
      thetax = 0.0;
      if (rot[1][0] >= 0.0) thetaz = acosf(rot[1][1]);
      else thetaz = 6.283185307 - acosf(rot[1][1]);
    }
    thetax = 180.0 / 3.14159265 * thetax;
    thetay = 180.0 / 3.14159265 * thetay;
    thetaz = 180.0 / 3.14159265 * thetaz;
    printf("    Current Rotation (theta_x theta_y teta_z) in degree : %f %f %f\n", thetax, thetay, thetaz);
    printf("        New Rotation (theta_x theta_y teta_z) in degree : ");
    ierr = scanf("%f %f %f", &thetax, &thetay, &thetaz);
    thetax = 3.14159265 / 180.0 * thetax;
    thetay = 3.14159265 / 180.0 * thetay;
    thetaz = 3.14159265 / 180.0 * thetaz;

    rot[0][0] = cosf(thetay)* cosf(thetaz);
    rot[0][1] = -cosf(thetay)* sin(thetaz);
    rot[0][2] = sinf(thetay);
    rot[1][0] = cosf(thetaz)* sinf(thetax)* sinf(thetay) + cosf(thetax)* sinf(thetaz);
    rot[1][1] = cosf(thetax) * cosf(thetaz) - sinf(thetax)* sinf(thetay)* sinf(thetaz);
    rot[1][2] = -cosf(thetay)* sinf(thetax);
    rot[2][0] = -cosf(thetax)* cosf(thetaz)* sinf(thetay) + sinf(thetax)* sinf(thetaz);
    rot[2][1] = cosf(thetaz)* sinf(thetax) + cosf(thetax)* sinf(thetay)* sinf(thetaz);
    rot[2][2] = cos(thetax)* cosf(thetay);

  }

  glutPostRedisplay();

}
/**
 * Change mouse function
 */
void menu_mouse(int value /**< [in] Selected menu*/) {
  /**/
  if (value == 1 && lmouse != 1) {
    lmouse = 1;
    glutPostRedisplay();
  }
  if (value == 2 && lmouse != 2) {
    lmouse = 2;
    glutPostRedisplay();
  }
  if (value == 3 && lmouse != 3) {
    lmouse = 3;
    glutPostRedisplay();
  }
  /**/
} /* menu_band */
/**
 * On / Off band
 */
void menu_band(int value /**< [in] Selected menu*/) {
  /**/
  if (draw_band[value] == 0) {
    draw_band[value] = 1;
  }
  else {
    draw_band[value] = 0;
  }
  glutPostRedisplay();
  /**/
} /* menu_band */
/**
 * Change background color
 */
void menu_bgcolor(int value /**<[in] Selected menu*/) {
  if (value == 1 && blackback != 1) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    blackback = 1;
    glutPostRedisplay();
  }
  else if (value == 2 && blackback != 0) {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    blackback = 0;
    glutPostRedisplay();
  }
  /**/
}/* bgcolor change*/
/**
 * Change color scale mode
 */
void menu_colorscale(int value /**<[in] Selected menu*/) {
  /**/
  if (value == 1 && fcscl != 1) {
    fcscl = 1;
    max_and_min();
    glutPostRedisplay();
  }
  else if (value == 2 && fcscl != 2) {
    fcscl = 2;
    max_and_min();
    glutPostRedisplay();
  }
  else if (value == 3 && fcscl != 3) {
    fcscl = 3;
    max_and_min();
    glutPostRedisplay();
  }
  else if (value == 4 && fcscl != 4) {
    fcscl = 4;
    max_and_min();
    glutPostRedisplay();
  }
  /**/
} /* menu_colorscale */
/**
 * Change Brillouin zone
 */
void menu_bzmode(int value /**<[in] Selected menu*/) {
  if (value == 1 && fbz != 1) {
    fbz = 1;
    /**/
    free_patch();
    query = 1;
    fermi_patch();
    query = 0;
    fermi_patch();
    calc_nodeline();
    /**/
    max_and_min();
    /**/
    glutPostRedisplay();
  }
  else if (value == 2 && fbz != -1) {
    fbz = -1;
    /**/
    free_patch();
    query = 1;
    fermi_patch();
    query = 0;
    fermi_patch();
    calc_nodeline();
    /**/
    max_and_min();
    /**/
    glutPostRedisplay();
  }
} /* menu_bzmode */
/**
 * On/Off Node line
 */
void menu_nodeline(int value /**<[in] Selected menu*/) {
  if (value == 1 && nodeline != 1) {
    nodeline = 1;
    glutPostRedisplay();
  }
  else if (value == 2 && nodeline != 0) {
    nodeline = 0;
    glutPostRedisplay();
  }
  /**/
} /* menu_nodeline */
/**
 * Tern stereogram
 */
void menu_stereo(int value /**<[in] Selected menu*/) {
  if (value == 1 && lstereo != 1) {
    lstereo = 1;
    glutPostRedisplay();
  }
  if (value == 2 && lstereo != 2) {
    lstereo = 2;
    glutPostRedisplay();
  }
  if (value == 3 && lstereo != 3) {
    lstereo = 3;
    glutPostRedisplay();
  }
} /* menu_stereo */
/**
 * On/Off Colorbar
 */
void menu_colorbar(int value /**<[in] Selected menu*/) {
  if (value == 1 && lcolorbar != 1) {
    lcolorbar = 1;
    glutPostRedisplay();
  }
  if (value == 2 && lcolorbar != 0) {
    lcolorbar = 0;
    glutPostRedisplay();
  }
} /* menu_colorbar */
/**
 * Change tetrahedron
 */
void menu_tetra(int value) /**<[in] Selected menu*/ {
  /**/
  if (value != itet) {
    printf("    Tetra patern %d \n", value + 1);
    itet = value;
    init_corner();
    free_patch();
    query = 1;
    fermi_patch();
    query = 0;
    fermi_patch();
    calc_nodeline();
    max_and_min();
    glutPostRedisplay();
  }
} /* menu_tetra */
/**
 * Munu
 */
void FS_ModifyMenu(int status)
{
  int ib;
  char menu_str[20] = { 0 };

  if (status == GLUT_MENU_IN_USE) {
    glutPostRedisplay();
  }
  else {
    /**
     * Operation with mouse drag
     */
    glutSetMenu(imenu_mouse);
    for (ib = 0; ib < 3; ib++) glutRemoveMenuItem(1);
    if (lmouse == 1) glutAddMenuEntry("[x] Rotate", 1);
    else glutAddMenuEntry("[ ] Rotate", 1);
    if (lmouse == 2) glutAddMenuEntry("[x] Scale", 2);
    else glutAddMenuEntry("[ ] Scale", 2);
    if (lmouse == 3) glutAddMenuEntry("[x] Translate", 3);
    else glutAddMenuEntry("[ ] Translate", 3);
    /**
     * Band menu
     */
    glutSetMenu(imenu_band);
    for (ib = 0; ib < nb; ib++) glutRemoveMenuItem(1);
    for (ib = 0; ib < nb; ib++) {
      if (draw_band[ib] == 1) sprintf(menu_str, "[x] band # %d", ib + 1);
      else sprintf(menu_str, "[ ] band # %d", ib + 1);
      glutAddMenuEntry(menu_str, ib);
    }
    /**
     * Operation with mouse drag
     */
    glutSetMenu(imenu_interpol);
    glutRemoveMenuItem(1);
    sprintf(menu_str, "Ratio : %d", interpol);
    glutAddMenuEntry(menu_str, 1);
    /**
     * Background color
     */
    glutSetMenu(imenu_bgcolor);
    for (ib = 0; ib < 2; ib++) glutRemoveMenuItem(1);
    if (blackback == 1) glutAddMenuEntry("[x] Black", 1);
    else glutAddMenuEntry("[ ] Black", 1);
    if (blackback == 0) glutAddMenuEntry("[x] White", 2);
    else glutAddMenuEntry("[ ] White", 2);
    /**
     * Color scale mode
     */
    glutSetMenu(imenu_colorscale);
    for (ib = 0; ib < 4; ib++) glutRemoveMenuItem(1);
    if (fcscl == 1) glutAddMenuEntry("[x] Auto", 1);
    else glutAddMenuEntry("[ ] Auto", 1);
    if (fcscl == 2) glutAddMenuEntry("[x] Manual", 2);
    else glutAddMenuEntry("[ ] Manual", 2);
    if (fcscl == 3) glutAddMenuEntry("[x] Unicolor", 3);
    else glutAddMenuEntry("[ ] Unicolor", 3);
    if (fcscl == 4) glutAddMenuEntry("[x] Periodic", 4);
    else glutAddMenuEntry("[ ] Periodic", 4);
    /**
     * Brillouin zone
     */
    glutSetMenu(imenu_bzmode);
    for (ib = 0; ib < 2; ib++) glutRemoveMenuItem(1);
    if (fbz == 1) glutAddMenuEntry("[x] First Brillouin zone", 1);
    else glutAddMenuEntry("[ ] First Brillouin zone", 1);
    if (fbz == -1) glutAddMenuEntry("[x] Primitive Brillouin zone", 2);
    else glutAddMenuEntry("[ ] Primitive Brillouin zone", 2);
    /**
     * Nodeline on/off
     */
    glutSetMenu(imenu_nodeline);
    for (ib = 0; ib < 2; ib++) glutRemoveMenuItem(1);
    if (nodeline == 1) glutAddMenuEntry("[x] On", 1);
    else glutAddMenuEntry("[ ] On", 1);
    if (nodeline == 0) glutAddMenuEntry("[x] Off", 2);
    else glutAddMenuEntry("[ ] Off", 2);
    /**
     * Colorbar on/off
     */
    glutSetMenu(imenu_colorbar);
    for (ib = 0; ib < 2; ib++) glutRemoveMenuItem(1);
    if (lcolorbar == 1) glutAddMenuEntry("[x] On", 1);
    else glutAddMenuEntry("[ ] On", 1);
    if (lcolorbar == 0) glutAddMenuEntry("[x] Off", 2);
    else glutAddMenuEntry("[ ] Off", 2);
    /**
     * Stereogram
     */
    glutSetMenu(imenu_stereo);
    for (ib = 0; ib < 3; ib++) glutRemoveMenuItem(1);
    if (lstereo == 1) glutAddMenuEntry("[x] None", 1);
    else glutAddMenuEntry("[ ] None", 1);
    if (lstereo == 2) glutAddMenuEntry("[x] Parallel", 2);
    else glutAddMenuEntry("[ ] Parallel", 2);
    if (lstereo == 3) glutAddMenuEntry("[x] Cross", 3);
    else glutAddMenuEntry("[ ] Cross", 3);
    /**
     * Tetrahedron
     */
    glutSetMenu(imenu_tetra);
    for (ib = 0; ib < 16; ib++) glutRemoveMenuItem(1);
    for (ib = 0; ib < 16; ib++) {
      if (itet == ib) sprintf(menu_str, "[x] tetra # %d", ib + 1);
      else sprintf(menu_str, "[ ] tetra # %d", ib + 1);
      glutAddMenuEntry(menu_str, ib);
    }
    glutPostRedisplay();
  }
}
/**
 * Munu
 */
void FS_CreateMenu()
{
  int ib, imenu_shiftEF, imenu_view;
  char menu_str[20] = { 0 };
  /**
   * Mouse drag works as ...
   */
  imenu_mouse = glutCreateMenu(menu_mouse);
  if (lmouse == 1) glutAddMenuEntry("[x] Rotate", 1);
  else glutAddMenuEntry("[ ] Rotate", 1);
  if (lmouse == 2) glutAddMenuEntry("[x] Scale", 2);
  else glutAddMenuEntry("[ ] Scale", 2);
  if (lmouse == 3) glutAddMenuEntry("[x] Translate", 3);
  else glutAddMenuEntry("[ ] Translate", 3);
  /**
   * On/Off  each band 
   */
  imenu_band = glutCreateMenu(menu_band);
  for (ib = 0; ib < nb; ib++) {
    if (draw_band[ib] == 1) sprintf(menu_str, "[x] band # %d", ib + 1);
    else sprintf(menu_str, "[ ] band # %d", ib + 1);
    glutAddMenuEntry(menu_str, ib);
  }
  /**
   * Shift Fermi energy
   */
  imenu_shiftEF = glutCreateMenu(menu_shiftEF);
  glutAddMenuEntry("Shift Fermi energy", 1);
  /**
   * Modify interpolation ratio
   */
  sprintf(menu_str, "Ratio : %d", interpol);
  imenu_interpol = glutCreateMenu(menu_interpol);
  glutAddMenuEntry(menu_str, 1);
  /**
   * Set view
   */
  imenu_view = glutCreateMenu(menu_view);
  glutAddMenuEntry("Scale", 1);
  glutAddMenuEntry("Position", 2);
  glutAddMenuEntry("Rotation", 3);
  /**
   * Background color
   */
  imenu_bgcolor = glutCreateMenu(menu_bgcolor);
  if (blackback == 1) glutAddMenuEntry("[x] Black", 1);
  else glutAddMenuEntry("[ ] Black", 1);
  if (blackback == 0) glutAddMenuEntry("[x] White", 2);
  else glutAddMenuEntry("[ ] White", 2);
  /**
   * Color scale mode
   */
  imenu_colorscale = glutCreateMenu(menu_colorscale);
  if (fcscl == 1) glutAddMenuEntry("[x] Auto", 1);
  else glutAddMenuEntry("[ ] Auto", 1);
  if (fcscl == 2) glutAddMenuEntry("[x] Manual", 2);
  else glutAddMenuEntry("[ ] Manual", 2);
  if (fcscl == 3) glutAddMenuEntry("[x] Unicolor", 3);
  else glutAddMenuEntry("[ ] Unicolor", 3);
  if (fcscl == 4) glutAddMenuEntry("[x] Periodic", 4);
  else glutAddMenuEntry("[ ] Periodic", 4);
  /**
   * Brillouin zone
   */
  imenu_bzmode = glutCreateMenu(menu_bzmode);
  if (fbz == 1) glutAddMenuEntry("[x] First Brillouin zone", 1);
  else glutAddMenuEntry("[ ] First Brillouin zone", 1);
  if (fbz == -1) glutAddMenuEntry("[x] Primitive Brillouin zone", 2);
  else glutAddMenuEntry("[ ] Primitive Brillouin zone", 2);
  /**
   * Nodeline on/off
   */
  imenu_nodeline = glutCreateMenu(menu_nodeline);
  if (nodeline == 1) glutAddMenuEntry("[x] On", 1);
  else glutAddMenuEntry("[ ] On", 1);
  if (nodeline == 0) glutAddMenuEntry("[x] Off", 2);
  else glutAddMenuEntry("[ ] Off", 2);
  /**
   * Colorbar on/off
   */
  imenu_colorbar = glutCreateMenu(menu_colorbar);
  if (lcolorbar == 1) glutAddMenuEntry("[x] On", 1);
  else glutAddMenuEntry("[ ] On", 1);
  if (lcolorbar == 0) glutAddMenuEntry("[x] Off", 2);
  else glutAddMenuEntry("[ ] Off", 2);
  /**
   * Stereogram
   */
  imenu_stereo = glutCreateMenu(menu_stereo);
  if (lstereo == 1) glutAddMenuEntry("[x] None", 1);
  else glutAddMenuEntry("[ ] None", 1);
  if (lstereo == 2) glutAddMenuEntry("[x] Parallel", 2);
  else glutAddMenuEntry("[ ] Parallel", 2);
  if (lstereo == 3) glutAddMenuEntry("[x] Cross", 3);
  else glutAddMenuEntry("[ ] Cross", 3);
  /**
   * Tetrahedron 
   */
  imenu_tetra = glutCreateMenu(menu_tetra);
  for (ib = 0; ib < 16; ib++) {
    if (itet == ib) sprintf(menu_str, "[x] tetra # %d", ib + 1);
    else sprintf(menu_str, "[ ] tetra # %d", ib + 1);
    glutAddMenuEntry(menu_str, ib);
  }
  /**
   * Main menu
   */
  imenu = glutCreateMenu(main_menu);
  glutAddSubMenu("Band", imenu_band);
  glutAddSubMenu("Mouse Drag", imenu_mouse);
  glutAddSubMenu("Shift Fermi energy", imenu_shiftEF);
  glutAddSubMenu("Interpolation", imenu_interpol);
  glutAddSubMenu("Set view", imenu_view);
  glutAddSubMenu("Background color", imenu_bgcolor);
  glutAddSubMenu("Color scale mode", imenu_colorscale);
  glutAddSubMenu("Brillouin zone", imenu_bzmode);
  glutAddSubMenu("Node lines", imenu_nodeline);
  glutAddSubMenu("Color bar On/Off", imenu_colorbar);
  glutAddSubMenu("Stereogram", imenu_stereo);
  glutAddSubMenu("Tetrahedron", imenu_tetra);
  glutAddMenuEntry("Exit", 9);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

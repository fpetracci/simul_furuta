#ifndef SIMUL_FURUTA_GUI_H
#define SIMUL_FURUTA_GUI_H

#include <stdio.h>
#include <math.h>
#include <allegro.h>

#include "ptask.h"
#include "condiviso.h"


// types per gui
typedef struct {
    int x;
    int y;
} Point; 

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
} TwoPoints;

typedef struct {
    float x;
    float y;
    float z;
} Vect;

typedef struct {
    float sin;
    float cos;
} AngleSinCos;



//inizializzazione di gui
int gui_init();

// task gui
void* gui(void* arg);

// rad
float rad(float ang);

// ThickLine: disegna una linea spessa
void thick_line(BITMAP *bmp, float x, float y, float x_, float y_, float thickness, int color);

// Circle_rifalpha: disegno un arco di circonf in prospettiva per visualizzare alpha
void circlerif_alpha(BITMAP *bmp, Point C, int r, AngleSinCos Lon, AngleSinCos Lat, float alpha, float refalpha,
                     int color);

// circlerif_theta: disegna un arco di circonf in prospettiva per visualizzare theta
void circlerif_theta(BITMAP *bmp, Point C, int r, int l1, AngleSinCos Alpha, AngleSinCos Lon, AngleSinCos Lat,
                     float theta, float reftheta, int color);
// circlerif_parup: disegna un cerchio parametrico che inizia dalla verticale
void circlerif_parup(BITMAP *bmp, Point C, int r, float ang, float refang, int color);

// proiez_ass proietto coordinate spaziali in coordinate di disegno secondo una proiezione assonometrica
Point proiez_asson(Vect P, AngleSinCos Lon, AngleSinCos Lat);

// disegna griglia in assonometria
void grid(Vect P1, Vect P2, int q, int posx, int posy, AngleSinCos Lon, AngleSinCos Lat, int col);

// disegni delle viste
void vista_asson(float alpha, float refalpha, float theta, float reftheta, float lon, float lat);
void vista_lato(float theta, float reftheta);
void vista_alto(float alpha, float refalpha);

// disegna le scritte
void scritte_draw(state_pc_t state,ref_t ref, par_ctrl_t par_control);

// disegna l'interfaccia
void gui_draw(state_pc_t state,ref_t ref, view_t view, par_ctrl_t par_control);


#endif //SIMUL_FURUTA_GUI_H

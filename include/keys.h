#ifndef KEYS_H
	#define KEYS_H

#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include "ptask.h"
#include "condiviso.h"

// task keys
void* keys(void* arg);

// key_action, secondo il tasto premuto compie un'azione
void key_action();

// key_par_control, parte dei tasti che vanno a modificare i parametri del controllore
void key_par_control();

// get_keycodes
void get_keycodes(char *scan, char *ascii);

#endif //SIMUL_FURUTA_KEYS_H

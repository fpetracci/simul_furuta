#include "gui.h"


//----------- variabili extern
extern unsigned int         period_control;
extern ref_t 				ref_pc;
extern pthread_mutex_t 		mux_ref_pc;
extern state_pc_t 			state_pc;
extern pthread_mutex_t 		mux_state_pc;
extern view_t 				view;
extern pthread_mutex_t 		mux_view;
extern par_ctrl_t 			par_control_pc;
extern par_ctrl_t			par_control_reset;
extern pthread_mutex_t 		mux_parcontr_pc;
extern float                pole_ref;
extern dn_t 				dn;
extern pthread_mutex_t		mux_dn;
extern par_dn_t 			par_dn;
extern int 					dl_miss_gui;
extern int 					dl_miss_keys;
extern int 					dl_miss_compc;
extern int 					dl_miss_control;
extern int 					dl_miss_state_update;
extern int 					dl_miss_comboard;
extern int 					end;


#ifdef EXTIME
extern int ex_time[6];
extern int wc_extime[6];
extern struct timespec monotime_i[6], monotime_f[6];
extern int ex_cnt[6];
extern long ex_sum[6];
#endif

//----------- variabili usate solo in gui.c
static BITMAP *scrbuf;
static Point asson0, lato0, alto0; //centro rettangoli
static TwoPoints resetasson, resetlato, resetalto; //angoli dell'area da resettare prima di disegnare di nuovo
static struct {
	int mdl;	//colore modello
	int mdl2;	//colore modello2
	int vert;	//colore asta verticale
	int rif;	//colore riferimento
	int scr;	//colore scritte
	int bck;	//colore background
	int rett;	//colore rettangoli
} col; //struttura con i colori

static struct{
	int x;
	int y[(int)(HSCREEN/DIST - 2)];
} scritte; // array per scrivere "parametricamente" le scritte

// stringhe di comunicazione che vengono aggiornate
	char refalphastr[30], alphastr[30], thetastr[30],voltagestr[15]; 
	char parcontrstralpha[42], parcontrstrtheta[42], parcontrstralphadown[59], pardnstr[65], pole_ref_str[49], control_period_str[50];
	char swingup_str[31];
	char realtime_str[60];
	

//----------- gui
void* gui(void* arg){
    cpu_set(0);
	int id;							// task index
	id = get_task_index(arg);		// retrieve the task index
	set_activation(id);
	
	// init
	//gui_init();

	// variabili
	static state_pc_t state_loc = {0};
	static ref_t ref_loc = {0};
	static view_t view_loc = {0};
	static par_ctrl_t par_control_loc = {0};
	
	while(!end){
#ifdef EXTIME
        start_extime(4, 1000/FPS);
#endif
		pthread_mutex_lock(&mux_state_pc);
			state_loc = state_pc;
		pthread_mutex_unlock(&mux_state_pc);
		pthread_mutex_lock(&mux_ref_pc);
			ref_loc = ref_pc;
		pthread_mutex_unlock(&mux_ref_pc);
		pthread_mutex_lock(&mux_state_pc);
			view_loc = view;
		pthread_mutex_unlock(&mux_state_pc);
		pthread_mutex_lock(&mux_parcontr_pc);
			par_control_loc = par_control_pc;
		pthread_mutex_unlock(&mux_parcontr_pc);
		
		gui_draw(state_loc, ref_loc, view_loc, par_control_loc);
		
		//--------- end task
		if(deadline_miss(id)){
			dl_miss_gui+=1;
		}
#ifdef EXTIME
        stop_extime(4);
#endif
		wait_for_period(id);		// wait to next period
	}
	return 0;
}

//----------- rad
float rad(float ang) {
    return ang / 180 * M_PI;
}

//----------- thick_line
void thick_line(BITMAP *bmp, float x, float y, float x_, float y_, float thickness, int color) {
	float dx = x - x_;
	float dy = y - y_;
	float d = sqrtf(dx * dx + dy * dy);
	if (!d)
		return;
	int v[4 * 2];

	/* left up */
	v[0] = x - thickness * dy / d;
	v[1] = y + thickness * dx / d;
	/* right up */
	v[2] = x + thickness * dy / d;
	v[3] = y - thickness * dx / d;
	/* right down */
	v[4] = x_ + thickness * dy / d;
	v[5] = y_ - thickness * dx / d;
	/* left down */
	v[6] = x_ - thickness * dy / d;
	v[7] = y_ + thickness * dx / d;

	polygon(bmp, 4, v, color);
}

//----------- Circle_rifalpha: disegno un arco di circonf in prospettiva per visualizzare alpha
void circlerif_alpha(BITMAP *bmp, Point C, int r, AngleSinCos Lon, AngleSinCos Lat, float alpha, float refalpha,
					 int color) {
	float initial, final, step;
	Vect P; Point PAlleg;

	step = 2*M_PI/NUM_POINTS;
	
//	alpha = ((int)alpha)%360;
	
	if (alpha < refalpha) {
		initial = rad(alpha);
		final = rad(refalpha);
	} else {
		initial = rad(refalpha);
		final = rad(alpha);
	}// end if

	for (float t = initial; t < final; t = t + step) {
		// mi fermo raggiunto alpharad; in un giro disegno NUM_POINTS punti.
		P.x = r*cosf(t);
		P.y = r*sinf(t);
		P.z = 0;
		PAlleg = proiez_asson(P, Lon, Lat);
		putpixel(bmp, C.x + PAlleg.x, C.y + PAlleg.y, color);
	} // end for
}
//----------- circlerif_theta: disegna un arco di circonf in prospettiva per visualizzare theta
void circlerif_theta(BITMAP *bmp, Point C, int r, int l1, AngleSinCos Alpha, AngleSinCos Lon, AngleSinCos Lat,
					 float theta, float reftheta, int color) {
	float initial, final, step;
	Vect P; Point PAlleg;

	step = 2*M_PI/NUM_POINTS;

	if (theta < reftheta) {
		initial = rad(theta);
		final = rad(reftheta);
	} else {
		initial = rad(reftheta);
		final = rad(theta);
	}// end if

	for (float t = initial; t < final; t = t + step) {
		// mi fermo raggiunto thetarad; in un giro disegno NUM_POINTS punti.
		P.x = l1*Alpha.cos - r*Alpha.sin*sinf(t);
		P.y = l1*Alpha.sin + r*Alpha.cos*sinf(t);
		P.z = r*cosf(t);
		PAlleg = proiez_asson(P, Lon, Lat);
		putpixel(bmp, C.x + PAlleg.x, C.y + PAlleg.y, color);
	} // end for
}
//----------- circlerif_parup: disegna un cerchio parametrico che inizia dalla verticale
void circlerif_parup(BITMAP *bmp, Point C, int r, float ang, float refang, int color) {
	float initial, final, step;
	Point PAlleg;

	step = 2*M_PI/NUM_POINTS;

	if (ang < refang) {
		initial = rad(ang);
		final = rad(refang);
	} else {
		initial = rad(refang);
		final = rad(ang);
	}// end if

	for (float t = initial; t < final; t = t + step) {
		// mi fermo raggiunto ang, in un giro disegno NUM_POINTS punti
		PAlleg.x = r*sinf(t);
		PAlleg.y = - r*cosf(t);
		putpixel(bmp, C.x + PAlleg.x, C.y + PAlleg.y, color);
	} // end for

}
//-----------  proiez_asson proietto coordinate spaziali in coordinate di disegno secondo una proiezione assonometrica
Point proiez_asson(Vect P, AngleSinCos Lon, AngleSinCos Lat) {
	Point PAlleg;
	PAlleg.x = P.y*Lon.cos - P.x * Lon.sin;
	PAlleg.y = P.x*Lon.cos * Lat.sin - P.z* Lat.cos + P.y * Lat.sin*Lon.sin;
	return PAlleg;
}
//----------- grid, disegna griglia in assonometria
void grid(Vect P1, Vect P2, int q, int posx, int posy, AngleSinCos Lon, AngleSinCos Lat, int col) {
	int lx, ly;
	int dx, dy;
	Vect A, B;
	Point a, b;

	lx = fabsf(P1.x - P2.x);
	ly = fabsf(P1.y - P2.y);
	dx = lx / q;
	dy = ly / q;
	A = P1;
	B = P1; B.y += ly;

	//Parte X
	if (P1.x < P2.x) {
		for (int t=P1.x; t <= P2.x; t = t + dx) {
			A.x = t;
			B.x = t;
			a = proiez_asson(A, Lon, Lat); a.x += posx; a.y += posy;
			b = proiez_asson(B, Lon, Lat); b.x += posx; b.y += posy;
			line(scrbuf, a.x, a.y, b.x, b.y, col);
		} // end for
	}// end if
	if (P1.x > P2.x) {
		for (int t=P2.x; t <= P1.x; t = t + dx) {
			A.x = t;
			B.x = t;
			a = proiez_asson(A, Lon, Lat); a.x += posx; a.y += posy;
			b = proiez_asson(B, Lon, Lat); b.x += posx; b.y += posy;
			line(scrbuf, a.x, a.y, b.x, b.y, col);
		} // end for
	}// end if

	//Parte Y
	A = P1;
	B = P1; B.x += lx;
	if (P1.y < P2.y) {
		for (int t=P1.y; t <= P2.y; t = t + dy) {
			A.y = t;
			B.y = t;
			a = proiez_asson(A, Lon, Lat); a.x += posx; a.y += posy;
			b = proiez_asson(B, Lon, Lat); b.x += posx; b.y += posy;
			line(scrbuf, a.x, a.y, b.x, b.y, col);
		} // end for
	}// end if
	if (P1.y > P2.y) {
		for (int t=P2.y; t <= P1.y; t = t + dy) {
			A.y = t;
			B.y = t;
			a = proiez_asson(A, Lon, Lat); a.x += posx; a.y += posy;
			b = proiez_asson(B, Lon, Lat); b.x += posx; b.y += posy;
			line(scrbuf, a.x, a.y, b.x, b.y, col);
		} // end for
	}// end if

}
//----------- vista_asson, disegna vista assonometrica
void vista_asson(float alpha, float refalpha, float theta, float reftheta, float lon, float lat) {
	int l1 = L1_ASSON, l2 = L2_ASSON;	//lunghezze aste
	AngleSinCos Alpha, RefAlpha, Theta, RefTheta, Lon, Lat;
	Vect Arif, Prif;
	Vect A0, P0; // rif, 0 indicano il sistema di riferimento
	TwoPoints riflink1asson, riflink2asson; // riferimenti link vista assonometrica
	TwoPoints link1asson, link2asson; // link vista assonometrica
	Vect P1, P2; // griglia

	Alpha.sin = sinf(rad(alpha));	Alpha.cos = cosf(rad(alpha));
	RefAlpha.sin = sinf(rad(refalpha)); RefAlpha.cos = cosf(rad(refalpha));
	Theta.sin = sinf(rad(theta));	Theta.cos = cosf(rad(theta));
	RefTheta.sin = sinf(rad(reftheta)); RefTheta.cos = cosf(rad(reftheta));
	Lat.sin = sinf(rad(lat)); Lat.cos = cosf(rad(lat));
	Lon.sin = sinf(rad(lon)); Lon.cos = cosf(rad(lon));

	// Reset della vista
	rectfill(scrbuf, resetasson.x1, resetasson.y1, resetasson.x2, resetasson.y2, col.bck);

	Arif.x = l1*RefAlpha.cos;
	Arif.y = l1*RefAlpha.sin;
	Arif.z = 0;
	Prif.x = l1*Alpha.cos - l2*Alpha.sin*RefTheta.sin;
	Prif.y = l1*Alpha.sin + l2*Alpha.cos*RefTheta.sin;
	Prif.z = l2*RefTheta.cos;

	A0.x = l1*Alpha.cos;
	A0.y = l1*Alpha.sin;
	A0.z = 0;
	P0.x = l1*Alpha.cos - l2*Alpha.sin*Theta.sin;
	P0.y = l1*Alpha.sin + l2*Alpha.cos*Theta.sin;
	P0.z = l2*Theta.cos;

	riflink1asson.x1 = asson0.x;
	riflink1asson.y1 = asson0.y;
	riflink1asson.x2 = asson0.x + proiez_asson(Arif, Lon, Lat).x;
	riflink1asson.y2 = asson0.y + proiez_asson(Arif, Lon, Lat).y;
	riflink2asson.x1 = asson0.x + proiez_asson(A0, Lon, Lat).x;
	riflink2asson.y1 = asson0.y + proiez_asson(A0, Lon, Lat).y;
	riflink2asson.x2 = asson0.x + proiez_asson(Prif, Lon, Lat).x;
	riflink2asson.y2 = asson0.y + proiez_asson(Prif, Lon, Lat).y;

	link1asson.x1 = asson0.x;
	link1asson.y1 = asson0.y;
	link1asson.x2 = asson0.x + proiez_asson(A0, Lon, Lat).x;
	link1asson.y2 = asson0.y + proiez_asson(A0, Lon, Lat).y;
	link2asson.x1 = asson0.x + proiez_asson(A0, Lon, Lat).x;
	link2asson.y1 = asson0.y + proiez_asson(A0, Lon, Lat).y;
	link2asson.x2 = asson0.x + proiez_asson(P0, Lon, Lat).x;
	link2asson.y2 = asson0.y + proiez_asson(P0, Lon, Lat).y;

	// Disegno riferimenti e oggeti

	// Griglia
	P1.x = -LX_GRID/2; P1.y = -LY_GRID/2; P1.z = -l1;
	P2.x = +LX_GRID/2; P2.y = +LY_GRID/2; P2.z = -l1;
	grid(P1, P2, NUM_GRID, asson0.x, asson0.y, Lon, Lat, col.rif);
	// asse verticale
	thick_line(scrbuf, asson0.x, asson0.y + l1*Lat.cos, asson0.x, asson0.y, THICK+1, col.vert);
	line(scrbuf, riflink1asson.x1, riflink1asson.y1, riflink1asson.x2, riflink1asson.y2, col.rif); //rif link1
	line(scrbuf, riflink2asson.x1, riflink2asson.y1, riflink2asson.x2, riflink2asson.y2, col.rif); //rif link2
	circlerif_alpha(scrbuf, asson0, l1, Lon, Lat, alpha, refalpha, col.rif); //rif alpha
	circlerif_theta(scrbuf, asson0, l2, l1, Alpha, Lon, Lat, theta, reftheta, col.rif); //rif theta

	// Disegno link
	thick_line(scrbuf, link1asson.x1, link1asson.y1, link1asson.x2, link1asson.y2, THICK, col.mdl); //link1
	thick_line(scrbuf, link2asson.x1, link2asson.y1, link2asson.x2, link2asson.y2, THICK, col.mdl2); //link2
}
//----------- vista_lato, disegna vista lato
void vista_lato(float theta, float reftheta) {
	int l2 = L2_LATO;
	AngleSinCos Theta, RefTheta;
	TwoPoints riflink2lato, link2lato;
	
	Theta.sin = sinf(rad(theta));	Theta.cos = cosf(rad(theta));
	RefTheta.sin = sinf(rad(reftheta)); RefTheta.cos = cosf(rad(reftheta));

	// Reset della vista
	rectfill(scrbuf, resetlato.x1, resetlato.y1, resetlato.x2, resetlato.y2, col.bck);

	riflink2lato.x1 = lato0.x;
	riflink2lato.y1 = lato0.y;
	riflink2lato.x2 = lato0.x + l2 * RefTheta.sin;
	riflink2lato.y2 = lato0.y - l2 * RefTheta.cos;
	link2lato.x1 = lato0.x;
	link2lato.y1 = lato0.y;
	link2lato.x2 = lato0.x + l2 * Theta.sin;
	link2lato.y2 = lato0.y - l2 * Theta.cos;
	line(scrbuf, riflink2lato.x1, riflink2lato.y1, riflink2lato.x2, riflink2lato.y2, col.rif); // rif link lato
	circlerif_parup(scrbuf, lato0, l2, theta, reftheta, col.rif); // rif angolo theta
	thick_line(scrbuf, link2lato.x1, link2lato.y1, link2lato.x2, link2lato.y2, THICK, col.mdl2); // link lato
}
// vista_alto, disegna vista alto
void vista_alto(float alpha, float refalpha) {
	int l1 = L1_ALTO;
	AngleSinCos Alpha, RefAlpha;
	TwoPoints riflink1alto, link1alto;

	Alpha.sin = sinf(rad(alpha));	Alpha.cos = cosf(rad(alpha));
	RefAlpha.sin = sinf(rad(refalpha)); RefAlpha.cos = cosf(rad(refalpha));

	// Reset della vista
	rectfill(scrbuf, resetalto.x1, resetalto.y1, resetalto.x2, resetalto.y2, col.bck);

	riflink1alto.x1 = alto0.x;
	riflink1alto.y1 = alto0.y;
	riflink1alto.x2 = alto0.x - l1 * RefAlpha.sin;
	riflink1alto.y2 = alto0.y - l1 * RefAlpha.cos;
	link1alto.x1 = alto0.x;
	link1alto.y1 = alto0.y;
	link1alto.x2 = alto0.x - l1 * Alpha.sin;
	link1alto.y2 = alto0.y - l1 * Alpha.cos;
	line(scrbuf, riflink1alto.x1, riflink1alto.y1, riflink1alto.x2, riflink1alto.y2, col.rif); // rif link alto
	circlerif_parup(scrbuf, alto0, l1, -alpha, -refalpha, col.rif); // rif angolo alpha
	thick_line(scrbuf, link1alto.x1, link1alto.y1, link1alto.x2, link1alto.y2, THICK, col.mdl); // link alto
}

//----------- init gui
int gui_init(){
	// Init allegro
	if (allegro_init() != 0)
		return 1;
	set_color_depth(COLOR_DEPTH); 	// RGB mode (32 bits)
	install_keyboard();		//posso usare la tastiera

	// dimensioni dei rettangoli
	int hrett1 = HSCREEN - 2 * DIST; // altezza rettangolo 1
	int wrett5 = WSCREEN - 3 * DIST - WRETT1;
	int hrett3 = wrett5 / 2;
	int wrett3 = wrett5 / 2;
	int hrett4 = hrett3;
	int wrett4 = wrett3;
	int hrett2 = HSCREEN - 2 * DIST - hrett3;
	int wrett2 = wrett5;

	int qdistvert = HSCREEN/ DIST - 2; // quante DIST entrano dentro il mio schermo, dimensione dell'array scritte
	scritte.x = 2*DIST;
	for (int i = 0; i < qdistvert; i++) {
		scritte.y[i] = DIST * (i+1);
	} //popolo l'array scritte con le posizioni xy delle scritte

	if (set_gfx_mode(GRAPHICS, WSCREEN, HSCREEN, 0, 0) != 0) {
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
		return 1;
	}

	scrbuf = create_bitmap(SCREEN_W, SCREEN_H);
	if (!scrbuf) {
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message("Unable to initialize page flipping\n");
		return 1;
	}

	// scelta colori rettangoli, scritte, background
	col.bck = COL_BCK;
	col.rett = COL_RETT;
	col.scr = COL_SCR;
	// colori modello, rif
	col.mdl = COL_MDL;
	col.mdl2 = COL_MDL2;
	col.vert = COL_VERT;
	col.rif = COL_RIF;

	// colora background
	clear_to_color(scrbuf, col.bck);

	// Disegno rettangoli e elementi statici
	//rett1 - parametri, sinistra
	rect(scrbuf, DIST, DIST, DIST + WRETT1, DIST + hrett1, col.rett);
	char exit[50], reset1[50], reset2[50], reset3[50], reset4[50], dist_pause[50], swingup_pause[43], reset_par_dn[28], reset_delay[30]; 
	sprintf(exit, "Premere Esc per uscire");
	textout_ex(scrbuf, font, exit, scritte.x, scritte.y[qdistvert-1], col.scr, col.bck);

	sprintf(reset1, "1 per resettare riferimento");
	textout_ex(scrbuf, font, reset1, scritte.x, scritte.y[qdistvert-2], col.scr, col.bck);

	sprintf(reset3, "2 per resettare la vista");
	textout_ex(scrbuf, font, reset3, scritte.x, scritte.y[qdistvert-3], col.scr, col.bck);
	
	sprintf(reset4, "3 per resettare i parametri del controllore");
	textout_ex(scrbuf, font, reset4, scritte.x, scritte.y[qdistvert-4], col.scr, col.bck);
	
	sprintf(reset_par_dn, "4 resetta disturbo e rumore");
	textout_ex(scrbuf, font, reset_par_dn, scritte.x, scritte.y[qdistvert-5], col.scr, col.bck);
	
	sprintf(reset_delay, "5 resetta ritardi");
	textout_ex(scrbuf, font, reset_delay, scritte.x, scritte.y[qdistvert-6], col.scr, col.bck);	
	
	
	sprintf(reset2, "Up, down, left, right per modificare la vista");
	textout_ex(scrbuf, font, reset2, scritte.x, scritte.y[qdistvert-7], col.scr, col.bck);

	sprintf(dist_pause, "D disturba il sistema");
	textout_ex(scrbuf, font, dist_pause, scritte.x, scritte.y[qdistvert-8], col.scr, col.bck);
	
	sprintf(swingup_pause, "Q attiva/disattiva swingup, default attivo");
	textout_ex(scrbuf, font, swingup_pause, scritte.x, scritte.y[qdistvert-9], col.scr, col.bck);

	
	//-----------  disegno rettangoli
	TwoPoints rett_asso, rett_alto, rett_lato;
	Point titolo_asso, titolo_alto, titolo_lato;
	char assonstr[50], altostr[50], latostr[50];
	//rett2 - Vista assonometrica
	rett_asso.x1 = DIST + WRETT1 + DIST;
	rett_asso.y1 = DIST;
	rett_asso.x2 = DIST + WRETT1 + DIST + wrett2;
	rett_asso.y2 = DIST + hrett2;
	rect(scrbuf, rett_asso.x1, rett_asso.y1, rett_asso.x2, rett_asso.y2, col.rett);
	sprintf(assonstr, "Vista Assonometrica");
	titolo_asso.x = DIST + WRETT1 + DIST + wrett2 /2 - 3.5*DIST;
	titolo_asso.y = 2*DIST;
	textout_ex(scrbuf, font, assonstr, titolo_asso.x, titolo_asso.y, col.scr, col.bck);
	//rett3 - vista alto
	rett_alto.x1 = DIST + WRETT1 + DIST;
	rett_alto.y1 = DIST + hrett2;
	rett_alto.x2 = DIST + WRETT1 + DIST + wrett3;
	rett_alto.y2 = DIST + hrett2 + hrett3;
	rect(scrbuf, rett_alto.x1, rett_alto.y1, rett_alto.x2, rett_alto.y2, col.rett);
	sprintf(altostr, "Vista Alto");
	titolo_alto.x = DIST + WRETT1 + DIST + wrett3 /2 - 2.1*DIST;
	titolo_alto.y = DIST + hrett2 + DIST;
	textout_ex(scrbuf, font, altostr, titolo_alto.x, titolo_alto.y,	col.scr, col.bck);
	//rett4 - vista Lato
	rett_lato.x1 = DIST + WRETT1 + DIST + wrett3;
	rett_lato.y1 = DIST + hrett2;
	rett_lato.x2 = DIST + WRETT1 + DIST + wrett3 + wrett4;
	rett_lato.y2 = DIST + hrett2 + hrett4;
	rect(scrbuf, rett_lato.x1, rett_lato.y1, rett_lato.x2, rett_lato.y2, col.rett);
	sprintf(latostr, "Vista Lato");
	titolo_lato.x = DIST + WRETT1 + DIST + wrett3 + wrett4/2 - 2.1*DIST;
	titolo_lato.y = DIST + hrett2 + DIST;
	textout_ex(scrbuf, font, latostr, titolo_lato.x, titolo_lato.y, col.scr, col.bck);

	//----------------------------------
	// Calcoli per la grafica
	// posizione centrale rettangolo v.assonometrica
	asson0.x = WRETT1 + wrett2/2 + 2*DIST;
	asson0.y = hrett2/2 + DIST; // posizione centrale rettangolo v.asson
	// posizione centrale rettangolo v.lato
	lato0.x = WRETT1 + wrett3 + wrett4/2 + 2*DIST;
	lato0.y = hrett2 + hrett3/2 + DIST;
	lato0.y += DIST;
	// posizione centrale rettangolo v.alto
	alto0.x = WRETT1 + wrett3/2 + 2*DIST;
	alto0.y = hrett2 + hrett3/2 + DIST;
	alto0.y += DIST;
	// reset viste
	resetasson.x1 = WRETT1 + 2*DIST + 1;
	resetasson.y1 = DIST + 1;
	resetasson.y1 += 2*DIST;
	resetasson.x2 = WRETT1 + wrett2 + 2*DIST - 1;
	resetasson.y2 = hrett2 + DIST - 1;
	resetalto.x1 = WRETT1 + 2*DIST + 1;
	resetalto.y1 = hrett2 + DIST + 1;
	resetalto.y1 += 2*DIST;
	resetalto.x2 = WRETT1 + wrett3 + 2*DIST - 1;
	resetalto.y2 = hrett2 + hrett3 + DIST - 1;
	resetlato.x1 = WRETT1 + wrett3 + 2*DIST + 1;
	resetlato.y1 = hrett2 + DIST + 1;
	resetlato.y1 += 2*DIST;
	resetlato.x2 = WRETT1 + wrett3 + wrett4 + 2*DIST - 1;
	resetlato.y2 = hrett2 + hrett4 + DIST - 1;

	return 0;
}

void scritte_draw(state_pc_t state,ref_t ref, par_ctrl_t par_control){
	// ref
	textout_ex(scrbuf, font, "Riferimento:", scritte.x, scritte.y[1], col.scr, col.bck);
	sprintf(refalphastr, "alpha = %5.2f, a/s -+%d  ", ref.alpha, INCR_ANG);
	textout_ex(scrbuf, font, refalphastr, scritte.x, scritte.y[2], col.scr, col.bck);
	// state
	textout_ex(scrbuf, font, "Stato:", scritte.x, scritte.y[4], col.scr, col.bck);
	sprintf(alphastr, "alpha = %5.2f    ", state.alpha);
	textout_ex(scrbuf, font, alphastr, scritte.x, scritte.y[5], col.scr, col.bck);
	sprintf(thetastr, "theta = %5.2f    ", state.theta);
	textout_ex(scrbuf, font, thetastr,scritte.x, scritte.y[6], col.scr, col.bck);
	sprintf(voltagestr, "Volt = %5.2f V", state.voltage);
	textout_ex(scrbuf, font, voltagestr,scritte.x, scritte.y[7], col.scr, col.bck);
	// par_control
	textout_ex(scrbuf, font, "Parametri dei controllori:", scritte.x, scritte.y[9], col.scr, col.bck);
	sprintf(parcontrstralpha, "Alpha: Kp = %5.2f r/t, Kd = %5.2f y/u    ", par_control.up_kp_alpha, par_control.up_kd_alpha);
	textout_ex(scrbuf, font, parcontrstralpha,scritte.x, scritte.y[10], col.scr, col.bck);
	sprintf(parcontrstrtheta, "Theta: Kp = %5.2f h/j, Kd = %5.2f k/l    ", par_control.up_kp_theta, par_control.up_kd_theta);
	textout_ex(scrbuf, font, parcontrstrtheta,scritte.x, scritte.y[11], col.scr, col.bck);
	sprintf(parcontrstralphadown, "Alpha, basso: Kp = %5.2f v/b, Kd = %5.2f n/m  ", par_control.down_kp_alpha, par_control.down_kd_alpha);
	textout_ex(scrbuf, font, parcontrstralphadown,scritte.x, scritte.y[12], col.scr, col.bck);
	// par_dn e dn
	textout_ex(scrbuf, font, "Disturbo, rumore e ritardi:", scritte.x, scritte.y[13], col.scr, col.bck);
	sprintf(pardnstr, "Dist:%5.2f N f/g, Rumore: %2hu x/c, Rit: %2hhu w/e ", par_dn.dist_amp, par_dn.noise_amp, dn.delay);
	textout_ex(scrbuf, font, pardnstr, scritte.x, scritte.y[14], col.scr, col.bck);
	// pole gen ref_loc
	sprintf(pole_ref_str, "Polo generatore di riferimento: %5.1f i/o  ", pole_ref);
	textout_ex(scrbuf, font, pole_ref_str, scritte.x, scritte.y[15], col.scr, col.bck);
	// tempo di esecuzione del task di controllo
	sprintf(control_period_str, "Periodo di control: %2u 9/0, def = 5  ", period_control);
	textout_ex(scrbuf, font, control_period_str, scritte.x, scritte.y[16], col.scr, col.bck);
	
	// swingup
	if(ref_pc.swingup){
		sprintf(swingup_str, "Controllore Swingup attivo!  ");
	}else{
		sprintf(swingup_str, "Controllore Swingup disattivo");
	}
	textout_ex(scrbuf, font, swingup_str,scritte.x, scritte.y[17], col.scr, col.bck);
	
	
#ifndef EXTIME
	// deadline_miss
	textout_ex(scrbuf, font, "Task,       ndmiss:", scritte.x, scritte.y[19], col.scr, col.bck);
	sprintf(realtime_str, "state_update:  %4d", dl_miss_state_update);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[20], col.scr, col.bck);
	sprintf(realtime_str, "control:       %4d", dl_miss_control);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[21], col.scr, col.bck);
	sprintf(realtime_str, "compc:         %4d", dl_miss_compc);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[22], col.scr, col.bck);
	sprintf(realtime_str, "comboard:      %4d", dl_miss_comboard);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[23], col.scr, col.bck);
	sprintf(realtime_str, "gui:           %4d", dl_miss_gui);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[24], col.scr, col.bck);
	sprintf(realtime_str, "keys:          %4d", dl_miss_keys);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[25], col.scr, col.bck);

#endif
	
	
#ifdef EXTIME
	
	textout_ex(scrbuf, font, "Task,        ndmiss, et us, wcet:", scritte.x, scritte.y[19], col.scr, col.bck);
	sprintf(realtime_str, "state_update:  %4d,  %4d,  %4d", dl_miss_state_update, ex_time[0], wc_extime[0]);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[20], col.scr, col.bck);
	sprintf(realtime_str, "control:       %4d,  %4d,  %4d", dl_miss_control, ex_time[1], wc_extime[1]);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[21], col.scr, col.bck);
	sprintf(realtime_str, "compc:         %4d,  %4d,  %4d", dl_miss_compc, ex_time[2], wc_extime[2]);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[22], col.scr, col.bck);
	sprintf(realtime_str, "comboard:      %4d,  %4d,  %4d", dl_miss_comboard, ex_time[3], wc_extime[3]);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[23], col.scr, col.bck);
	sprintf(realtime_str, "gui:           %4d,  %4d,  %4d", dl_miss_gui, ex_time[4], wc_extime[4]);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[24], col.scr, col.bck);
	sprintf(realtime_str, "keys:          %4d,  %4d,  %4d", dl_miss_keys, ex_time[5], wc_extime[5]);
	textout_ex(scrbuf, font, realtime_str,scritte.x, scritte.y[25], col.scr, col.bck);

#endif
	
}


//----------- gui_draw
void gui_draw(state_pc_t state,ref_t ref, view_t view, par_ctrl_t par_control){
	
	vista_alto(state.alpha, ref.alpha);
	vista_lato(state.theta, ref.theta);
	vista_asson(state.alpha, ref.alpha, state.theta, ref.theta, view.lon, view.lat);
	scritte_draw(state, ref, par_control);

	blit(scrbuf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);


}










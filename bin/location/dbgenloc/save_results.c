#include "dbgenloc.h"

int
save_results (Dbptr dbin, Dbptr dbout, 
	Pf *pf, 
	Tbl *ta, Tbl *tu,
	Location_options *o, 
	char *vmodel, 
	Hypocenter *hypo, 
	Tbl *residual, 
	int *oridp )
{
    int             i, n, orid, grn, srn, retcode = 0;
    char *algorithm ="dbgenloc";
    double delta, esaz, seaz, slores, duphi, azres, azimuth ;
    Arrival *a ; 
    Slowness_vector *u ;

    *oridp = orid = dbnextid(dbin, "orid" ) ;
    if ( orid < 1 ) {
        complain ( 0, "Can't write to lastid table\n" ) ;
	return -1 ;
    }

    dbout = dblookup ( dbout, 0, "origin", 0, 0 ) ; 
    dbquery ( dbout, dbRECORD_COUNT, &i ) ; 

    dbout.record = dbaddnull ( dbout ) ; 
    grn = grnumber(hypo->lat, hypo->lon) ;
    srn = srnumber ( grn ) ; 
    if (dbout.record < 0
       ||  dbputv(dbout, 0,
	   "orid", orid, 
	    "lat", hypo->lat,
	    "lon", hypo->lon,
	    "depth", hypo->z,
	    "time", hypo->time,
	   "jdate", yearday(hypo->time),
	   "dtype", o->fix[3] ? "r" : "f", 
	   "grn", grn,
	   "srn", srn,
	   "algorithm", algorithm,
	   "auth", pfget_string ( pf, "author" ),
	   "nass", maxtbl(ta) + maxtbl(tu),
	   "ndef", maxtbl(ta) + maxtbl(tu),
	   0)  ) {
	complain(0, "Couldn't add origin record to database.\n");
	retcode = -1;
      }

    if(dbaddv(dbout, "origerr", "orid", orid, "sdobs", hypo->rms_raw, 0) < 0 ) {
	complain (1,"couldn't add origerr record to database\n" ) ;
	retcode = -1 ;
    }
	

    n = maxtbl(ta) ;
    for ( i=0 ; i<n ; i++ ) {
	a = (Arrival *) gettbl(ta, i) ;
	dist(rad(hypo->lat),rad(hypo->lon),rad(a->sta->lat),rad(a->sta->lon), &delta,&esaz);
	dist(rad(a->sta->lat),rad(a->sta->lon),rad(hypo->lat),rad(hypo->lon), &delta,&seaz);

	if ( dbaddv(dbout, "assoc", 
		"orid", orid, 
		"arid", a->arid, 
		"sta", a->sta->name, 
		"phase", a->phase->name, 
		"delta", deg(delta),
		"seaz", deg(seaz),
		"esaz", deg(esaz),
		"timeres", (double) a->res.raw_residual,
		"timedef", "d",
		"vmodel", vmodel,
		"wgt", (double) a->res.residual_weight, 
		0 ) < 0 ) 
	    complain ( 0, "Can't add assoc record for station %s arid=%d orid=%d\n", 
	    	a->sta->name, a->arid, orid ) ;
    }


    n = maxtbl(tu) ;
    dbout = dblookup ( dbout, 0, "assoc", 0, 0 ) ;
    for ( i=0 ; i<n ; i++ ) {
	u = (Slowness_vector *) gettbl(tu, i) ;
	if ( (dbout.record = dbaddv(dbout, "assoc", 
		"orid", orid, 
		"arid", u->arid, 
		"vmodel", vmodel,
		0 )) < 0 ) {
	    complain ( 0, "Can't add assoc record for station %s arid=%d orid=%d\n", 
	    	a->sta->name, a->arid, orid ) ;
	} else {
	    slores = deg2km(sqrt(sqr(u->xres.raw_residual) + sqr(u->yres.raw_residual))) ;
	    azimuth = atan2 ( u->uy, u->ux ) ;
	    duphi = (u->ux*cos(azimuth) - u->uy*sin(azimuth)) / sqrt(sqr(u->ux)+sqr(u->uy)) ;
	    azres = deg(duphi);

	    if ( dbputv ( dbout, 0, 
		"slores", slores,
		"slodef", "d",
		"azres", azres,
		"azdef", "d", 
		0 ) < 0 ) 
		complain ( 0, "Can't add slowness and azimuth residuals to assoc record #%d\n",
			dbout.record ) ; 
	}
    }

    return retcode ;
}

/* $Id$ */

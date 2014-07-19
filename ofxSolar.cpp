#include "ofxSolar.h"

void ofxSolar::init(double latitude, double longitude, int timezone){
	sunMapped=false;
	lat=latitude;
	lon=longitude;
	tz=timezone;
	dls=0;
}
void ofxSolar::setDaylightSaving(int dlsav){
	dls=dlsav;
}
void ofxSolar::update(){
	calculateSunMap();
}

void ofxSolar::calculateSunMap()
{

	year = ofGetYear();
	month = ofGetMonth();
	day = ofGetDay();

	dayleng  = dayLength( year, month, day, lon, lat, -35.0/60.0, 1 );
	civlen  = dayLength( year, month, day, lon, lat, -6.0, 0 );
	nautlen = dayLength( year, month, day, lon, lat, -12.0, 0 );
	astrlen = dayLength( year, month, day, lon, lat, -18.0, 0 );

	rs   = sunriset( year, month, day, lon, lat, -35.0/60.0, 1, &rise, &set ); //sunrise and sunset
	civ  = sunriset( year, month, day, lon, lat, -6.0, 0, &civ_start, &civ_end ); //civil twilight
	naut = sunriset( year, month, day, lon, lat, -12.0, 0, &naut_start, &naut_end ); //nautical twilight
	astr = sunriset( year, month, day, lon, lat, -18.0, 0, &astr_start, &astr_end ); //astronomical twilight

	sunMapped=true;
}

string ofxSolar::decimalday_to_timestamp(double d_time){
	double hours = d_time;
	int h = (int) round(hours);
	double minutes = (hours - h) * 60;
	int m = (int) round(minutes);
	if(m<0){
		h-=1;
		m = 60+m;
		minutes = 60+minutes;
	}
	double seconds = (minutes - m) * 60;
	int s = (int) round(seconds);
	if (s<0){
		m-=1;
		if(m<0){
			h-=1;
			m = 60+m;
		}
		s = 60+s;
	}
	if(m<0)
		h-=1;
	h = h%12;

	tm tempTime;
	time_t rawtime;

	time ( &rawtime );
	tempTime =*localtime ( &rawtime );

	tempTime.tm_hour = h;
	tempTime.tm_min = m;
	tempTime.tm_sec = s;

	char buffer[25];

	strftime(buffer, 25, "%H:%M:%S", &tempTime);

	return ofToString(buffer);
}
void ofxSolar::printInfo(){
	printf( "Day length:                 %5.2f hours\n", dayLength() );
	printf( "With nautical twilight      %5.2f hours\n", nauticalTwilightDayLength() );
	printf( "With astronomical twilight  %5.2f hours\n", astronomicalTwilightDayLength() );
	printf( "Length of twilight: civil   %5.2f hours\n",
		(civilTwilightDayLength()-dayLength())/2.0);
	printf( "                  nautical  %5.2f hours\n",
		(nauticalTwilightDayLength()-dayLength())/2.0);
	printf( "              astronomical  %5.2f hours\n",
		(astronomicalTwilightDayLength()-dayLength())/2.0);
	printf( "Sun rises %s, sets %s\n",
		decimalday_to_timestamp(sunrise()).c_str(), decimalday_to_timestamp(sunset()).c_str());
	printf( "Civil twilight starts %s, "
		"ends %s\n", decimalday_to_timestamp(civilTwilightStart()).c_str(), decimalday_to_timestamp(civilTwilightEnd()).c_str());
	printf( "Nautical twilight starts %s, "
		"ends %s\n", decimalday_to_timestamp(nauticalTwilightStart()).c_str(), decimalday_to_timestamp(nauticalTwilightEnd()).c_str());
	printf( "Astronomical twilight starts %s, "
		"ends %s\n", decimalday_to_timestamp(astronomicalTwilightStart()).c_str(), decimalday_to_timestamp(astronomicalTwilightEnd()).c_str());

	printf("\n================================================================================ \n");
}
double ofxSolar::dayLength(){
	if(!sunMapped){
		calculateSunMap();
	}
	return dayleng;
}
double ofxSolar::civilTwilightDayLength(){
	if(!sunMapped){
		calculateSunMap();
	}
	return civlen;
}
double ofxSolar::nauticalTwilightDayLength(){
	if(!sunMapped){
		calculateSunMap();
	}
	return nautlen;
}
double ofxSolar::astronomicalTwilightDayLength(){
	if(!sunMapped){
		calculateSunMap();
	}
	return astrlen;
}
double ofxSolar::sunrise(){
	if(!sunMapped){
		calculateSunMap();
	}
	return rise+tz+dls;
}
double ofxSolar::sunset(){
	if(!sunMapped){
		calculateSunMap();
	}
	return set+tz+dls;
}
double ofxSolar::civilTwilightStart(){
	if(!sunMapped){
		calculateSunMap();
	}
	return civ_start+tz+dls;
}
double ofxSolar::nauticalTwilightStart(){
	if(!sunMapped){
		calculateSunMap();
	}
	return naut_start+tz+dls;
}
double ofxSolar::astronomicalTwilightStart(){
	if(!sunMapped){
		calculateSunMap();
	}
	return astr_start+tz+dls;
}
double ofxSolar::civilTwilightEnd(){
	if(!sunMapped){
		calculateSunMap();
	}
	return civ_end+tz+dls;
}
double ofxSolar::nauticalTwilightEnd(){
	if(!sunMapped){
		calculateSunMap();
	}
	return naut_end+tz+dls;
}
double ofxSolar::astronomicalTwilightEnd(){
	if(!sunMapped){
		calculateSunMap();
	}
	return astr_end+tz+dls;
}


/* The "workhorse" function for sun rise/set times */

int ofxSolar::sunriset( int year, int month, int day, double lon, double lat,
					   double altit, int upper_limb, double *trise, double *tset )
					   /***************************************************************************/
					   /* Note: year,month,date = calendar date, 1801-2099 only.             */
					   /*       Eastern longitude positive, Western longitude negative       */
					   /*       Northern latitude positive, Southern latitude negative       */
					   /*       The longitude value IS critical in this function!            */
					   /*       altit = the altitude which the Sun should cross              */
					   /*               Set to -35/60 degrees for rise/set, -6 degrees       */
					   /*               for civil, -12 degrees for nautical and -18          */
					   /*               degrees for astronomical twilight.                   */
					   /*         upper_limb: non-zero -> upper limb, zero -> center         */
					   /*               Set to non-zero (e.g. 1) when computing rise/set     */
					   /*               times, and to zero when computing start/end of       */
					   /*               twilight.                                            */
					   /*        *rise = where to store the rise time                        */
					   /*        *set  = where to store the set  time                        */
					   /*                Both times are relative to the specified altitude,  */
					   /*                and thus this function can be used to compute       */
					   /*                various twilight times, as well as rise/set times   */
					   /* Return value:  0 = sun rises/sets this day, times stored at        */
					   /*                    *trise and *tset.                               */
					   /*               +1 = sun above the specified "horizon" 24 hours.     */
					   /*                    *trise set to time when the sun is at south,    */
					   /*                    minus 12 hours while *tset is set to the south  */
					   /*                    time plus 12 hours. "Day" length = 24 hours     */
					   /*               -1 = sun is below the specified "horizon" 24 hours   */
					   /*                    "Day" length = 0 hours, *trise and *tset are    */
					   /*                    both set to the time when the sun is at south.  */
					   /*                                                                    */
					   /**********************************************************************/
{
	double  d,  /* Days since 2000 Jan 0.0 (negative before) */
		sr,         /* Solar distance, astronomical units */
		sRA,        /* Sun's Right Ascension */
		sdec,       /* Sun's declination */
		sradius,    /* Sun's apparent radius */
		t,          /* Diurnal arc */
		tsouth,     /* Time when Sun is at south */
		sidtime;    /* Local sidereal time */

	int rc = 0; /* Return cde from function - usually 0 */

	/* Compute d of 12h local mean solar time */
	d = days_since_2000_Jan_0(year,month,day) + 0.5 - lon/360.0;

	/* Compute the local sidereal time of this moment */
	sidtime = revolution( GMST0(d) + 180.0 + lon );

	/* Compute Sun's RA, Decl and distance at this moment */
	sun_RA_dec( d, &sRA, &sdec, &sr );

	/* Compute time when Sun is at south - in hours UT */
	tsouth = 12.0 - rev180(sidtime - sRA)/15.0;

	/* Compute the Sun's apparent radius in degrees */
	sradius = 0.2666 / sr;

	/* Do correction to upper limb, if necessary */
	if ( upper_limb )
		altit -= sradius;

	/* Compute the diurnal arc that the Sun traverses to reach */
	/* the specified altitude altit: */
	{
		double cost;
		cost = ( sind(altit) - sind(lat) * sind(sdec) ) /
			( cosd(lat) * cosd(sdec) );
		if ( cost >= 1.0 )
			rc = -1, t = 0.0;       /* Sun always below altit */
		else if ( cost <= -1.0 )
			rc = +1, t = 12.0;      /* Sun always above altit */
		else
			t = acosd(cost)/15.0;   /* The diurnal arc, hours */
	}

	/* Store rise and set times - in hours UT */
	*trise = tsouth - t;
	*tset  = tsouth + t;

	return rc;
}  /* sunriset */



/* The "workhorse" function */


double ofxSolar::dayLength( int year, int month, int day, double lon, double lat, double altit, int upper_limb )
	/**********************************************************************/
	/* Note: year,month,date = calendar date, 1801-2099 only.             */
	/*       Eastern longitude positive, Western longitude negative       */
	/*       Northern latitude positive, Southern latitude negative       */
	/*       The longitude value is not critical. Set it to the correct   */
	/*       longitude if you're picky, otherwise set to to, say, 0.0     */
	/*       The latitude however IS critical - be sure to get it correct */
	/*       altit = the altitude which the Sun should cross              */
	/*               Set to -35/60 degrees for rise/set, -6 degrees       */
	/*               for civil, -12 degrees for nautical and -18          */
	/*               degrees for astronomical twilight.                   */
	/*         upper_limb: non-zero -> upper limb, zero -> center         */
	/*               Set to non-zero (e.g. 1) when computing day length   */
	/*               and to zero when computing day+twilight length.      */
	/**********************************************************************/
{
	double  d,  /* Days since 2000 Jan 0.0 (negative before) */
		obl_ecl,    /* Obliquity (inclination) of Earth's axis */
		sr,         /* Solar distance, astronomical units */
		slon,       /* True solar longitude */
		sin_sdecl,  /* Sine of Sun's declination */
		cos_sdecl,  /* Cosine of Sun's declination */
		sradius,    /* Sun's apparent radius */
		t;          /* Diurnal arc */

	/* Compute d of 12h local mean solar time */
	d = days_since_2000_Jan_0(year,month,day) + 0.5 - lon/360.0;

	/* Compute obliquity of ecliptic (inclination of Earth's axis) */
	obl_ecl = 23.4393 - 3.563E-7 * d;

	/* Compute Sun's ecliptic longitude and distance */
	sunpos( d, &slon, &sr );

	/* Compute sine and cosine of Sun's declination */
	sin_sdecl = sind(obl_ecl) * sind(slon);
	cos_sdecl = sqrt( 1.0 - sin_sdecl * sin_sdecl );

	/* Compute the Sun's apparent radius, degrees */
	sradius = 0.2666 / sr;

	/* Do correction to upper limb, if necessary */
	if ( upper_limb )
		altit -= sradius;

	/* Compute the diurnal arc that the Sun traverses to reach */
	/* the specified altitude altit: */
	{
		double cost;
		cost = ( sind(altit) - sind(lat) * sin_sdecl ) /
			( cosd(lat) * cos_sdecl );
		if ( cost >= 1.0 )
			t = 0.0;                      /* Sun always below altit */
		else if ( cost <= -1.0 )
			t = 24.0;                     /* Sun always above altit */
		else  t = (2.0/15.0) * acosd(cost); /* The diurnal arc, hours */
	}
	return t;
}  /* daylen */


/* This function computes the Sun's position at any instant */

void ofxSolar::sunpos( double d, double *lon, double *r )
	/******************************************************/
	/* Computes the Sun's ecliptic longitude and distance */
	/* at an instant given in d, number of days since     */
	/* 2000 Jan 0.0.  The Sun's ecliptic latitude is not  */
	/* computed, since it's always very near 0.           */
	/******************************************************/
{
	double M,         /* Mean anomaly of the Sun */
		w,         /* Mean longitude of perihelion */
		/* Note: Sun's mean longitude = M + w */
		e,         /* Eccentricity of Earth's orbit */
		E,         /* Eccentric anomaly */
		x, y,      /* x, y coordinates in orbit */
		v;         /* True anomaly */

	/* Compute mean elements */
	M = revolution( 356.0470 + 0.9856002585 * d );
	w = 282.9404 + 4.70935E-5 * d;
	e = 0.016709 - 1.151E-9 * d;

	/* Compute true longitude and radius vector */
	E = M + e * RADEG * sind(M) * ( 1.0 + e * cosd(M) );
	x = cosd(E) - e;
	y = sqrt( 1.0 - e*e ) * sind(E);
	*r = sqrt( x*x + y*y );              /* Solar distance */
	v = atan2d( y, x );                  /* True anomaly */
	*lon = v + w;                        /* True solar longitude */
	if ( *lon >= 360.0 )
		*lon -= 360.0;                   /* Make it 0..360 degrees */
}

void ofxSolar::sun_RA_dec( double d, double *RA, double *dec, double *r )
	/******************************************************/
	/* Computes the Sun's equatorial coordinates RA, Decl */
	/* and also its distance, at an instant given in d,   */
	/* the number of days since 2000 Jan 0.0.             */
	/******************************************************/
{
	double lon, obl_ecl, x, y, z;

	/* Compute Sun's ecliptical coordinates */
	sunpos( d, &lon, r );

	/* Compute ecliptic rectangular coordinates (z=0) */
	x = *r * cosd(lon);
	y = *r * sind(lon);

	/* Compute obliquity of ecliptic (inclination of Earth's axis) */
	obl_ecl = 23.4393 - 3.563E-7 * d;

	/* Convert to equatorial rectangular coordinates - x is unchanged */
	z = y * sind(obl_ecl);
	y = y * cosd(obl_ecl);

	/* Convert to spherical coordinates */
	*RA = atan2d( y, x );
	*dec = atan2d( z, sqrt(x*x + y*y) );

}  /* sun_RA_dec */


/******************************************************************/
/* This function reduces any angle to within the first revolution */
/* by subtracting or adding even multiples of 360.0 until the     */
/* result is >= 0.0 and < 360.0                                   */
/******************************************************************/

#define INV360    ( 1.0 / 360.0 )

double ofxSolar::revolution( double x )
	/*****************************************/
	/* Reduce angle to within 0..360 degrees */
	/*****************************************/
{
	return( x - 360.0 * floor( x * INV360 ) );
}  /* revolution */

double ofxSolar::rev180( double x )
	/*********************************************/
	/* Reduce angle to within +180..+180 degrees */
	/*********************************************/
{
	return( x - 360.0 * floor( x * INV360 + 0.5 ) );
}  /* revolution */


/*******************************************************************/
/* This function computes GMST0, the Greenwich Mean Sidereal Time  */
/* at 0h UT (i.e. the sidereal time at the Greenwhich meridian at  */
/* 0h UT).  GMST is then the sidereal time at Greenwich at any     */
/* time of the day.  I've generalized GMST0 as well, and define it */
/* as:  GMST0 = GMST - UT  --  this allows GMST0 to be computed at */
/* other times than 0h UT as well.  While this sounds somewhat     */
/* contradictory, it is very practical:  instead of computing      */
/* GMST like:                                                      */
/*                                                                 */
/*  GMST = (GMST0) + UT * (366.2422/365.2422)                      */
/*                                                                 */
/* where (GMST0) is the GMST last time UT was 0 hours, one simply  */
/* computes:                                                       */
/*                                                                 */
/*  GMST = GMST0 + UT                                              */
/*                                                                 */
/* where GMST0 is the GMST "at 0h UT" but at the current moment!   */
/* Defined in this way, GMST0 will increase with about 4 min a     */
/* day.  It also happens that GMST0 (in degrees, 1 hr = 15 degr)   */
/* is equal to the Sun's mean longitude plus/minus 180 degrees!    */
/* (if we neglect aberration, which amounts to 20 seconds of arc   */
/* or 1.33 seconds of time)                                        */
/*                                                                 */
/*******************************************************************/

double ofxSolar::GMST0( double d )
{
	double sidtim0;
	/* Sidtime at 0h UT = L (Sun's mean longitude) + 180.0 degr  */
	/* L = M + w, as defined in sunpos().  Since I'm too lazy to */
	/* add these numbers, I'll let the C compiler do it for me.  */
	/* Any decent C compiler will add the constants at compile   */
	/* time, imposing no runtime or code overhead.               */
	sidtim0 = revolution( ( 180.0 + 356.0470 + 282.9404 ) +
		( 0.9856002585 + 4.70935E-5 ) * d );
	return sidtim0;
}  /* GMST0 */

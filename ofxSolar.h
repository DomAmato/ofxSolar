/*
defines from Artur Pogoda de la Vega's arduino solar calculator Astro 
https://github.com/apdlv72/Astro

/*

SUNRISET.C - computes Sun rise/set times, start/end of twilight, and
the length of the day at any date and latitude

Written as DAYLEN.C, 1989-08-16

Modified to SUNRISET.C, 1992-12-01

(c) Paul Schlyter, 1989, 1992

Released to the public domain by Paul Schlyter, December 1992

Adapted from C to C++ for use in openframeworks by Dominic Amato 2014

*/


#include "ofMain.h"

#define days_since_2000_Jan_0(y,m,d) \
	(367L*(y)-((7*((y)+(((m)+9)/12)))/4)+((275*(m))/9)+(d)-730530L)

/* Some conversion factors between radians and degrees */

#define RADEG     ( 180.0 / PI )
#define DEGRAD    ( PI / 180.0 )

#define LOCATION_TOKYO			 +35.683	,	+139.767		// Tokyo, Japan
#define LOCATION_BEIJING		 +39.929	,	+116.388		// Beijing, China
#define LOCATION_NEWDELHI		 +28.637	, 	 +77.225		// New Delhi, India
#define LOCATION_MOSCOW			 +55.750	,	 +37.617		// Moscow, Russia
#define LOCATION_ANKARA			 +39.917	, 	 +32.850		// Ankara, Turkey
#define LOCATION_CAIRO			 +30.056	, 	 +31.239		// Cairo, Egypt
#define LOCATION_KIEV			 +50.450	,	 +30.500		// Kiev, Ukraine
#define LOCATION_ISTANBUL		 +41.010	,	 +28.960		// Istanbul, Turkey
#define LOCATION_HELSINKI		 +60.167	, 	 +24.933		// Helsinki, Finland
#define LOCATION_WARSHAW		 +52.216	, 	 +21.033		// Warshaw, Poland
#define LOCATION_BUDAPEST		 +47.500	,	 +19.050		// Budapest, Hungary
#define LOCATION_STOCKHOLM		 +59.325	, 	 +18.050		// Stockholm, Sweden
#define LOCATION_VIENNA			 +48.208	, 	 +16.373		// Vienna, Austria
#define LOCATION_PRAGUE			 +50.089	, 	 +14.421		// Prague, Czech Republic
#define LOCATION_BERLIN          +52.516	,	 +13.400		// Berlin, Germany
#define LOCATION_COPENHAGEN		 +55.676	,	 +12.579		// Copenhagen, Denmark
#define LOCATION_ROME            +41.884	,	 +12.484		// Rome, Italy
#define LOCATION_MUNICH		     +48.137	,	 +11.576		// Munich, Germany
#define LOCATION_OSLO			 +59.910	,	 +10.750		// Oslo, Norway
#define LOCATION_HAMBURG		 +53.551	,	  +9.993		// Hamburg, Germany
#define LOCATION_FRANKFURTAM     +50.111	,	  +8.682		// Frankfurt a.M., Germany
#define LOCATION_COLOGNE         +51.025	,	  +7.040		// Cologne, Germany
#define LOCATION_AMSTERDAM       +52.370	, 	  +4.890		// Amsterdam, Netherlands
#define LOCATION_BRUSSELS		 +50.846	,	  +4.352		// Brussels, Belgium
#define LOCATION_PARIS			 +48.850	,	  +2.350		// Paris, France

#define LOCATION_LONDON          +51.510	,	  -0.117		// London, GB
#define LOCATION_MADRID			 +40.413	, 	  -3.704		// Madrid, Spain
#define LOCATION_RABAT			 +34.025	, 	  -6.836		// Rabat, Morocco
#define LOCATION_LISBON			 +38.716	, 	  -9.167		// Lisbon, Portugal
#define LOCATION_REYKJAVIK		 +64.150	, 	 -21.933		// Reykjavik, Iceland
#define LOCATION_NUUK			 +64.180	, 	 -51.720		// Nuuk, Greenland
#define LOCATION_NEWYORK		 +40.712	,    -74.005		// New York, USA
#define LOCATION_OTTAWA			 +45.411	, 	 -75.699		// Ottawa, Canada
#define LOCATION_WASHINGTONDC	 +38.895	, 	 -77.037		// Washington D.C., USA
#define LOCATION_TORONTO		 +43.661	,	 -79.383		// Toronto, Canada
#define LOCATION_AUSTINTX		 +30.267	, 	 -97.764		// Austin, Texas, USA
#define LOCATION_MILWAUKEE		 +43.038	,	 -87.899		// Milwaukee, Wisconsin, USA
#define LOCATION_MEXICOCITY		 +19.419	, 	 -99.146		// Mexico City, Mexico
#define	LOCATION_REGINA			 +50.447	,	-104.618		// Regina, Canada
#define LOCATION_SANFRANCISCO	 +37.779	,	-122.419		// San Francisco, USA
#define LOCATION_VANCOUVER		 +49.280	,	-123.122		// Vancouver, Canada

#define LOCATION_SYDNEY			 -33.850	, 	+151.200		// Sydney, Australia
#define LOCATION_JOHANNESBURG	 -26.200	, 	 +28.067		// Johannesburg, South Africa
#define LOCATION_CAPETOWN		 -33.923	,	 +18.417		// Capetown, South Africa

#define LOCATION_SAOPAULO		 -23.500	, 	 -46.617		// Sao Paulo, Brasilia
#define LOCATION_BUENOSAIRES	 -34.600	, 	 -58.381		// Buenos Aires, Argentina
#define LOCATION_LIMA			 -12.035	,	 -77.019		// Lima, Peru


#define TZ_NZST +12    // new zealand standard time
#define TZ_NFT  +11.5  // norfolk (island) time
#define TZ_AEST +10    // australian eastern standard time
#define TZ_ACST  +9.5  // australian central standard time
#define TZ_JST   +9    // japan/korea standard time
#define TZ_CNST  +8    // china standard time
#define TZ_ICT   +7    // indochina time
#define TZ_IST   +5.5  // indian standard time
#define TZ_USZ4  +5    // ural time
#define TZ_GST   +4    // gulf standard time
#define TZ_USZ3  +4    // volga time
#define TZ_MSK   +3    // moscow time
#define TZ_BT    +3    // baghdad time
#define TZ_IRT   +2    // iran time
#define TZ_EET   +2    // eastern europe time
#define TZ_CAT   +2    // central africa time
#define TZ_SAST  +2    // south africa standard time
#define TZ_CET   +1    // central europe time
#define TZ_WET    0    // western europe time
#define TZ_UTC    0    // coordinated universal time (~GMT)
#define TZ_NST   -3.5  // newfoundland standard time
#define TZ_VST   -4.5  // venezuelan standard time
#define TZ_AST	 -4    // atlantic standard time, USA
#define TZ_EST   -5    // eastern standard time, USA
#define TZ_CST	 -6    // central standard time, USA
#define TZ_MST	 -7    // mountain standard time, USA
#define TZ_PST	 -8    // pacific standard time, USA
#define TZ_AKST  -9    // alaska standard time
#define TZ_HAST -10    // hawaii-aleutian standard time
#define TZ_IDLW -12    // international date line west

/* The trigonometric functions in degrees */

#define sind(x)  sin((x)*DEGRAD)
#define cosd(x)  cos((x)*DEGRAD)
#define tand(x)  tan((x)*DEGRAD)

#define atand(x)    (RADEG*atan(x))
#define asind(x)    (RADEG*asin(x))
#define acosd(x)    (RADEG*acos(x))
#define atan2d(y,x) (RADEG*atan2(y,x))
#define round(x)  floor(x + 0.5)

class ofxSolar{

public:
	
	void init(double,double,int);
	void update();
	double dayLength();
	double civilTwilightDayLength();
	double nauticalTwilightDayLength();
	double astronomicalTwilightDayLength();
	double sunrise();
	double sunset();
	double civilTwilightStart();
	double nauticalTwilightStart();
	double astronomicalTwilightStart();
	double civilTwilightEnd();
	double nauticalTwilightEnd();
	double astronomicalTwilightEnd();
	void setDaylightSaving(int);
	string decimalday_to_timestamp(double);
	void printInfo();

private:

	void calculateSunMap();

	double dayLength( int year, int month, int day, double lon, double lat, double altit, int upper_limb );

	int sunriset( int year, int month, int day, double lon, double lat, double altit, int upper_limb, double *rise, double *set );

	void sunpos( double d, double *lon, double *r );

	void sun_RA_dec( double d, double *RA, double *dec, double *r );

	double revolution( double x );

	double rev180( double x );

	double GMST0( double d );

	int year,month,day;
	double lon, lat;
	double dayleng, civlen, nautlen, astrlen;
	double rise, set, civ_start, civ_end, naut_start, naut_end,
		astr_start, astr_end;
	int    rs, civ, naut, astr, dls, tz;

	bool sunMapped;

};
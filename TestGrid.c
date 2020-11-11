/*
 ***********************************************************
 *       FILENAME: TestGrid.c                              *
 *                                                         *
 *       PURPOSE:  To test HDF-EOS v.3 Grid interface      *
 *                                                         *
 *       Author:                                           *
 *           A. Muslimov       /Emergent IT Inc.           *
 *           Adura Adekunjo    /L3 Comm. EER corp          *
 *           Shen Zhao         L-3 Comm. GSI               *
 *                                                         *
 *       History:                                          *
 *        August 1999  AM   Initial version                *
 *        11-Apr-2001  AM   Updated                        *
 *        02-Dec-2004  AA   Updated with testdrivers for   *
 *                          some new functions (NCR 41197) *
 *        April  2005  SZ   Added the EASE grid test case  *
 *                                                         *
 *                                                         *
 ***********************************************************
 */

#include  <HE5_HdfEosDef.h>
#include  <tutils.h>

#define BUFSIZE  256

int main(int argc, char *argv[])
{
   FILE       *fp;

   herr_t     status        = FAIL;

   hid_t      gdfidc_simple = FAIL;
   hid_t      GDid_UTM3     = FAIL;
   hid_t      GDid_UTM      = FAIL;
   hid_t      GDid_GEO      = FAIL;
   hid_t      GDid_POLAR_N  = FAIL;
   hid_t      GDid_POLAR_S  = FAIL;
   hid_t      GDid_IGOODE   = FAIL;
   hid_t      GDid_SOM      = FAIL;
   hid_t      GDid_LAMAZ    = FAIL;
   hid_t      GDid_HOM      = FAIL;
   hid_t      GDid_LAMCON   = FAIL;
   hid_t      GDid_TM       = FAIL;
   hid_t      GDid_POLY     = FAIL;
   hid_t      GDid_IS       = FAIL;
   hid_t      GDid_MERCAT   = FAIL;
   hid_t      GDid_ALBERS   = FAIL;
   hid_t      RegionID      = FAIL;
   hid_t      PeriodID      = FAIL;
   hid_t      NewRegionID   = FAIL;
   hid_t      datatype      = FAIL;
   hid_t       dtype        = FAIL;
   hid_t      return_val    = FAIL;

   int        i, j;
   int        Verbosity     = 4;
   int        errcnt        = 0;
   int        ZoneCode;
   int        SphereCode;
   int        proj_code     = FAIL;
   int        z_code        = FAIL;
   int        s_code        = FAIL;
   int        origin_code   = FAIL;
   int        pixreg_code   = FAIL;
   int        fieldgroup    = FAIL;

   int        *rank;
   int        Rank          = FAIL;
   int        tilerank, tilecode;
   int        compcode      = 4;
   int        compparm[5]   = { 0, 0, 0, 0, 0};
   int        Attr[4];
   int        grpattr[3]    = {3,7,11};
   int        gattrvalOut[3]   = {0,0,0};
   int        Attr_out[4]   = {-1,-1,-1,-1};
   int        ndim          = FAIL; 
   int        nflds         = FAIL;

   hid_t      *ntype1;
   hid_t      *ntype;
   H5T_class_t  classid    = H5T_NO_CLASS;
   H5T_order_t  order      = H5T_ORDER_ERROR;


   long       list_size = FAIL;   
   long        strbufsize      = FAIL;
   long       *RowVal;
   long       *ColVal;
   long       BufSize   = FAIL;
   long       NumPix;
   long       index     = 0;
   long       xdim      = 0;
   long       ydim      = 0;
   long       number    = FAIL;
   
   float      attr[4] = {1.1,2.2,3.3,4.4}; 
   float      attrvalOut[4] = { 0, 0, 0, 0};
   float      fillvalue     = -1.11111;
   float      fillvalue_inq = -1.111111;
   float      cnt_UTM       = -799.0;
   float      cnt_LAMCON    = -134.5;
   float      cnt_time      = 35232487.2;
   float      *Buffer_UTM;
   float      *Buffer_LAMCON;
   float      *Buffer_mercat;
   float      *time;
   float      *DatBuf;
   float      *VoltDataOut;

   float      veg[200][120];
   
   double     uplft[2]      = {0, 0};
   double     lowrgt[2]     = {0, 0};
   double     *ProjParm;
   double     uplft_out[2]  = {-1., -1.};
   double     lowrgt_out[2] = {-1., -1.};
   double     *ProjParm_out;   
   double     CorLon[2];
   double     CorLat[2];
   double     StartTime;
   double     StopTime;
   double     LonVal[5];
   double     LatVal[5];
   double     *DatBuf2;
   
   char       *str_buf;
   char       *str_buf2;
   char       *ObjNamesPtr;

         
   hssize_t   start[2]  = { 0, 0 };

   hsize_t    ndims          = 0;  
   hsize_t    CurrentSize[1] = { 0 };
   hsize_t    stride[2]      = { 1, 1 };
   hsize_t    edge[2]        = {1, 1 };
   hsize_t    dims[8]        = {0, 0, 0, 0, 0, 0, 0, 0 };
   hsize_t    tiledims[8];
   hsize_t    count;
   hsize_t     *CountPtr       = (hsize_t *)NULL;
   hsize_t    Gsize[1]       = { 3 }; 

   size_t       size       = 0;

   hid_t      GDid_EASE      = FAIL;
   long       index1         = 0;
   long       index2         = 0;
   long       xdim_ease      = 1383; 
   long       ydim_ease      = 586;
   float      easecnt        = 2001.0;
   /*float      easeray[1383][586];
     float      easeray2[1383][586];*/
   float	*easeray;
   float	*easeray2;
   double     rcord[4], scord[4];
   int        npnts;
   long       xdimsize,ydimsize;
   double     longitude[4], latitude[4];
 


   /* 
	********************************************************************************
	*                 Get comand-line argument (Verbosity)                         *
	********************************************************************************
	*/
   
   
   for (i = 1; i < argc; i++)
	 {
	   if ((argc > i + 1) && ((HDstrcmp(argv[i], "-verbose") == 0) ||
								   (HDstrcmp(argv[i], "-v") == 0)))
		 {
		   if (argv[i + 1][0] == 'l')
			 Verbosity = 4;
		   else if (argv[i + 1][0] == 'm')
			 Verbosity = 6;
		   else if (argv[i + 1][0] == 'h')
			 Verbosity = 10;
		   else
			 Verbosity = atoi(argv[i + 1]);
		 } 
	 }
   



   /* Allocate memory for arrays */   
   
   Buffer_UTM     = (float *)calloc( 4800, sizeof(float) );
   Buffer_LAMCON  = (float *)calloc( 60021, sizeof(float) );
   Buffer_mercat  = (float *)calloc( 60021, sizeof(float) );
   time           = (float *)calloc( 4800, sizeof(float) );
   str_buf        = (char  *)calloc( 256, sizeof(char ) );
   str_buf2       = (char  *)calloc( 256, sizeof(char ) );
   
   
   ProjParm_out = (double *)calloc( 16, sizeof(double) );
   for ( i = 0; i < 16; i++)
	 {
       ProjParm_out[ i ] = -0.99999;
	 }
   
   ProjParm = (double *)calloc( 16, sizeof(double) );
   
   
   /* Populate arrays */
   
   while( index < 4800 ) 
	 {
	   Buffer_UTM[ index ] = cnt_UTM;
	   cnt_UTM += (float)0.4;
	   index++;
	 }
   
   
   index = 0;
   
   while( index < 60021 ) 
	 {
	   Buffer_LAMCON[ index ] = cnt_LAMCON;
	   cnt_LAMCON += 0.04;
	   index++;
	 }
   
   index = 0;
   
   while( index < 60021 ) 
	 {
	   Buffer_mercat[ index ] = cnt_LAMCON;
	   cnt_LAMCON += 0.2;
	   index++;
	 }
   
   index = 0;
   
   while(index < 4800)
	 {
	   time[ index ] = cnt_time;
	   cnt_time += 70.7;
	   index++;
	 }
   
   /*   index1 = 0;
   index2 = 0;

   while(index1 < xdim_ease) {
      while(index2 < ydim_ease) {
         easeray[index1][index2] = easecnt;
         index2++;
         easecnt = easecnt + 0.2;
      }
      index1++;
      index2 = 0;
   }

   index1 = 0;
   index2 = 0;

   while(index1 < xdim_ease) {
      while(index2 < ydim_ease) {
         easeray2[index1][index2] = easecnt;
         index2++;
         easecnt = easecnt - .9;
      }
      index1++;
      index2 = 0;
      }*/

   index1=0;
   index2=0;
   xdim_ease =1383;
   ydim_ease = 586;

   easeray =  (float *) malloc(1383 * 586 * sizeof(float));

   for (index1=0; index1<xdim_ease; index1++)
     {
       for (index2=0; index2<ydim_ease; index2++)
	 {
	   easeray[index1+index2*xdim_ease] = easecnt;
	   easecnt = easecnt + 0.2;
	 }
     }

  easeray2 =  (float *) malloc(1383 * (586 * sizeof(float)));

  for (index1=0; index1<xdim_ease; index1++)
     {
       for (index2=0; index2<ydim_ease; index2++)
	 {
	   easeray2[index1+index2*xdim_ease] = easecnt;
	   easecnt = easecnt - 0.9;
	 }
     }



   /* Fill veg array */
   for (i = 0; i < 200; i++)
	 for (j = 0; j < 120; j++)
	   veg[i][j] = (float)(10+i);
   
   

   MESSAGE(11,fp = fopen("TestGrid_c.txtout","w"););
   MESSAGE(4,printf("Testing Grid C interface \n"););
   MESSAGE(4,printf("=========================\n\n"););
   MESSAGE(11,fprintf(fp,"Testing Grid C interface \n"););
   MESSAGE(11,fprintf(fp,"=========================\n\n"););
   
   /*
	*********************************************************************************
	*                             Testing    HE5_GDopen                             *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDopen... \n"););
   MESSAGE(4,printf("\t=====================\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDopen... \n"););
   MESSAGE(11,fprintf(fp,"\t=====================\n"););
   
   gdfidc_simple = HE5_GDopen("SimpleGrid.he5", H5F_ACC_TRUNC);
   if (gdfidc_simple == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n");); 
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully opened the grid file ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully opened the grid file ****\n"););
	 }
   
   /*
	*********************************************************************************
	*                             Testing    HE5_GDcreate                           *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDcreate... \n"););
   MESSAGE(4,printf("\t=======================\n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDcreate... \n"););   
   MESSAGE(11,fprintf(fp,"\t=======================\n"););
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid UTM3 \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid UTM3 \n"););
   
   xdim      = 60;
   ydim      = 80;
   
   uplft[0]  = -512740.28306;
   uplft[1]  = 2733747.62890;
   
   lowrgt[0] = -12584.57301;
   lowrgt[1] = 1946984.64021;
   
   /* The UTM3 grid will have field with unlimited dimension */
   GDid_UTM3 = HE5_GDcreate(gdfidc_simple, "UTM3", xdim, ydim, uplft, lowrgt);
   if (GDid_UTM3 == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid UTM3 ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid UTM3 ****\n"););
       
	 }
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid UTM \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid UTM \n"););
   
   
   GDid_UTM = HE5_GDcreate(gdfidc_simple, "UTM", xdim, ydim, uplft, lowrgt);
   if (GDid_UTM == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid UTM ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid UTM ****\n"););
       
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid Geo \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid Geo \n"););
   
   
   uplft[0]  = - 126000000.00;
   uplft[1]  = - 64000000.00;
   
   lowrgt[0] = -120000000.00;
   lowrgt[1] = -72000000.00;
   
   GDid_GEO = HE5_GDcreate(gdfidc_simple, "Geo", xdim, ydim, uplft, lowrgt);
   if (GDid_GEO == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid Geo ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid Geo ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););
   MESSAGE(6,printf("\t\tCreating grid EASE \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid EASE \n"););
 

   xdim = xdim_ease;
   ydim = ydim_ease;

   uplft[0] = HE5_EHconvAng(-180.0, HE5_HDFE_DEG_DMS);
   uplft[1] = HE5_EHconvAng(86.72, HE5_HDFE_DEG_DMS);

   lowrgt[0] = HE5_EHconvAng(180.0,HE5_HDFE_DEG_DMS);
   lowrgt[1] = HE5_EHconvAng(-86.72,HE5_HDFE_DEG_DMS);


   GDid_EASE = HE5_GDcreate(gdfidc_simple, "Ease", xdim, ydim, uplft, lowrgt);
   if (GDid_EASE == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
       MESSAGE(4,printf("\t\t*** Successfully created the grid EASE ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid EASE ****\n"););
         }


   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid Polar_np \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid Polar_np \n"););
   
   
   xdim      = 360;
   ydim      = 90;
   
   uplft[0]  = -10447125.82759; 
   uplft[1]  = 10447125.82759;  
   
   lowrgt[0] = 10447125.82759;  
   lowrgt[1] = -10447125.82759; 
   
   
   GDid_POLAR_N = HE5_GDcreate(gdfidc_simple, "Polar_np", xdim, ydim, uplft, lowrgt);
   if (GDid_POLAR_N == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid Polar_np ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid Polar_np ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid Polar_sp \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid Polar_sp \n"););
   
   
   xdim      = 360;
   ydim      = 90;
   
   uplft[0]  = 10447125.82759;
   uplft[1]  = -10447125.82759;
   
   lowrgt[0] = -10447125.82759;
   lowrgt[1] = 10447125.82759;
   
   GDid_POLAR_S = HE5_GDcreate(gdfidc_simple, "Polar_sp", xdim, ydim, uplft, lowrgt);
   if (GDid_POLAR_S == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid Polar_sp ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid Polar_sp ****\n"););
	 }
   
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid IGoode \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid IGoode \n"););
   
   xdim      = 120;
   ydim      = 60;
   
   uplft[0]  = -11119487.42844;
   uplft[1]  = 8673539.24806;
   
   lowrgt[0] = 15567282.39984;
   lowrgt[1] = -8673539.24806;
   
   GDid_IGOODE = HE5_GDcreate(gdfidc_simple, "IGoode", xdim, ydim, uplft, lowrgt);
   if (GDid_IGOODE == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid IGoode ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid IGoode ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid SOM \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid SOM \n"););
   
   xdim      = 359;
   ydim      = 321;
   
   uplft[0]  = 49844710.48057;
   uplft[1]  = 884884.39883;
   
   lowrgt[0] = 30521379.68485;
   lowrgt[1] = 1152027.64253;
   
   GDid_SOM = HE5_GDcreate(gdfidc_simple, "SOM", xdim, ydim, uplft, lowrgt);
   if (GDid_SOM == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid SOM ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid SOM ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid Lamaz \n"););   
   MESSAGE(11,fprintf(fp,"\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid Lamaz \n"););
   
   
   xdim      = 719;
   ydim      = 180;
   
   uplft[0]  = 0.0000;
   uplft[1]  = 9009950.36324;
   
   lowrgt[0] = 0.0000;
   lowrgt[1] = -9009950.36324;
   
   GDid_LAMAZ = HE5_GDcreate(gdfidc_simple, "Lamaz", xdim, ydim, uplft, lowrgt);
   if (GDid_LAMAZ == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid Lamaz ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid Lamaz ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid hom \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid hom \n"););
   
   xdim      = 200;
   ydim      = 90;
   
   uplft[0]  = 3422259.57265;
   uplft[1]  = 6824822.05796;
   
   lowrgt[0] = -17519429.48100;
   lowrgt[1] = 4994368.88166;
   
   GDid_HOM = HE5_GDcreate(gdfidc_simple, "hom", xdim, ydim, uplft, lowrgt);
   if (GDid_HOM == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid hom ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid hom ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid Lamcon \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid Lamcon \n"););
   
   
   xdim      = 351;
   ydim      = 171;
   
   uplft[0]  = -2279109.37671;
   uplft[1]  = 12358083.24054;
   
   lowrgt[0] = -56342817.96247;
   lowrgt[1] = -24776979.34092;
   
   GDid_LAMCON = HE5_GDcreate(gdfidc_simple, "lamcon", xdim, ydim, uplft, lowrgt);
   if (GDid_LAMCON == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid Lamcon ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid Lamcon ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid tm \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid tm \n"););
   
   
   xdim      = 181;
   ydim      = 171;
   
   uplft[0]  = 4855670.77539;
   uplft[1]  = 9458558.92483;
   
   lowrgt[0] = 5201746.43983;
   lowrgt[1] = -10466077.24942;
   
   GDid_TM = HE5_GDcreate(gdfidc_simple, "tm", xdim, ydim, uplft, lowrgt);
   if (GDid_TM == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid tm ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid tm ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid poly \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid poly \n"););
   
   xdim      = 161;
   ydim      = 171;
   
   uplft[0]  = -250873.85859;
   uplft[1]  = 12669051.66767;
   
   lowrgt[0] = 850873.85859;
   lowrgt[1] = -7137259.12615;
   
   GDid_POLY = HE5_GDcreate(gdfidc_simple, "poly", xdim, ydim, uplft, lowrgt);
   if (GDid_POLY == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid poly ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid poly ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid IS \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid IS \n"););
   
   
   xdim      = 351;
   ydim      = 171;
   
   uplft[0]  = 1436267.12618;
   uplft[1]  = 9451564.31420;
   
   lowrgt[0] = 1343604.73094;
   lowrgt[1] = -9451564.31420;
   
   GDid_IS = HE5_GDcreate(gdfidc_simple, "is", xdim, ydim, uplft, lowrgt);
   if (GDid_IS == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid IS ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid IS ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid MercatGrid \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid MercatGrid \n"););
   
   xdim      = 351;
   ydim      = 171;
   
   uplft[0]  = -14944317.52891;
   uplft[1]  =  15287878.19573;
   
   lowrgt[0] =  14944317.52891;
   lowrgt[1] = -15287878.19573;
   
   GDid_MERCAT = HE5_GDcreate(gdfidc_simple, "MercatGrid", xdim, ydim, uplft, lowrgt);
   if (GDid_MERCAT == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid MercatGrid ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid MercatGrid ****\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tCreating grid AlbersGrid \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tCreating grid AlbersGrid \n"););
   
   xdim      = 351;
   ydim      = 171;
   
   uplft[0]  = -4801368.79521;
   uplft[1]  = 10158566.00882;
   
   lowrgt[0] = -15565960.14252;
   lowrgt[1] = 3058978.10208;
   
   GDid_ALBERS = HE5_GDcreate(gdfidc_simple, "AlbersGrid", xdim, ydim, uplft, lowrgt);
   if (GDid_ALBERS == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););   
       errcnt++;
	 }
   else
	 {
       MESSAGE(4,printf("\t\t*** Successfully created the grid AlbersGrid ****\n"););
       MESSAGE(11,fprintf(fp,"\t\t*** Successfully created the grid AlbersGrid ****\n"););
	 }
   
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdefproj                           *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDdefproj... \n"););
   MESSAGE(4,printf("\t========================\n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdefproj... \n"););   
   MESSAGE(11,fprintf(fp,"\t========================\n"););
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining projection for the grid UTM  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid UTM  \n"););
   
   ZoneCode   = -13;
   SphereCode = 0;
   
   for (i = 0; i < 16; i++)
	 {
       ProjParm[i] = 0.0;
	 }
   
   status = HE5_GDdefproj(GDid_UTM, HE5_GCTP_UTM, ZoneCode, SphereCode, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_UTM, HE5_GCTP_UTM, ZoneCode, SphereCode, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_UTM, HE5_GCTP_UTM, ZoneCode, SphereCode, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
      status = HE5_GDdefproj(GDid_UTM3, HE5_GCTP_UTM, ZoneCode, SphereCode, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_UTM3, HE5_GCTP_UTM, ZoneCode, SphereCode, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_UTM3, HE5_GCTP_UTM, ZoneCode, SphereCode, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid Polar_np  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid Polar_np  \n"););
   
   
   SphereCode    = 0;
   ProjParm[ 5 ] = 40000000.0;
   
   status = HE5_GDdefproj(GDid_POLAR_N, HE5_GCTP_PS, 0, SphereCode, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_POLAR_N, HE5_GCTP_PS, 0, SphereCode, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_POLAR_N, HE5_GCTP_PS, 0, SphereCode, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid IGoode  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid IGoode  \n"););
   
   for ( i = 0; i < 16; i++)
	 {
       ProjParm[ i ] = 0.0 ;
	 }
   
   
   status = HE5_GDdefproj(GDid_IGOODE, HE5_GCTP_GOOD, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_IGOODE, HE5_GCTP_GOOD, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_IGOODE, HE5_GCTP_GOOD, 0, 0, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid Lamaz  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid Lamaz  \n"););
   
   
   status = HE5_GDdefproj(GDid_LAMAZ, HE5_GCTP_LAMAZ, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_LAMAZ, HE5_GCTP_LAMAZ, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_LAMAZ, HE5_GCTP_LAMAZ, 0, 0, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n");); 
   MESSAGE(6,printf("\t\tDefining projection for the grid SOM  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid SOM  \n"););
   
   
   ProjParm[ 2 ]  = 3;
   ProjParm[ 3 ]  = 150;
   ProjParm[ 12 ] = 1;
   
   
   status = HE5_GDdefproj(GDid_SOM, HE5_GCTP_SOM, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_SOM, HE5_GCTP_SOM, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_SOM, HE5_GCTP_SOM, 0, 0, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   

   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid Geo  \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid Geo  \n"););
   
   
   status = HE5_GDdefproj(GDid_GEO, HE5_GCTP_GEO, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_GEO, HE5_GCTP_GEO, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_GEO, HE5_GCTP_GEO, 0, 0, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }


   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid EASE  \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid EASE  \n"););
 
 
   for ( i = 0; i < 16; i++)
         {
       ProjParm[ i ] = 0.0 ;
         }

   ProjParm[ 5 ]  = 30000000.00;
 
   status = HE5_GDdefproj(GDid_EASE, HE5_GCTP_BCEA, 0, 0, ProjParm);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_EASE, HE5_GCTP_BCEA, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_EASE, HE5_GCTP_BCEA, 0, 0, ProjParm);\n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
         }

   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid hom  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid hom  \n"););
   
   for ( i = 0; i < 16; i++)
	 {
       ProjParm[ i ] = 0.0 ;
	 }
   
   ProjParm[ 2 ]   = 0.9996;
   ProjParm[ 5 ]   = 20000000.00;
   ProjParm[ 8 ]   = -75000000.00;
   ProjParm[ 9 ]   = 10000000.00;
   ProjParm[ 10 ]  = -95000000.00;
   ProjParm[ 11 ]  = 30000000.00;
   
   
   status = HE5_GDdefproj(GDid_HOM, HE5_GCTP_HOM, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_HOM, HE5_GCTP_HOM, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_HOM, HE5_GCTP_HOM, 0, 0, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid Lamcon  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid Lamcon  \n"););
   
   for ( i = 0; i < 16; i++)
	 {
       ProjParm[ i ] = 0.0 ;
	 }
   
   ProjParm[ 2 ]   = 20000000.;
   ProjParm[ 3 ]   = 40000000.;
   ProjParm[ 4 ]   = -75000000.;
   
   status = HE5_GDdefproj(GDid_LAMCON, HE5_GCTP_LAMCC, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_LAMCON, HE5_GCTP_LAMCC, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_LAMCON, HE5_GCTP_LAMCC, 0, 0, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid tm  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid tm  \n"););
   
   for ( i = 0; i < 16; i++)
	 {
       ProjParm[ i ] = 0.0 ;
	 }
   
   ProjParm[ 2 ]   = 0.9996;
   ProjParm[ 4 ]   = -75000000.00;
   ProjParm[ 6 ]   = 5000000.00;
   
   status = HE5_GDdefproj(GDid_TM, HE5_GCTP_TM, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_TM, HE5_GCTP_TM, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_TM, HE5_GCTP_TM, 0, 0, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid poly  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid poly  \n"););
   
   for ( i = 0; i < 16; i++)
	 {
       ProjParm[ i ] = 0.0 ;
	 }
   
   ProjParm[ 4 ]   = 75000000.00;
   ProjParm[ 5 ]   = -25000000.00;
   ProjParm[ 6 ]   = 300000.00;
   
   status = HE5_GDdefproj(GDid_POLY, HE5_GCTP_POLYC, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_POLY, HE5_GCTP_POLYC, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_POLY, HE5_GCTP_POLYC, 0, 0, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid IS  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid IS \n"););
   
   
   for ( i = 0; i < 16; i++)
	 {
       ProjParm[ i ] = 0.0 ;
	 }
   
   ProjParm[ 4 ]   = 0.00;
   ProjParm[ 5 ]   = 40000000.00;
   
   status = HE5_GDdefproj(GDid_IS, HE5_GCTP_ISINUS, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_IS, HE5_GCTP_ISINUS, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_IS, HE5_GCTP_ISINUS, 0, 0, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid MercatGrid  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid MercatGrid \n"););
   
   
   for ( i = 0; i < 16; i++)
	 {
       ProjParm[ i ] = 0.0 ;
	 }
   
   ProjParm[ 4 ]   = 00000000.00;
   ProjParm[ 5 ]   = 40000000.00;
   
   SphereCode = 0;
   
   status = HE5_GDdefproj(GDid_MERCAT, HE5_GCTP_MERCAT, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_MERCAT, HE5_GCTP_MERCAT, 0, 0, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_MERCAT, HE5_GCTP_MERCAT, 0, 0, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining projection for the grid AlbersGrid  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining projection for the grid AlbersGrid \n"););
   
   
   for ( i = 0; i < 16; i++)
	 {
       ProjParm[ i ] = 0.0 ;
	 }
   
   ProjParm[ 2 ]   = 20000000.00;
   ProjParm[ 3 ]   = 40000000.00;
   ProjParm[ 4 ]   = -75000000.00;
   
   
   SphereCode = 0;
   
   status = HE5_GDdefproj(GDid_ALBERS, HE5_GCTP_ALBERS, 0, 0, ProjParm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefproj(GDid_ALBERS, HE5_GCTP_ALBERS, NULL, NULL, ProjParm);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefproj()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefproj(GDid_ALBERS, HE5_GCTP_ALBERS, NULL, NULL, ProjParm);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefproj()          %d\n",status););
	 }
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdefpixreg                         *
	*********************************************************************************
	*/
   
   
   MESSAGE(4,printf("\tTesting HE5_GDdefpixreg... \n"););
   MESSAGE(4,printf("\t==========================\n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdefpixreg... \n"););   
   MESSAGE(11,fprintf(fp,"\t==========================\n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining pixel registration for the grid UTM  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel registration for the grid UTM \n"););
   
   status = HE5_GDdefpixreg( GDid_UTM, HE5_HDFE_CORNER);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefpixreg(GDid_UTM, HE5_HDFE_CORNER);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefpixreg()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefpixreg(GDid_UTM, HE5_HDFE_CORNER);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefpixreg()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel registration for the grid Polar_np \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel registration for the grid Polar_np \n"););
   
   status = HE5_GDdefpixreg( GDid_POLAR_N, HE5_HDFE_CORNER);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefpixreg(GDid_POLAR_N, HE5_HDFE_CORNER);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefpixreg()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefpixreg(GDid_POLAR_N, HE5_HDFE_CORNER);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefpixreg()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel registration for the grid IGoode \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel registration for the grid IGoode \n"););
   
   status = HE5_GDdefpixreg( GDid_IGOODE, HE5_HDFE_CORNER);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefpixreg(GDid_IGOODE, HE5_HDFE_CORNER);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefpixreg()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefpixreg(GDid_IGOODE, HE5_HDFE_CORNER);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefpixreg()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel registration for the grid SOM  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel registration for the grid SOM \n"););
   
   status = HE5_GDdefpixreg( GDid_SOM, HE5_HDFE_CORNER);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefpixreg(GDid_SOM, HE5_HDFE_CORNER);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefpixreg()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefpixreg(GDid_SOM, HE5_HDFE_CORNER);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefpixreg()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel registration for the grid Lamcon \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel registration for the grid Lamcon \n"););
   
   status = HE5_GDdefpixreg( GDid_LAMCON, HE5_HDFE_CORNER);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefpixreg(GDid_LAMCON, HE5_HDFE_CORNER);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefpixreg()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefpixreg(GDid_LAMCON, HE5_HDFE_CORNER);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefpixreg()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel registration for the grid Geo  \n"););  
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel registration for the grid Geo \n"););
   
   status = HE5_GDdefpixreg( GDid_GEO, HE5_HDFE_CORNER);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefpixreg(GDid_GEO, HE5_HDFE_CORNER);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefpixreg()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefpixreg(GDid_GEO, HE5_HDFE_CORNER);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefpixreg()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel registration for the grid EASE \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel registration for the grid EASE \n"););
  
   status = HE5_GDdefpixreg(GDid_EASE, HE5_HDFE_CORNER);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefpixreg(GDid_EASE, HE5_HDFE_CORNER);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefpixreg()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefpixreg(GDid_EASE, HE5_HDFE_CORNER);\n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefpixreg()          %d\n",status););
         }

   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel registration for the grid MercatGrid  \n"););  
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel registration for the grid MercatGrid \n"););
   
   status = HE5_GDdefpixreg( GDid_MERCAT, HE5_HDFE_CORNER);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefpixreg(GDid_MERCAT, HE5_HDFE_CORNER);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefpixreg()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefpixreg(GDid_MERCAT, HE5_HDFE_CORNER);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefpixreg()          %d\n",status););
	 }
   
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel registration for the grid AlbersGrid  \n"););  
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel registration for the grid AlbersGrid \n"););
   
   status = HE5_GDdefpixreg( GDid_ALBERS, HE5_HDFE_CORNER);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefpixreg(GDid_ALBERS, HE5_HDFE_CORNER);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefpixreg()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefpixreg(GDid_ALBERS, HE5_HDFE_CORNER);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefpixreg()          %d\n",status););
	 }
   
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdeforigin                         *
	*********************************************************************************
	*/
   
   
   MESSAGE(4,printf("\tTesting HE5_GDdeforigin... \n"););
   MESSAGE(4,printf("\t==========================\n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdeforigin... \n"););   
   MESSAGE(11,fprintf(fp,"\t==========================\n"););
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining pixel origin for the grid UTM  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel origin for the grid UTM \n"););
   
   status = HE5_GDdeforigin(GDid_UTM, HE5_HDFE_GD_UL );
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeforigin(GDid_UTM, HE5_HDFE_GD_UL);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeforigin()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeforigin(GDid_UTM, HE5_HDFE_GD_UL);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeforigin()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel origin for the grid Polar_np  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel origin for the grid Polar_np \n"););
   
   status = HE5_GDdeforigin(GDid_POLAR_N, HE5_HDFE_GD_UR );
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeforigin(GDid_POLAR_N, HE5_HDFE_GD_UR);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeforigin()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeforigin(GDid_POLAR_N, HE5_HDFE_GD_UR);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeforigin()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel origin for the grid IGoode  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel origin for the grid IGoode \n"););
   
   status = HE5_GDdeforigin(GDid_IGOODE, HE5_HDFE_GD_LL );
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeforigin(GDid_IGOODE, HE5_HDFE_GD_LL);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeforigin()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeforigin(GDid_IGOODE, HE5_HDFE_GD_LL);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeforigin()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel origin for the grid SOM  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel origin for the grid SOM \n"););
   
   status = HE5_GDdeforigin(GDid_SOM, HE5_HDFE_GD_LR );
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeforigin(GDid_SOM, HE5_HDFE_GD_LR);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeforigin()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeforigin(GDid_SOM, HE5_HDFE_GD_LR);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeforigin()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel origin for the grid Lamcon \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel origin for the grid Lamcon \n"););
   
   status = HE5_GDdeforigin(GDid_LAMCON, HE5_HDFE_GD_UL );
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeforigin(GDid_LAMCON, HE5_HDFE_GD_UL);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeforigin()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeforigin(GDid_LAMCON, HE5_HDFE_GD_UL);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeforigin()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel origin for the grid Geo \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel origin for the grid Geo \n"););
   
   status = HE5_GDdeforigin(GDid_GEO, HE5_HDFE_GD_UR );
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeforigin(GDid_GEO, HE5_HDFE_GD_UR);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeforigin()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeforigin(GDid_GEO, HE5_HDFE_GD_UR);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeforigin()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel origin for the grid EASE \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel origin for the grid EASE \n"););
  
   status = HE5_GDdeforigin(GDid_EASE, HE5_HDFE_GD_UL);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {  
       MESSAGE(4,printf("\n"););  
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeforigin(GDid_EASE, HE5_HDFE_GD_UL);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeforigin()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeforigin(GDid_EASE, HE5_HDFE_GD_UL);\n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeforigin()          %d\n",status););
         }


   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel origin for the grid MercatGrid \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel origin for the grid MercatGrid \n"););
   
   status = HE5_GDdeforigin(GDid_MERCAT, HE5_HDFE_GD_UR );
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeforigin(GDid_MERCAT, HE5_HDFE_GD_UR);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeforigin()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeforigin(GDid_MERCAT, HE5_HDFE_GD_UR);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeforigin()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining pixel origin for the grid AlbersGrid \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining pixel origin for the grid AlbersGrid \n"););
   
   status = HE5_GDdeforigin(GDid_ALBERS, HE5_HDFE_GD_UR );
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeforigin(GDid_ALBERS, HE5_HDFE_GD_UR);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeforigin()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeforigin(GDid_ALBERS, HE5_HDFE_GD_UR);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeforigin()          %d\n",status););
	 }
   
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdefdim                            *
	*********************************************************************************
	*/
   
   
   MESSAGE(4,printf("\tTesting HE5_GDdefdim... \n"););
   MESSAGE(4,printf("\t=======================\n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdefdim... \n"););   
   MESSAGE(11,fprintf(fp,"\t=======================\n"););
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining dimensions for the grid UTM  \n"););   
   MESSAGE(11,fprintf(fp,"\n"););
   MESSAGE(11,fprintf(fp,"\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining dimensions for the grid UTM \n"););
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"Conduction\" for the grid UTM  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"Conduction\"  for the grid UTM \n"););
   
   
   status = HE5_GDdefdim(GDid_UTM, "Conduction",25);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_UTM, \"Conduction\", 25);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_UTM, \"Conduction\", 25);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"Timedim\" for the grid UTM  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"Timedim\"  for the grid UTM \n"););
   
   status = HE5_GDdefdim(GDid_UTM, "Timedim",4800);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_UTM, \"Timedim\", 4800);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_UTM, \"Timedim\", 4800);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"Hghtdim\" for the grid UTM  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"Hghtdim\"  for the grid UTM \n"););
   
   status = HE5_GDdefdim(GDid_UTM, "Hghtdim",80);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_UTM, \"Hghtdim\", 80);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_UTM, \"Hghtdim\", 80);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"Unlim\" for the grid UTM  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"Unlim\"  for the grid UTM \n"););
   
   
   status = HE5_GDdefdim(GDid_UTM3, "Unlim", H5S_UNLIMITED);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_UTM, \"Unlim\", H5S_UNLIMITED);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_UTM, \"Unlim\", H5S_UNLIMITED);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }
   
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"Convection\" for the grid Polar_np  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"Convection\"  for the grid Polar_np \n"););
   
   status = HE5_GDdefdim(GDid_POLAR_N, "Convection",75);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_POLAR_N, \"Convection\", 75);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_POLAR_N, \"Convection\", 75);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"Radiant\" for the grid IGoode  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"Radiant\"  for the grid IGoode \n"););
   
   status = HE5_GDdefdim(GDid_IGOODE, "Radiant",45);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_IGOODE, \"Radiant\", 45);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_IGOODE, \"Radiant\", 45);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"Emission\" for the grid SOM  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"Emission\"  for the grid SOM \n"););
   
   
   status = HE5_GDdefdim(GDid_SOM, "Emission",55);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_SOM, \"Emission\", 55);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_SOM, \"Emission\", 55);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"Flux\" for the grid Lamaz  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"Flux\"  for the grid Lamaz \n"););
   
   status = HE5_GDdefdim(GDid_LAMAZ, "Flux",65);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_LAMAZ, \"Flux\", 65);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_LAMAZ, \"Flux\", 65);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"Gradient\" for the grid Geo  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"Gradient\"  for the grid Geo \n"););
   
   status = HE5_GDdefdim(GDid_GEO, "Gradient",85);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_GEO, \"Gradient\", 85); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_GEO, \"Gradient\", 85); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }   
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"Width\" for the grid EASE  \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"Width\"  for the grid EASE \n"););
  
   status = HE5_GDdefdim(GDid_EASE, "Width", xdim_ease);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {  
       MESSAGE(4,printf("\n"););  
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_EASE, \"Width\", xdim_ease); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_EASE, \"Width\", xdim_ease); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
         }  


   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"VerticalGradient\" for the grid MercatGrid  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"VerticalGradient\"  for the grid MercatGrid \n"););
   
   status = HE5_GDdefdim(GDid_MERCAT, "VerticalGradient",85);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_MERCAT, \"VerticalGradient\", 85); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_MERCAT, \"VerticalGradient\", 85); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }   
   
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining dimension \"DammyGradient\" for the grid AlbersGrid  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining dimension \"DammyGradient\"  for the grid AlbersGrid \n"););
   
   status = HE5_GDdefdim(GDid_ALBERS, "DammyGradient",85);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_ALBERS, \"DammyGradient\", 85); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_ALBERS, \"DammyGradient\", 85); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
	 }   

    status = HE5_GDdefdim(GDid_UTM, "ExtDim",60);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefdim(GDid_UTM, \"ExtDim\", 60);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefdim()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefdim(GDid_UTM, \"ExtDim\", 60);\n"););
           MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefdim()          %d\n",status););
         }

   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdetach                            *
	*********************************************************************************
	*/
   
   
   MESSAGE(4,printf("\tTesting HE5_GDdetach... \n"););
   MESSAGE(4,printf("\t=======================\n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdetach... \n"););   
   MESSAGE(11,fprintf(fp,"\t=======================\n"););
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDetaching from the grid UTM3  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid UTM3  \n"););
   
   status = HE5_GDdetach(GDid_UTM3);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid UTM3 **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid UTM3 **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid UTM  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid UTM \n"););
   
   status = HE5_GDdetach(GDid_UTM);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid UTM **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid UTM **\n"););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid Geo  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid Geo \n"););
   
   status = HE5_GDdetach(GDid_GEO); 
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid Geo **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid Geo **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid Polar_np  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid Polar_np \n"););
   
   status = HE5_GDdetach(GDid_POLAR_N); 
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid Polar_np **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid Polar_np **\n"););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid Polar_sp  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid Polar_sp \n"););
   
   status = HE5_GDdetach(GDid_POLAR_S);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid Polar_sp **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid Polar_sp **\n"););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid IGoode  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid IGoode \n"););
   
   status = HE5_GDdetach(GDid_IGOODE); 
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid IGoode **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid IGoode **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid Som  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid Som \n"););
   
   status = HE5_GDdetach(GDid_SOM);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid Som **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid Som **\n"););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid Lamaz  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid Lamaz \n"););
   
   status = HE5_GDdetach(GDid_LAMAZ); 
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid Lamaz **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid Lamaz **\n"););
	 }
   

   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid EASE \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid EASE \n"););
  
   status = HE5_GDdetach(GDid_EASE);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t** Successfully detached from the grid EASE **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid EASE **\n"););
         }

   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid hom  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid hom \n"););
   
   status = HE5_GDdetach(GDid_HOM);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid hom **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid hom **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid Lamcon  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid Lamcon \n"););
   
   status = HE5_GDdetach(GDid_LAMCON);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid Lamcon **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid Lamcon **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid tm  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid tm \n"););
   
   status = HE5_GDdetach(GDid_TM);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid tm **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid tm **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid Poly  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid Poly \n"););
   
   status = HE5_GDdetach(GDid_POLY);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid Poly **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid Poly **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid IS  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid IS \n"););
   
   status = HE5_GDdetach(GDid_IS);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid IS **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid IS **\n"););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid MercatGrid \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid MercatGrid \n"););
   
   status = HE5_GDdetach(GDid_MERCAT);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid MercatGrid **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid MercatGrid **\n"););
	 }
   
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid AlbersGrid \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid AlbersGrid \n"););
   
   status = HE5_GDdetach(GDid_ALBERS);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid AlbersGrid **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid AlbersGrid **\n"););
	 }
   
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDattach                            *
	*********************************************************************************
	*/
   
   
   MESSAGE(4,printf("\tTesting HE5_GDattach... \n"););
   MESSAGE(4,printf("\t=======================\n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDattach... \n"););   
   MESSAGE(11,fprintf(fp,"\t=======================\n"););
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tAttaching to the grid UTM  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tAttaching to the grid UTM  \n"););
   
   GDid_UTM  = HE5_GDattach(gdfidc_simple,"UTM"); 
   if (GDid_UTM == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully attached to the grid UTM **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid UTM **\n"););
	 }

   GDid_UTM3  = HE5_GDattach(gdfidc_simple,"UTM3"); 
   if (GDid_UTM == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully attached to the grid UTM3 **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid UTM3 **\n"););
	 }
   
   
   MESSAGE(6,printf("\n"););
   MESSAGE(6,printf("\t\tAttaching to the grid EASE  \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tAttaching to the grid EASE  \n"););
  
   GDid_EASE  = HE5_GDattach(gdfidc_simple,"Ease");
   if (GDid_EASE == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t** Successfully attached to the grid EASE **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid EASE **\n"););
         }


   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tAttaching to the grid MercatGrid  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tAttaching to the grid MercatGrid  \n"););
   
   GDid_MERCAT  = HE5_GDattach(gdfidc_simple,"MercatGrid"); 
   if (GDid_MERCAT == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully attached to the grid MercatGrid **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid MercatGrid **\n"););
	 }
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tAttaching to the grid AlbersGrid  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tAttaching to the grid AlbersGrid  \n"););
   
   GDid_ALBERS  = HE5_GDattach(gdfidc_simple,"AlbersGrid"); 
   if (GDid_ALBERS == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully attached to the grid AlbersGrid **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid AlbersGrid **\n"););
	 }
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDsetfillvalue                      *
	*********************************************************************************
	*                                                                               *
	*       NOTE:  This call should always precede the call to GDdeffield()         * 
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDsetfillvalue... \n"););
   MESSAGE(4,printf("\t============================= \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDsetfillvalue... \n"););   
   MESSAGE(11,fprintf(fp,"\t============================= \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tSetting fill value for the field \"Voltage\" in grid UTM  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tSetting fill value for the field \"Voltage\" in grid UTM \n"););
   
   status = HE5_GDsetfillvalue(GDid_UTM, "Voltage", H5T_NATIVE_FLOAT, &fillvalue);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDsetfillvalue(GDid_UTM,\"Voltage\",H5T_NATIVE_FLOAT,&fillvalue); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDsetfillvalue()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDsetfillvalue(GDid_UTM,\"Voltage\",H5T_NATIVE_FLOAT,&fillvalue); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDsetfillvalue()          %d\n",status););
	 } 
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdeffield                          *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDdeffield... \n"););
   MESSAGE(4,printf("\t=========================\n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdeffield... \n"););   
   MESSAGE(11,fprintf(fp,"\t=========================\n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining the field \"Voltage\" for the grid UTM \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining the field \"Voltage\" for the grid UTM  \n"););
   
   status = HE5_GDdeffield(GDid_UTM,"Voltage","XDim,YDim",NULL,H5T_NATIVE_FLOAT,0);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeffield(GDid_UTM,\"Voltage\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeffield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeffield(GDid_UTM,\"Voltage\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeffield()          %d\n",status););
	 } 
   
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining the field \"Drift\" for the grid UTM \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\t Defining the field \"Drift\" for the grid UTM\n"););
   
   status = HE5_GDdeffield(GDid_UTM,"Drift","XDim,YDim",NULL,H5T_NATIVE_FLOAT,0);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeffield(GDid_UTM,\"Drift\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeffield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeffield(GDid_UTM,\"Drift\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeffield()          %d\n",status););
	 } 
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining the field \"VerticalD\" for the grid MercatGrid \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\t Defining the field \"VerticalD\" for the grid MercatGrid \n"););
   
   status = HE5_GDdeffield(GDid_MERCAT,"VerticalD","XDim,YDim",NULL,H5T_NATIVE_FLOAT,0);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeffield(GDid_MERCAT,\"VerticalD\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeffield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeffield(GDid_MERCAT,\"VerticalD\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeffield()          %d\n",status););
	 } 
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining the field \"Intensity\" for the grid EASE \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\t Defining the field \"Intensity\" for the grid EASE \n"););
  
   status = HE5_GDdeffield(GDid_EASE,"Intensity","XDim,YDim",NULL,H5T_NATIVE_FLOAT,0);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {  
       MESSAGE(4,printf("\n"););  
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeffield(GDid_EASE,\"Intensity\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeffield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeffield(GDid_EASE,\"Intensity\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeffield()          %d\n",status););
         }


   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining the field \"Intervals\" for the grid EASE \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Defining the field \"Intervals\" for the grid EASE \n"););
 
   status = HE5_GDdeffield(GDid_EASE,"Intervals","Width,YDim",NULL,H5T_NATIVE_FLOAT,0);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         { 
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeffield(GDid_EASE,\"Intervals\",\"Width,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeffield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeffield(GDid_EASE,\"Intervals\",\"Width,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeffield()          %d\n",status););
         }


   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining the field \"MaximumDepth\" for the grid AlbersGrid \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\t Defining the field \"MaximumDepth\" for the grid AlbersGrid \n"););
   
   status = HE5_GDdeffield(GDid_ALBERS,"MaximumDepth","XDim,YDim",NULL,H5T_NATIVE_FLOAT,0);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeffield(GDid_ALBERS,\"MaximumDepth\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeffield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeffield(GDid_ALBERS,\"MaximumDepth\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeffield()          %d\n",status););
	 } 
   
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining the field \"Time\" for the grid UTM \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\t Defining the field \"Time\" for the grid UTM\n"););
   
   status = HE5_GDdeffield(GDid_UTM,"Time","Timedim",NULL,H5T_NATIVE_FLOAT,0);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeffield(GDid_UTM,\"Time\",\"Timedim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeffield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeffield(GDid_UTM,\"Drift\",\"Timedim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeffield()          %d\n",status););
	 } 
         
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining the external data field \"ExtData\" for the grid UTM \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\t Defining the external data  field \"ExtData\" for the grid UTM\n"););
  
   status = HE5_GDdeffield(GDid_UTM,"ExtData","ExtDim",NULL,H5T_NATIVE_FLOAT,0);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
  else
         {  
       MESSAGE(4,printf("\n"););  
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeffield(GDid_UTM,\"ExtData\",\"ExtDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeffield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeffield(GDid_UTM,\"ExtData\",\"ExtDim\",NULL,H5T_NATIVE_FLOAT,0 ); \n"););
           MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeffield()          %d\n",status););
         }
 
   
   /*
	*********************************************************************************
	*                             Testing  Appendable field                         *
	*********************************************************************************
	*/
   
   tilerank    = 2;
   tiledims[0] = 10;
   tiledims[1] = 10;
   compparm[0] = 5;
   
   
   MESSAGE(4,printf("\tTesting HE5_GDdeftile... \n"););
   MESSAGE(4,printf("\t======================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdeftile... \n"););   
   MESSAGE(11,fprintf(fp,"\t======================== \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining tiling for the grid UTM \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining tiling for the grid UTM  \n"););
   
   status = HE5_GDdeftile(GDid_UTM, HE5_HDFE_TILE, tilerank, tiledims);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeftile(GDid_UTM, HE5_HDFE_TILE, tilerank, tiledims); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeftile()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeftile(GDid_UTM, HE5_HDFE_TILE, tilerank, tiledims); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeftile()          %d\n",status););
	 } 
   
      status = HE5_GDdeftile(GDid_UTM3, HE5_HDFE_TILE, tilerank, tiledims);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeftile(GDid_UTM3, HE5_HDFE_TILE, tilerank, tiledims); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeftile()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeftile(GDid_UTM3, HE5_HDFE_TILE, tilerank, tiledims); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeftile()          %d\n",status););
	 } 
   
   MESSAGE(4,printf("\tTesting HE5_GDdefcomp... \n"););
   MESSAGE(4,printf("\t======================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdefcomp... \n"););   
   MESSAGE(11,fprintf(fp,"\t======================== \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining compression for the grid UTM \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining compression for the grid UTM  \n"););
   
   status = HE5_GDdefcomp(GDid_UTM, compcode, compparm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefcomp(GDid_UTM, compcode, compparm); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefcomp()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefcomp(GDid_UTM, compcode, compparm); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefcomp()          %d\n",status););
	 } 
      MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining compression for the grid UTM3 \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining compression for the grid UTM3  \n"););
   
   status = HE5_GDdefcomp(GDid_UTM3, compcode, compparm);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdefcomp(GDid_UTM3, compcode, compparm); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdefcomp()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdefcomp(GDid_UTM3, compcode, compparm); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdefcomp()          %d\n",status););
	 } 
   
   MESSAGE(4,printf("\tTesting HE5_GDdeffield... \n"););
   MESSAGE(4,printf("\t=========================\n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdeffield... \n"););   
   MESSAGE(11,fprintf(fp,"\t=========================\n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining appendable field \"Vegetation\" for the grid UTM3 \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDefining appendable field \"Vegetation\" for the grid UTM3  \n"););
   
   status = HE5_GDdeffield(GDid_UTM3,"Vegetation","XDim,YDim","Unlim,Unlim",H5T_NATIVE_FLOAT,0);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeffield(GDid_UTM3,\"Vegetation\",\"XDim,YDim\",\"Unlim,Unlim\",H5T_NATIVE_FLOAT,0 ); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeffield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeffield(GDid_UTM3,\"Voltage\",\"XDim,YDim\",\"Unlim,Unlim\",H5T_NATIVE_FLOAT,0 ); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeffield()          %d\n",status););
	 } 
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid UTM3  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid UTM3 \n"););
   
   status = HE5_GDdetach(GDid_UTM3);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid UTM3 **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid UTM3 **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid UTM  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid UTM \n"););
   
   status = HE5_GDdetach(GDid_UTM);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid UTM **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid UTM **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tAttaching to the grid lamcon  \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tAttaching to the grid lamcon  \n"););
   
   GDid_LAMCON = HE5_GDattach(gdfidc_simple,"lamcon"); 
   if (GDid_LAMCON == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully attached to the grid lamcon **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid lamcon **\n"););
	 }
   
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDefining the field \"Bypass\" for the grid lamcon \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\t Defining the field \"Bypass\" for the grid lamcon\n"););
   
   status = HE5_GDdeffield(GDid_LAMCON,"Bypass","XDim,YDim",NULL,H5T_NATIVE_FLOAT,0);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDdeffield(GDid_LAMCON,\"Bypass\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDdeffield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDdeffield(GDid_LAMCON,\"Bypass\",\"XDim,YDim\",NULL,H5T_NATIVE_FLOAT,0); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDdeffield()          %d\n",status););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid Lamcon \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid Lamcon \n"););
   
   status = HE5_GDdetach(GDid_LAMCON);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid lamcon **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid lamcon **\n"););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tAttaching to the grid UTM \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tAttaching to the grid UTM \n"););
   
   GDid_UTM3  = HE5_GDattach(gdfidc_simple,"UTM3"); 
   if (GDid_UTM3 == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully attached to the grid UTM3 **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid UTM3 **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tAttaching to the grid UTM \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tAttaching to the grid UTM \n"););
   
   GDid_UTM  = HE5_GDattach(gdfidc_simple,"UTM"); 
   if (GDid_UTM == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully attached to the grid UTM **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid UTM **\n"););
	 }
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDwritefield                        *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDwritefield... \n"););
   MESSAGE(4,printf("\t=========================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDwritefield... \n"););   
   MESSAGE(11,fprintf(fp,"\t=========================== \n"););
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tWriting field \"Voltage\" to the grid UTM  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tWriting field \"Voltage\" to the grid UTM  \n"););
   
   
   start[0]  = 0;
   start[1]  = 0;
   stride[0] = 1;
   stride[1] = 1;
   edge[0]   = 50;
   edge[1]   = 80;
   
   status = HE5_GDwritefield(GDid_UTM, "Voltage", start, stride, edge, Buffer_UTM);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwritefield(GDid_UTM, \"Voltage\", start, stride, edge, Buffer_UTM); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritefield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritefield(GDid_UTM, \"Voltage\", start, stride, edge, Buffer_UTM); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritefield()          %d\n",status););
	 } 
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\t Writing field \"Drift\" to the grid UTM \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\t Writing field \"Drift\" to the grid UTM \n"););
   
   start[0] = 0;
   start[1] = 0;
   
   stride[0] = 1;
   stride[1] = 1;
   
   edge[0] = 60;
   edge[1] = 80;
   
   status = HE5_GDwritefield(GDid_UTM, "Drift",start, stride, edge, Buffer_UTM);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwritefield(GDid_UTM, \"Drift\", start, stride, edge, Buffer_UTM); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritefield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritefield(GDid_UTM, \"Drift\", start, stride, edge, Buffer_UTM); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritefield()          %d\n",status););
	 } 
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\t Writing field \"Intensity\" to the grid EASE \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Writing field \"Intensity\" to the grid EASE \n"););

   start[0] = 0;
   start[1] = 0;
   stride[0] = 1;
   stride[1] = 1;
   edge[0] = 1386;
   edge[1] = 586;
  
   status = HE5_GDwritefield(GDid_EASE, "Intensity", start, stride, edge, easeray);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {  
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwritefield(GDid_EASE, \"Intensity\", start, stride, edge, easeray); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritefield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritefield(GDid_EASE, \"Intensity\", start, stride, edge, easeray); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritefield()          %d\n",status););
         }


   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\t Writing field \"Intervals\" to the grid EASE \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Writing field \"Intervals\" to the grid EASE \n"););

   free(easeray);

   status = HE5_GDwritefield(GDid_EASE, "Intervals", start, stride, edge, easeray2);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         { 
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwritefield(GDid_EASE, \"Intervals\", start, stride, edge, easeray2); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritefield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritefield(GDid_EASE, \"Intervals\", start, stride, edge, easeray2); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritefield()          %d\n",status););
         }

   free(easeray2);

   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\t Writing field \"VerticalD\" to the grid MercatGrid \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\t Writing field \"VerticalD\" to the grid MercatGrid \n"););
   
   start[0] = 0;
   start[1] = 0;
   
   stride[0] = 1;
   stride[1] = 1;
   
   edge[0] = 351;
   edge[1] = 171;
   
   status = HE5_GDwritefield(GDid_MERCAT, "VerticalD", start, stride, edge, Buffer_mercat);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwritefield(GDid_MERCAT, \"VerticalD\", start, stride, edge, Buffer_mercat); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritefield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritefield(GDid_MERCAT, \"VerticalD\", start, stride, edge, Buffer_mercat); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritefield()          %d\n",status););
	 } 
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\t Writing field \"MaximumDepth\" to the grid AlbersGrid \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\t Writing field \"MaximumDepth\" to the grid AlbersGrid \n"););
   
   start[0] = 0;
   start[1] = 0;
   
   stride[0] = 1;
   stride[1] = 1;
   
   edge[0] = 351;
   edge[1] = 171;
   
   status = HE5_GDwritefield(GDid_ALBERS, "MaximumDepth", start, stride, edge, Buffer_mercat);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwritefield(GDid_ALBERS, \"MaximumDepth\", start, stride, edge, Buffer_mercat); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritefield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritefield(GDid_ALBERS, \"MaximumDepth\", start, stride, edge, Buffer_mercat); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritefield()          %d\n",status););
	 } 
   
   
   free(Buffer_mercat);
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\t Writing field \"Time\" to the grid UTM \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\t Writing field \"Time\" to the grid UTM \n"););
   
   edge[0] = 60;
   edge[1] = 80;
   
   status = HE5_GDwritefield(GDid_UTM, "Time",start, stride, edge, time);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwritefield(GDid_UTM, \"Time\", start, stride, edge, time); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritefield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritefield(GDid_UTM, \"Time\", start, stride, edge, time); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritefield()          %d\n",status););
	 }    
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tWriting appendable field \"Vegetation\" to the grid UTM3  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tWriting appendable field \"Vegetation\" to the grid UTM3  \n"););
   
   
   edge[0]   = 200;
   edge[1]   = 120;
   
   status = HE5_GDwritefield(GDid_UTM3, "Vegetation", start, NULL, edge, veg);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwritefield(GDid_UTM3, \"Vegetation\", start, NULL, edge, veg); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritefield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritefield(GDid_UTM3, \"Vegetation\", start, NULL, edge, veg); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritefield()          %d\n",status););
	 } 
   
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tAttaching to the grid lamcon  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););    
   MESSAGE(11,fprintf(fp,"\t\tAttaching to the grid lamcon  \n"););
   
   GDid_LAMCON = HE5_GDattach(gdfidc_simple,"lamcon"); 
   if (GDid_LAMCON == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully attached to the grid lamcon **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid lamcon **\n"););
	 }
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\t Writing field \"Bypass\" to the grid lamcon \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\t Writing field \"Bypass\" to the grid lamcon \n"););
   
   edge[0] = 351;
   edge[1] = 171;
   
   status = HE5_GDwritefield(GDid_LAMCON, "Bypass", start, NULL, edge, Buffer_LAMCON);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwritefield(GDid_LAMCON, \"Bypass\", start, NULL, edge, Buffer_LAMCON); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritefield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritefield(GDid_LAMCON, \"Bypass\", start, NULL, edge, Buffer_LAMCON); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritefield()          %d\n",status););
	 } 
   
   free(Buffer_UTM);
   free(Buffer_LAMCON);
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDwriteattr                         *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDwriteattr... \n"););
   MESSAGE(4,printf("\t========================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDwriteattr... \n"););   
   MESSAGE(11,fprintf(fp,"\t========================== \n"););
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tWriting attribute \"Resistance\" to the grid UTM  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tWriting attribute \"Resistance\" to the grid UTM \n"););
   
   CurrentSize[0] = 4 ;
   
   Attr[0] = 1;
   Attr[1] = 2;
   Attr[2] = 3;
   Attr[3] = 4;
   
   
   status = HE5_GDwriteattr(GDid_UTM, "Resistance", H5T_NATIVE_INT, CurrentSize,Attr);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwriteattr(GDid_UTM, \"Resistance\", H5T_NATIVE_INT, CurrentSize, Attr); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwriteattr()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwriteattr(GDid_UTM, \"Resistance\", H5T_NATIVE_INT, CurrentSize, Attr);\n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwriteattr()          %d\n",status););
	 }
   
  
/*
        *********************************************************************************
        *                             Testing   HE5_GDwritegrpattr                      *
        ********************************************************************************* 
  */ 

       MESSAGE(4,printf("\tTesting HE5_GDwritegrpattr... \n"););
       MESSAGE(4,printf("\t========================== \n"););
  
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\tTesting HE5_GDwritegrpattr... \n"););
       MESSAGE(11,fprintf(fp,"\t========================== \n"););

   
       MESSAGE(6,printf("\n"););
       MESSAGE(6,printf("\t\tWriting attribute \"GroupAttribute\" to the grid UTM  \n"););
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\t\tWriting attribute \"GroupAttribute\" to the grid UTM \n"););

       status = HE5_GDwritegrpattr(GDid_UTM, "GroupAttribute", H5T_NATIVE_INT, Gsize, grpattr);
       if (status == FAIL)
       {
         MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
         MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
         errcnt++;
       }
      else
      {
        MESSAGE(4,printf("\n"););
        MESSAGE(4,printf("\t\t********Passed Test**********\n"););
        MESSAGE(11,fprintf(fp,"\n"););
        MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
        MESSAGE(10,printf("\t\tstatus = HE5_GDwritegrpattr(GDid_UTM, \"GroupAttribute\", H5T_NATIVE_INT, CurrentSize, Attr); \n"););
        MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritegrpattr()           %d\n",status););
        MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritegrpattr(GDid_UTM, \"GroupAtribute\", H5T_NATIVE_INT, CurrentSize, Attr);\n"););
        MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritegrpattr()          %d\n",status););
     }
 
/*
        *********************************************************************************
        *                             Testing   HE5_GDwritelocattr                      *
        *********************************************************************************
*/  
 
       MESSAGE(4,printf("\tTesting HE5_GDwritelocattr... \n"););
       MESSAGE(4,printf("\t========================== \n"););
 
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\tTesting HE5_GDwritelocattr... \n"););
       MESSAGE(11,fprintf(fp,"\t========================== \n"););
     
       MESSAGE(6,printf("\n"););
       MESSAGE(6,printf("\t\tWriting attribute \"LocaAttribute\" to the field \"Vegetation\"  \n"););
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\t\tWriting attribute \"LocalAttribute\" to the \"Vegetation\" \n"););
      
       edge[0] = 4; 
       status = HE5_GDwritelocattr(GDid_UTM3, "Vegetation", "LocalAttribute", H5T_NATIVE_FLOAT, edge, attr);
       if (status == FAIL)
       {
         MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
         MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
         errcnt++;
       }
      else
      {
        MESSAGE(4,printf("\n"););
        MESSAGE(4,printf("\t\t********Passed Test**********\n"););
        MESSAGE(11,fprintf(fp,"\n"););
        MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
        MESSAGE(10,printf("\t\tstatus =HE5_GDwritelocattr(GDid_UTM3, \"Vegetation\", \"LocalAttribute\", H5T_NATIVE_FLOAT, edge, attr); \n"););
        MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritelocattr()         %d\n",status););
        MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritelocattr(GDid_UTM3, \"Vegetation\", \"LocalAttribute\", H5T_NATIVE_FLOAT, edge, attr); \n");); 
        MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritelocattr()         %d\n",status););

      }


/*	*********************************************************************************
	*                             Testing   HE5_GDgridinfo                          *
	*********************************************************************************
	*/
   
         MESSAGE(4,printf("\tTesting HE5_GDgridinfo... \n"););
         MESSAGE(4,printf("\t========================= \n"););
   
         MESSAGE(11,fprintf(fp,"\t\n"););
         MESSAGE(11,fprintf(fp,"\tTesting HE5_GDgridinfo... \n"););   
         MESSAGE(11,fprintf(fp,"\t========================= \n"););
   
   
         MESSAGE(6,printf("\n"););   
         MESSAGE(6,printf("\t\tRetrieving information about the grid UTM \n"););   
         MESSAGE(11,fprintf(fp,"\t\n"););
         MESSAGE(11,fprintf(fp,"\t\tRetrieving information about the grid UTM \n"););
   
        status = HE5_GDgridinfo(GDid_UTM, &xdim, &ydim, uplft_out, lowrgt_out);
        if (status == FAIL)
	 {
        MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
        MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDgridinfo(GDid_UTM, &xdim, &ydim, uplft_out, lowrgt_out); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDgridinfo()           %d \n",status););
       MESSAGE(8,printf("\t\t\tX-dimension size is  %li\n", xdim););
       MESSAGE(8,printf("\t\t\tY-dimension size is  %li\n", ydim););
       MESSAGE(8,printf("\t\tCoordinates of UL point: %lf,  %lf \n", uplft_out[0], uplft_out[1] ););
       MESSAGE(8,printf("\t\tCoordinates of LR point: %lf,  %lf \n", lowrgt_out[0], lowrgt_out[1] ););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDgridinfo(GDid_UTM, &xdim, &ydim, uplft_out, lowrgt_out); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDgridinfo()           %d \n",status););
       MESSAGE(11,fprintf(fp,"\t\t\t X-dimension size is  %li\n", xdim););
       MESSAGE(11,fprintf(fp,"\t\t\t Y-dimension size is  %li\n", ydim););
       MESSAGE(11,fprintf(fp,"\t\tCoordinates of UL point: %lf,  %lf \n",  uplft_out[0], uplft_out[1] ););
       MESSAGE(11,fprintf(fp,"\t\tCoordinates of LR point: %lf,  %lf \n", lowrgt_out[0], lowrgt_out[1] ););
	 }

     /*
        *********************************************************************************
        *                             Testing    HE5_GDgrpattrinfo                         *
        *********************************************************************************
        */
 
      MESSAGE(4,printf("\n"););
      MESSAGE(4,printf("\tTesting HE5_GDgrpattrinfo... \n"););
      MESSAGE(4,printf("\t=========================\n"););
 
      MESSAGE(11,fprintf(fp,"\n"););
      MESSAGE(11,fprintf(fp,"\tTesting HE5_GDgrpattrinfo... \n"););
      MESSAGE(11,fprintf(fp,"\t=========================\n"););
 
      CountPtr        = (hsize_t *)calloc(1, sizeof(hsize_t));
 
      MESSAGE(6,printf("\n"););
      MESSAGE(6,printf("\t\tRetrieve information about attribute \"GroupAttribute\" \n"););
      MESSAGE(11,fprintf(fp,"\n"););
      MESSAGE(11,fprintf(fp,"\t\tRetrieve information about attribute \"GroupAttribute\" \n"););
 
      status=HE5_GDgrpattrinfo(GDid_UTM,"GroupAttribute",&dtype,CountPtr);
      if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
   { 
       MESSAGE(4,printf("\n");); 
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus=HE5_GDgrpattrinfo(GDid_UTM,\"GroupAttribute\",&dtype,CountPtr);\n"););
       MESSAGE(8,printf("\t\tstatus returned by GDgrpattrinfo %d\n",status););
       MESSAGE(6,printf("\t\tData type   Number of attribute elements:\n"););
       MESSAGE(6,printf("\t\t   %d            %d\n", dtype,(int)*CountPtr););
       MESSAGE(11,fprintf(fp,"\t\tstatus=HE5_GDgrpattrinfo(GDid_UTM,\"GroupAttribute\",&dtype,CountPtr);\n"););

       MESSAGE(11,fprintf(fp,"\t\tStaus returned by GDgrpattrinfo %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tData type   Number of attribute elements:\n"););
       MESSAGE(11,fprintf(fp,"\t\t   %d            %d\n", dtype,(int)*CountPtr););
         }
 

   free(CountPtr);


      /*
        *********************************************************************************
        *                             Testing    HE5_GDlocattrinfo                      *
        *********************************************************************************
        */
        MESSAGE(4,printf("\n"););
        MESSAGE(4,printf("\tTesting HE5_GDlocattrinfo... \n"););
        MESSAGE(4,printf("\t=========================\n"););

        MESSAGE(11,fprintf(fp,"\n"););
        MESSAGE(11,fprintf(fp,"\tTesting HE5_GDlocattrinfo... \n"););
        MESSAGE(11,fprintf(fp,"\t=========================\n"););

        CountPtr        = (hsize_t *)calloc(1, sizeof(hsize_t));

        MESSAGE(6,printf("\n"););
        MESSAGE(6,printf("\t\tRetrieve information about attribute \"LocalAttribute\" \n"););
        MESSAGE(11,fprintf(fp,"\n"););
        MESSAGE(11,fprintf(fp,"\t\tRetrieve information about attribute \"LocalAttribute\" \n"););

        status=HE5_GDlocattrinfo(GDid_UTM3,"Vegetation", "LocalAttribute",&dtype,CountPtr);
        if (status == FAIL)
         {
        MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
        MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
        errcnt++;
         }
   else
   {
     MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus=HE5_GDlocattrinfo(GDid_UTM3, \"Vegetation\", \"LocalAttribute\", &dtype,CountPtr);\n"););
       MESSAGE(8,printf("\t\tstatus returned by GDlocattrinfo %d\n",status););
       MESSAGE(6,printf("\t\tData type   Number of attribute elements:\n"););
       MESSAGE(6,printf("\t\t   %d            %d\n", dtype,(int)*CountPtr););
       MESSAGE(11,fprintf(fp,"\t\tstatus=HE5_GDlocattrinfo(GDid_UTM3, \"Vegetation\", \"LocalAttribute\",&dtype,CountPtr);\n"););

       MESSAGE(11,fprintf(fp,"\t\tStaus returned by GDlocattrinfo %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tData type   Number of attribute elements:\n"););
       MESSAGE(11,fprintf(fp,"\t\t   %d            %d\n", dtype,(int)*CountPtr););
         }


   free(CountPtr);

  /*
         *********************************************************************************
         *                             Testing   HE5_GDtileinfo                          *
         *********************************************************************************
    */
         MESSAGE(4,printf("\tTesting HE5_GDtileinfo............\n"););
         MESSAGE(4,printf("\t======================== \n"););
         MESSAGE(11,fprintf(fp,"\t\n"););
         MESSAGE(11,fprintf(fp,"\tTesting HE5_GDtileinfo ... \n"););
         MESSAGE(11,fprintf(fp,"\t======================== \n"););
         MESSAGE(6,printf("\n"););
         MESSAGE(6,printf("\t\tRetrieving tiling information about  a specific field in the  grid\n"););
         MESSAGE(11,fprintf(fp,"\t\n"););
         MESSAGE(11,fprintf(fp,"\t\t Retrieving information about  a specific field in the  grid\n"););

         status = HE5_GDtileinfo(GDid_UTM3, "Vegetation",&tilecode,&tilerank,tiledims);
         if (status == FAIL)
         {
         MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
         MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
         errcnt++;
         }
    else
         {
         MESSAGE(4,printf("\n"););
         MESSAGE(4,printf("\t\t********Passed Test**********\n"););
         MESSAGE(11,fprintf(fp,"\n"););
         MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
         MESSAGE(10,printf("\t\tstatus = HE5_GDtileinfo(GDid_UTM3, \"Vegetation\", &tilecode, &tilerank, tiledims); \n"););
         MESSAGE(8,printf("\t\tStatus returned by HE5_GDtileinfo()           %d \n",status););
         MESSAGE(8,printf("\t\t\tVegetation tile code: %d\n", tilecode););
         MESSAGE(8,printf("\t\t\tVegetation tile rank: %d\n", tilerank););
         for (i = 0; i < tilerank; i++)
         
         MESSAGE(11,printf("\t\tVegetation tile dimension %d: %lu\n",i+1, (unsigned long)tiledims[i]););
         MESSAGE(11,printf("\t\tStatus returned by HE5_GDtileinfo()           %d \n",status););
         MESSAGE(11,printf("\t\t\tVegetation tile code: %d\n", tilecode););
         MESSAGE(11,printf("\t\t\tVegetation tile rank: %d\n", tilerank););
         for (i = 0; i < tilerank; i++)
         MESSAGE(11,printf("\t\tVegetation tile dimension %d: %lu\n",i+1, (unsigned long)tiledims[i]););
         } 



     /*
         *********************************************************************************
         *                             Testing   HE5_GDinqdatatype                       *
         *********************************************************************************
    */
       MESSAGE(4,printf("\tTesting HE5_GDinqdatatype... \n"););
       MESSAGE(4,printf("\t======================== \n"););
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\tTesting HE5_GDinqdatatype... \n"););
       MESSAGE(11,fprintf(fp,"\t======================== \n"););

       MESSAGE(6,printf("\n"););
       MESSAGE(6,printf("\t\tRetrieving information about field data in a grid\n"););
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\t\t Retrieving information about field data in a grid\n"););
    
      fieldgroup = HE5_HDFE_DATAGROUP;
      status = HE5_GDinqdatatype(GDid_UTM3, "Vegetation", NULL, fieldgroup, &datatype, &classid, &order, &size);
      if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
    else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
  /*   MESSAGE(4,printf("\tdatatype:    %d \n", (int)datatype););
       MESSAGE(4,printf("\tclass ID:    %d \n", (int)classid););
       MESSAGE(4,printf("\torder:       %d \n", (int)order);); 
       MESSAGE(4,printf("\tsize:        %d \n", (int)size);); */
       }
    
   fieldgroup = HE5_HDFE_ATTRGROUP;
   status = HE5_GDinqdatatype(GDid_UTM, NULL, "Resistance", fieldgroup, &datatype, &classid, &order, &size);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
 else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
 /*    MESSAGE(4,printf("\tdatatype:    %d \n", (int)datatype););
       MESSAGE(4,printf("\tclass ID:    %d \n", (int)classid););
       MESSAGE(4,printf("\torder:       %d \n", (int)order););
       MESSAGE(4,printf("\tsize:        %d \n", (int)size);); */
       }
  
   fieldgroup = HE5_HDFE_GRPATTRGROUP;
   status = HE5_GDinqdatatype(GDid_UTM, NULL, "GroupAttribute", fieldgroup, &datatype, &classid, &order, &size);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
 else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
 /*    MESSAGE(4,printf("\tdatatype:    %d \n", (int)datatype););
       MESSAGE(4,printf("\tclass ID:    %d \n", (int)classid););
       MESSAGE(4,printf("\torder:       %d \n", (int)order););
       MESSAGE(4,printf("\tsize:        %d \n", (int)size);); */
       }

  fieldgroup = HE5_HDFE_LOCATTRGROUP;
  status = HE5_GDinqdatatype(GDid_UTM3,"Vegetation", "LocalAttribute", fieldgroup, &datatype, &classid, &order, &size);
  if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
        }
else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
 /*    MESSAGE(4,printf("\tdatatype:    %d \n", (int)datatype););
       MESSAGE(4,printf("\tclass ID:    %d \n", (int)classid););
       MESSAGE(4,printf("\torder:       %d \n", (int)order););
       MESSAGE(4,printf("\tsize:        %d \n", (int)size);); */
       }




   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDinqdims                           *
	*********************************************************************************
	*/
   
       MESSAGE(4,printf("\tTesting HE5_GDinqdims... \n"););
       MESSAGE(4,printf("\t======================== \n"););
   
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\tTesting HE5_GDinqdims... \n"););   
       MESSAGE(11,fprintf(fp,"\t======================== \n"););
   
   
       MESSAGE(6,printf("\n"););   
       MESSAGE(6,printf("\t\tRetrieving dimension information for the grid UTM \n"););   
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\t\tRetrieving dimension information for the grid UTM3 \n"););
   
       strcpy(str_buf,"NO ENTRY !");
   
       ndim = HE5_GDinqdims(GDid_UTM3, str_buf, dims);
       if (ndim == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tndim = HE5_GDinqdims(GDid_UTM3, str_buf, dims); \n"););
       MESSAGE(8,printf("\t\tValue returned by HE5_GDinqdims()           %d\n", ndim););
       MESSAGE(8,printf("\t\tDimension list (%d):  %s\n", ndim, str_buf););

       MESSAGE(11,fprintf(fp,"\t\tndim = HE5_GDinqdims(GDid_UTM3, str_buf, dims);  \n"););
       MESSAGE(11,fprintf(fp,"\t\tValue returned by HE5_GDinqdims()           %d\n",  ndim););
       MESSAGE(11,fprintf(fp,"\t\tDimension list (%d):  %s\n", ndim, str_buf););
	 }
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdiminfo                           *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDdiminfo... \n"););
   MESSAGE(4,printf("\t======================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdiminfo... \n"););   
   MESSAGE(11,fprintf(fp,"\t======================== \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving information about dimension \"Conduction\" in the grid UTM \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about dimension \"Conduction\" in the grid UTM\n"););
   
   ndims = HE5_GDdiminfo(GDid_UTM, "Conduction");
   if (ndims == 0)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tndims = HE5_GDdiminfo(GDid_UTM, \"Conduction\");\n"););
       MESSAGE(8,printf("\t\tValue returned by HE5_GDdiminfo()           %lu\n", (unsigned long)ndims););
       MESSAGE(8,printf("\t\t\"Conduction\" defines %lu - element dimension \n", (unsigned long)ndims););

       MESSAGE(11,fprintf(fp,"\t\tndims = HE5_GDdiminfo(GDid_UTM, \"Conduction\"); \n"););
       MESSAGE(11,fprintf(fp,"\t\tValue returned by HE5_GDdiminfo()           %lu \n",  (unsigned long)ndims););
       MESSAGE(11,fprintf(fp,"\t\t\"Conduction\" defines %lu - element dimension \n", (unsigned long)ndims););
	 }
   
       MESSAGE(6,printf("\t\n"););
       MESSAGE(6,printf("\t\n"););
       MESSAGE(6,printf("\t\tRetrieving information about dimension \"Timedim\" in the grid UTM \n");); 
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\t\n"););     
       MESSAGE(11,fprintf(fp,"\t\t Retrieving information about dimension \"Timedim\" in the grid UTM\n"););
   
       ndims = HE5_GDdiminfo(GDid_UTM, "Timedim");
       if (ndims == 0)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
        MESSAGE(4,printf("\n"););   
        MESSAGE(4,printf("\t\t********Passed Test**********\n"););
        MESSAGE(11,fprintf(fp,"\n"););
        MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

        MESSAGE(10,printf("\t\tndims = HE5_GDdiminfo(GDid_UTM, \"Timedim\");\n"););
        MESSAGE(8,printf("\t\tValue returned by HE5_GDdiminfo()           %lu \n", (unsigned long)ndims););
        MESSAGE(8,printf("\t\t\"Timedim\" defines %lu - element dimension \n", (unsigned long)ndims););

        MESSAGE(11,fprintf(fp,"\t\tndims = HE5_GDdiminfo(GDid_UTM, \"Timedim\"); \n"););
        MESSAGE(11,fprintf(fp,"\t\tValue returned by HE5_GDdiminfo()           %lu \n",(unsigned long)ndims););
        MESSAGE(11,fprintf(fp,"\t\t\"Timedim\" defines %lu - element dimension \n", (unsigned long)ndims););
	 }
 
        ndims = HE5_GDdiminfo(GDid_UTM, "ExtDim");
        if (ndims == 0)
         {
        MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
        MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
        errcnt++;
         }
   else
         {  
        MESSAGE(4,printf("\n"););  
        MESSAGE(4,printf("\t\t********Passed Test**********\n"););
        MESSAGE(11,fprintf(fp,"\n"););
        MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

        MESSAGE(10,printf("\t\tndims = HE5_GDdiminfo(GDid_UTM, \"ExtDim\");\n"););
        MESSAGE(8,printf("\t\tValue returned by HE5_GDdiminfo()           %lu \n", (unsigned long)ndims););
        MESSAGE(8,printf("\t\t\"ExtDim\" defines %lu - element dimension \n", (unsigned long)ndims););

        MESSAGE(11,fprintf(fp,"\t\tndims = HE5_GDdiminfo(GDid_UTM, \"ExtDim\"); \n"););
        MESSAGE(11,fprintf(fp,"\t\tValue returned by HE5_GDdiminfo()           %lu \n",(unsigned long)ndims););
        MESSAGE(11,fprintf(fp,"\t\t\"ExtDim\" defines %lu - element dimension \n", (unsigned long)ndims););
         }

   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDprojinfo                          *
	*********************************************************************************
	*/
   
       MESSAGE(4,printf("\tTesting HE5_GDprojinfo... \n"););
       MESSAGE(4,printf("\t========================= \n"););
   
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\tTesting HE5_GDprojinfo... \n"););   
       MESSAGE(11,fprintf(fp,"\t========================= \n"););
   
       MESSAGE(6,printf("\n"););   
       MESSAGE(6,printf("\t\tRetrieving projection information from the grid UTM \n"););   
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\t\t Retrieving projection information from the grid UTM\n"););
   
       status = HE5_GDprojinfo(GDid_UTM, &proj_code, &z_code, &s_code, ProjParm_out);
       if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDprojinfo(GDid_UTM, &proj_code, &z_code, &s_code, ProjParm_out); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDprojinfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\t Projection Code =  %d\n", proj_code););
       MESSAGE(8,printf("\t\t\t Z Code          =  %d\n", z_code););
       MESSAGE(8,printf("\t\t\t S Code          =  %d\n", s_code););
       MESSAGE(8,printf("\t\t\t Projection Parameters: \n"););
	   for ( i = 0; i < 16; i++ )
	     {
		   MESSAGE(8,printf("\t\t\t     Parameter # %d  Value = %lf\n", i + 1, ProjParm_out[i]););
		 
	     }

       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDprojinfo(GDid_UTM, &proj_code, &z_code, &s_code, ProjParm_out); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDprojinfo()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\t Projection Code =  %d\n", proj_code););
       MESSAGE(11,fprintf(fp,"\t\t\t Z Code          =  %d\n", z_code););
       MESSAGE(11,fprintf(fp,"\t\t\t S Code          =  %d\n", s_code););
       MESSAGE(11,fprintf(fp,"\t\t\t Projection Parameters: \n"););
	   for ( i = 0; i < 16; i++ )
	     {
		   MESSAGE(11,fprintf(fp,"\t\t\t     Parameter # %d  Value = %lf\n", i + 1, ProjParm_out[i]););
		 
	     }


	 }
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDorigininfo                        *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDorigininfo... \n"););
   MESSAGE(4,printf("\t=========================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDorigininfo... \n"););   
   MESSAGE(11,fprintf(fp,"\t=========================== \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving origin code for the grid UTM \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving origin code for the grid UTM\n"););
   
   status = HE5_GDorigininfo(GDid_UTM, &origin_code);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDorigininfo(GDid_UTM, &origin_code);  \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDorigininfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\t Origin Code =  %d\n", origin_code););

       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDorigininfo(GDid_UTM, &origin_code); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDorigininfo()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\t Origin Code =  %d\n", origin_code););

	 }
   
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDpixreginfo                        *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDpixreginfo... \n"););
   MESSAGE(4,printf("\t=========================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDpixreginfo... \n"););   
   MESSAGE(11,fprintf(fp,"\t=========================== \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving pixel registration code for the grid UTM \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving pixel registration code for the grid UTM\n"););
   
   status = HE5_GDpixreginfo(GDid_UTM, &pixreg_code);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDpixreginfo(GDid_UTM, &pixreg_code);  \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDpixreginfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\tPixel Registration Code =  %d\n", pixreg_code););

       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDpixreginfo(GDid_UTM, &pixreg_code); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDpixreginfo()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\tPixel Registration Code =  %d\n", pixreg_code););
	 }
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDinqfields                         *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDinqfields... \n"););
   MESSAGE(4,printf("\t========================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDinqfields... \n"););   
   MESSAGE(11,fprintf(fp,"\t========================== \n"););
   
   strcpy(str_buf, "NO ENTRY !");
   ntype1 = (hid_t *)calloc(10, sizeof(hid_t)); 
   rank = (int *)calloc(8, sizeof(int));
   for ( i = 0; i < 8; i++)
	 {
	   rank[i] = FAIL;
	 }
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving data fields information for the grid UTM \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving data fields information  for the grid UTM\n"););
   
   nflds = HE5_GDinqfields(GDid_UTM, str_buf, rank, ntype1);
   if (nflds == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tnflds = HE5_GDinqfields(GDid_UTM, str_buf, rank, ntype1); \n"););
       MESSAGE(8,printf("\t\tValue (# of data fields) returned by HE5_GDinqfields() %d \n", nflds););
       MESSAGE(8,printf("\t\tList of data fields in the Grid:\n"););
       MESSAGE(8,printf("\t\t\t%s\n", str_buf););
       MESSAGE(8,printf("\t\tField data types:\n"););

       for (i = 0; i < nflds; i++)
		 {
		   MESSAGE(8,printf("\t\t %d\n", (int)ntype1[i] ););
		 }
       MESSAGE(8,printf("\t\tField ranks:\n"););

       for (i = 0; i < nflds; i++)
		 {
		   MESSAGE(8,printf("\t\t %d\n", rank[i] ););
		 }

       MESSAGE(11,fprintf(fp,"\t\tnflds = HE5_GDinqfields(GDid_UTM, str_buf, rank, ntype1); \n"););
       MESSAGE(11,fprintf(fp,"\t\tValue (# of data fields) returned by HE5_GDinqfields()        %d\n", nflds););
       MESSAGE(11,fprintf(fp,"\t\tList of data fields in the Grid:\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t%s\n", str_buf););
       MESSAGE(11,fprintf(fp,"\t\tField data types:\n"););

       for (i = 0; i < nflds; i++)
		 {
		   MESSAGE(11,fprintf(fp,"\t\t %d\n", (int)ntype1[i] ););
		 }
       MESSAGE(11,fprintf(fp,"\t\tField ranks:\n"););

       for (i = 0; i < nflds; i++)
		 {
		   MESSAGE(11,fprintf(fp,"\t\t %d\n", rank[i] ););
		 }

	 }
   
   free(rank);
   free(ntype1);
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDfieldinfo                         *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDfieldinfo... \n"););
   MESSAGE(4,printf("\t========================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDfieldinfo... \n"););   
   MESSAGE(11,fprintf(fp,"\t========================== \n"););
   
   
   strcpy(str_buf, "NO ENTRY !");
   ntype1 = (hid_t *)calloc(1, sizeof(hid_t));
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving information about \"Voltage\" field \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about \"Voltage\" field \n"););
   
   status = HE5_GDfieldinfo(GDid_UTM, "Voltage", &Rank, dims, ntype1, str_buf, NULL);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDfieldinfo(GDid_UTM, \"Voltage\", &Rank, dims, ntype1, str_buf,NULL);  \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDfieldinfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\tRank of the field \"Voltage\" :\n"););
       MESSAGE(8,printf("\t\t\t    %d\n", Rank););
       MESSAGE(8,printf("\t\t\tDimension list for the field \"Voltage\" : \n"););
       MESSAGE(8,printf("\t\t\t    %s \n", str_buf););
       MESSAGE(8,printf("\t\t\tDimensions of the field \"Voltage\" : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(8,printf("\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(8,printf("\t\t\tData type of the field \"Voltage\" : \n"););
       MESSAGE(8,printf("\t\t\t    %d \n", (int)ntype1[0]););


       
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDfieldinfo(GDid_UTM, \"Voltage\", &Rank, dims, ntype1, str_buf,NULL);  \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDfieldinfo() %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\tRank of the field \"Voltage\" :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d\n", Rank););
       MESSAGE(11,fprintf(fp,"\t\t\tDimension list for the field \"Voltage\" : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %s \n", str_buf););
       MESSAGE(11,fprintf(fp,"\t\t\tDimensions of the field \"Voltage\" : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(11,fprintf(fp,"\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(11,fprintf(fp,"\t\t\tData type of the field \"Voltage\" : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d \n", (int)ntype1[0]););


	 }
   
   
   
   strcpy(str_buf, "NO ENTRY !");
   Rank = FAIL;
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving information about \"Drift\" field \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about \"Drift\" field \n"););
   
   status = HE5_GDfieldinfo(GDid_UTM, "Drift", &Rank, dims, ntype1, str_buf, NULL);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDfieldinfo(GDid_UTM, \"Drift\", &Rank,dims, ntype1, str_buf, NULL);  \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDfieldinfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\tRank of the field \"Drift\" :\n"););
       MESSAGE(8,printf("\t\t\t    %d\n", Rank););
       MESSAGE(8,printf("\t\t\tDimension list for the field \"Drift\" : \n"););
       MESSAGE(8,printf("\t\t\t    %s \n", str_buf););
       MESSAGE(8,printf("\t\t\tDimensions of the field \"Drift\" : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(8,printf("\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(8,printf("\t\t\tData type of the field \"Drift\" : \n"););
       MESSAGE(8,printf("\t\t\t    %d \n", (int)ntype1[0]););


       
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDfieldinfo(GDid_UTM, \"Drift\",&Rank, dims, ntype1, str_buf, NULL);  \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDfieldinfo() %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\tRank of the field \"Drift\" :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d\n", Rank););
       MESSAGE(11,fprintf(fp,"\t\t\tDimension list for the field \"Drift\" : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %s \n", str_buf););
       MESSAGE(11,fprintf(fp,"\t\t\tDimensions of the field \"Drift\" : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(11,fprintf(fp,"\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(11,fprintf(fp,"\t\t\tData type of the field \"Drift\" : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d \n", (int)ntype1[0]););
	 }
   
   
   
   
   strcpy(str_buf, "NO ENTRY !");
   strcpy(str_buf2, "NO ENTRY !");
   
   Rank = FAIL;
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving information about appendable \"Vegetation\" field \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about appendable \"Vegetation\" field \n"););
   
   status = HE5_GDfieldinfo(GDid_UTM3, "Vegetation", &Rank, dims, ntype1, str_buf, str_buf2);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDfieldinfo(GDid_UTM3, \"Vegetation\", &Rank,dims, ntype1, str_buf, str_buf2);  \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDfieldinfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\tRank of the field \"Vegetation\" :\n"););
       MESSAGE(8,printf("\t\t\t    %d\n", Rank););
       MESSAGE(8,printf("\t\t\tDimension list for the field \"Vegetation\" : \n"););
       MESSAGE(8,printf("\t\t\t    %s \n", str_buf););
       MESSAGE(8,printf("\t\t\tMaximum Dimension list for the field \"Vegetation\" : \n"););
       MESSAGE(8,printf("\t\t\t    %s \n", str_buf2););
       MESSAGE(8,printf("\t\t\tDimensions of the field \"Vegetation\" : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(8,printf("\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(8,printf("\t\t\tData type of the field \"Vegetation\" : \n"););
       MESSAGE(8,printf("\t\t\t    %d \n", (int)ntype1[0]););
       
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDfieldinfo(GDid_UTM3, \"Vegetation\",&Rank, dims, ntype1, str_buf, str_buf2);  \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDfieldinfo() %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\tRank of the field \"Vegetation\" :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d\n", Rank););
       MESSAGE(11,fprintf(fp,"\t\t\tDimension list for the field \"Vegetation\" : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %s \n", str_buf););
       MESSAGE(11,fprintf(fp,"\t\t\tMaximum Dimension list for the field \"Vegetation\" : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %s \n", str_buf2););
       MESSAGE(11,fprintf(fp,"\t\t\tDimensions of the field \"Vegetation\" : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(11,fprintf(fp,"\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(11,fprintf(fp,"\t\t\tData type of the field \"Vegetation\" : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d \n", (int)ntype1[0]););
	 }
   
   
   free(ntype1);
   free(str_buf2);
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDnentries                          *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDnentries... \n"););
   MESSAGE(4,printf("\t========================= \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDnentries... \n"););   
   MESSAGE(11,fprintf(fp,"\t========================= \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving number of Dimension entries in the grid UTM \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving number of Dimension entries in the grid UTM\n"););
   
   number = HE5_GDnentries(GDid_UTM, HE5_HDFE_NENTDIM, &list_size);
   if (number == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tnumber = HE5_GDnentries(GDid_UTM, HE5_HDFE_NENTDIM, &list_size); \n"););
       MESSAGE(8,printf("\t\tNumber of Dimension entries in the grid =  %li\n", number););
       MESSAGE(8,printf("\t\tThe size (in bytes) of dimension list string %li \n", list_size););

       MESSAGE(11,fprintf(fp,"\t\tnumber = HE5_GDnentries(GDid_UTM, HE5_HDFE_NENTDIM, &list_size);\n"););
       MESSAGE(11,fprintf(fp,"\t\tNumber of Dimension entries in the grid =  %li\n", number););
       MESSAGE(11,fprintf(fp,"\t\tThe size (in bytes) of dimension list string %li \n", list_size););

	 }
   
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tRetrieving number of Data Field entries in the grid UTM \n"););  
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving number of Data Field entries in the grid UTM\n"););
   
   list_size = FAIL;
   
   number = HE5_GDnentries(GDid_UTM, HE5_HDFE_NENTDFLD, &list_size);
   if (number == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tnumber = HE5_GDnentries(GDid_UTM, HE5_HDFE_NENTDFLD, &list_size); \n"););
       MESSAGE(8,printf("\t\tNumber of Data Field entries in the grid =  %li\n", number););
       MESSAGE(8,printf("\t\tThe size (in bytes) of a field list string %li \n", list_size););

       MESSAGE(11,fprintf(fp,"\t\tnumber = HE5_GDnentries(GDid_UTM, HE5_HDFE_NENTDFLD, &list_size);\n"););
       MESSAGE(11,fprintf(fp,"\t\tNumber of Data Field entries in the grid =  %li\n", number););
       MESSAGE(11,fprintf(fp,"\t\tThe size (in bytes) of a field list string %li \n", list_size););

	 }
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDinqattrs                          *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDinqattrs... \n"););
   MESSAGE(4,printf("\t========================= \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDinqattrs... \n"););   
   MESSAGE(11,fprintf(fp,"\t========================= \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving information about attributes in the grid UTM \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about attributes in the grid UTM\n"););
   
   list_size = FAIL;
   
   number = HE5_GDinqattrs(GDid_UTM, NULL, &list_size);
   if (number == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tnumber = HE5_GDinqattrs(GDid_UTM, NULL, &list_size); \n"););
       MESSAGE(8,printf("\t\tNumber of attributes in the grid =  %li\n", number););
       MESSAGE(8,printf("\t\tThe size (in bytes) of attribute list string %li \n", list_size););

       MESSAGE(11,fprintf(fp,"\t\tnumber = HE5_GDinqattrs(GDid_UTM, NULL, &list_size); \n"););
       MESSAGE(11,fprintf(fp,"\t\tNumber of attributes in the grid =  %li\n", number););
       MESSAGE(11,fprintf(fp,"\t\tThe size (in bytes) of attribute list string %li \n", list_size););

	 }
   
   
   strcpy(str_buf,"NO ENTRY !");
   list_size = FAIL;
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tRetrieving more information about attributes in the grid UTM \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving more information about attributes in the grid UTM\n"););
   
   number = HE5_GDinqattrs(GDid_UTM, str_buf, &list_size);
   if (number == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tnumber = HE5_GDinqattrs(GDid_UTM, str_buf, &list_size); \n"););
       MESSAGE(8,printf("\t\tNumber of attributes in the grid =  %li\n", number););
       MESSAGE(8,printf("\t\tThe list of attributes: %s \n", str_buf););
       MESSAGE(8,printf("\t\tThe size (in bytes) of attribute list string %li \n", list_size););

       MESSAGE(11,fprintf(fp,"\t\tnumber = HE5_GDinqattrs(GDid_UTM, str_buf, &list_size); \n"););
       MESSAGE(11,fprintf(fp,"\t\tNumber of attributes in the grid =  %li\n", number););
       MESSAGE(11,fprintf(fp,"\t\tThe list of attributes:  %s \n", str_buf););
       MESSAGE(11,fprintf(fp,"\t\tThe size (in bytes) of attribute list string %li \n", list_size););
	 }
  
/*
        *********************************************************************************
        *                             Testing    HE5_GDinqlocattrs                      *
        *********************************************************************************
        */
        MESSAGE(4,printf("\n"););
        MESSAGE(4,printf("\tTesting HE5_GDinqlocattrs... \n"););
        MESSAGE(4,printf("\t========================= \n"););

        MESSAGE(11,fprintf(fp,"\t\n"););
        MESSAGE(11,fprintf(fp,"\tTesting HE5_GDinqlocattrs... \n"););
        MESSAGE(11,fprintf(fp,"\t========================= \n"););

        MESSAGE(6,printf("\n"););
        MESSAGE(6,printf("\t\tRetrieve information about attributes defined in Vegetation field \n"););
        MESSAGE(11,fprintf(fp,"\n"););
        MESSAGE(11,fprintf(fp,"\t\tRetrieve information about attributes defined in Vegetation \n"););
        
        number = HE5_GDinqlocattrs(GDid_UTM3, "Vegetation", NULL, &strbufsize);
        if (number == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
           MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tnumber=HE5_GDinqlocattrs(GDid_UTM3, \"Vegetation\", NULL, &strbufsize);\n"););
       MESSAGE(11,fprintf(fp,"\t\tnumber=HE5_GDinqlocattrs(GDid_UTM3, \"Vegetation\", NULL, &strbufsize);\n"););
       MESSAGE(8,printf("\t\tValue (number of attributes) returned by GDinqlocattrs %li\n",number););
       MESSAGE(11,fprintf(fp,"\t\tValue (number of attributes) returned by GDinqlocattrs %li\n",number););
       MESSAGE(8,printf("\t\tString length of attribute list:     %li\n",strbufsize););
       MESSAGE(11,fprintf(fp,"\t\tString length of attribute list:    %li\n",strbufsize););
        }
     
       MESSAGE(6,printf("\n"););
       MESSAGE(6,printf("\t\tRetrieve more information about attributes defined in vegetation field \n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\tRetrieve more information about attributes defined in vegetation field \n"););
       
       ObjNamesPtr     = (char  *)calloc( BUFSIZE, sizeof(char));
       number = HE5_GDinqlocattrs(GDid_UTM3, "Vegetation", ObjNamesPtr, &strbufsize);
       if (number == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
            MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tnumber=HE5_GDinqlocattrs(GDid_UTM3, Vegetation, ObjNamesPtr, &strbufsize);\n"););
       MESSAGE(11,fprintf(fp,"\t\tnumber=HE5_GDinqlocattrs(GDid_UTM3, Vegetation, ObjNamesPtr,&strbufsize);\n"););
       MESSAGE(8,printf("\t\tNumber of attributes returned by GDinqlocattrs   %li\n",number););
       MESSAGE(11,fprintf(fp,"\t\tNumber of attributes returned by GDinqlocattr   %li\n",number););
       MESSAGE(8,printf("\t\tAttribute list:\n"););
       MESSAGE(8,printf("\t\t    %s\n", ObjNamesPtr););
       MESSAGE(11,fprintf(fp,"\t\tAttribute list: \n"););
       MESSAGE(11,fprintf(fp,"\t\t    %s\n", ObjNamesPtr););
       MESSAGE(8,printf("\t\tString length of attribute list:     %li\n",strbufsize););
       MESSAGE(11,fprintf(fp,"\t\tString length of attribute list:      %li\n",strbufsize););
         }
  
   free(ObjNamesPtr);
   ObjNamesPtr = NULL;

    
    /*
        *********************************************************************************
        *                             Testing    HE5_GDinqgrpattrs                      *
        *********************************************************************************
        */

           MESSAGE(4,printf("\n"););
           MESSAGE(4,printf("\tTesting HE5_GDinqgrpattrs... \n"););
           MESSAGE(4,printf("\t========================= \n"););

           MESSAGE(11,fprintf(fp,"\t\n"););
           MESSAGE(11,fprintf(fp,"\tTesting HE5_GDinqgrpattrs... \n"););
           MESSAGE(11,fprintf(fp,"\t========================= \n"););

           MESSAGE(6,printf("\n"););
           MESSAGE(6,printf("\t\tRetrieve information about group attributes defined in grid UTM \n"););
           MESSAGE(11,fprintf(fp,"\n"););
           MESSAGE(11,fprintf(fp,"\t\tRetrieve information about group attributes defined in grid UTM \n"););

          number = HE5_GDinqgrpattrs(GDid_UTM, NULL, &strbufsize);
          if (number == FAIL)
         {
           MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
           MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
           errcnt++;
         }
        else
        {
          MESSAGE(4,printf("\n"););
          MESSAGE(4,printf("\t\t********Passed Test**********\n"););
          MESSAGE(11,fprintf(fp,"\n"););
          MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
          MESSAGE(10,printf("\t\tnumber=HE5_GDinqgrpattrs(GDid_UTM, NULL, &strbufsize);\n"););
          MESSAGE(11,fprintf(fp,"\t\tnumber=HE5_GDinqgrpattrs(GDid_UTM, NULL, &strbufsize);\n"););
          MESSAGE(8,printf("\t\tValue (number of group attributes) returned by GDingrpqattrs %li\n",number););
          MESSAGE(11,fprintf(fp,"\t\tValue (number of group attributes) returned by GDinqgrpattrs %li\n",number);)
          MESSAGE(8,printf("\t\tString length of group attribute list:     %li\n",strbufsize););
          MESSAGE(11,fprintf(fp,"\t\tString length of group attribute list:    %li\n",strbufsize););
         }


          MESSAGE(6,printf("\n"););
          MESSAGE(6,printf("\t\tRetrieve more information about group attributes defined in swath INDEX \n"););
          MESSAGE(11,fprintf(fp,"\n"););
          MESSAGE(11,fprintf(fp,"\t\tRetrieve more information about group attributes defined in swath INDEX \n"););
          ObjNamesPtr     = (char  *)calloc( BUFSIZE, sizeof(char));
          number = HE5_GDinqgrpattrs(GDid_UTM, ObjNamesPtr, &strbufsize);
          if (number == FAIL)
         {
          MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
          MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
          errcnt++;
         }
      else
        {
          MESSAGE(4,printf("\n"););
          MESSAGE(4,printf("\t\t********Passed Test**********\n"););
          MESSAGE(11,fprintf(fp,"\n"););
          MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
          MESSAGE(10,printf("\t\tnumber=HE5_GDinqgrpattrs(GDid_UTM, ObjNamesPtr, &strbufsize);\n"););
          MESSAGE(11,fprintf(fp,"\t\tnumber=HE5_GDinqgrpattrs(GDid_UTM, ObjNamesPtr,&strbufsize);\n"););
          MESSAGE(8,printf("\t\tNumber of attributes returned by GDinqgrpattr    %li\n",number););
          MESSAGE(11,fprintf(fp,"\t\tNumber of attributes returned by GDinqgrpattr    %li\n",number););
          MESSAGE(8,printf("\t\tGroup Attribute list:\n"););
          MESSAGE(8,printf("\t\t    %s\n", ObjNamesPtr););
          MESSAGE(11,fprintf(fp,"\t\tGroup Attribute list: \n"););
          MESSAGE(11,fprintf(fp,"\t\t    %s\n", ObjNamesPtr););
          MESSAGE(8,printf("\t\tString length of Group attribute list:     %li\n",strbufsize););
          MESSAGE(11,fprintf(fp,"\t\tString length of Group attribute list:      %li\n",strbufsize););
         }

      free(ObjNamesPtr);
      ObjNamesPtr = NULL;
 
   /*
	*********************************************************************************
	*                             Testing   HE5_GDattrinfo                          *
	*********************************************************************************
	*/
   
         MESSAGE(4,printf("\tTesting HE5_GDattrinfo... \n"););
         MESSAGE(4,printf("\t========================= \n"););
   
         MESSAGE(11,fprintf(fp,"\t\n"););
         MESSAGE(11,fprintf(fp,"\tTesting HE5_GDattrinfo... \n"););   
         MESSAGE(11,fprintf(fp,"\t========================= \n"););
   
   count = 0;
   ntype = (hid_t *)calloc(1, sizeof(hid_t));   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving information about \"Resistance\" attribute \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about \"Resistance\" attribute \n"););
   
   status = HE5_GDattrinfo(GDid_UTM, "Resistance", ntype, &count );
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDattrinfo(GDid_UTM, \"Resistance\", ntype, &count ); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDattrinfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\tData type of attribute \"Resistance\" :\n"););
       MESSAGE(8,printf("\t\t\t    %d\n", (int)*ntype););
       MESSAGE(8,printf("\t\t\tNumber of attribute elements : \n"););
       MESSAGE(8,printf("\t\t\t    %lu \n", (unsigned long)count););

       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDattrinfo(GDid_UTM, \"Resistance\", ntype, &count ); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDattrinfo()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\tData type of attribute \"Resistance\" :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d\n", (int)*ntype););
       MESSAGE(11,fprintf(fp,"\t\t\tNumber of attribute elements : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %lu \n", (unsigned long)count););

	 }
   
   free(ntype);
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDreadattr                          *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDreadattr... \n"););
   MESSAGE(4,printf("\t========================= \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDreadattr... \n"););   
   MESSAGE(11,fprintf(fp,"\t========================= \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving the values of \"Resistance\" attribute \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving the values of \"Resistance\" attribute \n"););
   
   status = HE5_GDreadattr(GDid_UTM, "Resistance", Attr_out);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDreadattr(GDid_UTM, \"Resistance\", Attr_out); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDreadattr()           %d\n", status););
       MESSAGE(8,printf("\t\tThe values of attribute \"Resistance\" :\n"););
       for (i = 0; i < (unsigned long)count; i++)
		 {
		   MESSAGE(8,printf("\t\t\t    %d \n", Attr_out[i]););
		 }

       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDreadattr(GDid_UTM, \"Resistance\", Attr_out); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDreadattr()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\tThe values of attribute \"Resistance\" :\n"););
       for (i = 0; i < (unsigned long)count; i++)
		 {
		   MESSAGE(11,fprintf(fp,"\t\t\t    %d \n", Attr_out[i]););
		 }
       
	 }
   
  
/*
        *********************************************************************************
        *                             Testing   HE5_GDreadfield                        *
        **********************************************************************************
        */
        
        MESSAGE(4,printf("\tTesting HE5_GDreadfield... \n"););
        MESSAGE(4,printf("\t=========================== \n"););
  
        MESSAGE(11,fprintf(fp,"\t\n"););
        MESSAGE(11,fprintf(fp,"\tTesting HE5_GDreadfield... \n"););
        MESSAGE(11,fprintf(fp,"\t=========================== \n"););
       
        MESSAGE(6,printf("\n"););
        MESSAGE(6,printf("\t\tReading field \"Voltage\" from the grid UTM  \n"););
        MESSAGE(11,fprintf(fp,"\t\n"););
        MESSAGE(11,fprintf(fp,"\t\treading field \"Voltage\" from the grid UTM  \n"););  

        start[0]  = 0;
        start[1]  = 0;
        stride[0] = 1;
        stride[1] = 1;
        edge[0]   = 50;
        edge[1]   = 80;

        VoltDataOut = (float *)calloc( 4800, sizeof(float) );
        status = HE5_GDreadfield(GDid_UTM,"Voltage", start, stride, edge, VoltDataOut);
        if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
       else
       {
       MESSAGE(4,printf("\n"););  
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus=HE5_GDreadfield(GDid_UTM,\"Voltage\",start,stride,edge,VoltDataOut);\n"););
       MESSAGE(8,printf("\t\tstatus returned by GDreadfield %d\n", status ););
       MESSAGE(11,fprintf(fp,"\t\tstatus=HE5_GDreadfield(GDid_UTM,\"Voltage\",start,stride,edge,VoltDataOut);\n"););
       MESSAGE(11,fprintf(fp,"\t\tstatus returned by GDreadfield %d\n", status ););
       MESSAGE(11,fprintf(fp,"\t\tFirst five values of \"Voltage\" field read\n"););
       MESSAGE(11,fprintf(fp,"\t\tFirst five values of \"Voltage\" field read\n");); 
       MESSAGE(8,printf("\n"););
       MESSAGE(11,fprintf(fp,"\n"););
         for (i = 0; i < 5; i++)
          { 
            MESSAGE(8 ,printf("\t\t %lf \n", VoltDataOut[i]););
            MESSAGE(11,fprintf(fp,"\t\t       %lf\n", VoltDataOut[i]););
          }
       } 
  
   free(VoltDataOut);
   VoltDataOut = NULL; 

/*
        *********************************************************************************
        *                             Testing   HE5_GDreadgrpattr                       *
        *********************************************************************************
        */


     MESSAGE(4,printf("\n"););
     MESSAGE(4,printf("\tTesting HE5_GDreadgrpattr... \n"););
     MESSAGE(4,printf("\t=========================\n"););
     MESSAGE(11,fprintf(fp,"\n"););
     MESSAGE(11,fprintf(fp,"\tTesting HE5_GDreadgrpattr... \n"););
     MESSAGE(11,fprintf(fp,"\t=========================\n"););
     MESSAGE(6,printf("\n"););
     MESSAGE(6,printf("\t\tReading attribute GroupAttribute from grid UTM \n"););
     MESSAGE(11,fprintf(fp,"\n"););
     MESSAGE(11,fprintf(fp,"\t\tReading attribute GroupAttribute from grid UTM \n"););
   
   status = HE5_GDreadgrpattr(GDid_UTM,"GroupAttribute", gattrvalOut);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
   {
      MESSAGE(4,printf("\n"););
      MESSAGE(4,printf("\t\t********Passed Test**********\n"););
      MESSAGE(11,fprintf(fp,"\n"););
      MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
      MESSAGE(10,printf("\t\tstatus = HE5_GDreadgrpattr(SWid_simple,\"GroupAttribute\", gattrvalOut);\n"););
      MESSAGE(8,printf("\t\tValue returned by GDreadgrpattr for attribute GroupAttribute in grid UTM %d\n",status););
      MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDreadgrpattr(GDid_UTM,\"GroupAttribute\", gattrvalOut);\n"););
      MESSAGE(11,fprintf(fp,"\t\tValue returned by GDreadgrpattr for attribute GroupAttribute in grid UTM %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\t Attribute elements read \n"););
       MESSAGE(8,printf("\t\t Attribute elements read \n"););
       MESSAGE(8,printf("\n"););
       MESSAGE(11,fprintf(fp,"\n"););
           for( i = 0; i < 4 ; i++ )
                 {
                   MESSAGE(11,fprintf(fp,"\t\t            %d \n", gattrvalOut[ i ] ););
                   MESSAGE(8,printf("\t\t               %d \n", gattrvalOut[ i ] ););
                 }
         }

      /*
        *********************************************************************************
        *                             Testing   HE5_GDreadlocattr                       *
        *********************************************************************************
        */


   MESSAGE(4,printf("\n"););
   MESSAGE(4,printf("\tTesting HE5_GDreadlocattr... \n"););
   MESSAGE(4,printf("\t=========================\n"););
   MESSAGE(11,fprintf(fp,"\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDreadlocattr... \n"););
   MESSAGE(11,fprintf(fp,"\t=========================\n"););
   MESSAGE(6,printf("\n"););
   MESSAGE(6,printf("\t\tReading attribute LocalAttribute from Vegetation field \n"););
   MESSAGE(11,fprintf(fp,"\n"););
   MESSAGE(11,fprintf(fp,"\t\tReading attribute LocalAttribute from Vegetation field \n"););

   status = HE5_GDreadlocattr(GDid_UTM3, "Vegetation", "LocalAttribute", attrvalOut);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
   {
      MESSAGE(4,printf("\n"););
      MESSAGE(4,printf("\t\t********Passed Test**********\n"););
      MESSAGE(11,fprintf(fp,"\n"););
      MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
      MESSAGE(10,printf("\t\tstatus = HE5_GDreadlocattr(GDid_UTM3,\"Vegetation\", \"LocalAttribute\", attrvalOut);\n"););
      MESSAGE(8,printf("\t\tValue returned by GDreadlocattr for attribute LocalAttribute in field Vegetation %d\n",status););
      MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDreadlocattr(GDid_UTM3,\"Vegetation\", \"LocalAttribute\", attrvalOut);\n"););
      MESSAGE(11,fprintf(fp,"\t\tValue returned by GDreadlocattr for attribute LocalAttribute in field Vegetation %d\n",status);)
;
       MESSAGE(11,fprintf(fp,"\t\t Attribute elements read \n"););
       MESSAGE(8,printf("\t\t Attribute elements read \n"););
       MESSAGE(8,printf("\n"););
       MESSAGE(11,fprintf(fp,"\n"););
           for( i = 0; i < 4 ; i++ )
                 {
                   MESSAGE(11,fprintf(fp,"\t\t            %f \n", attrvalOut[ i ] ););
                    MESSAGE(8,printf("\t\t               %f \n", attrvalOut[ i ] ););
                 }
         }

/*
	*********************************************************************************
	*                             Testing   HE5_GDgetfillvalue                      *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDgetfillvalue... \n"););
   MESSAGE(4,printf("\t============================= \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDgetfillvalue... \n"););   
   MESSAGE(11,fprintf(fp,"\t============================= \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving the fill value for the \"Voltage\" data field \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving the fill value for the \"Voltage\" data field \n"););
   
   status = HE5_GDgetfillvalue(GDid_UTM, "Voltage", &fillvalue_inq);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDgetfillvalue(GDid_UTM, \"Voltage\", &fillvalue_inq);\n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDgetfillvalue()           %d\n", status););
       MESSAGE(8,printf("\t\tThe fill value for field \"Voltage\" :\n"););
       MESSAGE(8,printf("\t\t\t      \"%f\" \n", fillvalue_inq ););

       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDgetfillvalue(GDid_UTM, \"Voltage\", &fillvalue_inq);\n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDgetfillvalue()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\tThe fill value for field \"Voltage\" :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t     \"%f\" \n", fillvalue_inq ););
	 }
   
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDgetpixels                         *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDgetpixels... \n"););
   MESSAGE(4,printf("\t========================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDgetpixels... \n"););   
   MESSAGE(11,fprintf(fp,"\t========================== \n"););
   
   MESSAGE(6,printf("\t\tRetrieving the pixel rows and columns for specified longitude and latitude pairs  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving the pixel rows and columns for specified longitude and latitude pairs  \n"););
   
   LonVal[0] = -130.0;
   LatVal[0] = 45.5;
   
   LonVal[1] = -128.0;
   LatVal[1] = 44.5;
   
   LonVal[2] = -127.0;
   LatVal[2] = 43.5;
   
   LonVal[3] = -126.0;
   LatVal[3] = 42.5;
   
   LonVal[4] = -125.0;
   LatVal[4] = 41.5;
   
   
   NumPix = 5;
   
   RowVal = (long *)calloc(NumPix, sizeof(long));
   ColVal = (long *)calloc(NumPix, sizeof(long));
   
   for (i =0; i < NumPix; i++)
	 {
       RowVal[i] = -1;
       ColVal[i] = -1; 
	 }
   
   
   status = HE5_GDgetpixels(GDid_LAMCON, NumPix, LonVal, LatVal, RowVal, ColVal);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDgetpixels(GDid_LAMCON, NumPix, LonVal, LatVal, RowVal, ColVal); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDgetpixels()           %d\n", status););
       MESSAGE(8,printf("\t\t    Row        Column \n"););
       for (i = 0; i < NumPix; i++)
		 {
		   MESSAGE(8,printf("\t\t    %li         %li \n", RowVal[i], ColVal[i]););
		 }

       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDgetpixels(GDid_LAMCON, NumPix, LonVal, LatVal, RowVal, ColVal); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDgetpixels()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t    Row        Column \n"););
       for (i = 0; i < NumPix; i++)
		 {
		   MESSAGE(11,fprintf(fp,"\t\t    %li         %li \n", RowVal[i], ColVal[i]););
		 }
       
	 }
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDgetpixvalues                      *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDgetpixvalues... \n"););
   MESSAGE(4,printf("\t============================= \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDgetpixvalues... \n"););   
   MESSAGE(11,fprintf(fp,"\t============================= \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving the pixel values for specified row and column  pairs  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving the pixel values for specified row and column pairs  \n"););
   
   
   NumPix = 3;
   
   RowVal[0] = 1;
   ColVal[0] = 1;
   
   RowVal[1] = 1;
   ColVal[1] = 2;
   
   RowVal[2] = 1;
   ColVal[2] = 3;
   
   DatBuf = (float *)calloc(NumPix, sizeof(float));
   
   
   BufSize = HE5_GDgetpixvalues(GDid_LAMCON,NumPix,RowVal,ColVal,"Bypass",NULL);
   if (BufSize == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tBufSize = HE5_GDgetpixvalues(GDid_LAMCON,NumPix,RowVal,ColVal,\"Bypass\",NULL); \n"););
       MESSAGE(8,printf("\t\tThe buffer size (in bytes) returned by HE5_GDgetpixvalues(): \n"););
       MESSAGE(8,printf("\t\t    %li   (bytes)  \n", BufSize););

       MESSAGE(11,fprintf(fp,"\t\tBufSize = HE5_GDgetpixvalues(GDid_LAMCON,NumPix,RowVal,ColVal,\"Bypass\",NULL); \n"););
       MESSAGE(11,fprintf(fp,"\t\tThe buffer size (in bytes) returned by HE5_GDgetpixvalues(): \n"););
       MESSAGE(11,fprintf(fp,"\t\t    %li   (bytes)  \n", BufSize););

       MESSAGE(6,printf("\t\n"););
       MESSAGE(6,printf("\t\n"););
       MESSAGE(6,printf("\t\tRetrieving the data  \n"););  
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\t\n");); 
       MESSAGE(11,fprintf(fp,"\t\t Retrieving the data  \n"););

       BufSize = HE5_GDgetpixvalues(GDid_LAMCON,NumPix,RowVal,ColVal,"Bypass",DatBuf);
       if (BufSize == FAIL)
		 {
		   MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
		   MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
		 }
       else
		 {   
           MESSAGE(4,printf("\n"););   
		   MESSAGE(4,printf("\t\t********Passed Test**********\n"););
		   MESSAGE(11,fprintf(fp,"\n"););
		   MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

		   MESSAGE(10,printf("\t\tBufSize = HE5_GDgetpixvalues(GDid_LAMCON,NumPix,RowVal,ColVal,\"Bypass\",DatBuf);\n"););
		   MESSAGE(8,printf("\t\tBuffer values returned by HE5_GDgetpixvalues(): \n"););
		   for (i = 0; i < NumPix; i++)
			 {
			   MESSAGE(8,printf("\t\t     %f \n", DatBuf[i]););
			 }

		   MESSAGE(11,fprintf(fp,"\t\tBufSize = HE5_GDgetpixvalues(GDid_LAMCON,NumPix,RowVal,ColVal,\"Bypass\",DatBuf);\n"););
		   MESSAGE(11,fprintf(fp,"\t\tBuffer values returned by HE5_GDgetpixvalues(): \n"););
		   for (i = 0; i < NumPix; i++)
			 {
			   MESSAGE(11,fprintf(fp,"\t\t     %f \n", DatBuf[i]););
			 }
	    
		 }
  
	 }
   
   
   
   free(DatBuf);
   free(RowVal);
   free(ColVal);
   
   
   /*
	  -----------------------------------------------------------
	  |         R E G I O N    S U B S E T T I N G              |
	  -----------------------------------------------------------
	  */
   
   
   CorLon[0] = -150.0;
   CorLon[1] = 120.0;
   CorLat[0] = 60.0;
   CorLat[1] = -40.0;
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdefboxregion                      *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDdefboxregion... \n"););
   MESSAGE(4,printf("\t============================= \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdefboxregion... \n"););   
   MESSAGE(11,fprintf(fp,"\t============================= \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining the box region for the grid lamcon  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining the box region for the grid lamcon \n"););
   
   RegionID = HE5_GDdefboxregion(GDid_LAMCON, CorLon, CorLat);
   if (RegionID == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tRegionID = HE5_GDdefboxregion(GDid_LAMCON, CorLon, CorLat); \n"););
       MESSAGE(8,printf("\t\tRegion ID returned by HE5_GDdefboxregion()           %d\n", RegionID););

       MESSAGE(11,fprintf(fp,"\t\tRegionID = HE5_GDdefboxregion(GDid_LAMCON, CorLon, CorLat); \n"););
       MESSAGE(11,fprintf(fp,"\t\tRegion ID returned by HE5_GDdefboxregion()           %d\n", RegionID););
	 }
   
   
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDregioninfo                        *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDregioninfo... \n"););
   MESSAGE(4,printf("\t=========================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDregioninfo... \n"););   
   MESSAGE(11,fprintf(fp,"\t=========================== \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving information about defined region  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about defined region \n"););
   
   Rank = FAIL; list_size = FAIL;
   ntype1 = (hid_t *)calloc(1, sizeof(hid_t));
   
   status = HE5_GDregioninfo(GDid_LAMCON, RegionID, "Bypass", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus =   GDregioninfo(GDid_LAMCON, RegionID, \"Bypass\", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDregioninfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\tData type of region \"Bypass\" :\n"););
       MESSAGE(8,printf("\t\t\t    %i\n", (int)*ntype1););
       MESSAGE(8,printf("\t\t\tRank of the region :\n"););
       MESSAGE(8,printf("\t\t\t    %d\n", Rank););
       MESSAGE(8,printf("\t\t\tSize of the region :\n"););
       MESSAGE(8,printf("\t\t\t    %li  (bytes)\n", list_size););
       MESSAGE(8,printf("\t\t\tDimensions of the region : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(8,printf("\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(8,printf("\t\t\tCoordinates of the UL corner of the region : \n"););
       MESSAGE(8,printf("\t\t\t    %lf      %lf \n", uplft_out[0], uplft_out[1] ););
       MESSAGE(8,printf("\t\t\tCoordinates of the LR corner of the region : \n"););
       MESSAGE(8,printf("\t\t\t    %lf      %lf \n", lowrgt_out[0], lowrgt_out[1] ););

       MESSAGE(11,fprintf(fp,"\t\tstatus =   GDregioninfo(GDid_LAMCON, RegionID, \"Bypass\", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDregioninfo()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\tData type of region \"Bypass\" :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %i\n", (int)*ntype1););
       MESSAGE(11,fprintf(fp,"\t\t\tRank of the region :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d\n", Rank););
       MESSAGE(11,fprintf(fp,"\t\t\tSize of the region :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %li  (bytes)\n", list_size););
       MESSAGE(11,fprintf(fp,"\t\t\tDimensions of the region : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(11,fprintf(fp,"\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(11,fprintf(fp,"\t\t\tCoordinates of the UL corner of the region : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %lf      %lf \n", uplft_out[0], uplft_out[1] ););
       MESSAGE(11,fprintf(fp,"\t\t\tCoordinates of the LR corner of the region : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %lf      %lf \n", lowrgt_out[0], lowrgt_out[1] ););
	 }
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDextractregion                     *
	*********************************************************************************
	*/
   
          MESSAGE(4,printf("\tTesting HE5_GDextractregion... \n"););
          MESSAGE(4,printf("\t============================== \n"););
   
          MESSAGE(11,fprintf(fp,"\t\n"););
          MESSAGE(11,fprintf(fp,"\tTesting HE5_GDextractregion... \n"););   
          MESSAGE(11,fprintf(fp,"\t============================== \n"););
   
          MESSAGE(6,printf("\n"););   
          MESSAGE(6,printf("\t\tRead data into a buffer from a subsetted region  \n"););   
          MESSAGE(11,fprintf(fp,"\t\n"););
          MESSAGE(11,fprintf(fp,"\t\tRead data into a buffer from a subsetted region \n"););
   
          DatBuf = (float *)malloc(list_size);
          status = HE5_GDextractregion(GDid_LAMCON, RegionID, "Bypass", DatBuf);
         if (status == FAIL)
	 {
         MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDextractregion(GDid_LAMCON, RegionID, \"Bypass\", DatBuf); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDextractregion()           %d\n", status););
       MESSAGE(8,printf("\t\tThe first ten values from region \"Bypass\" are: \n"););
       for (i = 0; i < 10; i++)
		 {
		   MESSAGE(8,printf("\t\t    %f \n", DatBuf[i]););
		 }
    
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDextractregion(GDid_LAMCON, RegionID, \"Bypass\", DatBuf); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDextractregion()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\tThe first ten values from region \"Bypass\" are: \n"););
       for (i = 0; i < 10; i++)
		 {
		   MESSAGE(11,fprintf(fp,"\t\t    %f \n", DatBuf[i]););
		 }

	 }
   
   free(DatBuf);
   free(ntype1);
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdupregion                         *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDdupregion... \n"););
   MESSAGE(4,printf("\t========================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdupregion... \n"););   
   MESSAGE(11,fprintf(fp,"\t========================== \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDuplicating  the box region \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDuplicating the box region \n"););
   
   NewRegionID = HE5_GDdupregion(RegionID);
   if (NewRegionID == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tNewRegionID = HE5_GDdupregion(RegionID); \n"););
       MESSAGE(8,printf("\t\tNew region ID returned by HE5_GDdupregion()           %d\n", NewRegionID););
       MESSAGE(11,fprintf(fp,"\t\tNewRegionID = HE5_GDdupregion(RegionID); \n"););
       MESSAGE(11,fprintf(fp,"\t\tNew region ID returned by HE5_GDdupregion()           %d\n", NewRegionID););
	 }
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdefboxregion                      *
	*********************************************************************************
	*/
   
   
   CorLon[ 0 ] = -175.;
   CorLon[ 1 ] =  175.;
   CorLat[ 0 ] =   85.;
   CorLat[ 1 ] =  -85.;
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining the box region for the grid MercatGrid  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining the box region for the grid MercatGrid \n"););
   
   RegionID = HE5_GDdefboxregion(GDid_MERCAT, CorLon, CorLat);
   if (RegionID == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tRegionID = HE5_GDdefboxregion(GDid_MERCAT, CorLon, CorLat); \n"););
       MESSAGE(8,printf("\t\tRegion ID returned by HE5_GDdefboxregion()           %d\n", RegionID););

       MESSAGE(11,fprintf(fp,"\t\tRegionID = HE5_GDdefboxregion(GDid_MERCAT, CorLon, CorLat); \n"););
       MESSAGE(11,fprintf(fp,"\t\tRegion ID returned by HE5_GDdefboxregion()           %d\n", RegionID););
	 }
   
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDregioninfo                        *
	*********************************************************************************
	*/
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving information about defined region  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about defined region \n"););
   
   Rank = FAIL;
   list_size = FAIL;
   ntype1 = (hid_t *)calloc(1, sizeof(hid_t));
   
   status = HE5_GDregioninfo(GDid_MERCAT, RegionID, "VerticalD", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus =   GDregioninfo(GDid_MERCAT, RegionID, \"VerticalD\", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDregioninfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\tData type of region \"VerticalD\" :\n"););
       MESSAGE(8,printf("\t\t\t    %i\n", (int)*ntype1););
       MESSAGE(8,printf("\t\t\tRank of the region :\n"););
       MESSAGE(8,printf("\t\t\t    %d\n", Rank););
       MESSAGE(8,printf("\t\t\tSize of the region :\n"););
       MESSAGE(8,printf("\t\t\t    %li  (bytes)\n", list_size););
       MESSAGE(8,printf("\t\t\tDimensions of the region : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(8,printf("\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(8,printf("\t\t\tCoordinates of the UL corner of the region : \n"););
       MESSAGE(8,printf("\t\t\t    %lf      %lf \n", uplft_out[0], uplft_out[1] ););
       MESSAGE(8,printf("\t\t\tCoordinates of the LR corner of the region : \n"););
       MESSAGE(8,printf("\t\t\t    %lf      %lf \n", lowrgt_out[0], lowrgt_out[1] ););

       MESSAGE(11,fprintf(fp,"\t\tstatus =   GDregioninfo(GDid_MERCAT, RegionID, \"VerticalD\", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDregioninfo()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\tData type of region \"VerticalD\" :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %i\n", (int)*ntype1););
       MESSAGE(11,fprintf(fp,"\t\t\tRank of the region :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d\n", Rank););
       MESSAGE(11,fprintf(fp,"\t\t\tSize of the region :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %li  (bytes)\n", list_size););
       MESSAGE(11,fprintf(fp,"\t\t\tDimensions of the region : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(11,fprintf(fp,"\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(11,fprintf(fp,"\t\t\tCoordinates of the UL corner of the region : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %lf      %lf \n", uplft_out[0], uplft_out[1] ););
       MESSAGE(11,fprintf(fp,"\t\t\tCoordinates of the LR corner of the region : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %lf      %lf \n", lowrgt_out[0], lowrgt_out[1] ););
	 }
   

   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDextractregion                     *
	*********************************************************************************
	*/
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRead data into a buffer from a subsetted region  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tRead data into a buffer from a subsetted region \n"););
   
   DatBuf = (float *)malloc(list_size);
   status = HE5_GDextractregion(GDid_MERCAT, RegionID, "VerticalD", DatBuf);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDextractregion(GDid_MERCAT, RegionID, \"VerticalD\", DatBuf); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDextractregion()           %d\n", status););
       MESSAGE(8,printf("\t\tThe first ten values from region \"VerticalD\" are: \n"););
       for (i = 0; i < 10; i++)
		 {
		   MESSAGE(8,printf("\t\t    %f \n", DatBuf[i]););
		 }
    
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDextractregion(GDid_MERCAT, RegionID, \"VerticalD\", DatBuf); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDextractregion()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\tThe first ten values from region \"VerticalD\" are: \n"););
       for (i = 0; i < 10; i++)
		 {
		   MESSAGE(11,fprintf(fp,"\t\t    %f \n", DatBuf[i]););
		 }

	 }
   
   free(DatBuf);
   free(ntype1);
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdefboxregion                      *
	*********************************************************************************
	*/
   
   
   CorLon[ 0 ] = -175.;
   CorLon[ 1 ] =  175.;
   CorLat[ 0 ] =   85.;
   CorLat[ 1 ] =  -85.;
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining the box region for the grid AlbersGrid  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining the box region for the grid AlbersGrid \n"););
   
   RegionID = HE5_GDdefboxregion(GDid_ALBERS, CorLon, CorLat);
   if (RegionID == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tRegionID = HE5_GDdefboxregion(GDid_ALBERS, CorLon, CorLat); \n"););
       MESSAGE(8,printf("\t\tRegion ID returned by HE5_GDdefboxregion()           %d\n", RegionID););

       MESSAGE(11,fprintf(fp,"\t\tRegionID = HE5_GDdefboxregion(GDid_ALBERS, CorLon, CorLat); \n"););
       MESSAGE(11,fprintf(fp,"\t\tRegion ID returned by HE5_GDdefboxregion()           %d\n", RegionID););
	 }
   
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDregioninfo                        *
	*********************************************************************************
	*/
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving information about defined region  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about defined region \n"););
   
   Rank = FAIL;
   list_size = FAIL;
   ntype1 = (hid_t *)calloc(1, sizeof(hid_t));
   
   status = HE5_GDregioninfo(GDid_ALBERS, RegionID, "MaximumDepth", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus =   GDregioninfo(GDid_ALBERS, RegionID, \"MaximumDepth\", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDregioninfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\tData type of region \"MaximumDepth\" :\n"););
       MESSAGE(8,printf("\t\t\t    %i\n", (int)*ntype1););
       MESSAGE(8,printf("\t\t\tRank of the region :\n"););
       MESSAGE(8,printf("\t\t\t    %d\n", Rank););
       MESSAGE(8,printf("\t\t\tSize of the region :\n"););
       MESSAGE(8,printf("\t\t\t    %li  (bytes)\n", list_size););
       MESSAGE(8,printf("\t\t\tDimensions of the region : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(8,printf("\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(8,printf("\t\t\tCoordinates of the UL corner of the region : \n"););
       MESSAGE(8,printf("\t\t\t    %lf      %lf \n", uplft_out[0], uplft_out[1] ););
       MESSAGE(8,printf("\t\t\tCoordinates of the LR corner of the region : \n"););
       MESSAGE(8,printf("\t\t\t    %lf      %lf \n", lowrgt_out[0], lowrgt_out[1] ););

       MESSAGE(11,fprintf(fp,"\t\tstatus =   GDregioninfo(GDid_ALBERS, RegionID, \"MaximumDepth\", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDregioninfo()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\tData type of region \"MaximumDepth\" :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %i\n", (int)*ntype1););
       MESSAGE(11,fprintf(fp,"\t\t\tRank of the region :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d\n", Rank););
       MESSAGE(11,fprintf(fp,"\t\t\tSize of the region :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %li  (bytes)\n", list_size););
       MESSAGE(11,fprintf(fp,"\t\t\tDimensions of the region : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(11,fprintf(fp,"\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(11,fprintf(fp,"\t\t\tCoordinates of the UL corner of the region : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %lf      %lf \n", uplft_out[0], uplft_out[1] ););
       MESSAGE(11,fprintf(fp,"\t\t\tCoordinates of the LR corner of the region : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %lf      %lf \n", lowrgt_out[0], lowrgt_out[1] ););
	 }
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDextractregion                     *
	*********************************************************************************
	*/
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRead data into a buffer from a subsetted region  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tRead data into a buffer from a subsetted region \n"););
   
   DatBuf = (float *)malloc(list_size);
   status = HE5_GDextractregion(GDid_ALBERS, RegionID, "MaximumDepth", DatBuf);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDextractregion(GDid_ALBERS, RegionID, \"MaximumDepth\", DatBuf); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDextractregion()           %d\n", status););
       MESSAGE(8,printf("\t\tThe first ten values from region \"MaximumDepth\" are: \n"););
       for (i = 0; i < 10; i++)
		 {
		   MESSAGE(8,printf("\t\t    %f \n", DatBuf[i]););
		 }
    
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDextractregion(GDid_ALBERS, RegionID, \"MaximumDepth\", DatBuf); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDextractregion()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\tThe first ten values from region \"MaximumDepth\" are: \n"););
       for (i = 0; i < 10; i++)
		 {
		   MESSAGE(11,fprintf(fp,"\t\t    %f \n", DatBuf[i]););
		 }

	 }
   
   free(DatBuf);
   free(ntype1);
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdeftimeperiod                     *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDdeftimeperiod... \n"););
   MESSAGE(4,printf("\t============================== \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdeftimeperiod... \n"););   
   MESSAGE(11,fprintf(fp,"\t============================== \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tDefining a time period for the grid UTM  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining a time period for the grid UTM \n"););
   
   StartTime = 35233000.8;
   StopTime  = 35435050.9;
   
   
   PeriodID = HE5_GDdeftimeperiod(GDid_UTM, HE5_HDFE_NOPREVSUB, StartTime,StopTime);
   if (PeriodID == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\t PeriodID = HE5_GDdeftimeperiod(GDid_UTM, HE5_HDFE_NOPREVSUB, StartTime,StopTime);\n"););
       MESSAGE(8,printf("\t\tPeriod ID returned by HE5_GDdeftimeperiod()           %d\n", PeriodID););
       MESSAGE(11,fprintf(fp,"\t\t PeriodID = HE5_GDdeftimeperiod(GDid_UTM, HE5_HDFE_NOPREVSUB, StartTime,StopTime);\n"););
       MESSAGE(11,fprintf(fp,"\t\tPeriod ID returned by HE5_GDdeftimeperiod()           %d\n", PeriodID););
	 }
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDinterpolate                       *
	*********************************************************************************
	*/
   
   MESSAGE(4,printf("\tTesting HE5_GDinterpolate... \n"););
   MESSAGE(4,printf("\t============================ \n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDinterpolate... \n"););   
   MESSAGE(11,fprintf(fp,"\t============================ \n"););
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tPerforming bilinear interpolation on a grid field  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Performing bilinear interpolation on a grid field\n"););
   
   LonVal[0] = -129.5;
   LatVal[0] =  45.8;
   
   LonVal[1] = -128.5;
   LatVal[1] =  44.8;
   
   LonVal[2] = -127.5;
   LatVal[2] =  43.8;
   
   LonVal[3] = -126.5;
   LatVal[3] =  42.8;
   
   LonVal[4] = -125.5;
   LatVal[4] =  41.8;
   
   BufSize = HE5_GDinterpolate(GDid_LAMCON,5,LonVal,LatVal,"Bypass",NULL);
   if (BufSize == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tBufSize = HE5_GDinterpolate(GDid_LAMCON,5,LonVal,LatVal,\"Bypass\",NULL);\n"););
       MESSAGE(8,printf("\t\tSize of interpolated data values returned by HE5_GDinterpolate(): \n"););
       MESSAGE(8,printf("\t\t    %li (bytes) \n", BufSize););

       MESSAGE(11,fprintf(fp,"\t\tBufSize = HE5_GDinterpolate(GDid_LAMCON,5,LonVal,LatVal,\"Bypass\",NULL);\n"););
       MESSAGE(11,fprintf(fp,"\t\tSize of interpolated data values returned by HE5_GDinterpolate(): \n"););
       MESSAGE(11,fprintf(fp,"\t\t    %li (bytes) \n", BufSize););


	   DatBuf2 = (double *)malloc(BufSize);
	   for ( i = 0; i < BufSize/sizeof(double); i++)
		 {
		   DatBuf2[ i ] = - 1.111111111 ;
		 }

       MESSAGE(6,printf("\t\n"););
       MESSAGE(6,printf("\t\n"););
       MESSAGE(6,printf("\t\tContinue bilinear interpolation on a grid field \n");); 
       MESSAGE(11,fprintf(fp,"\t\n"););
       MESSAGE(11,fprintf(fp,"\t\n");); 
       MESSAGE(11,fprintf(fp,"\t\t Continue bilinear interpolation on a grid field\n"););

	   BufSize = HE5_GDinterpolate(GDid_LAMCON,5,LonVal,LatVal,"Bypass",DatBuf2);
	   if (BufSize == FAIL)
		 {
           MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
           MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
		 }
	   else
		 {   
		   MESSAGE(4,printf("\n"););   
		   MESSAGE(4,printf("\t\t********Passed Test**********\n"););
		   MESSAGE(11,fprintf(fp,"\n"););
		   MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

		   MESSAGE(10,printf("\t\tBufSize = HE5_GDinterpolate(GDid_LAMCON,5,LonVal,LatVal,\"Bypass\",DatBuf2);\n"););
		   MESSAGE(8,printf("\t\tBuffer values returned by HE5_GDinterpolate(): \n"););
		   for (i = 0; i < BufSize/sizeof(double); i++)
			 {
			   MESSAGE(8,printf("\t\t   %lf\n", DatBuf2[i]););
			 }

		   MESSAGE(11,fprintf(fp,"\t\tBufSize = HE5_GDinterpolate(GDid_LAMCON,5,LonVal,LatVal,\"Bypass\",DatBuf2);\n"););
		   MESSAGE(11,fprintf(fp,"\t\tBuffer values returned by HE5_GDinterpolate():  \n"););
		   for (i = 0; i < BufSize/sizeof(double); i++)
			 {
			   MESSAGE(11,fprintf(fp,"\t\t   %lf\n", DatBuf2[i]););
			 }
		 }

	   free(DatBuf2);
	 }
   
   
   
   
   /*
	*********************************************************************************
	*                             Testing   HE5_GDdetach                            *
	*********************************************************************************
	*/
   
   
   MESSAGE(4,printf("\tTesting HE5_GDdetach... \n"););
   MESSAGE(4,printf("\t=======================\n"););
   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDdetach... \n"););   
   MESSAGE(11,fprintf(fp,"\t=======================\n"););
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid UTM3  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid UTM3 \n"););
   
   status = HE5_GDdetach(GDid_UTM3);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid UTM3 **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid UTM3 **\n"););
	 }
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tAttaching to the grid UTM3  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tAttaching to the grid UTM3  \n"););
   
   GDid_UTM3  = HE5_GDattach(gdfidc_simple,"UTM3"); 
   if (GDid_UTM == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully attached to the grid UTM3 **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid UTM3 **\n"););
	 }
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tWriting appendable field \"Vegetation\" to the grid UTM3  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tWriting appendable field \"Vegetation\" to the grid UTM3  \n"););
   
   
   start[0]  = 100;
   start[1]  = 200;
   
   edge[0]   = 200;
   edge[1]   = 120;
   
   status = HE5_GDwritefield(GDid_UTM3, "Vegetation", start, NULL, edge, veg);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
       MESSAGE(10,printf("\t\tstatus = HE5_GDwritefield(GDid_UTM3, \"Vegetation\", start, NULL, edge, veg); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDwritefield()           %d\n",status););
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDwritefield(GDid_UTM3, \"Vegetation\", start, NULL, edge, veg); \n"););
	   MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDwritefield()          %d\n",status););
	 } 
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid UTM3  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid UTM3 \n"););
   
   status = HE5_GDdetach(GDid_UTM3);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid UTM3 **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid UTM3 **\n"););
	 }
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tAttaching to the grid UTM3  \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tAttaching to the grid UTM3  \n"););
   
   GDid_UTM3  = HE5_GDattach(gdfidc_simple,"UTM3"); 
   if (GDid_UTM == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully attached to the grid UTM3 **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully attached to the grid UTM3 **\n"););
	 }
   ntype1 = (hid_t *)calloc(2, sizeof(hid_t)); 
   str_buf2  = (char *)calloc(256, sizeof(char));
   
   strcpy(str_buf, "NO ENTRY !");
   strcpy(str_buf2, "NO ENTRY !");
   
   Rank = FAIL;
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tRetrieving information about appendable \"Vegetation\" field \n"););   
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about appendable \"Vegetation\" field \n"););
   
   status = HE5_GDfieldinfo(GDid_UTM3, "Vegetation", &Rank, dims, ntype1, str_buf, str_buf2);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus = HE5_GDfieldinfo(GDid_UTM3, \"Vegetation\", &Rank,dims, ntype1, str_buf, str_buf2);  \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDfieldinfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\tRank of the field \"Vegetation\" :\n"););
       MESSAGE(8,printf("\t\t\t    %d\n", Rank););
       MESSAGE(8,printf("\t\t\tDimension list for the field \"Vegetation\" : \n"););
       MESSAGE(8,printf("\t\t\t    %s \n", str_buf););
       MESSAGE(8,printf("\t\t\tMaximum Dimension list for the field \"Vegetation\" : \n"););
       MESSAGE(8,printf("\t\t\t    %s \n", str_buf2););
       MESSAGE(8,printf("\t\t\tDimensions of the field \"Vegetation\" : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(8,printf("\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(8,printf("\t\t\tData type of the field \"Vegetation\" : \n"););
       MESSAGE(8,printf("\t\t\t    %d \n", (int)ntype1[0]););
       
       MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDfieldinfo(GDid_UTM3, \"Vegetation\",&Rank, dims, ntype1, str_buf, str_buf2);  \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDfieldinfo() %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\tRank of the field \"Vegetation\" :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d\n", Rank););
       MESSAGE(11,fprintf(fp,"\t\t\tDimension list for the field \"Vegetation\" : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %s \n", str_buf););
       MESSAGE(11,fprintf(fp,"\t\t\tMaximum Dimension list for the field \"Vegetation\" : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %s \n", str_buf2););
       MESSAGE(11,fprintf(fp,"\t\t\tDimensions of the field \"Vegetation\" : \n"););
       for (i = 0; i < Rank; i++ )
		 {
		   MESSAGE(11,fprintf(fp,"\t\t\t    %lu \n", (unsigned long)dims[i]););
		 }
       MESSAGE(11,fprintf(fp,"\t\t\tData type of the field \"Vegetation\" : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d \n", (int)ntype1[0]););
	 }
   
   
   free(ntype1);
   free(str_buf2);
   
   

   MESSAGE(6,printf("\n"););
   MESSAGE(6,printf("\t\tDefining the box region for the grid EASE \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tDefining the box region for the grid EASE \n"););

   CorLon[ 0 ] = -180.0;
   CorLon[ 1 ] =  0.0;
   CorLat[ 0 ] =  86.72;
   CorLat[ 1 ] =  0.0;

   RegionID = HE5_GDdefboxregion(GDid_EASE, CorLon, CorLat);
   if (RegionID == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tRegionID = HE5_GDdefboxregion(GDid_EASE, CorLon, CorLat); \n"););
       MESSAGE(8,printf("\t\tRegion ID returned by HE5_GDdefboxregion()           %d\n", RegionID););

       MESSAGE(11,fprintf(fp,"\t\tRegionID = HE5_GDdefboxregion(GDid_EASE, CorLon, CorLat); \n"););
       MESSAGE(11,fprintf(fp,"\t\tRegion ID returned by HE5_GDdefboxregion()           %d\n", RegionID););
         }


   MESSAGE(6,printf("\n"););
   MESSAGE(6,printf("\t\tRetrieving information about defined region  \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\t Retrieving information about defined region \n"););

   Rank = FAIL;
   list_size = FAIL;
   ntype1 = (hid_t *)calloc(1, sizeof(hid_t));

   status = HE5_GDregioninfo(GDid_EASE, RegionID, "Intensity", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t********Passed Test**********\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););

       MESSAGE(10,printf("\t\tstatus =   GDregioninfo(GDid_EASE, RegionID, \"Intensity\", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out); \n"););
       MESSAGE(8,printf("\t\tStatus returned by HE5_GDregioninfo()           %d\n", status););
       MESSAGE(8,printf("\t\t\tData type of region \"Intensity\" :\n"););
       MESSAGE(8,printf("\t\t\t    %i\n", (int)*ntype1););
       MESSAGE(8,printf("\t\t\tRank of the region :\n"););
       MESSAGE(8,printf("\t\t\t    %d\n", Rank););
       MESSAGE(8,printf("\t\t\tSize of the region :\n"););
       MESSAGE(8,printf("\t\t\t    %li  (bytes)\n", list_size););
       MESSAGE(8,printf("\t\t\tDimensions of the region : \n"););
       for (i = 0; i < Rank; i++ )
                 {
                   MESSAGE(8,printf("\t\t\t    %lu \n", (unsigned long)dims[i]););
                 }
       MESSAGE(8,printf("\t\t\tCoordinates of the UL corner of the region : \n"););
       MESSAGE(8,printf("\t\t\t    %lf      %lf \n", uplft_out[0], uplft_out[1] ););
       MESSAGE(8,printf("\t\t\tCoordinates of the LR corner of the region : \n"););
       MESSAGE(8,printf("\t\t\t    %lf      %lf \n", lowrgt_out[0], lowrgt_out[1] ););

       MESSAGE(11,fprintf(fp,"\t\tstatus =   GDregioninfo(GDid_EASE, RegionID, \"Intensity\", ntype1, &Rank, dims,&list_size, uplft_out, lowrgt_out); \n"););
       MESSAGE(11,fprintf(fp,"\t\tStatus returned by HE5_GDregioninfo()           %d\n", status););
       MESSAGE(11,fprintf(fp,"\t\t\tData type of region \"Intensity\" :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %i\n", (int)*ntype1););
       MESSAGE(11,fprintf(fp,"\t\t\tRank of the region :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %d\n", Rank););
       MESSAGE(11,fprintf(fp,"\t\t\tSize of the region :\n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %li  (bytes)\n", list_size););
       MESSAGE(11,fprintf(fp,"\t\t\tDimensions of the region : \n"););
       for (i = 0; i < Rank; i++ )
                 {
                   MESSAGE(11,fprintf(fp,"\t\t\t    %lu \n", (unsigned long)dims[i]););
                 }
       MESSAGE(11,fprintf(fp,"\t\t\tCoordinates of the UL corner of the region : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %lf      %lf \n", uplft_out[0], uplft_out[1] ););
       MESSAGE(11,fprintf(fp,"\t\t\tCoordinates of the LR corner of the region : \n"););
       MESSAGE(11,fprintf(fp,"\t\t\t    %lf      %lf \n", lowrgt_out[0], lowrgt_out[1] ););
         }

   free(ntype1);



   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid UTM3  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid UTM3 \n"););
   
   status = HE5_GDdetach(GDid_UTM3);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid UTM3 **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid UTM3 **\n"););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid MercatGrid  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid MercatGrid \n"););
   
   status = HE5_GDdetach(GDid_MERCAT);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid MercatGrid **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid MercatGrid **\n"););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid AlbersGrid  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid AlbersGrid \n"););
   
   status = HE5_GDdetach(GDid_ALBERS);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid AlbersGrid **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid AlbersGrid **\n"););
	 }
   
   
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid EASE  \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n");); 
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid EASE \n"););
  
   status = HE5_GDdetach(GDid_EASE);
   if (status == FAIL)
         {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
         }
   else
         {
       MESSAGE(4,printf("\n"););
       MESSAGE(4,printf("\t\t** Successfully detached from the grid EASE **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid EASE **\n"););
         }


   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\n"););
   MESSAGE(6,printf("\t\tDetaching from the grid Lamcon  \n");); 
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););  
   MESSAGE(11,fprintf(fp,"\t\tDetaching from the grid Lamcon \n"););
   
   status = HE5_GDdetach(GDid_LAMCON);
   if (status == FAIL)
	 {
       MESSAGE(4,printf("\t\t********Error unexpected**********\n"););   
       MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););          
       errcnt++;
	 }
   else
	 {   
       MESSAGE(4,printf("\n"););   
       MESSAGE(4,printf("\t\t** Successfully detached from the grid Lamcon **\n"););
       MESSAGE(11,fprintf(fp,"\n"););
       MESSAGE(11,fprintf(fp,"\t\t** Successfully detached from the grid Lamcon **\n"););
	 }
   
   
/* Test Case -- GDrs2ll */

   uplft[0] = HE5_EHconvAng(-180.0, HE5_HDFE_DEG_DMS);
   uplft[1] = HE5_EHconvAng(86.72, HE5_HDFE_DEG_DMS);
   lowrgt[0] = HE5_EHconvAng( 180.0,HE5_HDFE_DEG_DMS);
   lowrgt[1] = HE5_EHconvAng( -86.72,HE5_HDFE_DEG_DMS);
   npnts = 4;
   rcord[0] = 0.;
   scord[0] = 0.;
   rcord[1] = 0.;
   scord[1] = 293.;
   rcord[2] = 691.5;
   scord[2] = 0.;
   rcord[3] = 691.5;
   scord[3] = 293.;
   xdimsize = xdim_ease;
   ydimsize = ydim_ease;
   MESSAGE(4,printf("\tTesting GDrs2ll for EASE grid... \n"););
   MESSAGE(4,printf("\t================================ \n"););
   MESSAGE(11,fprintf(fp,"\f\n"););
   MESSAGE(11,fprintf(fp,"\tTesting GDrs2ll for EASE grid... \n"););
   MESSAGE(11,fprintf(fp,"\t================================ \n"););
   status = HE5_GDrs2ll(HE5_GCTP_BCEA, ProjParm, xdimsize, ydimsize, uplft, lowrgt, npnts, rcord, scord, longitude, latitude, HE5_HDFE_CORNER, HE5_HDFE_GD_UL);
   if (status == FAIL)
   {
      MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
      MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
      errcnt++;
   }
   else
   {
      MESSAGE(4,printf("\n"););
      MESSAGE(4,printf("\t\t********Passed Test**********\n"););
      MESSAGE(11,fprintf(fp,"\n"););
      MESSAGE(11,fprintf(fp,"\t\t********Passed Test**********\n"););
      MESSAGE(10,printf("\t\tstatus = HE5_GDrs2ll(HE5_GCTP_BCEA, ProjParm, xdimsize, ydimsize, npnts, scord, rcord, longitude, latitude, HE5_HDFE_CORNER, HE5_HDFE_GD_UL);\n"););
      MESSAGE(8,printf("\t\tValue returned by HE5_GDrs2ll %d\n\n", status););
      MESSAGE(11,fprintf(fp,"\t\tstatus = HE5_GDrs2ll(HE5_GCTP_BCEA, ProjParm, xdimsize, ydimsize, npnts, scord, rcord, longitude, latitude, HE5_HDFE_CORNER, HE5_HDFE_GD_UL);\n"););
      MESSAGE(11,fprintf(fp,"\t\tValue returned by HE5_GDrs2ll %d\n\n", status););
      for(i = 0; i < 4; i++)
      {
            
             MESSAGE(8,printf("\t\tr[%d]=%f s[%d]=%f lon[%d]=%f lat[%d]=%f\n", i, rcord[i], i, scord[i], i, longitude[i], i, latitude[i]););
             MESSAGE(11,printf("\t\tr[%d]=%f s[%d]=%f lon[%d]=%f lat[%d]=%f\n", i, rcord[i], i, scord[i], i, longitude[i], i, latitude[i]););
      }
   }


   /* Release allocated memory */
  
   free(time);
  
   free(str_buf);
   free(ProjParm_out);
   free(ProjParm);


   MESSAGE(4,printf("\tTesting HE5_GDclose... \n"););
   MESSAGE(4,printf("\t====================== \n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\tTesting HE5_GDclose... \n"););
   MESSAGE(11,fprintf(fp,"\t====================== \n"););
   
   
   MESSAGE(6,printf("\n"););   
   MESSAGE(6,printf("\t\tClosing the file SimpleGrid.he5\n"););
   MESSAGE(11,fprintf(fp,"\t\n"););
   MESSAGE(11,fprintf(fp,"\t\tClosing the file SimpleGrid.he5\n"););
   
   
   status = HE5_GDclose(gdfidc_simple);
   if (status == FAIL)
	 {
	   MESSAGE(4,printf("\t\t********Error unexpected**********\n"););
	   MESSAGE(11,fprintf(fp,"\t\t********Error unexpected**********\n"););
       errcnt++;
	 }
   else
	 {
	   MESSAGE(4,printf("\t\tSuccessfully clossed the SimpleGrid.he5 file \n"););
	   MESSAGE(11,fprintf(fp,"\t\tSuccessfully clossed the SimpleGrid.he5 file \n"););
	   MESSAGE(4,printf("\n"););
	   MESSAGE(4,printf("\t ........... End of Testing. \n"););
	   MESSAGE(11,fprintf(fp,"\n"););
	   MESSAGE(11,fprintf(fp,"\t ............. End of Testing. \n"););
	 }
   
   if (errcnt == 0)
	 {
	   MESSAGE(11,printf("\n"););
	   MESSAGE(11,printf("\n"););
	   MESSAGE(11,printf("\t\t A L L    T E S T S     P A S S E D \n"););
	   MESSAGE(11,fprintf(fp,"\n"););
	   MESSAGE(11,fprintf(fp,"\n"););
	   MESSAGE(11,fprintf(fp,"\t\t A L L    T E S T S     P A S S E D \n"););
           return_val = 0;
	 }
   else
	 {
	   MESSAGE(11,printf("\n"););
	   MESSAGE(11,printf("\n"););
	   MESSAGE(11,printf("\t\t%d    T E S T S     F A I L E D \n",errcnt););
	   MESSAGE(11,fprintf(fp,"\n"););
	   MESSAGE(11,fprintf(fp,"\n"););
	   MESSAGE(11,fprintf(fp,"\t\t%d    T E S T S     F A I L E D \n",errcnt););
           return_val = 1;
	 }
   
   MESSAGE(11,fclose(fp););
   return return_val;

}













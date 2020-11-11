/*
* In this example we will (1) open an HDF-EOS file, and (2) create
* three point objects within the file.
*/
#include <HE5_HdfEosDef.h>
/* he5_pt_setup */
int main()
{
 herr_t status;
 hid_t ptfid, PTid1, PTid2, PTid3;
/*
 Open the HDF-EOS point file, "Point.he5". Assuming that this file
 may not exist, we are using the "H5F_ACC_TRUNC" access code.
 The "HE5_PTopen" function returns the point file ID, ptfid,
7-2 EED2-175-001
 which is used to identify the file in subsequent calls to the
 HDF-EOS library functions.
*/
 ptfid = HE5_PTopen("Point.he5", H5F_ACC_TRUNC);
 /* Set up the point structures */
 PTid1 = HE5_PTcreate(ptfid, "Simple Point");
 PTid2 = HE5_PTcreate(ptfid, "FixedBuoy Point");
 PTid3 = HE5_PTcreate(ptfid, "FloatBuoy Point");
 /* Close the point interface */
 status = HE5_PTdetach(PTid1);
 status = HE5_PTdetach(PTid2);
 status = HE5_PTdetach(PTid3);

 /* Close the point file */
 status = HE5_PTclose(ptfid);
 return 0;
}
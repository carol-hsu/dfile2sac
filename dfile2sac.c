#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* itoa(int value, char* result, int base) {
		// check that the base if valid
		if (base < 2 || base > 36) { *result = '\0'; return result; }

		char* ptr = result, *ptr1 = result, tmp_char;
		int tmp_value;

		do {
			tmp_value = value;
			value /= base;
			*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
		} while ( value );

		// Apply negative sign
		if (tmp_value < 0) *ptr++ = '-';
		*ptr-- = '\0';
		while(ptr1 < ptr) {
			tmp_char = *ptr;
			*ptr--= *ptr1;
			*ptr1++ = tmp_char;
		}
		return result;
}
	

 typedef struct
 {
 	int sta;
 	int inst;
 	int flag;
 } LIST;
 
 typedef struct {
	char sta[5];
	char inst[5];
	float lon;
	float lat;
	int num;
}NSTA;

typedef struct {
	int yr;
	int mo;
	int dy;
	int hr;
	int mn;
	int se;
	int mse;
}DTIME;

typedef struct{
	short int z;
	short int n;
	short int e;
} WAVEFORM;

 
 #define data_len 1296
	
		typedef struct
		{	
		   /* FILE_HEADER */

		   short int dummy;	// 0
		   short int sta;       // 1
		   short int comp; 	// 2
		   short int sp;	// 3
		   short int yr;        // 4
		   short int mo_dy; 	// 5
		   short int hr;	// 6
		   short int mn;        // 7
		   short int se_i; 	// 8
		   short int se_d;	// 9
		   short int tmp1;      // 10
		   short int tmp2; 	// 11
		   short int tmp3;	// 12
		   short int inst;      // 13
		   short int id; 	// 14		   		   		   		   
		 } D_file_header;
		 
		typedef struct
		{			   
		   short int num;       // 1
		   short int comp; 	// 2   		   		   		   
		   short int inst;      // 3
		   char sta[6];		// 4
		   float lon;		// 5
		   float lat;		// 6		 
		 } D_file_header2;	
 
 int go_through_dfile(char *fname, int fyr, int fmo, int fdy);
int read_nstaa(char *fname, NSTA *data1, int num, int yr, int mo, int dy);
 int julian(int yr, int mo, int dy);
 void disp_time (int yr, int mo, int dy, int hr, int mn, int se, char *ss, int type);
 
 int main(int argc, char* argv[])
 {
	char tmp;
	int yr, mo, dy;
	int err = 0;
 
 	if(argc!=5)
 	{
 		printf("Usage: Dfile2SAC <Dfile> <Year> <Month> <Day> \n");
 		printf("Ex: Dfile2SAC 09192330.p96 1996 9 19 \n");		
 		return;
 	}
	
	yr = atoi(argv[2]);
	mo = atoi(argv[3]);
	dy = atoi(argv[4]);
	
	if(yr<0 || yr > 3000) {printf("Error in <Year> ! \n"); err=1;}
	if(mo<0 || mo > 12)   {printf("Error in <Year> ! \n"); err=1;}
	if(dy<0 || dy > 31)   {printf("Error in <Year> ! \n"); err=1;}
	
	if(err==1) return 0;
	
 	go_through_dfile(argv[1], yr, mo, dy);

 		
 	return 0;      
 }
 

int go_through_dfile(char *fname, int fyr, int fmo, int fdy)
{
	FILE *fp, *fz, *fn, *fe;
	D_file_header DH, DH0;
	int count, list_num, j, flag;

	NSTA data1[1000];	
	char ss[100];
	int tt;
	
	LIST list[2000];
	
	char mo_dy[5], tmp[5], inst[10];
	DTIME htime, htime_i;
	
	short int data[data_len];	
	WAVEFORM *all_data;
	
	char ffname[100];
	
	
	float delta[70], dd, indata;
	int i, kk, yr, hr;
	char k8[24][9],kk16[16],kk8[8];
	int nn[35];
	
	int k, stano, sp, len, total_pt=0, total_sta=0;
	int ii=0;
	
	int z_num,n_num,e_num;

	int err=0;
	
		
	fp = fopen(fname,"rb");
	if (fp == NULL) {
		printf("%s Open failed..\n", fname);
		return 0;
	}
	
//==================================================================== initial
	for(i=0; i<70; i++)  // float
			delta[i]=-12345.0;

	for(i=0; i<35; i++)  // int
			nn[i]=-12345;
	
//	for(i=0; i<5; i++) // int
		kk=1;
	
	for(i=3; i<24; i++)  // string
		sprintf(k8[i],"-12345");
		sprintf(kk8,"-12345");
		sprintf(kk16,"-12345");
//====================================================================		
	
	list_num=0;
	for(i=0;i<1000;i++)
	{
		list[i].sta=0;
		list[i].flag=0;
		list[i].inst=0;
	}

	count=0;
	list_num=0;
	while(1)
	{
		fread(&DH,sizeof(D_file_header),1,fp);
		fread(data,data_len*sizeof(short int),1,fp);	
        if(DH.sta>200) { err=DH.sta;break;}

		if(count==0)	// Record the beginning time
		{
		   DH0.dummy  =  DH.dummy;
		   DH0.sta    =  DH.sta  ;
		   DH0.comp   =  DH.comp ;
		   DH0.sp	  =  DH.sp	 ;
		   DH0.yr     =  DH.yr   ;
		   DH0.mo_dy  =  DH.mo_dy;
		   DH0.hr	  =  DH.hr	 ;
		   DH0.mn     =  DH.mn   ;
		   DH0.se_i   =  DH.se_i ;
		   DH0.se_d   =  DH.se_d ;
		   DH0.tmp1   =  DH.tmp1 ;
		   DH0.tmp2   =  DH.tmp2 ;
		   DH0.tmp3   =  DH.tmp3 ;
		   DH0.inst   =  DH.inst ;
		   DH0.id     =  DH.id   ;
		   
			itoa(DH0.mo_dy,mo_dy,10);
			if(DH0.mo_dy>999) {
				sprintf(tmp,"%c%c",mo_dy[0],mo_dy[1]);
				htime.mo=atoi(tmp);
				sprintf(tmp,"%c%c",mo_dy[2],mo_dy[3]);
				htime.dy=atoi(tmp);				
			} else {
				sprintf(tmp,"%c",mo_dy[0]);
				htime.mo=atoi(tmp);
				sprintf(tmp,"%c%c",mo_dy[1],mo_dy[2]);
				htime.dy=atoi(tmp);				
			}				
				//printf("month: %d , day: %d \n", htime->mo, htime->dy);											
					htime.yr=DH0.yr;
					htime.hr=DH0.hr;
					htime.mn=DH0.mn;
					htime.se=DH0.se_i;
					htime.mse=DH0.se_d;
		}
				
			itoa(DH.mo_dy,mo_dy,10);
			if(DH.mo_dy>999) {
				sprintf(tmp,"%c%c",mo_dy[0],mo_dy[1]);
				htime_i.mo=atoi(tmp);
				sprintf(tmp,"%c%c",mo_dy[2],mo_dy[3]);
				htime_i.dy=atoi(tmp);				
			} else {
				sprintf(tmp,"%c",mo_dy[0]);
				htime_i.mo=atoi(tmp);
				sprintf(tmp,"%c%c",mo_dy[1],mo_dy[2]);
				htime_i.dy=atoi(tmp);				
			}														
					htime_i.yr=DH.yr;
					htime_i.hr=DH.hr;
					htime_i.mn=DH.mn;
					htime_i.se=DH.se_i;
					htime_i.mse=DH.se_d;			
				
	//	printf("--- dummy=%d  sta=%3d, comp=%d, sp=%d, inst=%d, yr=%d, mo_dy=%d, hr=%2d, mn=%2d, se_i=%2d, se_d=%2d, id=%2d \n", 
	//			DH.dummy, DH.sta, DH.comp, DH.sp, DH.inst, 
	//			DH.yr, DH.mo_dy, DH.hr, DH.mn, DH.se_i, DH.se_d, DH.id);	

		
		for(i=0;i<1000;i++)
			if(list[i].flag==0)break;
		flag=0;			
		for(j=0;j<1000;j++)
		{
			if(list[j].flag==1)
			{
				if( DH.sta==list[j].sta && DH.inst==list[j].inst)
				{ 
				  flag=1;
				  //printf("DH.sta : %d, list[j].sta : %d \n",DH.sta ,list[j].sta);
				  break;
				}
			}	
		}
		if(flag==0) 
		{
			list[i].flag  = 1;
			list[i].sta   = DH.sta;
			list[i].inst  = DH.inst;
			list_num++;
		}
	

		
		count ++;
		if(feof(fp)) break;
	}
	fclose(fp);
	printf("count: %d , list_num: %d \n", count, list_num);	
	
	all_data = (WAVEFORM*)malloc(count*data_len*sizeof(WAVEFORM));
	
	// for(i=0;i<list_num;i++)
	// {	
	// 	if(list[i].sta>200 && i< list_num-1 ) 
	// 	{
	// 		printf("Error !!! \n");
	// 		return 0;
	// 	}
	// 	if(list[i].sta>200)
	// 	{
	// 		printf("Error in the last record !!! \n");
	// 		list_num--;
	// 	}
	// 	//printf("%d \n", list[i].sta);
	// }	
		
		
		
	for(i=0;i<list_num;i++)
	{

 		read_nstaa("NSTA.DAT", &data1[i], list[i].sta, fyr, fmo, fdy);	
	
		printf("list[%02d].sta: %02d -- %4s %7.3f %6.3f %3d \n"
			, i, list[i].sta, data1[i].sta, data1[i].lon, data1[i].lat, list[i].inst);
										
		//0:S13 data, 1:RTD data, 2:BB DATA, 3:BB FBA, 4:JAPAN
		if(list[i].inst==0) sprintf(inst,"S13");
		if(list[i].inst==1) sprintf(inst,"RTD");
		if(list[i].inst==2) sprintf(inst,"BB");
		if(list[i].inst==3) sprintf(inst,"BB_FBA");
		if(list[i].inst==4) sprintf(inst,"JAPAN");
					
		sprintf(ffname,"/tmp/%s_%s_%s_Z.sac", fname, data1[i].sta, inst);	
		fz = fopen(ffname,"wb");
		printf("Creating %s \n", ffname);	
		
		sprintf(ffname,"/tmp/%s_%s_%s_N.sac", fname, data1[i].sta, inst);	
		fn = fopen(ffname,"wb");
		printf("Creating %s \n", ffname);			
		
		sprintf(ffname,"/tmp/%s_%s_%s_E.sac", fname, data1[i].sta, inst);	
		fe = fopen(ffname,"wb");
		printf("Creating %s \n", ffname);		
				
			
	
		fp = fopen(fname,"rb");
		if (fp == NULL) 
		{
			printf("%s Open failed..\n", fname);
			return 0;
		}
					
					
		z_num=0; n_num=0; e_num=0;						
		while(1)
		{
			fread(&DH,sizeof(D_file_header),1,fp);
			fread(data,data_len*sizeof(short int),1,fp);
			
			//if(list[i].inst!=0) continue;
		
			if( DH.sta==list[i].sta && DH.inst==list[i].inst )
			{
				if(DH.comp==1)				
					for(j=0;j<data_len;j++)
					{
						all_data[z_num].z = data[j];		
						z_num++;	
					}
				if(DH.comp==2)
					for(j=0;j<data_len;j++)
					{
						all_data[n_num].n = data[j];
						n_num++;	
					}
				if(DH.comp==3)
					for(j=0;j<data_len;j++)
					{
						all_data[e_num].e = data[j];	
						e_num++;		
					}
			}		
			if(feof(fp)) break;
		}	
		fclose(fp);
//-------------------------------Header					
		disp_time (htime.yr, htime.mo, htime.dy, htime.hr, htime.mn, htime.se, ss, 2);	
		
		delta[0]=1.0/(float)DH0.sp;	  //delta
		printf("-------sp: %d %f \n", DH0.sp, delta[0]);		
		//printf("delta[0]=%f -- %f\n", delta[0], 1/(float)fh.sp);
		delta[5]=0.0;	  //begin
	    delta[6]=(float)z_num/(float)DH0.sp;	  //end
		delta[31]=data1[i].lat;  //lat
		delta[32]=data1[i].lon; //lon	
	 
		nn[0]=htime.yr;       //yr
		nn[1]=julian(nn[0], htime.mo, htime.dy);         //jdy
		nn[2]=htime.hr;	  //hr
		nn[3]=htime.mn;	  //min
		nn[4]=htime.se;	  //sec
		nn[5]=htime.mse;  //msec
		nn[6]=6;	  //SAC version
		nn[9]=(int)z_num;	  //npts
		nn[15]=1;		//iftype 1:time series
		nn[16]=5;		//idep 5:unknown,6:dis,7:vel,8:acc,50:volt 
		nn[17]=5;		//iztype 5:unknown						
	      				      	     
    		sprintf(k8[0],"%s", data1[i].sta);		
//-------------------------------Z		        
			sprintf(k8[20],"CHZ");			
      			// float
      			for(j=0; j<70; j++)    fwrite(&delta[j],sizeof(float),1,fz); 
      			// int
      			for(j=0; j<35; j++)    fwrite(&nn[j],sizeof(int),1,fz);  
      			// int
      			for(j=0; j<5; j++)     fwrite(&kk,sizeof(int),1,fz); 
      			//string
      			fwrite(k8[0],sizeof(char),8,fz);
      			fwrite(kk16,sizeof(char),16,fz);
      			for(j=3; j<24; j++)    fwrite(k8[j],sizeof(char),8,fz);  
      						
      			// float
      			for(j=0; j<nn[9]; j++) 
      			{
      				indata = (float)all_data[j].z;
      				fwrite(&indata,sizeof(float),1,fz);
      			}
		fclose(fz);      			
//-------------------------------N	
			sprintf(k8[20],"CHN");	
      			// float
      			for(j=0; j<70; j++)    fwrite(&delta[j],sizeof(float),1,fn);
      			// int
      			for(j=0; j<35; j++)    fwrite(&nn[j],sizeof(int),1,fn);
      			// int
      			for(j=0; j<5; j++)     fwrite(&kk,sizeof(int),1,fn);
      			//string
      			fwrite(k8[0],sizeof(char),8,fn);
      			fwrite(kk16,sizeof(char),16,fn);
      			for(j=3; j<24; j++)    fwrite(k8[j],sizeof(char),8,fn);
      			// float
      			for(j=0; j<nn[9]; j++) 
      			{
      				indata = (float)all_data[j].n;
      				fwrite(&indata,sizeof(float),1,fn);
      			}	
		fclose(fn);      			
//-------------------------------E	
			sprintf(k8[20],"CHE");	
      			// float
      			for(j=0; j<70; j++)    fwrite(&delta[j],sizeof(float),1,fe);
      			// int
      			for(j=0; j<35; j++)    fwrite(&nn[j],sizeof(int),1,fe);
      			// int
      			for(j=0; j<5; j++)     fwrite(&kk,sizeof(int),1,fe);
      			//string
      			fwrite(k8[0],sizeof(char),8,fe);
      			fwrite(kk16,sizeof(char),16,fe);
      			for(j=3; j<24; j++)    fwrite(k8[j],sizeof(char),8,fe);
      			// float
      			for(j=0; j<nn[9]; j++) 
      			{
      				indata = (float)all_data[j].e;
      				fwrite(&indata,sizeof(float),1,fe);
      			}	
		fclose(fe);      				
//-------------------------------												
	}
	free(all_data);
	if(err>0) 
		printf("Err: %d \n", err);
	
	return 1;
}
 
 
 
 
int read_nstaa(char *fname, NSTA *data1, int num, int yr, int mo, int dy)         
{
	FILE *fp;
	char ss[200];
	NSTA data[1000];

      	char tmp[6], s2[50], s3[50];
      	float lat, lat_m, lon, lon_m;
      	int j,i;
      	int  b_secs, e_secs, min=2147000000, max=0;

	int st_yr, st_mo, st_dy;
	int ed_yr, ed_mo, ed_dy;	
	
	double stime, etime, dtime;

	int check_date;
	
	fp=fopen(fname,"r");
	if(fp==NULL) { printf("%s not exist !", fname); return -1;}

	j=0;
	i=0;
	while(fgets(ss,199,fp)!=NULL){
		
		 sprintf(tmp,"%c%c%c", ss[28], ss[29], ss[30]);
		 data[i].num = atof(tmp);					
		
		 sprintf(tmp,"%c%c", ss[66], ss[67]);		 st_yr = atoi(tmp);		
		 sprintf(tmp,"%c%c", ss[68], ss[69]);		 st_mo = atoi(tmp);
		 sprintf(tmp,"%c%c", ss[70], ss[71]);		 st_dy = atoi(tmp);		
		 
		 sprintf(tmp,"%c%c", ss[73], ss[74]);		 ed_yr = atoi(tmp);		
		 sprintf(tmp,"%c%c", ss[75], ss[76]);		 ed_mo = atoi(tmp);
		 sprintf(tmp,"%c%c", ss[77], ss[78]);		 ed_dy = atoi(tmp);				 

					//printf("************** %c%c%c%c%c%c %c%c%c%c%c%c \n\n\n\n\n\n\n"
					//			, ss[66], ss[67], ss[68], ss[69], ss[70], ss[71]
					//			, ss[73], ss[74], ss[75], ss[76], ss[77], ss[78] );		 
		 
		if(st_mo>12)
		{
			st_yr += 2000;
			st_mo -= 12;
		}
		else
		{
			st_yr += 1900;		
		}
		
		if(ed_mo>12)
		{
			ed_yr += 2000;
			ed_mo -= 12;			
		}	
		else
		{
			ed_yr += 1900;		
		}
		
		stime = ((double)st_yr-1900)*365+(double)st_mo*30.0+(double)st_dy;
		etime = ((double)ed_yr-1900)*365+(double)ed_mo*30.0+(double)ed_dy;
		dtime = ((double)yr-1900)*365+(double)mo*30.0+(double)dy;		

		
		check_date =0;

		if( dtime >= stime && dtime <= etime )
		{
			check_date =1;					
					
			// printf("===============%d-%d-%d--%.1f %c%c%c%c%c%c--%d %d %d--%.1f   %c%c%c%c%c%c-%d %d %d--%.1f \n\n\n\n\n\n\n"
			// 			, yr, mo, dy
			// 			, dtime
			// 			, ss[66], ss[67], ss[68], ss[69], ss[70], ss[71]
			// 			, st_yr, st_mo, st_dy
			// 			, stime
			// 			, ss[73], ss[74], ss[75], ss[76], ss[77], ss[78]
			// 			, ed_yr, ed_mo, ed_dy	
			// 			, etime	);
		}

		if( num==data[i].num && check_date==1 ) {	
		

			
		 sprintf(tmp,"%c%c", ss[4], ss[5]);
		 lat = atof(tmp);
		 sprintf(tmp,"%c%c%c%c%c", ss[6], ss[7], ss[8], ss[9], ss[10]);
		 lat_m = atof(tmp);
		 data[i].lat = lat+lat_m/60;
		 
		 sprintf(tmp,"%c%c%c", ss[12], ss[13], ss[14]);
		 lon = atof(tmp);
		 sprintf(tmp,"%c%c%c%c%c", ss[15], ss[16], ss[17], ss[18], ss[19]);
		 lon_m = atof(tmp);
		 data[i].lon = lon+lon_m/60;
		// 
		
		//
		if( isalnum(ss[3]) )
			sprintf((data+i)->sta,"%c%c%c%c",ss[0],ss[1],ss[2],ss[3]);  
		else               
			sprintf((data+i)->sta,"%c%c%c",ss[0],ss[1],ss[2]);  		

		*(data1)=*(data+i);
		
		break;

 		}
			
		
		i++;	 
	}

	fclose(fp);	
	return 1;
	
}
 
 
int julian(int yr, int mo, int dy)
{
	int day=0;	
	int jan=31, feb=28, mar=31, apr=30, may=31, jun=30, jul=31, aug=31, sep=30, oct=31, nov=30, dec=31;

	if(yr%400==0 ) {
		feb=29;
	} 
	else if(yr%4==0 && yr%100!=0) {
		feb=29;
	}

	
	switch(mo)
	{
		case 12:
			if(mo==12) day= dy;
		case 11:
			if(mo==11) day= dy - nov;
			day = day + nov;
		case 10:
			if(mo==10) day= dy - oct;
			day = day + oct;
		case 9:
			if(mo==9) day= dy - sep;
			day = day + sep;
		case 8:
			if(mo==8) day= dy - aug;
			day = day + aug;
		case 7:
			if(mo==7) day= dy - jul;
			day = day + jul;
		case 6:
			if(mo==6) day= dy - jun;
			day = day + jun;
		case 5:
			if(mo==5) day= dy - may;
			day = day + may;
		case 4:
			if(mo==4) day= dy - apr;
			day = day + apr;
		case 3:
			if(mo==3) day= dy - mar;
			day = day + mar;
		case 2:
			if(mo==2) day= dy - feb;
			day = day + feb;
		case 1:
			if(mo==1) day= dy - jan; 
			day = day + jan;
	}	



	return day;
}

 
/*******************************************************
* Transfer yr mo dy hr mn se into 
* seconds from 1970/01/01 08:00:00
* Type = 1 : 20080202020202
* Type = 2 : 20080202 0202
********************************************************/
void disp_time (int yr, int mo, int dy, int hr, int mn, int se, char *ss, int type)
{
	char mon[3], day[3], hor[3], min[3], sec[3];
	
	if(mo<10) {
		sprintf(mon,"0%d",mo);
	} else {
		sprintf(mon,"%d",mo);		
	}
	if(dy<10) {
		sprintf(day,"0%d",dy);
	} else {
		sprintf(day,"%d",dy);		
	}	
	if(hr<10) {
		sprintf(hor,"0%d",hr);
	} else {
		sprintf(hor,"%d",hr);		
	}	
	if(mn<10) {
		sprintf(min,"0%d",mn);
	} else {
		sprintf(min,"%d",mn);		
	}	
	if(se<10) {
		sprintf(sec,"0%d",se);
	} else {
		sprintf(sec,"%d",se);		
	}	
	
	if(type==1){
		sprintf(ss,"%d%s%s%s%s%s",yr,mon,day,hor,min,sec);
	}
	else {
		sprintf(ss,"%d%s%s_%s%s",yr,mon,day,hor,min);
	}
} 
 

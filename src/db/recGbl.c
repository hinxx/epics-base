/* recGbl.c - Global record processing routines */
/* share/src/db/recGbl.c	$Id$ */

/*
 *      Author:          Marty Kraimer
 *      Date:            11-7-90
 *
 *      Experimental Physics and Industrial Control System (EPICS)
 *
 *      Copyright 1991, the Regents of the University of California,
 *      and the University of Chicago Board of Governors.
 *
 *      This software was produced under  U.S. Government contracts:
 *      (W-7405-ENG-36) at the Los Alamos National Laboratory,
 *      and (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *      Initial development by:
 *              The Controls and Automation Group (AT-8)
 *              Ground Test Accelerator
 *              Accelerator Technology Division
 *              Los Alamos National Laboratory
 *
 *      Co-developed with
 *              The Controls and Computing Group
 *              Accelerator Systems Division
 *              Advanced Photon Source
 *              Argonne National Laboratory
 *
 * Modification Log:
 * -----------------
 * .01  11-16-91        jba     Added recGblGetGraphicDouble, recGblGetControlDouble
 * .02  02-28-92        jba     ANSI C changes
 * .03	05-19-92	mrk	Changes for internal database structure changes
 * .04  07-16-92        jba     changes made to remove compile warning msgs
 * .05  07-21-92        jba     Added recGblGetAlarmDouble
 * .06  08-07-92        jba     Added recGblGetLinkValue, recGblPutLinkValue
 * .07  08-07-92        jba     Added RTN_SUCCESS check for status
 * .08  09-15-92        jba     changed error parm in recGblRecordError calls
 * .09  09-17-92        jba     ANSI C changes
 * .10  01-27-93        jba     set pact to true during calls to dbPutLink
 */

#include	<vxWorks.h>
#include	<limits.h>
#include	<types.h>
#include	<stdioLib.h>
#include	<strLib.h>

#include	<choice.h>
#include	<dbDefs.h>
#include	<dbBase.h>
#include	<dbRecType.h>
#include	<dbRecDes.h>
#include	<dbAccess.h>
#include	<devSup.h>
#include	<rec/dbCommon.h>
#include	<sdrHeader.h>

extern struct dbBase *pdbBase;


/***********************************************************************
* The following are the global record processing rouitines
*
*void recGblDbaddrError(status,paddr,pcaller_name)
*     long              status;
*     struct dbAddr	*paddr;
*     char		*pcaller_name;	* calling routine name *
*
*void recGblRecordError(status,precord,pcaller_name)
*     long              status;
*     void		*precord;	* addr of record	*
*     char		*pcaller_name;	* calling routine name *
*
*void recGblRecSupError(status,paddr,pcaller_name,psupport_name)
*     long              status;
*     struct dbAddr	*paddr;
*     char		*pcaller_name;	* calling routine name *
*     char		*psupport_name;	* support routine name	*
*
*void recGblGetGraphicDouble(paddr,pgd)
*     struct dbAddr	  *paddr;
*     struct dbr_grDouble *pgd;
*
*void recGblGetControlDouble(paddr,pcd)
*     struct dbAddr	  *paddr;
*     struct dbr_ctrlDouble *pcd;
*
*void recGblGetAlarmDouble(paddr,pad)
*     struct dbAddr	  *paddr;
*     struct dbr_alDouble *pad;
*
*void recGblGetPrec(paddr,pprecision)
*     struct dbAddr	  *paddr;
*     long		  *pprecision;
*
*long recGblGetLinkValue(plink,precord,dbrType,pdest,poptions,pnRequest)
*	struct link	*plink;
*	struct dbCommon	*precord;
*	short		dbrType;
*	void		*pdest;
*	long		*poptions;
*	long		*pnRequest;
*
*long recGblPutLinkValue(plink,precord,dbrType,psource,pnRequest)
*	struct link	*plink;
*	struct dbCommon *precord;
*	short           dbrType;
*	void            *psource;
*	long		*pnRequest;
**************************************************************************/


/* local routines */
static void getVarRangeValue();
static void getConRangeValue();
static void getMaxRangeValues();



void recGblDbaddrError(
	long	status,
	struct dbAddr	*paddr,	
	char	*pcaller_name	/* calling routine name*/
)
{
	char		buffer[200];
	struct dbCommon *precord;
	int		i,n;
	struct fldDes	*pfldDes=(struct fldDes *)(paddr->pfldDes);

	buffer[0]=0;
	if(paddr) { /* print process variable name */
		precord=(struct dbCommon *)(paddr->precord);
		strcat(buffer,"PV: ");
		strncat(buffer,precord->name,PVNAME_SZ);
		n=strlen(buffer);
		for(i=n; (i>0 && buffer[i]==' '); i--) buffer[i]=0;
		strcat(buffer,".");
		strncat(buffer,pfldDes->fldname,FLDNAME_SZ);
		strcat(buffer," ");
	}
	if(pcaller_name) {
		strcat(buffer,"error detected in routine: ");
		strcat(buffer,pcaller_name);
	}
	errMessage(status,buffer);
	return;
}

void recGblRecordError(
long		status,
struct dbCommon *precord,	
char		*pcaller_name 	/* calling routine name*/
)
{
	char buffer[200];
	int i,n;

	buffer[0]=0;
	if(precord) { /* print process variable name */
		strcat(buffer,"PV: ");
		strncat(buffer,precord->name,PVNAME_SZ);
		n=strlen(buffer);
		for(i=n; (i>0 && buffer[i]==' '); i--) buffer[i]=0;
		strcat(buffer,"  ");
	}
	if(pcaller_name) {
		strcat(buffer,"error detected in routine: ");
		strcat(buffer,pcaller_name);
	}
	errMessage(status,buffer);
	return;
}

void recGblRecSupError(
long		status,
struct dbAddr	*paddr,
char		*pcaller_name,
char		*psupport_name 
)
{
	char buffer[200];
	char *pstr;
	struct dbCommon *precord;
	int		i,n;
	struct fldDes	*pfldDes=(struct fldDes *)(paddr->pfldDes);

	buffer[0]=0;
	strcat(buffer,"Record Support Routine (");
	if(psupport_name)
		strcat(buffer,psupport_name);
	else
		strcat(buffer,"Unknown");
	strcat(buffer,") not available.\nRecord Type is ");
	if(pstr=GET_PRECNAME(pdbBase->precType,paddr->record_type))
		strcat(buffer,pstr);
	else
		strcat(buffer,"BAD");
	if(paddr) { /* print process variable name */
		precord=(struct dbCommon *)(paddr->precord);
		strcat(buffer,", PV is ");
		strncat(buffer,precord->name,PVNAME_SZ);
		n=strlen(buffer);
		for(i=n; (i>0 && buffer[i]==' '); i--) buffer[i]=0;
		strcat(buffer,".");
		strncat(buffer,pfldDes->fldname,FLDNAME_SZ);
		strcat(buffer,"  ");
	}
	if(pcaller_name) {
		strcat(buffer,"\nerror detected in routine: ");
		strcat(buffer,pcaller_name);
	}
	errMessage(status,buffer);
	return;
}

void recGblGetPrec(
    struct dbAddr *paddr,
    long           *precision
)
{
    struct fldDes               *pfldDes=(struct fldDes *)(paddr->pfldDes);

    switch(pfldDes->field_type){
    case(DBF_SHORT):
         *precision = 0;
         break;
    case(DBF_USHORT):
         *precision = 0;
         break;
    case(DBF_LONG):
         *precision = 0;
         break;
    case(DBF_ULONG):
         *precision = 0;
         break;
    case(DBF_FLOAT):
         break;
    case(DBF_DOUBLE):
         break;
    default:
         break;
    }
    return;
}

void recGblGetGraphicDouble(
    struct dbAddr *paddr,
    struct dbr_grDouble *pgd
)
{
    struct fldDes               *pfldDes=(struct fldDes *)(paddr->pfldDes);

    /* get upper display limit */
    if(pfldDes->highfl==VAR) getVarRangeValue(paddr,pfldDes->range2.fldnum,&pgd->upper_disp_limit);
    else getConRangeValue(pfldDes->field_type,pfldDes->range2,&pgd->upper_disp_limit);

    /* get lower display limit */
    if(pfldDes->lowfl==VAR) getVarRangeValue(paddr,pfldDes->range1.fldnum,&pgd->lower_disp_limit);
    else getConRangeValue(pfldDes->field_type,pfldDes->range1,&pgd->lower_disp_limit);

    if(pgd->lower_disp_limit>=pgd->upper_disp_limit)
          getMaxRangeValues(pfldDes->field_type,&pgd->upper_disp_limit,&pgd->lower_disp_limit);

    return;
}

void recGblGetAlarmDouble(
    struct dbAddr *paddr,
    struct dbr_alDouble *pad
)
{
    pad->upper_alarm_limit = 0;
    pad->upper_alarm_limit = 0;
    pad->lower_warning_limit = 0;
    pad->lower_warning_limit = 0;

    return;
}

void recGblGetControlDouble(
    struct dbAddr *paddr,
    struct dbr_ctrlDouble *pcd
)
{
    struct fldDes               *pfldDes=(struct fldDes *)(paddr->pfldDes);

    /* get upper control limit */
    if(pfldDes->highfl==VAR) getVarRangeValue(paddr,pfldDes->range2.fldnum,&pcd->upper_ctrl_limit);
    else getConRangeValue(pfldDes->field_type,pfldDes->range2,&pcd->upper_ctrl_limit);

    /* get lower control limit */
    if(pfldDes->lowfl==VAR) getVarRangeValue(paddr,pfldDes->range1.fldnum,&pcd->lower_ctrl_limit);
    else getConRangeValue(pfldDes->field_type,pfldDes->range1,&pcd->lower_ctrl_limit);

    if(pcd->lower_ctrl_limit>=pcd->upper_ctrl_limit)
          getMaxRangeValues(pfldDes->field_type,&pcd->upper_ctrl_limit,&pcd->lower_ctrl_limit);

    return;
}

long recGblGetLinkValue(
	struct link	*plink,
	struct dbCommon *precord,
	short           dbrType,
	void            *pdest,
	long		*poptions,
	long		*pnRequest
)
{
	long		status=0;
	unsigned char   pact;

	pact = precord->pact;
	precord->pact = TRUE;
	switch (plink->type){
		case(CONSTANT):
			break;
		case(DB_LINK):
			status=dbGetLink(&(plink->value.db_link),
				precord,dbrType,pdest,poptions,pnRequest);
			if(!RTN_SUCCESS(status))
				recGblSetSevr(precord,LINK_ALARM,INVALID_ALARM);
			break;
		case(CA_LINK):
			status=dbCaGetLink(plink);
			if(!RTN_SUCCESS(status))
				recGblSetSevr(precord,LINK_ALARM,INVALID_ALARM);
			break;
		default:
			status=-1;
			recGblSetSevr(precord,SOFT_ALARM,INVALID_ALARM);
	}
	precord->pact = pact;
	return(status);
}

long recGblPutLinkValue(
	struct link	*plink,
	struct dbCommon *precord,
	short           dbrType,
	void            *psource,
	long		*pnRequest
)
{
	long		options=0;
	long		status=0;
	unsigned char   pact;

	pact = precord->pact;
	precord->pact = TRUE;
	switch (plink->type){
		case(CONSTANT):
			break;
		case(DB_LINK):
			status=dbPutLink(&(plink->value.db_link),
				precord,dbrType,psource,*pnRequest);
			if(!RTN_SUCCESS(status))
				recGblSetSevr(precord,LINK_ALARM,INVALID_ALARM);
			break;
		case(CA_LINK):
			status = dbCaPutLink(plink, &options, pnRequest);
			if(!RTN_SUCCESS(status))
				recGblSetSevr(precord,LINK_ALARM,INVALID_ALARM);
			break;
		default:
			status=-1;
			recGblSetSevr(precord,SOFT_ALARM,INVALID_ALARM);
	}
	precord->pact = pact;
	return(status);
}

static void getConRangeValue(field_type,range,plimit)
     short            field_type;
     struct range     range;
     double           *plimit;
{
    *plimit=0.0;
    switch(field_type){
    case(DBF_SHORT):
         *plimit = (double)range.value.short_value;
         break;
    case(DBF_ENUM):
    case(DBF_USHORT):
         *plimit = (double)range.value.ushort_value;
         break;
    case(DBF_LONG):
         *plimit = (double)range.value.long_value;
         break;
    case(DBF_ULONG):
         *plimit = (double)range.value.ulong_value;
         break;
    case(DBF_FLOAT):
         *plimit = (double)range.value.float_value;
         break;
    case(DBF_DOUBLE):
         *plimit = (double)range.value.double_value;
         break;
    }
    return;
}

static void getMaxRangeValues(field_type,pupper_limit,plower_limit)
    short           field_type;
    double          *pupper_limit;
    double          *plower_limit;
{
    switch(field_type){
    case(DBF_SHORT):
         *pupper_limit = (double)SHRT_MAX;
         *plower_limit = (double)SHRT_MIN;
         break;
    case(DBF_ENUM):
    case(DBF_USHORT):
         *pupper_limit = (double)USHRT_MAX;
         *plower_limit = (double)0;
         break;
    case(DBF_LONG):
	/* long did not work using cast to double */
         *pupper_limit = 2147483647.;
         *plower_limit = -2147483648.;
         break;
    case(DBF_ULONG):
         *pupper_limit = (double)ULONG_MAX;
         *plower_limit = (double)0;
         break;
    case(DBF_FLOAT):
         *pupper_limit = (double)1e+30;
         *plower_limit = (double)-1e30;
         break;
    case(DBF_DOUBLE):
         *pupper_limit = (double)1e+30;
         *plower_limit = (double)-1e30;
         break;
    }
    return;
}

static void getVarRangeValue(paddr,fldnum,prangeValue)
struct dbAddr	*paddr;	
long 		fldnum;	
double		*prangeValue;
{
        short			recType;
        struct recTypDes	*precTypDes;
        struct dbAddr		dbAddr;
        long			nRequest,options;
        void			*pfl=NULL;
	char			name[PVNAME_SZ+FLDNAME_SZ+2];
	struct dbCommon		*precord;
	int			i,n;
	struct fldDes		*pfldDes;
	long			status;

        *prangeValue=0;
	precord=(struct dbCommon *)(paddr->precord);
        recType=paddr->record_type;

        if(!(precTypDes=GET_PRECTYPDES(pdbBase->precDes,recType))){
                recGblRecordError(S_sdr_noSdrType,(void *)precord,"getVarRangeValue(GET_PRECTYPDES)");
                return;
        }
        if(!(pfldDes=GET_PFLDDES(precTypDes,fldnum))){
                recGblRecordError(S_sdr_noSdrType,(void *)precord,"getVarRangeValue(GET_PFLDDES)");
                return;
        }
        /* get &dbAddr for range VAR field */
        name[PVNAME_SZ+1] = 0;
        strncpy(name,precord->name,PVNAME_SZ);
	n=strlen(name);
	for(i=n; (i>0 && name[i]==' '); i--) name[i]=0;
	strcat(name,".");
	strncat(name,pfldDes->fldname,FLDNAME_SZ);
	strcat(name,"\0");
        if (status=dbNameToAddr(name,&dbAddr)){
                recGblRecordError(status,(void *)precord,"getVarRangeValue(dbNameToAddr)");
                return;
        }

        /* get value of range VAR field */
        options = 0;
        nRequest = 1;
        if(status=dbGetField(&dbAddr,DBR_DOUBLE,prangeValue,&options,&nRequest,pfl)){
                recGblRecordError(status,(void *)precord,"getVarRangeValue(dbGetField)");
                return;
        }
        return;
}

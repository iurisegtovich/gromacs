/*
 *       $Id$
 *
 *       This source code is part of
 *
 *        G   R   O   M   A   C   S
 *
 * GROningen MAchine for Chemical Simulations
 *
 *            VERSION 2.0
 * 
 * Copyright (c) 1991-1997
 * BIOSON Research Institute, Dept. of Biophysical Chemistry
 * University of Groningen, The Netherlands
 * 
 * Please refer to:
 * GROMACS: A message-passing parallel molecular dynamics implementation
 * H.J.C. Berendsen, D. van der Spoel and R. van Drunen
 * Comp. Phys. Comm. 91, 43-56 (1995)
 *
 * Also check out our WWW page:
 * http://rugmd0.chem.rug.nl/~gmx
 * or e-mail to:
 * gromacs@chem.rug.nl
 *
 * And Hey:
 * GRoups of Organic Molecules in ACtion for Science
 */
static char *SRCID_add_par_c = "$Id$";

#include <string.h>
#include "typedefs.h"
#include "smalloc.h"
#include "grompp.h"
#include "toputil.h"
#include "hackblock.h"

static void clear_atom_list(int i0, atom_id a[])
{
  int i;
  
  for (i=i0; i < MAXATOMLIST; i++)
    a[i]=-1;
}

static void clear_force_param(int i0, real c[])
{
  int i;
  
  for (i=i0; i < MAXFORCEPARAM; i++)
    c[i]=NOTSET;
}

void add_param(t_params *ps,int ai,int aj, real *c, char *s)
{
  int i;
  
  if ((ai < 0) || (aj < 0)) 
    fatal_error(0,"Trying to add impossible atoms: ai=%d, aj=%d",ai,aj);
  pr_alloc(1,ps);
  ps->param[ps->nr].AI=ai;
  ps->param[ps->nr].AJ=aj;
  clear_atom_list(2, ps->param[ps->nr].a);
  if (c==NULL) 
    clear_force_param(0, ps->param[ps->nr].c);
  else
    for(i=0; (i < MAXFORCEPARAM); i++)
      ps->param[ps->nr].c[i]=c[i];
  if (s)
    ps->param[ps->nr].s = strdup(s);
  else
    ps->param[ps->nr].s = strdup("");
  ps->nr++;
}

void add_imp_param(t_params *ps,int ai,int aj,int ak,int al,real c0, real c1,
		   char *s)
{
  pr_alloc(1,ps);
  ps->param[ps->nr].AI=ai;
  ps->param[ps->nr].AJ=aj;
  ps->param[ps->nr].AK=ak;
  ps->param[ps->nr].AL=al;
  clear_atom_list  (4, ps->param[ps->nr].a);
  ps->param[ps->nr].C0=c0;
  ps->param[ps->nr].C1=c1;
  clear_force_param(2, ps->param[ps->nr].c);
  if (s)
    ps->param[ps->nr].s = strdup(s);
  else
    ps->param[ps->nr].s = strdup("");
  ps->nr++;
}

void add_dih_param(t_params *ps,int ai,int aj,int ak,int al,real c0, real c1,
		   real c2,char *s)
{
  pr_alloc(1,ps);
  ps->param[ps->nr].AI=ai;
  ps->param[ps->nr].AJ=aj;
  ps->param[ps->nr].AK=ak;
  ps->param[ps->nr].AL=al;
  clear_atom_list  (4, ps->param[ps->nr].a);
  ps->param[ps->nr].C0=c0;
  ps->param[ps->nr].C1=c1;
  ps->param[ps->nr].C2=c2;
  clear_force_param(3, ps->param[ps->nr].c);
  if (s)
    ps->param[ps->nr].s = strdup(s);
  else
    ps->param[ps->nr].s = strdup("");
  ps->nr++;
}

void add_dum2_atoms(t_params *ps,int ai,int aj,int ak)
{
  pr_alloc(1,ps);
  ps->param[ps->nr].AI=ai;
  ps->param[ps->nr].AJ=aj;
  ps->param[ps->nr].AK=ak;
  clear_atom_list  (3, ps->param[ps->nr].a);
  clear_force_param(0, ps->param[ps->nr].c);
  ps->param[ps->nr].s = strdup("");
  ps->nr++;
}

void add_dum3_param(t_params *ps,int ai,int aj,int ak,int al, 
		    real c0, real c1)
{
  pr_alloc(1,ps);
  ps->param[ps->nr].AI=ai;
  ps->param[ps->nr].AJ=aj;
  ps->param[ps->nr].AK=ak;
  ps->param[ps->nr].AL=al;
  clear_atom_list  (4, ps->param[ps->nr].a);
  ps->param[ps->nr].C0=c0;
  ps->param[ps->nr].C1=c1;
  clear_force_param(2, ps->param[ps->nr].c);
  ps->param[ps->nr].s = strdup("");
  ps->nr++;
}

void add_dum3_atoms(t_params *ps,int ai,int aj,int ak,int al, bool bSwapParity)
{
  pr_alloc(1,ps);
  ps->param[ps->nr].AI=ai;
  ps->param[ps->nr].AJ=aj;
  ps->param[ps->nr].AK=ak;
  ps->param[ps->nr].AL=al;
  clear_atom_list  (4, ps->param[ps->nr].a);
  clear_force_param(0, ps->param[ps->nr].c);
  if (bSwapParity)
    ps->param[ps->nr].C1=-1;
  ps->param[ps->nr].s = strdup("");
  ps->nr++;
}

void add_dum4_atoms(t_params *ps,int ai,int aj,int ak,int al,int am)
{
  pr_alloc(1,ps);
  ps->param[ps->nr].AI=ai;
  ps->param[ps->nr].AJ=aj;
  ps->param[ps->nr].AK=ak;
  ps->param[ps->nr].AL=al;
  ps->param[ps->nr].AM=am;
  clear_atom_list  (5, ps->param[ps->nr].a);
  clear_force_param(0, ps->param[ps->nr].c);
  ps->param[ps->nr].s = strdup("");
  ps->nr++;
}

int search_jtype(t_restp *rtp,char *name,bool bNterm)
{
  int  j,k,kmax,jmax;
  char *rtpname,searchname[12];
  
  strcpy(searchname,name);
  
  /* Do a best match comparison */
  /* for protein N-terminus, rename H1, H2 and H3 to H */
  if ( bNterm && (strlen(searchname)==2) && (searchname[0] == 'H') && 
       ( (searchname[1] == '1') || (searchname[1] == '2') || 
	 (searchname[1] == '3') ) )
    searchname[1]='\0';
  kmax=0;
  jmax=-1;
  for(j=0; (j<rtp->natom); j++) {
    rtpname=*(rtp->atomname[j]);
    if (strcasecmp(searchname,rtpname) == 0) {
      jmax=j;
      kmax=strlen(searchname);
      break;
    }
    for(k=0; k < min(strlen(searchname), strlen(rtpname)); k++) 
      if (searchname[k] != rtpname[k])
	break;
    if (k > kmax) {
      kmax=k;
      jmax=j;
    }
  }
  if (jmax == -1)
    fatal_error(0,"Atom %s not found in database in residue %s",
		searchname,rtp->resname);
  if (kmax != strlen(searchname))
    fatal_error(0,"Atom %s not found in database in residue %s, "
		"it looks a bit like %s",
		searchname,rtp->resname,*(rtp->atomname[jmax]));
  return jmax;
}


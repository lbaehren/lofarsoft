/***************************************************************************/
/*                                                                         */
/* function create_outputname                                              */
/*                                                                         */
/* it creates the name for the outputfile according to the selected device */
/*                                                                         */
/* RB 04 Oct 08: appropriate extentions to the output png files           */
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

void create_outputname( char inpfile[], char inpdev[], char outputfile[],
			int plotnum, char add_work[])
{
  char *found = 0;
  char ext[8];

  if (!( (strings_compare(inpdev,"/xs")) || (strings_compare(inpdev,"/XS")) ||
	 (strings_compare(inpdev,"/xw")) || (strings_compare(inpdev,"/XW")) ))
  {
    if (plotnum==0)
    {
      if (strstr(inpfile, "bps") != NULL) strcpy(ext,".bps");
      else if (strstr(inpfile, "bp") != NULL) strcpy(ext,".bp");
      else if (strstr(inpfile, "ts") != NULL) strcpy(ext,".ts");
      else strcpy(ext,"");
    }

    strcpy (outputfile,inpfile);

    found = strchr (outputfile,'.');
    if (found)
      *found = '\0';

    if (plotnum == 0)
      strcat (outputfile, ext);
    else
    {
      strcat(outputfile,".");
      strcat(outputfile,add_work);
    }

    strcpy(ext,inpdev);

    while((found=strpbrk(ext,"/"))!=NULL) *found='.';

    strcat(outputfile,ext);
    strcat(outputfile,inpdev);
  }
  else
  {
    outputfile[0] = '1' + plotnum;
    outputfile[1] = '\0';
    strcat(outputfile,inpdev);
    printf(" The output file will be directed to %s screen \n",outputfile);
  }
}

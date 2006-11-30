
#include <readdat.h>

//FILE *errfd;

// =============================================================================

GlishArray readDataFromFile (const String filename,
			     const int type,
			     const int block,
			     const int offset)
{
  Bool verboseON (False);
  Vector<Float> array(block);
  short tmp16;           // For i386 Linux. May be different on other machines.
  short *tmppoint;       // 
  uint32_t *tmp32point,tmp32;       // 
  int len(0);            // 
  int i(0),j(0);         // 
  float tmpfloat(0.0);   // 
  float *tmpfloatpoint;   // 
  GlishArray retval;     // Array of value returned.

   // ----------------------------------------------------------------
   // Provide some feedback

   if (verboseON) {
     cout << "[readdat::readDataFromFile]" << endl;
     cout << " - Filename ........ : " << filename.c_str() << endl;
     cout << " - Type of data file : " << type << endl;
     cout << " - Data block ...... : " << block << endl;
     cout << " - Offset .......... : " << offset << endl;
   }

  // -----------------------------------------------------------------
  // Open file for reading and check if operation is possible; if not 
  // throw an error.

  const char *datafileName = filename.c_str();
  //fprintf(errfd,"open file %s  ;",datafileName);
  FILE *fd = fopen(datafileName,"r");
  //fprintf(errfd,"opend file %s fd: %d \n",datafileName,fd==NULL ? 1 : 0);
  //fflush(errfd);
  if (fd == NULL) {
    cerr << "[readDataFromFile] Can't open file :" << datafileName << endl;
    retval = False;
  } else {
    switch (type) {
    case 1:  // Filetype : int16
      if (offset >0) {
	for (i=0;i<offset;i++) {
	  fread(&tmp16, sizeof(tmp16), 1, fd);
	}
      }
      i=0;
      while ( (! feof(fd)) && (i<block) ){
	len = fread(&tmp16, sizeof(tmp16), 1, fd);
	if (len) {
	  array(i) = tmp16;
	}
	i++;
      };
      if (i<block) {
	array.resize(i,True);
      }
      retval = array;
      break;
    case 2:  // Filetype : tab
      if (offset >0) {
	for (i=0;i<offset;i++) {
	  fscanf(fd,"%f",&tmpfloat);
	}
      }
      i=0;
      while ( (! feof(fd)) && (i<block) ){
	len = fscanf(fd,"%f",&tmpfloat);
	if (len) {
	  array(i) = tmpfloat;
	}
	i++;
      };
      if (i<block) {
	array.resize(i,True);
      }
      retval = array;
      break;
    case 3:  // Filetype : int16b  
      if (offset >0) fseek(fd, (offset*sizeof(tmp16)), SEEK_SET);
      tmppoint = (short *)calloc(block, sizeof(tmp16) );
      if (tmppoint != NULL ) {
	i = fread(tmppoint, sizeof(tmp16), block, fd);
	retval= Vector<short>(IPosition(1,i),tmppoint,SHARE);     
	free(tmppoint);
      } else {
	retval = False;
      };
      break;
    case 4:  // Filetype : int16be (int16, big-endian, offset=bytes)
      if (offset >0) fseek(fd, (offset), SEEK_SET);
      tmppoint = (short *)calloc(block, sizeof(tmp16) );
      if (tmppoint != NULL ) {
	j = fread(tmppoint, sizeof(tmp16), block, fd);
	for (i=0;i<j;i++) { //Yes this isn't fast, but I don't know a better way
	  array(i) = (short)ntohs(tmppoint[i]);
	};
	if (i<block) {
	  array.resize(i,True);
	};
	retval = array;
	free(tmppoint);
      } else {
	retval = False;
      };
      break;
     case 5:  // Filetype : int16o (int16o, little-endian, offset=bytes)
      if (offset >0) fseek(fd, offset, SEEK_SET);
      tmppoint = (short *)calloc(block, sizeof(tmp16) );
      if (tmppoint != NULL ) {
	i = fread(tmppoint, sizeof(tmp16), block, fd);
	retval= Vector<short>(IPosition(1,i),tmppoint,SHARE);     
	free(tmppoint);
      } else {
	retval = False;
      };
      break;
    case 6:  // Filetype : float32 (float32, big-endian, offset=bytes)
      if (offset >0) fseek(fd, (offset), SEEK_SET);
      tmp32point = (uint32_t *)calloc(block, 4 ); //hardcoded to 4 bytes/word
      if (tmp32point != NULL ) {
	j = fread(tmp32point, 4, block, fd);//hardcoded to 4 bytes/word
        tmpfloatpoint =(float *)&tmp32;//typecast ho! You probably don't want to know what this does...
	for (i=0;i<j;i++) { //Yes this isn't fast, but I don't know a better way
	    tmp32 = ntohl(tmp32point[i]);
            array(i) = *tmpfloatpoint/1000; //If this works you are really lucky!
	};
	if (i<block) {
	  array.resize(i,True);
	};
	retval = array;
	free(tmp32point);
      } else {
	retval = False;
      };
      break;
    case 7:  // Filetype : float32le (float32le, little-endian, offset=bytes)
      if (offset >0) fseek(fd, offset, SEEK_SET);
      tmpfloatpoint = (float *)calloc(block, sizeof(float) );
      if (tmpfloatpoint != NULL ) {
	i = fread(tmpfloatpoint, sizeof(float), block, fd);
	retval = Vector<float>(IPosition(1,i),tmpfloatpoint,SHARE);     
	free(tmpfloatpoint);
      } else {
	retval = False;
      };
      break;
   default:
      retval = False;
    };
  }

  // Close the filestream
  fclose(fd);
  
  // Return the data read from file
  return retval;
}

// --------------------------------------------------------------------- readfile

Bool readfile (GlishSysEvent &event, void *) {

   GlishSysEventSource *glishBus = event.glishSource();

   // ----------------------------------------------------------------
   // Check if the provided event value type is indeed a Glish record

   if (event.val().type() != GlishValue::RECORD) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };

   // ----------------------------------------------------------------
   // Extract the filename

   String filename;

   GlishRecord filerec = event.val();
   // get filename; mandatory!
   if (! filerec.exists("filename") ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishArray gtmp = filerec.get("filename");
   gtmp.get(filename);

   // ----------------------------------------------------------------
   // Get type, block and offset

   int type(1);
   int block = 0x1000000;   // default blocksize 16M
   int offset(0);
   String filetype;

   if ( filerec.exists("type") ) {
     gtmp = filerec.get("type");
     gtmp.get(filetype);
     if (filetype == "int16") {
       type = 1;
     } else if (filetype == "tab") {
       type = 2;
     } else if (filetype == "int16b") {
       type = 3;
     } else if (filetype == "int16be") { //16bit integers big endian
       type = 4;
     } else if (filetype == "int16o") { //16bit integers byte-offset
       type = 5;
     } else if (filetype == "float32") {//32bit floats, byte-offset, big endian
       type = 6;
     } else if (filetype == "float32le") {//32bit floats, byte-offset, little endian
       type = 7;
     } else { // Unknown type do nothing!
       if (glishBus->replyPending()) {
	 glishBus->reply(GlishArray(False));
       };
       return True;
     };
   };
   if ( filerec.exists("block") ) {
     gtmp = filerec.get("block");
     gtmp.get(block);
     if (block < 0) block =0x1000000; // set to default blocksize 16M
   };
   if ( filerec.exists("offset") ) {
     gtmp = filerec.get("offset");
     gtmp.get(offset);
   };

   // ----------------------------------------------------------------
   // Read data from file

   GlishArray retval = readDataFromFile (filename,
					 type,
					 block,
					 offset);

   if (glishBus->replyPending()) {
     glishBus->reply(retval);
   };
   return True;   
}

// -------------------------------------------------------------------- writefile

Bool writefile(GlishSysEvent &event, void *) {
   GlishSysEventSource *glishBus = event.glishSource();

   if (event.val().type() != GlishValue::RECORD) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishRecord filerec = event.val();
   // get filename; mandatory!
   if (! filerec.exists("filename") ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishArray gtmp = filerec.get("filename");
   String tmpString;
   gtmp.get(tmpString);
   const char *filename = tmpString.c_str();
   FILE *fd = fopen(filename,"w");
   if (fd == NULL) {
     cout << "Can't open file :" << filename << endl;
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };

   // get type, 
   int type = 1; // default blocksize 16M
   if ( filerec.exists("type") ) {
     gtmp = filerec.get("type");
     gtmp.get(tmpString);
     if (tmpString == "int16") {
       type = 1;
     } else if (tmpString == "tab") {
       type = 2;
     } else if (tmpString == "int16be") {
       type = 3;
     } else if (tmpString == "int32") {
       type = 10;
     } else if (tmpString == "int32be") {
       type = 11;
     } else { // Unknown type do nothing!
       if (glishBus->replyPending()) {
	 glishBus->reply(GlishArray(False));
       };
       return True;
     };
   };

   // get the data
   if (! filerec.exists("data") ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   gtmp = filerec.get("data");
   
   Bool deleteit;
   unsigned int len,i=0,tmp;
   GlishArray retval;
   if (type<10){ //data has type "short int"
     Vector<Short> data16;
     if (! gtmp.get(data16) ) {
       if (glishBus->replyPending()) {
	 glishBus->reply(GlishArray(False));
       };
       return True;
     };
     short *tmp16point,stmp; // For i386 Linux. May be different on other machines.
     len=data16.nelements();
     
     tmp16point = data16.getStorage(deleteit);
     switch (type) {
     case 1: 
       fwrite(tmp16point, sizeof(*tmp16point), len, fd);
       retval = True;
       break;
     case 2:
       i=0;
       while ( (i<len) ){
	 fprintf(fd,"%i \n",tmp16point[i]);
	 i++;
       };
       retval = True;
       break;
     case 3: //16-bit integers big-endian
       i=0;
       while ( (i<len) ){
	 stmp = htons(tmp16point[i]);
	 fwrite(&stmp, sizeof(*tmp16point), 1, fd);
	 i++;
       };
       retval = True;
       break;
     default:
       retval = False;
     };
     data16.freeStorage((const short *)tmp16point,deleteit);
   } else if (type < 20){ //data has type "int"
     Vector<Int> data32;
     if (! gtmp.get(data32) ) {
       if (glishBus->replyPending()) {
	 glishBus->reply(GlishArray(False));
       };
       return True;
     };
     int *tmp32point; // For i386 Linux. May be different on other machines.
     len=data32.nelements();
     
     tmp32point = data32.getStorage(deleteit);
     switch (type) {
     case 10: 
       fwrite(tmp32point, sizeof(*tmp32point), len, fd);
       retval = True;
       break;
     case 11: 
       i=0;
       while ( (i<len) ){
	 tmp = htonl(tmp32point[i]);
	 fwrite(&tmp, sizeof(*tmp32point), 1, fd);
	 i++;
       };
       retval = True;
       break;
     default:
       retval = False;
     };
     data32.freeStorage((const int *)tmp32point,deleteit);

   } else { //unimplemented type
     retval = False;
   };
   fclose(fd);

   if (glishBus->replyPending()) {
     glishBus->reply(retval);
   };
   return True;   
}

// -------------------------------------------------------------- getVisibilities

Bool getVisibilities (GlishSysEvent &event, void *) {
  
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord record = event.val();
  GlishArray gtmp;
  
  // Declaration of variables
  String metafile;
  String antennafile;
  
  if (event.val().type() != GlishValue::RECORD) {
    if (glishBus->replyPending()) {
      cout << "Event is not a record" << endl;
      glishBus->reply(GlishArray(False));
    };
    //return True;
  };
  cout << "Event type is a record" << endl;
  
  if ((! (record.exists("metafile"))) && (! (record.exists("antennafile"))))
    {
      cerr << "Need record with: metafile and antennafile" << endl;
      if (glishBus->replyPending()) {
	glishBus->reply(GlishArray(False));
      }
    } else {
      cout << "Both the metafile and antennafile files exist" << endl;
    };
  
  // Extract data from Glish record
  
  gtmp = record.get("metafile");
  gtmp.get(metafile);
  
  gtmp = record.get("antennafile");
  gtmp.get(antennafile);
  
  cout << "[readdat::getVisibilities] Extracting the visibility values" << endl;
  
  cout << " - Creating DataCC object ..." << flush;
  DataCC cc (metafile,antennafile);
  cout << "Done." << endl;

  // Read in cc-datacube from disk
  cc.readDataCC();
  
  cout << " - Extracting the visibility data ... " << flush;
  Matrix<DComplex> visibilities = cc.visibilities();
  Matrix<Int> baselines = cc.baselines();
  cout << "Done." << endl;
  
  cout << " - Adding values to the record ... " << flush;
  record.add("visibilities",visibilities);
  record.add("baselines",baselines);
  cout << "Done." << endl;
  
  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[getVisibilities]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
  }
  
  return True;
}


// =============================================================================
//
//  Main routine
//
// =============================================================================

int main(int argc, char *argv[])
{
  //errfd = fopen("readdat.err","w");

   GlishSysEventSource glishStream(argc, argv);

   glishStream.addTarget(readfile, "readfile");
   glishStream.addTarget(writefile, "writefile");
   glishStream.addTarget(getVisibilities, "getVisibilities");

   glishStream.loop();
}

void GetDistanceToShowerAxisRadio(double CorePosX, double CorePosY, double *AntPosCoordinates, double Zenith, double Azimuth, double *distance, double *error){
  *distance = 0;
  double c_err = 7;
  double ZenithRad = Zenith * TMath::Pi() / 180.;
  double AzimuthRad = (180-(Azimuth)) * TMath::Pi() / 180.;
  
  //direction vector: b, given by spherical coordinates
  double b1 = sin(ZenithRad) * cos(AzimuthRad);
  double b2 = sin(ZenithRad) * sin(AzimuthRad);
  double b3 = cos(ZenithRad);
  
  //Shower Core Position taken from Grande: c
  double c1 = CorePosY;
  double c2 = -1*CorePosX;
  double c3 = 0;
  
  //antenna position vector a
  double a[3];
  a[0] = AntPosCoordinates[1];
  a[1] = -1*AntPosCoordinates[0];
  a[2] = 0;

  
  double a1 = a[0];
  double a2 = a[1];
  double a3 = a[2];
  
  //distance d = (|b x ( f )| ) / | b| ) with f = a - c
  double f1 = a1 - c1;
  double f2 = a2 - c2;
  double f3 = a3 - c3;
  
  //absolute value of b
  double absb = sqrt( pow(b1,2.) + pow(b2,2.) + pow(b3,2.));

  *distance = sqrt( pow(b2*f3-b3*f2,2.) + pow(b3*f1-b1*f3,2.) + pow(b1*f2-b2*f1,2.) ) / absb ;
  
  double tmperror = pow((pow(b3,2.) + pow(b2,2.))*(2*a1 - 2*c1) - 2*b1*b3*(a3-c3) - 2*b1*b2*(a2-c2),2.) * pow(c_err,2.) 
  		 + pow((pow(b3,2.) + pow(b1,2.))*(2*a2 - 2*c2) - 2*b2*b3*(a3-c3) - 2*b1*b2*(a1-c1),2.) * pow(c_err,2.) ;

  *error = *distance * 0.5 * sqrt(tmperror) / pow(*distance,2.);
};

/**********************************************************************************************/
void GetAntPos(const char *AntPos, const int daq_id, double *AntCoordinate){
// AntCoordinate[0] : X value in m
// AntCoordinate[1] : Y value in m
// AntCoordinate[2] : Z value in m


  if(daq_id == 17 || daq_id == 0){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] =  47.233; AntCoordinate[1] = -283.658; AntCoordinate[2] = 126.803;}
    if(strcmp(AntPos,"060")==0){ AntCoordinate[0] = 109.353; AntCoordinate[1] = -247.433; AntCoordinate[2] = 126.235;}
    if(strcmp(AntPos,"120")==0){ AntCoordinate[0] = 111.854; AntCoordinate[1] = -318.309; AntCoordinate[2] = 127.982;}
    if(strcmp(AntPos,"180")==0){ AntCoordinate[0] =  47.836; AntCoordinate[1] = -355.645; AntCoordinate[2] = 126.878;}
  }
  else{
  if(daq_id == 19){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] = -188.793; AntCoordinate[1] = -273.553; AntCoordinate[2] = 127.298;}
    if(strcmp(AntPos,"030")==0){ AntCoordinate[0] = -162.258; AntCoordinate[1] = -221.831; AntCoordinate[2] = 128.010;}
  
  }
  else{
  if(daq_id == 30){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] = -186.004; AntCoordinate[1] = -471.813; AntCoordinate[2] = 126.826;}
    if(strcmp(AntPos,"240")==0){ AntCoordinate[0] = -227.652; AntCoordinate[1] = -498.038; AntCoordinate[2] = 128.327;}
    if(strcmp(AntPos,"300")==0){ AntCoordinate[0] = -228.187; AntCoordinate[1] = -447.265; AntCoordinate[2] = 127.604;}
    if(strcmp(AntPos,"360")==0){ AntCoordinate[0] = -186.455; AntCoordinate[1] = -422.287; AntCoordinate[2] = 127.218;}
  
  }
  else{
  if(daq_id == 42){

    if(strcmp(AntPos,"CTR")==0){ AntCoordinate[0] = -26973.0; AntCoordinate[1] = -5699.0; AntCoordinate[2] = 1423.0;}
    if(strcmp(AntPos,"030")==0){ AntCoordinate[0] = -26900.0; AntCoordinate[1] = -5771.0; AntCoordinate[2] = 1423.0;}
    if(strcmp(AntPos,"090")==0){ AntCoordinate[0] = -26874.0; AntCoordinate[1] = -5673.0; AntCoordinate[2] = 1423.0;}
  
  }
  else{
  if(daq_id == 50){

    if(strcmp(AntPos,"000")==0){ AntCoordinate[0] = -26973.0; AntCoordinate[1] = -5699.0; AntCoordinate[2] = 1423.0;}
    if(strcmp(AntPos,"001")==0){ AntCoordinate[0] = -26900.0; AntCoordinate[1] = -5771.0; AntCoordinate[2] = 1423.0;}
    if(strcmp(AntPos,"002")==0){ AntCoordinate[0] = -26874.0; AntCoordinate[1] = -5673.0; AntCoordinate[2] = 1423.0;}
  
  }
  else{
  cerr << "daq id " << daq_id << " is not defined" << endl;
  }}}}}
  
}

/**********************************************************************************************/

void CheckDistanceShwoerAxis(){
  char AntPos[1024];
  sprintf(AntPos,"060");
  int daq_id = 17;
  double AntCoordinate[3];
  
  GetAntPos(AntPos, daq_id, AntCoordinate);
  cout << "ant pos = " << AntCoordinate[0] << " -- " << AntCoordinate[1] << " -- " << AntCoordinate[2] << endl;
  
  //Grande
  double Xcg=8.60609;
  double Ycg=-368.093;
  double Azg=354.552-15.7;
  double Zeg=26.135;
  
/*  AntCoordinate[0] = -267.3;
  AntCoordinate[1] = -38.31;
  AntCoordinate[2] = 0;  
  Xcg=-332.7;
  Ycg=-29.8;
 */
/*  AntCoordinate[1] = -267.793;
  AntCoordinate[0] = 38.318;
  AntCoordinate[2] = 0;  
  Ycg=-356.1;
  Xcg=-91.3;
 */
/*  Ycg=-107.01;
  Xcg=-351.55;*/
//  Azg=185.442;
// Zeg=0;
  
  
  double distance = 0;
  double error = 0;
  GetDistanceToShowerAxisRadio(Xcg, Ycg, AntCoordinate, Zeg, Azg, &distance, &error);
  
  cout << "Distance = " << distance << " +/- " << error << endl;
}

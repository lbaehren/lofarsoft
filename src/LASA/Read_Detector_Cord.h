void Read_Detector_Cord(FILE *fp)
{
	char content[200] ;
	char det[50],x[50],y[50] ;
	float X,Y ;
	int i=0 ;
	while(fgets(content,200,fp)!=NULL)
	{
		if(strncmp(content,"//",strlen("//"))==0) continue ;
		std::istrstream M(content) ;
		M>>det>>x>>y ;
		std::istrstream XX(x) ;
		std::istrstream YY(y) ;
		XX>>X ;
		YY>>Y ;
		Det_X[i]=X ; 
		Det_Y[i]=Y ; 
		printf("\n%d\t%f\t%f\n\n",i,Det_X[i],Det_Y[i]) ;
		i++ ;
	}
}

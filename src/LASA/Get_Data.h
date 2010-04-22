//This function reads the data from the Master and the Slave units send by LASAs

void Read_Detector_Cord()
{
	char content[200] ;
	char det[50],x[50],y[50],z[50] ;
	float X,Y,Z ;
	int i=0 ;
	FILE *fp ;
	fp=fopen("./data/Detector_Cord.dat","r") ;
	while(fgets(content,200,fp)!=NULL)
	{
		if(strncmp(content,"//",strlen("//"))==0) continue ;
		std::istrstream M(content) ;
		M>>det>>x>>y>>z ;
		std::istrstream XX(x) ;
		std::istrstream YY(y) ;
		std::istrstream ZZ(z) ;
		XX>>X ;
		YY>>Y ;
		ZZ>>Z ;
		X_Cor[i]=X ; 
		Y_Cor[i]=Y ; 
		Z_Cor[i]=Z ; 
		i++ ;
		if(i==NO_LASAs*4) break ;
	}
	fclose(fp) ;
}

void Reset_Final_Data()
{
	for(int i=0;i<NO_LASAs;i++)
	{
		final_data[i].master[0].YMD=0 ;
		final_data[i].master[1].YMD=0 ;
		final_data[i].slave[0].YMD=0 ;
		final_data[i].slave[1].YMD=0 ;
		final_data[i].master[0].GPS_time_stamp=0 ;
		final_data[i].master[1].GPS_time_stamp=0 ;
		final_data[i].slave[0].GPS_time_stamp=0 ;
		final_data[i].slave[1].GPS_time_stamp=0 ;
		final_data[i].master[0].nsec=0 ;
		final_data[i].master[1].nsec=0 ;
		final_data[i].slave[0].nsec=0 ;
		final_data[i].slave[1].nsec=0 ;
		final_data[i].master[0].CTD=0 ;
		final_data[i].master[1].CTD=0 ;
		final_data[i].slave[0].CTD=0 ;
		final_data[i].slave[1].CTD=0 ;
		final_data[i].master[0].Trigg_pattern=0 ;
		final_data[i].master[1].Trigg_pattern=0 ;
		final_data[i].slave[0].Trigg_pattern=0 ;
		final_data[i].slave[1].Trigg_pattern=0 ;
		final_data[i].master[0].Total_counts=0 ;
		final_data[i].master[1].Total_counts=0 ;
		final_data[i].slave[0].Total_counts=0 ;
		final_data[i].slave[1].Total_counts=0 ;
		final_data[i].master[0].Pulse_height=0 ;
		final_data[i].master[1].Pulse_height=0 ;
		final_data[i].slave[0].Pulse_height=0 ;
		final_data[i].slave[1].Pulse_height=0 ;
		final_data[i].master[0].Pulse_width=0 ;
		final_data[i].master[1].Pulse_width=0 ;
		final_data[i].slave[0].Pulse_width=0 ;
		final_data[i].slave[1].Pulse_width=0 ;
		memset(final_data[i].master[0].counts,0,sizeof(final_data[i].master[0].counts)) ;
		memset(final_data[i].master[1].counts,0,sizeof(final_data[i].master[1].counts)) ;
		memset(final_data[i].slave[0].counts,0,sizeof(final_data[i].slave[0].counts)) ;
		memset(final_data[i].slave[1].counts,0,sizeof(final_data[i].slave[1].counts)) ;
		final_data[i].Trigg_Status=0 ;
	}
}

void Initialise_Variables()
{
	for(int i=0;i<NO_LASAs;i++)
	{
		//if(lasa_select[i]==0) continue ;
		lasa[i].tot_bytes_read_master=0 ;
		lasa[i].tot_bytes_read_slave=0 ;
		lasa[i].pointer_data_master=0 ;
		lasa[i].pointer_data_slave=0 ;
		lasa[i].flag_master=0 ;
		lasa[i].flag_slave=0 ;

		sec_data[i].Lasa=i+1 ;
		sec_data[i].CTP=0 ;

		log_book[i].master[0].Detector=4*i+1 ;
		log_book[i].master[1].Detector=4*i+2 ;
		log_book[i].slave[0].Detector=4*i+3 ;
		log_book[i].slave[1].Detector=4*i+4 ;
		
		final_data[i].master[0].detector=4*i+1 ;
		final_data[i].master[1].detector=4*i+2 ;
		final_data[i].slave[0].detector=4*i+3 ;
		final_data[i].slave[1].detector=4*i+4 ;
		Reset_Final_Data() ;
			
		last_trigger_events[i].time_stamp=0 ;
		printf("final_data[%d].master[0].detector=%d\n",i,final_data[i].master[0].detector) ;
		printf("final_data[%d].master[1].detector=%d\n",i,final_data[i].master[1].detector) ;
		printf("final_data[%d].slave[0].detector=%d\n",i,final_data[i].slave[0].detector) ;
		printf("final_data[%d].slave[1].detector=%d\n",i,final_data[i].slave[1].detector) ;
	
		Calib_Master_Done[i]=0 ;
		Calib_Slave_Done[i]=0 ;

		if(lasa_select[i]==0) continue ;
		lasa[i].Master->Initialise_Variables() ;
		lasa[i].Slave->Initialise_Variables() ;

		//Calib_Master_Done[i]=0 ;
		//Calib_Slave_Done[i]=0 ;
	}

	for(int i=0;i<Max_Event;i++)
		array_event_times[i].time_stamp=0 ;

	pointer_event=0 ;
	time0.year=0 ;
	counter=0 ;
	counter_sec=0 ;
	counter_log=0 ;
	counter_Monitor2=1 ;
	counter_Monitor_rate=1 ;
	counter_Event_Size=1 ;
	time1=time2=0 ;
	Read_Detector_Cord() ;
}

void Create_ROOT_File()	//Creating ROOT file
{
	f=new TFile("./output/lasa.root","recreate") ;

	tree_sec=new TTree("Tree_sec","Title") ;	//One sec tree
	tree_sec->SetAutoSave(1000000) ;		//Autosave every 1Mbyte of data
	char detector[20],num[10] ;
	for(int i=0;i<NO_LASAs;i++)
	{
		strcpy(detector,"") ;
		strcpy(detector,"Lasa") ;
		sprintf(num,"%d",i+1) ;
		strcat(detector,num) ;
		tree_sec->Branch(detector,&sec_data[i].Lasa,"Lasa/i:YMD:GPS_time_stamp:sync:CTP:quant/F:Channel_1_Thres_count_high/s:Channel_1_Thres_count_low:Channel_2_Thres_count_high:Channel_2_Thres_count_low:Satellite_info[61]/b") ;
	}

	//-------------
	tree_log=new TTree("Tree_log","Title") ;
	tree_log->SetAutoSave(100000) ;			//Autosave every 100Kbyte of data
	for(int i=0;i<NO_LASAs;i++)
	{
		strcpy(detector,"") ;
		strcpy(detector,"Det") ;
		sprintf(num,"%d",4*i+1) ;
		strcat(detector,num) ;
		tree_log->Branch(detector,&log_book[i].master[0],"Detector/i:YMD/i:HMS:Time_stamp:Channel_offset_pos/s:Channel_offset_neg:Channel_gain_pos:Channel_gain_neg:Common_offset_adj:Full_scale_adj:Channel_inte_time:Comp_thres_low:Comp_thres_high:Channel_HV:Channel_thres_low:Channel_thres_high:Trigger_condition:Pre_coin_time:Coin_time:Post_coin_time") ;

		strcpy(detector,"") ;
		strcpy(detector,"Det") ;
		sprintf(num,"%d",4*i+2) ;
		strcat(detector,num) ;
		tree_log->Branch(detector,&log_book[i].master[1],"Detector/i:YMD/i:HMS:Time_stamp:Channel_offset_pos/s:Channel_offset_neg:Channel_gain_pos:Channel_gain_neg:Common_offset_adj:Full_scale_adj:Channel_inte_time:Comp_thres_low:Comp_thres_high:Channel_HV:Channel_thres_low:Channel_thres_high:Trigger_condition:Pre_coin_time:Coin_time:Post_coin_time") ;

		strcpy(detector,"") ;
		strcpy(detector,"Det") ;
		sprintf(num,"%d",4*i+3) ;
		strcat(detector,num) ;
		tree_log->Branch(detector,&log_book[i].slave[0],"Detector/i:YMD/i:HMS:Time_stamp:Channel_offset_pos/s:Channel_offset_neg:Channel_gain_pos:Channel_gain_neg:Common_offset_adj:Full_scale_adj:Channel_inte_time:Comp_thres_low:Comp_thres_high:Channel_HV:Channel_thres_low:Channel_thres_high:Trigger_condition:Pre_coin_time:Coin_time:Post_coin_time") ;

		strcpy(detector,"") ;
		strcpy(detector,"Det") ;
		sprintf(num,"%d",4*i+4) ;
		strcat(detector,num) ;
		tree_log->Branch(detector,&log_book[i].slave[1],"Detector/i:YMD/i:HMS:Time_stamp:Channel_offset_pos/s:Channel_offset_neg:Channel_gain_pos:Channel_gain_neg:Common_offset_adj:Full_scale_adj:Channel_inte_time:Comp_thres_low:Comp_thres_high:Channel_HV:Channel_thres_low:Channel_thres_high:Trigger_condition:Pre_coin_time:Coin_time:Post_coin_time") ;
	}

	//----------------------------------
	tree_event=new TTree("Tree_event","Title") ;	//Event tree
	tree_event->SetAutoSave(100000000) ;		//Autosave every 100Mbyte of data
	for(int i=0;i<NO_LASAs;i++)
	{
		strcpy(detector,"") ;
		strcpy(detector,"Det") ;
		sprintf(num,"%d",4*i+1) ;
		strcat(detector,num) ;
		tree_event->Branch(detector,&final_data[i].master[0].detector,"detector/i:YMD:GPS_time_stamp:CTD:nsec:Trigg_condition:Trigg_pattern:Total_counts:Pulse_height:Pulse_width:counts[4000]/s") ;
		strcpy(detector,"") ;
		strcpy(detector,"Det") ;
		sprintf(num,"%d",4*i+2) ;
		strcat(detector,num) ;
		tree_event->Branch(detector,&final_data[i].master[1].detector,"detector/i:YMD:GPS_time_stamp:CTD:nsec:Trigg_condition:Trigg_Pattern:Total_counts:Pulse_height:Pulse_width:counts[4000]/s") ;
		strcpy(detector,"") ;
		strcpy(detector,"Det") ;
		sprintf(num,"%d",4*i+3) ;
		strcat(detector,num) ;
		tree_event->Branch(detector,&final_data[i].slave[0].detector,"detector/i:YMD:GPS_time_stamp:CTD:nsec:Trigg_condition:Trigger_pattern:Total_counts:Pulse_height:Pulse_width:counts[4000]/s") ;
		strcpy(detector,"") ;
		strcpy(detector,"Det") ;
		sprintf(num,"%d",4*i+4) ;
		strcat(detector,num) ;
		tree_event->Branch(detector,&final_data[i].slave[1].detector,"detector/i:YMD:GPS_time_stamp:CTD:nsec:Trigg_condition:Trigger_pattern:Total_counts:Pulse_height:Pulse_width:counts[4000]/s") ;
	}
}

void Write_ROOT_File()	//Writing event file
{
	tree_event->Write() ;
	tree_sec->Write() ;
	tree_log->Write() ;
	f->Close() ;
}

void Get_Data()
{
	fd_set readfds ;
	FD_ZERO(&readfds) ;

	for(int i=0;i<NO_LASAs;i++)
	{
		if(lasa_select[i]==0) continue ;
		FD_SET(lasa[i].sockfd_master,&readfds) ;
		FD_SET(lasa[i].sockfd_slave,&readfds) ;
	}

	select(sockfd_Max+1,&readfds,NULL,NULL,NULL) ;

	for(int i=0;i<NO_LASAs;i++)
	{
	if(lasa_select[i]==0) continue ;
	if(FD_ISSET(lasa[i].sockfd_master,&readfds))
	{
		if(Calibration==On && Calib_Master_Done[i]==0)
		{
			if((bytes_read = read(lasa[i].sockfd_master,lasa[i].data_master+lasa[i].pointer_data_master,sizeof (lasa[i].data_master)-lasa[i].pointer_data_master))==-1)
				perror("read(): Error\n") ;
			printf("Offset [Lasa %2d]: Master: [1+]=%d [1-]=%d [2+]=%d [2-]=%d\n",i+1,lasa[i].data_master[0],lasa[i].data_master[1],lasa[i].data_master[2],lasa[i].data_master[3]) ;	
			printf("Gain   [Lasa %2d]: Master: [1+]=%d [1-]=%d [2+]=%d [2-]=%d\n",i+1,lasa[i].data_master[4],lasa[i].data_master[5],lasa[i].data_master[6],lasa[i].data_master[7]) ;
			printf("Common Offset [Lasa %2d]: Master: %d\n",i+1,lasa[i].data_master[8]) ;	
			Calib_Master_Done[i]=1 ;
			Calib_Done++ ;	
		}

		if(Calibration==Off)
		{
			if(lasa[i].flag_master==1)
			{
				lasa[i].data_master[0]=0x99 ;
				lasa[i].pointer_data_master=1 ;
				lasa[i].tot_bytes_read_master=1 ;
				lasa[i].flag_master=0 ;
			}
			if((bytes_read = read(lasa[i].sockfd_master,lasa[i].data_master+lasa[i].pointer_data_master,sizeof (lasa[i].data_master)-lasa[i].pointer_data_master))==-1)
				perror("read(): Error\n") ;

			//printf("lasa[%2d] (Master): Bytes read=%d\n",i+1,bytes_read) ;
			if(bytes_read==0) 
			{
				Write_ROOT_File() ;
				exit(1) ;
			}
			lasa[i].tot_bytes_read_master=lasa[i].tot_bytes_read_master+bytes_read ;
			if(lasa[i].tot_bytes_read_master<=2)
			{
				lasa[i].pointer_data_master=lasa[i].tot_bytes_read_master ;
			}
			if(lasa[i].tot_bytes_read_master>2)
			{
				lasa[i].flag_master=lasa[i].Master->Data_Processing(lasa[i].data_master,lasa[i].tot_bytes_read_master,i,1) ;
				lasa[i].pointer_data_master=0 ;
				lasa[i].tot_bytes_read_master=0 ;
			}
		}
	}

	if(FD_ISSET(lasa[i].sockfd_slave,&readfds))
	{
		if(Calibration==On && Calib_Slave_Done[i]==0)
		{
			if((bytes_read = read(lasa[i].sockfd_slave,lasa[i].data_slave+lasa[i].pointer_data_slave,sizeof (lasa[i].data_slave)-lasa[i].pointer_data_slave))==-1)
				perror("read(): Error\n") ;
			printf("Offset [Lasa %2d]: Slave:  [1+]=%d [1-]=%d [2+]=%d [2-]=%d\n",i+1,lasa[i].data_slave[0],lasa[i].data_slave[1],lasa[i].data_slave[2],lasa[i].data_slave[3]) ;	
			printf("Gain   [Lasa %2d]: Slave:  [1+]=%d [1-]=%d [2+]=%d [2-]=%d\n",i+1,lasa[i].data_slave[4],lasa[i].data_slave[5],lasa[i].data_slave[6],lasa[i].data_slave[7]) ;
			printf("Common Offset [Lasa %2d]: Slave: %d\n",i+1,lasa[i].data_slave[8]) ;	
			Calib_Slave_Done[i]=1 ;	
			Calib_Done++ ;	
		}
		
		if(Calibration==Off)
		{
			if(lasa[i].flag_slave==1)
			{
				lasa[i].data_slave[0]=0x99 ;
				lasa[i].pointer_data_slave=1 ;
				lasa[i].tot_bytes_read_slave=1 ;
				lasa[i].flag_slave=0 ;
			}
			if((bytes_read = read(lasa[i].sockfd_slave,lasa[i].data_slave+lasa[i].pointer_data_slave,sizeof (lasa[i].data_slave)-lasa[i].pointer_data_slave))==-1)
				perror("read(): Error\n") ;

			//printf("lasa[%2d] (Slave): Bytes read=%d\n",i+1,bytes_read) ;
			if(bytes_read==0)
			{
				Write_ROOT_File() ;
				exit(1) ;
			}
			lasa[i].tot_bytes_read_slave=lasa[i].tot_bytes_read_slave+bytes_read ;
			if(lasa[i].tot_bytes_read_slave<=2)
			{
				lasa[i].pointer_data_slave=lasa[i].tot_bytes_read_slave ;
			}
			if(lasa[i].tot_bytes_read_slave>2)
			{
				lasa[i].flag_slave=lasa[i].Slave->Data_Processing(lasa[i].data_slave,lasa[i].tot_bytes_read_slave,i,0) ;
				lasa[i].pointer_data_slave=0 ;
				lasa[i].tot_bytes_read_slave=0 ;
			}
		}
	}
	}	
}

void Store_Sec_Data()
{
	for(int i=0;i<NO_LASAs;i++)
	{
		if(lasa_select[i]==0) continue ;
		lasa[i].Master->Store_Sec_Messages(i) ;
	}
	
	tree_sec->Fill() ;
	if(counter_sec==NO_One_Sec-1) counter_sec=0 ;
	else counter_sec++ ;
}

void Store_Control_Data()
{
	tree_log->Fill() ;
}

void Check_Coincidence()	//Checking coincidence
{
	int k ;
	int jmin=0 ;
	long long unsigned tmin=0 ;
	long long unsigned time3=0 ;	//To calculate time difference between 2 events	
	unsigned int Event_Size=0 ;
	
	//For arrival direction calculation 
	/*unsigned int Max_Count ;	//Detector no. having the maximum event count
	float cdt[NO_LASAs*4] ;		//Arrival time difference between detectors
	float weight[NO_LASAs*4] ;	//Event weight
	Max_Count=0 ;*/

	Reset_Final_Data() ;
	float SumX,SumY,X_Core,Y_Core ;
	SumX=0 ;
	SumY=0 ;

	for(int j=0;j<Spy_Event+NO_LASAs;j++)
	{
		if(temp_array[j].time_stamp>0)
		{	
			printf("Ev no1[%d] [%d] time=%llu\n",x[j],j,temp_array[j].time_stamp) ;
			lasa[temp_array[j].lasa].Master->Build_Final_Events(temp_array[j].lasa,1,temp_array[j].event_no) ;
			lasa[temp_array[j].lasa].Slave->Build_Final_Events(temp_array[j].lasa,0,temp_array[j].event_no) ;
			final_data[temp_array[j].lasa].Trigg_Status=1 ;
			tmin=temp_array[j].time_stamp ;
			//printf("\ttmin=%llu\n",tmin) ;
			jmin=j ;
			k=j ;
			break ;
		}
	}
	if(tmin>0)
	{
	for(int j=k+1;j<Spy_Event+NO_LASAs;j++)
	{
		if(temp_array[j].time_stamp<=(tmin+Coin_Window))
		{
			printf("Ev no2[%d] [%d] time=%llu\ttmin=%llu\tCoin=%llu\t(tmin+Coin)=%llu\n",x[j],j,temp_array[j].time_stamp,tmin,Coin_Window,tmin+Coin_Window) ;
			lasa[temp_array[j].lasa].Master->Build_Final_Events(temp_array[j].lasa,1,temp_array[j].event_no) ;
			lasa[temp_array[j].lasa].Slave->Build_Final_Events(temp_array[j].lasa,0,temp_array[j].event_no) ;
			final_data[temp_array[j].lasa].Trigg_Status=1 ;
			continue ;
		}
		else
		{
		//tmin=temp_array[j].time_stamp ;
		//jmin=j ;
		//final_data[temp_array[j].lasa].Trigg_Status=0 ;
		if(final_data[0].Trigg_Status+final_data[1].Trigg_Status+final_data[2].Trigg_Status+final_data[3].Trigg_Status+final_data[4].Trigg_Status==Station_Trigger)
		{
			tree_event->Fill() ;		//Storing events for all the detectors
			printf("\n") ;
			printf("Ev no3[%d] [%d] time=%llu\n",x[j],j,temp_array[j].time_stamp) ;
			printf("\tGPS1=%u\tnsec1=%u\tTrigg1=%u\tGPS2=%u\tnsec2=%u\tTrigg2=%u\n",final_data[0].master[0].GPS_time_stamp,final_data[0].master[0].nsec,final_data[0].Trigg_Status,final_data[1].master[0].GPS_time_stamp,final_data[1].master[0].nsec,final_data[1].Trigg_Status);

			int w=0 ;
			for(int i=0;i<NO_LASAs;i++)
			{
				if(lasa_select[i]==0)
				{
					//w=w+4 ;
					continue ;
				}
				Event_Size=Event_Size+final_data[i].master[0].Total_counts ;
				Event_Size=Event_Size+final_data[i].master[1].Total_counts ;
				Event_Size=Event_Size+final_data[i].slave[0].Total_counts ;
				Event_Size=Event_Size+final_data[i].slave[1].Total_counts ;

				SumX=SumX+final_data[i].master[0].Total_counts*X_Cor[4*i+0] ;
				SumX=SumX+final_data[i].master[1].Total_counts*X_Cor[4*i+1] ;
				SumX=SumX+final_data[i].slave[0].Total_counts*X_Cor[4*i+2] ;
				SumX=SumX+final_data[i].slave[1].Total_counts*X_Cor[4*i+3] ;

				SumY=SumY+final_data[i].master[0].Total_counts*Y_Cor[4*i+0] ;
				SumY=SumY+final_data[i].master[1].Total_counts*Y_Cor[4*i+1] ;
				SumY=SumY+final_data[i].slave[0].Total_counts*Y_Cor[4*i+2] ;
				SumY=SumY+final_data[i].slave[1].Total_counts*Y_Cor[4*i+3] ;

				/*if(final_data[i].master[0].Total_counts>Max_Count) Max_Count=final_data[i].master[0].detector ;
				if(final_data[i].master[1].Total_counts>Max_Count) Max_Count=final_data[i].master[1].detector ;
				if(final_data[i].slave[0].Total_counts>Max_Count) Max_Count=final_data[i].slave[0].detector ;
				if(final_data[i].slave[1].Total_counts>Max_Count) Max_Count=final_data[i].slave[1].detector ;
				weight[w]=final_data[i].master[0].Total_counts ;
				weight[w+1]=final_data[i].master[1].Total_counts ;
				weight[w+2]=final_data[i].slave[0].Total_counts ;
				weight[w+3]=final_data[i].slave[1].Total_counts ;
				w=w+4 ;*/
			}
			//Arrival_Direction->Get_Origin(Max_Count,&cdt[0],&weight[0]) ;
			Monitoring->Fill_Event_Size(Event_Size) ;

			X_Core=SumX/Event_Size ;
			Y_Core=SumY/Event_Size ;
			Monitoring->Fill_Event_Core(X_Core,Y_Core) ;
			Event_Size=0 ;
			SumX=0 ;
			SumY=0 ;

			if(time3==0) time3=temp_array[j].time_stamp ;
			else
			{
				Monitoring->Fill_Time_Diff((float)((temp_array[j].time_stamp-time3)/1000.0)) ;	//in micro secs.
				time3=temp_array[j].time_stamp ;
			}
			if(counter_Monitor2==Monitor2 && Monitoring->Click2_return()==1)
			{
				for(int i=0;i<NO_LASAs;i++)
				{
					if(lasa_select[i]==0) continue ;
					Monitoring->Fill_XY_display(final_data[i].master[0].detector,final_data[i].master[0].Total_counts) ;
					Monitoring->Fill_XY_display(final_data[i].master[1].detector,final_data[i].master[1].Total_counts) ;
					Monitoring->Fill_XY_display(final_data[i].slave[0].detector,final_data[i].slave[0].Total_counts) ;
					Monitoring->Fill_XY_display(final_data[i].slave[1].detector,final_data[i].slave[1].Total_counts) ;
				}
				Monitoring->DRAW_XY_display() ;		//Online event display
				counter_Monitor2=1 ;
			}
			else counter_Monitor2++ ;
			if(counter_Monitor2>Monitor2) counter_Monitor2=1 ;
			
			if(counter_Monitor_rate==Monitor_rate && Monitoring->Click2_return()==1)
			{
				if(time1==0) time1=temp_array[j].time_stamp ;
				else
				{
					time2=temp_array[j].time_stamp ;
					event_rate=(double)(Monitor2*(1000000000.0)/(time2-time1)) ;//in Hz
					if(time1==time2) event_rate=1.e6 ;	//Set some arb. high value 
					Monitoring->DRAW_Event_Rate(event_rate) ;	//Display event rate
					time1=time2 ;
				}
				counter_Monitor_rate=1 ;
			}
			else counter_Monitor_rate++ ;
			if(counter_Monitor_rate>Monitor_rate) counter_Monitor_rate=1 ;

			if(counter_Event_Size==Monitor1 && Monitoring->Click2_return()==1)
			{
				Monitoring->DRAW_Event_Size() ;	//Event size distribution
				Monitoring->DRAW_Event_Core() ;	//Event core distribution
				Monitoring->DRAW_Time_Diff() ;	//Time difference between 2 events
				counter_Event_Size=1 ;
			}
			else counter_Event_Size++ ;
			if(counter_Event_Size>Monitor1) counter_Event_Size=1 ;

			if(counter_Event_Size==Monitor1 && Monitoring->Click1_return()==1)
				Monitoring->DRAW_HV() ;		//HV display
			
			/*Reset_Final_Data() ;
			lasa[temp_array[j].lasa].Master->Build_Final_Events(temp_array[j].lasa,1,temp_array[j].event_no) ;
			lasa[temp_array[j].lasa].Slave->Build_Final_Events(temp_array[j].lasa,0,temp_array[j].event_no) ;
			final_data[temp_array[j].lasa].Trigg_Status=1 ;*/
		}
		tmin=temp_array[j].time_stamp ;
		jmin=j ;
		Reset_Final_Data() ;
		lasa[temp_array[j].lasa].Master->Build_Final_Events(temp_array[j].lasa,1,temp_array[j].event_no) ;
		lasa[temp_array[j].lasa].Slave->Build_Final_Events(temp_array[j].lasa,0,temp_array[j].event_no) ;
		final_data[temp_array[j].lasa].Trigg_Status=1 ;
		}
	}
	//-----------Storing in buffer those events belonging to the last trigger---------
	for(int j=0;j<NO_LASAs;j++)
	{
		if(lasa_select[j]==0) continue ;
		last_trigger_events[j].time_stamp=temp_array[jmin].time_stamp ;
		last_trigger_events[j].lasa=temp_array[jmin].lasa ;
		last_trigger_events[j].event_no=temp_array[jmin].event_no ;
		printf("\n[%d] time=%llu\n\n",j,last_trigger_events[j].time_stamp) ;
		jmin++ ;
	}
	//-----------------------------------------
	}
}

void Sort_Event_Times(int pointer)	//Sorting event times in increasing order
{
	short unsigned temp_lasa,temp_event ;
	long long unsigned temp_time ;

	if(pointer<0) pointer=pointer+Max_Event ;
	for(int j=0;j<NO_LASAs;j++)
	{
		memmove(temp_array+j,last_trigger_events+j,sizeof(temp_array[j])) ;
		last_trigger_events[j].time_stamp=0 ;
		x[j]=0 ;
	}
	for(int j=NO_LASAs;j<Spy_Event+NO_LASAs;j++)
	{
		memmove(temp_array+j,array_event_times+pointer,sizeof(temp_array[j])) ;
		x[j]=pointer ;
		if(pointer==(Max_Event-1)) pointer=0 ;
		else pointer++ ;
	}	
	for(int j=0;j<Spy_Event+NO_LASAs;j++)
	{
		temp_time=temp_array[j].time_stamp ;
		temp_lasa=temp_array[j].lasa ;
		temp_event=temp_array[j].event_no ;
		for(int i=j+1;i<Spy_Event+NO_LASAs;i++)
		{
			if(temp_array[i].time_stamp<temp_time)
			{
				temp_array[j].time_stamp=temp_array[i].time_stamp ;
				temp_array[j].lasa=temp_array[i].lasa ;	
				temp_array[j].event_no=temp_array[i].event_no ;	

				temp_array[i].time_stamp=temp_time ;
				temp_array[i].lasa=temp_lasa ;
				temp_array[i].event_no=temp_event ;

				temp_time=temp_array[j].time_stamp ;
				temp_lasa=temp_array[j].lasa ;
				temp_event=temp_array[j].event_no ;
			}
		}
	}
}

#include <fcntl.h>
#include <TFile.h>
#include <TTree.h>
#include <stdio.h>
#include <strstream>
#include <iomanip.h>

int main(int argc, char** argv)
{
	//--------Writing *.root file-----------
	/*unsigned char count1[3000000],COUNT,COUNT1 ; 
	TFile *f=new TFile("test1.root","recreate") ;
	TTree *tree=new TTree("Tree","Title") ;
	tree->Branch("Count",count1,"count1[3000000]/C") ;
	
	int inhandle ;
	inhandle=open(argv[1],O_RDONLY) ;
	int bytes ;

	int outhandle1 ;
	outhandle1=open("test_root_out",O_CREAT|O_WRONLY,0444) ;

	int i=0 ;
	while(1)
	{
		bytes=read(inhandle,&COUNT,1) ;
		if(i<=100) printf("(%d) %x\t%d\n",i,COUNT,COUNT) ;
		count1[i]=COUNT ;
		if(bytes<=0) break ;
		i++ ;
	
		//tree->Fill() ;
		COUNT1=count1[i] ;
		write(outhandle1,&COUNT1,1) ;
	}
	close(outhandle1) ;
	printf("i=%d\n",i) ;
	tree->Fill() ;
	close(inhandle) ;
	tree->Write() ;
	tree->Print() ;
	f->Close() ;

	//--------Reading *.root file--------------
	int outhandle2 ;
	outhandle2=open(argv[2],O_CREAT|O_WRONLY,0444) ;

	TFile *f1=new TFile("test1.root") ;
	TTree *tree1=(TTree*)f1->Get("Tree") ;
	unsigned char count2[3000000],COUNT2 ;
	tree1->SetBranchAddress("Count",count2) ;
	int no_entries=(int)tree1->GetEntries() ;
	printf("no_entires=%d\n",no_entries) ;
	for(int i=0;i<no_entries;i++)
	{
		tree1->GetEntry(i) ;
		//for(int j=0;j<3000000;j++)
		//{
			//COUNT2=count2[j] ;
			write(outhandle2,count2,1) ;
			//if(j<=100) printf("(%d) %x\t%d\n",j,count2[j],count2[j]) ;
		//}
	}
	
	for(int j=0;j<=100;j++)
		printf("(%d) %x\t%d\n",j,count2[j],count2[j]) ;
	close(outhandle2) ;
	f1->Close() ;

	//----------------1 more test-----------
	unsigned char count_test[200] ;
	unsigned char count3[200] ;
	//std::ostrstream s(count3,1000,ios::ate) ;
	inhandle=open(argv[1],O_RDONLY) ;
	
	//bytes=read(inhandle,&count_test,100) ;
	//strcpy(count3,count_test) ;
	
	for(int i=1;i<=1;i++)
	{
		bytes=read(inhandle,count_test,200) ;
		//printf("(%d) %x\t%d",i-1,count_test[0],count_test[0]) ;
		memcpy(count3,count_test,200) ;
		//printf("\t %x\n",count3[1]) ;
		//s<<(char*)count_test ;
		//s<<ends ;
		//strcat(count3,count_test) ;
	}
	for(int i=0;i<=100;i++)
		printf("(%d) %x\t%d\n",i,count3[i],count3[i]) ;
		
	close(inhandle) ;*/

	struct DATA
	{
		unsigned int a ;
		short unsigned int b ;
		short unsigned int c1 ;
		short unsigned int c2 ;
		short unsigned int c3 ;
		short unsigned int c4 ;
		unsigned int d ;
		unsigned int e ;
		unsigned int f ;
	} data ;
	data.a=20 ;
	data.b=100 ;
	data.c1=10 ;
	data.c2=500 ;
	data.c3=1000 ;
	data.c3=2500 ;
	data.d=2234567890 ;
	data.e=3334567890 ;
	data.f=4134567890 ;
	TFile *f=new TFile("test2.root","recreate") ;
	TTree *tree=new TTree("Tree","Title") ;
	tree->Branch("test",&data.a,"a/i:b/s:c1:c2:c3:c4:d/i:e:f") ;
	tree->Fill() ;
	tree->Write() ;
	f->Close() ;
}

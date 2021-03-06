// 16-02-09: Programa para implementar Global consensus en networks
//18-2-09: añadida rutina para calcular la GC de links en consenso (Smax).
//20-2-09: añadida rutina para calcular la Sf_max  (componente gigante nodos que comparten el valor del rasgo f) ojoo! ahora el convenio es distinto (para la rutina S_max)
//25-2-09: uso de una matriz D auxiliar, de links no bloqueados, para acelerar
//   el proceso de eleccion aleatoria de links cerca del final de la simu.

//10-3-09: crear archivos con todos los datos, sin hacer medias (estas iran en un programilla de análisis)
//19-3-09: implemento la restriccion del uso del mecanismo de D_no_blocked solo a partir de que el 80 por ciento de los links esten ya bloqueados
//4-5-09: estudio la dependencia de <S> con q y con f (eliminado av_path length)
//4-5-09: añado bucle iteraciones (no las medias)
//4-5-09: cambio la forma de acabar una simu-->> qdo el número de links bloqueados sea igual al máximo posible
//11-5-09: estudio de la dinámica en redes ER en lugar de SF. OJO!!!!!! ahora no calculo la matriz y despues la D, sino que directamente al construir la red, obtengo C[][] y D[][]
// OJO!!! ahora el max num de links no es fijo!!!  -->> lo calculo al construir la matriz D y ya
// ademas, lo uso como limite en los bucles en los que aparece D[] y prob_D[]

//20-5-09: estudio de los nodos frontera en el estado final
//21-5-09: estudio del tamaño de los clusters al final de cada iter
//25-5-09: hitograma de tamaños de clusters

//1-5-09: me cai de la parra: va muchisimo mas rápido si solo calculo n_links_blocked y S_max cada multiplo veces!!!!!!!!

//11-6-09: basado en el programa  consenso_dependenciaQ_iter_ER_frontera_cl_size_mio_fast.c   ahora calculo tb el <l> de la GC de consenso total y por capas
//12-6-09:  calculo del clustering coef de la GC de consenso tot y por capas
//16-6-09 :  restriccion del cálculo del CC sólo qdo GC>2  (else, CC=0)
//29-6-09: elimino la rutina del <l> y la del CC rasgos, dejando solo la del CC tot, para acelerar la simu y poder hacer mas estadistica

//3-7-09: calculo la pk de la GC de consenso y por capas
//9-7-09  en lugar de calcular esta pk a intervalos de tiempo, lo hago para 
//        ciertos tamaños de la Smax (tot y por layers)

//29-10-09: dinamica Axelrod sobre redes Reales



# include <stdio.h>
# include <stdlib.h>
# include <math.h>


# define Niter  12




# define N  1133     // email.dat

//# define N 12722      // SNC.dat
//# define N 198      // jazz.dat
//# define N 13259      // ASTRO.dat   //OJO PQ ESTA EMPIEZA A NUMERAR LOS NODOS EN CERO (por lo que hay uno mas de lo que parece)
                                      //  y ademas tiene tres columnas!!! las de los pares de vecinos son la primera y la segunda




//numero de filas del archivo de entrada == numero total links de la red:



# define filas 5451   // email.dat       //tambien es el numero de links del sistema!!!!
//# define filas 39967   // SNC.dat
//# define filas 2742   // jazz.dat
//# define filas 123838   // ASTRO.dat











# define Q  30   // numero de valores distintos que puede tomar un rasgo concreto (es igual para todos los rasgos)
# define f 10       // numero de rasgos de un nodo

# define multiplo 10000      //cada cuantos pasos de tiempo calculo maginitudes y escribo archivos

# define N_size 5  //numero de tamaños (de Smax) para los que miro la pk_GC




///////////////////   Generacion de numeros aleatorios

#define FNORM (2.3283063671E-10F)
#define RANDOM ((ira[ip++]=ira[ip1++]+ira[ip2++])^ira[ip3++])
//numero aleatorio flotante en el intervalo (0,1]
#define FRANDOM (FNORM*RANDOM)
unsigned char ip,ip1,ip2,ip3;
unsigned ira[256];

//////////////////////////////////////////



char file[256],file3[256];
char file9[256],file12[256],file13[256],file14[256],file15[256],nombre1[256];

 
int k[N+1],k_GC[N+1];                    //conectividad topológica y de Preferential Attachment (PA)


double PK[N+1], PK_tot[N+1];        //  para la distribucion P(k)
double r,v;                                  //para guardar los aleatorios
int  i, j, jj,  w,g, d, q, x[N+1],y,z, C[N+1][N+1],C_GC[N+1][N+1],n, ValorA, steps,s;
double dnorma_aleat;
int si[N+1], cont,cont2,iter;


            


/////// variables para el consenso

int v_i[N+1][f+1]; //matriz que guarda los valores de los f rasgos de los N nodos
double S_ij;
int D[filas+1][3];	//matriz de pares de links 




int n_links_blocked,n_links_S1,tpo,S_max,Sf_max[f+1];

int NN[N+1],Phase[N+1][N+1];
int rasgo,Simil[filas+1],n_links, contador,ii;
double GComp;    //guarda la GC de consenso (total o de un rasgo)
double Sf_max_media,aux;
int chosen,cambio, imitador=0;  //guarda las prob de cada lin
int Nclusters,NCL[f+1],NCLUSTERS;   // numero de clusters de consenso, de cada rasgo,su media y aux para la rutina
double dist_tot,D_topol, av_p_l_rasgo[f+1],av_p_l,aux_links,aux_N;
int flag=0,N_links_max=0,sizes[N+1];
double Pk_frontera_1_tot[N+1],Pk_frontera_2_tot[N+1],histogr_sizes_tot[N+1];
int analyse_GC[N+1]; //guarda los nodos que forman parte de la GC 
double cl_coef, clustering,clustering_rasgos[f+1],clustering_topol;
double PK_GC[N+1][f+1][N_size+1]; //la posicion rasgo=0 es para el consenso tot
double pk_topol_GC[N+1][f+1][N_size+2];

int  n_size_tot,n_size_rasgo[f+1], size_pk_tot,size_pk_rasgo[f+1],aux_entero;





void inicia_rand(int semilla);

void leer_red();   // y calculo la pk
void leer_red_ASTRO();
void construir_C_k();
void histograma_pk();

int time();

void contar_blocked_links();
void contar_links_S1();
void marcar_link_consenso();
void marcar_link_consenso_rasgo();
void find_Smax();
void consenso_link();
void ver_nodos_frontera();
void average_path_length_topol();  
void average_path_length_consenso (); //calcula <l> de la GC de consenso total
void crear_C_y_k_GC() ;
void clustering_coef_topol();
void clustering_coef_consenso();
void histograma_PK_GC(int);


FILE *fich, *fich3;
FILE *fich9,*fich12,*fich13,*fich14,*fich15,*archivo1;





int main()
{
    
    

//Guarda la Pk
      sprintf(file3,"Pk_N%dreal_Q%d_f%d_%diter.dat",N,Q,f,Niter);
    fich3=fopen(file3,"wt");
    fclose(fich3);
    
  



    
    inicia_rand(time(0));
    
    
    printf("\n\n\nPrograma Global consensus on Real Networks  N=%d  Q=%d  F=%d (%d iter)\n\n",N,Q,f,Niter);  
    


    
    for(i = 0; i <= N ; i++)
    {
	PK_tot[i] = 0.0;
	//	Pk_frontera_1_tot[i]=0.0;
	//	Pk_frontera_2_tot[i]=0.0;
	histogr_sizes_tot[i]=0;
    }
    
    
 


    
/////////archivo de entrada



     sprintf(nombre1,"email_1133nodoskmed4.81.txt");
//       sprintf(nombre1,"SCN_12722nodos_kmed3.14.txt");
//     sprintf(nombre1,"jazz_198nodos_kmed13.85.txt");
//    sprintf(nombre1,"ASTRO_13259nodos_kmed9.34.txt");
    
    
    
    
    
    
     
       leer_red();          //solo leo la red real una vez, pq la estadistica sera solo sobre c.i.
       
//     leer_red_ASTRO();      //es distinta funcion pq este archivo tiene 3 columnas, y los demas solo dos
       
       
      
       
       construir_C_k();
      
       histograma_pk();
       
       
      
	D_topol=0.;
	clustering_topol=0.;
	


	N_links_max=filas;  


/*	average_path_length_topol();
	D_topol=dist_tot;


	clustering_coef_topol();
	clustering_topol=cl_coef; 


	printf("<l> y CC topologicos calculados\n");  */




    for(iter=1;iter<=Niter;iter++)
    {
	printf("\n\niter%d\n",iter);  
	
      size_pk_tot=100; //tamaño Smax_tot para el que escribo la pk_GC     
      n_size_tot=1;	  //indice de los tamaños (escribo en 4 ocasiones)
      
      
      for(i=1;i<=f;i++)
	{
	  size_pk_rasgo[i]=100;//tamaño Smax[rasgo] para el que escribo la pk_GC
	  n_size_rasgo[i]=1; //indice de los tamaños (escribo en 4 ocasiones)
	  
	}
      







	S_max=0;
	flag=0;
	


	
//Guarda S_max, <Sf_max> y Sf_max[rasgo] en cada paso de tiempo
	sprintf(file,"Consenso_N%dreal_Q%d_f%d_iter%d.dat",N,Q,f,iter);
	fich=fopen(file,"wt");
	fclose(fich);




	for(i=1;i<=N;i++)       //////////////inicializo los valores de los rasgos de los nodos	
	{
	    for(j=1;j<=f;j++)
	    { 
		v=FRANDOM;
		v_i[i][j]=v*Q;    //da valores entre 0 y Q-1 a cada rasgo	   	    
	    }	
	}
	

	
	
	tpo=0;
	
	contador=0;
			
	while(1)    //bucle del tiempo de la simulacion
	{				
	    

	    
	    consenso_link();    //en esta rutina elijo un link y con prob S_ij, n1 imita a n2		
	    
	    
	    
	    
	    
	    if(((tpo % multiplo)==0.0) || (flag==1))   //calculo y escribo en los ficheros solo cada multiplo veces y tb la última vez cuando ya hay consenso global
	    {				
		
		
		contar_blocked_links();	


		if(n_links_blocked==N_links_max)
		  { 
		    flag=1;         //escribir los archivos la ultima vez	    	   	    	    	
		  }
		

		
		
		contar_links_S1();
		
		marcar_link_consenso(); // ahora marco los nodos de un link como 1 si están en de acuerdo 
		


		rasgo=0;  //para indicarle a la siguiente rutina que estoy calculando la GC de consenso global
			       // (pq qdo valga 1,...,f, estará calculando la GC por capas)

		find_Smax();              // y como  0 si no. tambien obtengo NN[] y Phase[][]
		
		
		S_max=GComp;
		Nclusters=NCLUSTERS;
		

		
		if( (S_max >= size_pk_tot ) || (flag==1) )   //cuando alcanzo los tamaños intermedios indicados
		  {
		    
		    // printf("calculando pk tot para size%d\n",size_pk_tot);
		    histograma_PK_GC(n_size_tot);//le paso la posicion de pk_GC[][][ n_size_tot]	
		  // en la que escribira	
		    
		    
		    
		    //Guarda la PK de la GC consenso tot
		    sprintf(file12,"Pk_GC_tot_N%dreal_Q%d_f%d_ER_iter%d_%dsize.dat",N,Q,f,iter,size_pk_tot);
		    fich12=fopen(file12,"wt");
		    fclose(fich12);
		    
		    
		    
		    
		    fich12=fopen(file12,"at");
		    for(i=1;i<=GComp;i++)
		      {
			fprintf(fich12,"%d   %f\n",i,PK_GC[i][0][n_size_tot]);	
		      }
		    
		    //  fprintf(fich12,"\n\n");//separo con dos lineas pks de distintos instantes de tiempo
		    fclose(fich12);





		    //Guarda la PK topologica de la GC consenso tot
		    sprintf(file14,"Pk_topol_GC_tot_N%dreal_Q%d_f%d_ER_iter%d_%dsize.dat",N,Q,f,iter,size_pk_tot);
		    fich14=fopen(file14,"wt");
		    fclose(fich14);		  		  
		    
		    
		    fich14=fopen(file14,"at");
		    for(i=1;i<=GComp;i++)
		      {
			fprintf(fich14,"%d   %f\n",i,pk_topol_GC[i][0][n_size_tot]);	
		      }
		    
		    fclose(fich14);
		    
		    
		    size_pk_tot=size_pk_tot*2;
		    n_size_tot++;
    	    	  
		  
		  }
		




		
		for (rasgo=1;rasgo<=f;rasgo++)     //calculo las f Sf_max
		{
		    marcar_link_consenso_rasgo(); // ahora marco un link  como 1,2,...f   si están 
		    find_Smax();                       // de acuerdo en ese rasgo concreto, y 0 si no 
		   	
		    
		    Sf_max[rasgo]=GComp;
		    NCL[rasgo]=NCLUSTERS;


		    if(( Sf_max[rasgo] >= size_pk_rasgo[rasgo]) || (flag==1))   
		      {
			//	printf("calculando pk para rasgo%d y size%d\n",rasgo,size_pk_rasgo[rasgo]);
			aux_entero=n_size_rasgo[rasgo];
			histograma_PK_GC(aux_entero);

			n_size_rasgo[rasgo]++;//pq cada rasgo puede crecer a distinto ritmo 
			size_pk_rasgo[rasgo]=size_pk_rasgo[rasgo]*2;


		      }
		    
		    
		}									
		

	
		
		
		//	printf("S_max(t=%d)=%d  n_links_bl=%d  n_links_S1=%d  N_cl=%d  <l>cons:%f  CC:%f\n\n",contador,S_max,n_links_blocked,n_links_S1,NCLUSTERS,av_p_l,clustering);	
		
		


		aux_links=N_links_max;
		aux_N=N;
		fich=fopen(file,"at");
		fprintf(fich,"%d   %f   %f   %f",contador,n_links_blocked/aux_links,n_links_S1/aux_links,S_max/aux_N);	
		for(i=1;i<=f;i++)       
		{
		    fprintf(fich,"   %f",Sf_max[i]/aux_N);	
		}
		fprintf(fich,"\n");	
		fclose(fich);	  
		
	



		contador++;			
	    }
	    
	    
	  


 
	    if( flag==1)
	    { 	    	
	      //	ver_nodos_frontera();
		
		break;       // acabar la simulacion 
	    }	    	    	    
	    
	    

	    
	    tpo++; //pasos de tiempo reales de la simulacion

	}     ////////////////////////////////////////fin de la simulacion    
	
	
	marcar_link_consenso(); // ahora marco los nodos de un link como 1 si están en de acuerdo                                    
	find_Smax();   




 
      //escribo la pk_GC por capas par los 4 instantes del crecimiento
      
      n_size_tot=100;
      for(ii=1;ii<=N_size;ii++)
	{		    
	  
	  
	  //Guarda la PK de la GC por capas
	  sprintf(file13,"Pk_GC_rasgos_N%dreal_Q%d_f%d_ER_iter%d_%dsize.dat",N,Q,f,iter,n_size_tot);
	  fich13=fopen(file13,"wt");
	  fclose(fich13);
	  
	  
	  
	  fich13=fopen(file13,"at");
	  for(i=1;i<=N;i++)
	    {				 		
	      fprintf(fich13,"%d",i);		
	      for(rasgo=1;rasgo<=f;rasgo++)
		{
		  fprintf(fich13,"    %f",PK_GC[i][rasgo][ii]);	
		}
	      
	      fprintf(fich13,"\n");
	    }
	  fclose(fich13);
	  
	  







	  //Guarda la PK topol de la GC por capas
	  sprintf(file15,"Pk_topol_GC_rasgos_N%dreal_Q%d_f%d_ER_iter%d_%dsize.dat",N,Q,f,iter,n_size_tot);
	  fich15=fopen(file15,"wt");
	  fclose(fich15);
	  	  
	  
	  fich15=fopen(file15,"at");
	  for(i=1;i<=N;i++)
	    {				 		
	      fprintf(fich15,"%d",i);		
	      for(rasgo=1;rasgo<=f;rasgo++)
		{
		  fprintf(fich15,"    %f",pk_topol_GC[i][rasgo][ii]);	
		}
	      
	      fprintf(fich15,"\n");
	    }
	  fclose(fich15);





	  n_size_tot=n_size_tot*2;
	 
	  
	}	









			


    }   /////////////////////////////////fin bucle iteraciones

 

/* Normalizo y escribo las PK y Pk de nodos frontera */

    for( i = 1; i <= N; i++)
    {
	PK_tot[i] = PK_tot[i]/Niter;

	//	Pk_frontera_1_tot[i]=Pk_frontera_1_tot[i]/Niter;
	//Pk_frontera_2_tot[i]=Pk_frontera_2_tot[i]/Niter;
    }
    


     fich3=fopen(file3,"wt");   
    for(i=1;i<=N;i++)       
    {
	fprintf(fich3,"%d   %f\n",i,PK_tot[i]);	
    }  
    fclose(fich3);	 
    






    
    
}



/////////////////////////////
////////////////////////////
//////////////////////


void inicia_rand(int semilla)
{

int i;
int dummy;

srand((unsigned)semilla);
for(i=0;i<111;i++)
  rand();
ip=128;
ip1=ip-24;
ip2=ip-55;
ip3=ip-61;
for(i=0;i<256;i++)
  ira[i]=(unsigned)rand()+(unsigned)rand();
for(i=0;i<1111;i++)
  dummy=RANDOM;
}

///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
/////////////////////////////////////////////////





void leer_red()
{
    
    int i,c1,c2;
    
    


    for(i=1;i<=filas;i++)
    {
	D[i][1]=0;
	D[i][2]=0;
    }
   

    archivo1=fopen(nombre1,"r");  
    for(i=1;i<=filas;i++)
    {
	fscanf(archivo1,"%d %d\n",&c1,&c2);

	D[i][1]=c1;
	D[i][2]=c2;
    }
    fclose(archivo1);


    /*  for(i=1;i<=filas;i++)   //comprobacion
    {
	printf("%d-%d\n",D[i][1],D[i][2]);
    }
    getchar();*/

}



//////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////




void histograma_pk()
{

    int i;
    double aux;
               
   
//calculo la P(k)
    
    for(i=0; i<N; i++)
	PK[i] = 0;
    
    
    for(j=1; j<=N; j++)
        PK[k[j]]++;
    
    
    
    
    /* Normalizo PK */
    aux=N;
    for(i=1;i<=N;i++)
    {
	PK[i] = PK[i]/aux;
	PK_tot[i]+=PK[i];
    }         

    
    //printf("red y pk ok\n");   




}  


/////////////////////////////
//////////////////////////////////
//////////////////////////////////////







void leer_red_ASTRO()    //el fichero ASTRO tiene tres columnas, las dos primeras son las que me interesan
{                        // ademas, empieza a numerar los nodos en 0, no en 1 !!!!!!

    int i,c1,c2,c0;
 
    


    for(i=1;i<=filas;i++)
    {
	D[i][1]=0;
	D[i][1]=0;
	D[i][2]=0;
    }
   

    archivo1=fopen(nombre1,"r");  
    for(i=1;i<=filas;i++)
    {
	fscanf(archivo1,"%d %d %d\n",&c1,&c2,&c0);

	D[i][0]=c0;    //la tercera columna de datos no me interesa
	D[i][1]=c1+1;
	D[i][2]=c2+1;   
    }
    fclose(archivo1);


    /*  for(i=1;i<=filas;i++)   //comprobacion
    {
	printf("%d-%d\n",D[i][1],D[i][2]);
    }
    getchar();*/
}







//////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////




void contar_blocked_links()
{
    
    int i,j,n1=0,n2=0;
   
    
    n_links_blocked=0;
    
    
    for(i=1;i<=N_links_max;i++)    
    {
	n1=D[i][1];         //OJO, ESTO VALE SI m=2, si no, hay que modificarlo!!
	n2=D[i][2];


	S_ij=0.0;
	for(j=1;j<=f;j++)  
	{
	    if(v_i[n1][j]==v_i[n2][j])
	    {
		S_ij++;	   
	    }
	}	

	if(S_ij==0.0 || S_ij==f)   
	{
	    n_links_blocked++;
	}
    }
    //printf("n_links_bloqued%d\n",n_links_blocked); 

}



void contar_links_S1()
{
    
    int i,j,n1=0,n2=0;
   
    
    n_links_S1=0;
    
    
    for(i=1;i<=N_links_max;i++)    
    {
	n1=D[i][1];         //OJO, ESTO VALE SI m=2, si no, hay que modificarlo!!
	n2=D[i][2];


	S_ij=0.0;
	for(j=1;j<=f;j++)  
	{
	    if(v_i[n1][j]==v_i[n2][j])
	    {
		S_ij++;	   
	    }
	}
	   
	
	if(S_ij==f)   
	{
	    n_links_S1++;
	}
    }
    
}




////////////////////////////////
///////////////////////////////
////////////////////////////////




void find_Smax()    //basado en la rutina TopologiaCP (Mi convenio era: -1 fluct, 0 coop, +1 defect) 
{                         //ahora =i es un link de consenso en el rasgo i, y =0 es no consenso
    
    
    int J, I,h,ii,kk,i,todos_nodos[N+1],aislados,analyse[N+1],visitado[N+1],size[N+1];//,gg;
    double histogr_sizes[N+1],aux;
    
    
    //nota: matrices NN[] y Phase inicializadas y obtenidas en la rutina:  marcar_links_consenso() y  marcar_links_consenso_rasgo()
    
    
    for(I=0;I<=N;I++)
    {
	visitado[I]=0;
	size[I]=0;
	histogr_sizes[I]=0;
	todos_nodos[I]=0;
    }
    
    ii=0;
    aislados=0;
    NCLUSTERS=0;  
    GComp=0;
    
    for(I=1;I<=N;I++)     //recorro la red
    {
	kk=0;
	for(J=1;J<=N;J++)
	{
	    analyse[J]=0;      //guardara los nodos que forman parte del cluster actual
	}
	
	if(NN[I]>0) //si no es un nodo asilado
	{
	    kk=1;
	    if(visitado[I]==0)   //si el nodo no ha sido visitado antes
	    {
		visitado[I]=1;   //lo marco como visitado
		analyse[kk]=I;   //lo marco como perteneciente al cluster actual
		todos_nodos[I]=I;
		
		
		for(jj=1;jj<=kk;jj++)
		{
		    g=analyse[jj];    //el nodo en el que estoy
		    
		    for (i=1;i<=NN[g];i++)    //miro sus vecinos
		    {
			h=Phase[g][i];
			
			if(visitado[h]==0)
			{
			    visitado[h]=1;
			    kk++;
			    analyse[kk]=h;
			    todos_nodos[h]=h;
			}
		    }
		}
	    }
	    
	}	
	else    //si el nodo esta aislado
	{
	    visitado[I]=1; 
	    aislados++;  
	    histogr_sizes[1]++;   
	    
	   
	}	
	
    	///////////////////////////////////////////cuando acabo con el cluster actual:
	
	
	if(kk>GComp)      //guardo el mayor de los clusters como GComp del sistema y tb los nodos que lo forman
	{
	    GComp=kk;
	    
	    
	    for(J=1;J<=N;J++)
	    {		
		analyse_GC[J]=0;   //guardara los nodos que forman parte de la GComp
	    }	    	    
	    
             //  printf("\n(GC:%f)  ",GComp); 
	    
	    for(J=1;J<=N;J++)
	    {
		if(analyse[J]!=0)
		{
		    analyse_GC[J]=analyse[J];
		    // printf("%d   ",analyse_GC[J]); 
		}
	    }
	    
	    crear_C_y_k_GC();
	    
	    
	    
	}
	
	
	if(kk>1)
	{
	    NCLUSTERS++;  
	}
	
	
	if(flag==1)    //solo al acabar la simu
	{	
	    if(kk>1)
	    {
		/*	printf("\ntamaño:%d       ",kk);
		for(J=1;J<=kk;J++)
		{
		    printf("%d   ",analyse[J]);
		}
		printf("\n");*/
		
		
		ii++;
		size[ii]=kk;     //guardo el tamaño del cluster que he encontrado
		histogr_sizes[kk]++;   //acumulo para el histograma de tamaños
		
		
		
		
		/*	for(jj=1;jj<=kk;jj++)   
		{			
		    fich6=fopen(file6,"at");
		    fprintf(fich6,"nodo:%d (k:%d)          ",analyse[jj],k[analyse[jj]]);
		    
		    for(gg=1;gg<=f;gg++)       
		    {
			fprintf(fich6,"   %d",v_i[analyse[jj]][gg]);	
		    }
		    fprintf(fich6,"\n");	
		    fclose(fich6);	 		    		   		    		    
		}
	
		fich6=fopen(file6,"at");
		fprintf(fich6,"\n\n");	
		fclose(fich6);*/		
	    }	
	}    
		
    }   /////////////////////////////////fin del bucle a todos los nodos de la red
    
    
    
    if(rasgo==0) //si estoy calculando la GC de consenso global
      {	
	// printf("consenso tot:");
	//average_path_length_consenso();
	//av_p_l=dist_tot;
	
	
	/*	if(GComp>10)   //solo se def CC si GC>2
		{
		clustering_coef_consenso();
		clustering=cl_coef;
		}
		else
		clustering=0.;*/
	
	
      }
    else    //si estoy calculando la GC por capas
      {	
	//	printf("rasgos:");
	//average_path_length_consenso();
	//	av_p_l_rasgo[rasgo]=dist_tot;
	
	if(GComp>2)   //solo se def CC si GC>2
	  {
	    clustering_coef_consenso();
	    clustering_rasgos[rasgo]=cl_coef;
	  }
	else
	  clustering_rasgos[rasgo]=0.;
	
	
      }
    
    
    



    
    if(flag==1)      //solo cuando la iter ya ha acabado		
    {
	aux=aislados+NCLUSTERS;    	
	for(I=1;I<=N;I++)
	{	   	    
	    histogr_sizes[I]=histogr_sizes[I]/aux;	    	   
	    histogr_sizes_tot[I]= histogr_sizes_tot[I] + histogr_sizes[I];	    

//	    printf("%d    %f   (%f)\n",I,histogr_sizes[I],aux);
	    
	}
	
	
/*	for(jj=1;jj<=N;jj++)        //guardo tb los rasgos de los nodos aislados
	{
	    if(todos_nodos[jj]==0)
	    {			
		fich6=fopen(file6,"at");
		fprintf(fich6,"nodo:%d (k:%d)          ",jj,k[jj]);
		
		for(gg=1;gg<=f;gg++)       
		{
		    fprintf(fich6,"   %d",v_i[jj][gg]);	
		}
		fprintf(fich6,"\n\n");	
		fclose(fich6);	 
		
	    }
	}
	printf("GComp:%f    NCLUSTERS:%d    aislados:%d",GComp,NCLUSTERS,aislados);*/
    }
    



        
}



//////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////



void marcar_link_consenso ()
{
    int i,j,n1=0,n2=0;  
    
    
    for(i=1;i<=N_links_max;i++)   //inicializo el vector que guardará la similitud de un link (en total, no por rasgos)
    {
	Simil[i]=0;
    }
    
    
    for(j=1;j<=N;j++)    
    {
	NN[j]=0;              //NN[] es k[] pero solo enlaces de consenso
	for(i=1;i<=N;i++)
	{
	    Phase[j][i]=0;  //Phase[][] es C[][] pero solo entran links de consenso
	}
    }


    for(j=1;j<=N_links_max;j++)  //recorro la matriz de links
    {
	n1=D[j][1];    
	n2=D[j][2];
	
	
	Simil[j]=0;    //vector que guarda la similitud en un link (el tamagno de vector es m*N)
	
	for(i=1;i<=f;i++)  //calculo S_ij
	{	       
	    if(v_i[n1][i]==v_i[n2][i])
	    {
		Simil[j]++;
	    }
	}
	

	if(Simil[j]==f)   //construyo las matrices para el subgrafo de consenso    
	{	  
	    NN[n1]++;
	    NN[n2]++;

	    Phase[n1][NN[n1]]=n2;
	    Phase[n2][NN[n2]]=n1;
	    	   
	} 	
	


    }
       

}



///////////////////////////////////////
////////////////////////////////////////
///////////////////////////////


void marcar_link_consenso_rasgo ()
{
    int i,j,n1=0,n2=0;  
    
    
    for(i=1;i<=N_links_max;i++)   //inicializo el vector que guardará la similitud de un link respecto al rasgo que este mirando
    {	
	Simil[i]=0;	      
    }
    
    
    for(j=1;j<=N;j++)   
    {
	NN[j]=0;                //NN[] es k[] pero solo enlaces de consenso
	for(i=1;i<=N;i++)
	{
	    Phase[j][i]=0;  //Phase[][] es C[][] pero solo entran nodos que comparten links de consenso
	}
    }        



//estamos mirando un rasgo concreto (rasgo =1,2,...f)

	for(j=1;j<=N_links_max;j++)  //recorro la matriz de links
	{
	    
	    n1=D[j][1];    //OJO!!!  VALIDO SOLO SI m=2, si no, hay que añadir lineas
	    n2=D[j][2];
	    
	    if(v_i[n1][rasgo]==v_i[n2][rasgo])    //si hay consenso en el link sobre el rasgo
	    {
		Simil[j]++;	      
	    }


	    if(Simil[j]==1)
	    {
		NN[n1]++;
		NN[n2]++;

		Phase[n1][NN[n1]]=n2;
		Phase[n2][NN[n2]]=n1;
	    }
	}
 
    

}

/////////////////////////////////////////////////////
//////////////////////////////////////////////////
/////////////////////////////////////////





void consenso_link()   // rutina usada en el programa consenso_Sf_max.c
{
    
    double aux_w=0.0, aux_r=0.0, aux=0.0;
    int i,w,r,bandera;
    int nodo1=0,nodo2=0,n1=0,n2=0;
    
    
    
    aux_w=FRANDOM;     //elijo un link y guardo los nodos que une
    aux_w=aux_w*N_links_max;
    w=aux_w+1;   //por que genero numeros de 0 a  N*m -1
    
    nodo1=D[w][1];       
    nodo2=D[w][2];
    
    //printf("nodos: %d y %d\n",nodo1,nodo2);
    
    aux_r=FRANDOM;
    if(aux_r<0.5) //elijo al azar cuál imitará a cuál
    {
	n1=nodo1;
	n2=nodo2;
    }	
    else
    {
	n1=nodo2;
	n2=nodo1;	
    }
    
    S_ij=0.0;
    for(i=1;i<=f;i++)  //calculo la probabilidad de cambio (S_ij)
    {
	if(v_i[n1][i]==v_i[n2][i])
	{
	    S_ij++;	   
	}
    }
    aux=f;
    S_ij=S_ij/aux;   
    
    
    
    aux_w=FRANDOM; 
    if(aux_w<S_ij)    //prob de que n1 imite a n2 en un rasgo que no tengan igual
    {	
	bandera=0;
	
	while(bandera==0)
	{	    
	    if(S_ij!=0.0 && S_ij!=1.0 )
	    {    
		aux_r=FRANDOM;
		aux_r=aux_r*f; 
		r=aux_r+1;       //elijo un rasgo  (ojo!!!! la matriz se llena de 1 a f y el generador va de 0 a f-1)
		
		
		if(v_i[n1][r] != v_i[n2][r])
		{		   
		    v_i[n1][r]=v_i[n2][r];  //n1 imita a n2		  
		    bandera=1;	
		}
		
	    }
	    else              //  PARA QUE NO SE QUEDE ATASCADO EN UN BUCLE QUE ESTE BLOQUEADO
	    {			    		
		bandera=1;		    		    		
	    }
	}
    }
    
    
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////



void ver_nodos_frontera()
{

    int i,j,ii,n1,n2,contador_S0,N1,N2;
    double Pk_frontera_1[N+1],Pk_frontera_2[N+1],aux;
    
    for(i=1;i<=N;i++)
    {
	Pk_frontera_1[i]=0;    //pk de nodos frontera con al menos un link de S=0
	Pk_frontera_2[i]=0;     // id. al menos 2
    }

    N1=N2=0;
    
    for(i=1;i<=N;i++)    //recorro toda la red
    {
	contador_S0=0;   //recuento del numero de links con S=0 del nodo

	n1=i;
	for(j=1;j<=k[i];j++)       //recorro todos los vecinos del nodo
	{
	    n2=C[i][j];
	    
	    S_ij=0.0;
	    for(ii=1;ii<=f;ii++)  //calculo la probabilidad de cambio (S_ij)
	    {
		if(v_i[n1][ii]==v_i[n2][ii])
		{
		    S_ij++;	   
		}
	    }
	    

	    if(S_ij==0)
	    {
		contador_S0++;
	    }
	}

	if(contador_S0==1) //tipo 1
	{
	    Pk_frontera_1[k[i]]++;
	    N1++;              //la norma de los nodos frontera tipo 1 (=num tot nodos tipo 1)
	}
	
	if(contador_S0==2)  //tipo 2
	{
	    Pk_frontera_2[k[i]]++;
	    N2++;         //la norma de los nodos frontera tipo 2 (=num tot nodos tipo 2)
	}

   } //fin bucle nodos de la red


    
    /* aux1=N1;             //NO ESTOY SEGURA DE LA NORMALIZACIÓN!!!!!!
    aux2=N2;
    for(i=1;i<=N;i++)
    {
	
	if(N1!=0)
	{
	    Pk_frontera_1[i]= Pk_frontera_1[i]/aux1;
	    Pk_frontera_1_tot[i]+=Pk_frontera_1[i];
	}
	
	
	if(N2!=0)
	{
	    Pk_frontera_2[i]= Pk_frontera_2[i]/aux2;	    
	    Pk_frontera_2_tot[i]+=Pk_frontera_2[i];
	}
	

	}     */



    aux=N;
    for(i=1;i<=N;i++)
    {		
	Pk_frontera_1[i]= Pk_frontera_1[i]/aux;
	Pk_frontera_1_tot[i]+=Pk_frontera_1[i];		
	
	Pk_frontera_2[i]= Pk_frontera_2[i]/aux;	    
	Pk_frontera_2_tot[i]+=Pk_frontera_2[i];		
	
    }     
    


    
    

}




/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////





void average_path_length_topol ()
{
    int i,j,h,dist, di[N+1],actual[N+1],cont;
    double aux;
   
    dist_tot=0.0;


    for(i=1;i<=N;i++)  //bucle a todos los nodos de la red
    {

	for(j=1;j<=N;j++)
	{
	    di[j]=-100;
	}


	cont=1;
	actual[cont]=i; //lista de nodos visitados (empiezo por el nodo en el que estoy)
	di[i]=0;  //distancia a mi mismo es cero

	for(j=1;j<=cont;j++)   //lista de nodos que voy mirando (limite superior dinamico): actual[j] es el nodo que miro
	{
	    for(h=1;h<=k[actual[j]];h++)  //miro todos los vecinos de actual[j]
	    {
		if(di[C[actual[j]][h]]==-100)  //si no he visitado el nodo
		{
		    cont++;
		    actual[cont]=C[actual[j]][h];   //lo añado a la lista
		    di[actual[cont]]=di[actual[j]]+1;		   
		}
	    }
	}

	dist=0;
	for(j=1;j<=N;j++)//recuento de distancias del nodo i a todos lo demas
	{
	    if(di[j]>0)
	    {
		dist=dist+di[j];		
	    }
	}

	dist_tot=dist_tot+dist;    //acumulo


    }//fin del bucle a todos los nodos de la red
    

    aux=N-1.;   
    aux=aux*N;
    dist_tot=dist_tot/aux;         //calculo av_path_l
    
    //printf("\ndist_tot_top:%lf\n",dist_tot);
   
}






/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////





void average_path_length_consenso()  //igual que la rutina average_path_length_topol()   pero recorro analyse_GC[] en lugar de todo N
{                                    // y uso C_GC[][] y k_GC[] en lugar de C[][] y k[]
    int i,ii,j,h,dist, di[N+1],actual[N+1],cont;
    double aux;
   
    dist_tot=0.0;




    for(ii=1;ii<=GComp;ii++)  //bucle a todos los nodos de la GComp
    {
	i=analyse_GC[ii];  //indice del nodo que estoy mirando 
	//printf("\n%d",i);

	for(j=1;j<=N;j++)
	{
	    di[j]=-100;
	}

	cont=1;
	actual[cont]=i;
	di[i]=0;
	for(j=1;j<=cont;j++) //lim superior dinamico
	{	   
	    for(h=1;h<=k_GC[actual[j]];h++)  //miro los vecinos del nodo actual
	    {	
		if(di[C_GC[actual[j]][h]]==-100)
		{
		    cont++;
		    actual[cont]=C_GC[actual[j]][h];
		    di[C_GC[actual[j]][h]]=di[actual[j]]+1;
		}				
	    }	    	    
	}


	dist=0;
	for(j=1;j<=N;j++)
	{
	    //printf("\ndi[%d]:%d  (GC:%f)\n",j,di[j],GComp);
	    if(di[j]>0)
	    {
		dist=dist+di[j];
	    }
	}	    
	//printf("\ndist:%d  (GC:%f)\n",dist,GComp);
	dist_tot=dist_tot+dist;


    }//fin del bucle a todos los nodos de la red
    
    
 
    aux=GComp-1.;   
    aux=aux*GComp;
    dist_tot=dist_tot/aux;         //calculo av_path_l
    
    //  printf("\ndist_tot_consenso:%lf  (GC:%f)\n",dist_tot,GComp);
}







/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////








void crear_C_y_k_GC()    //es como la Phase[][] y la NN[] pero sólo de la GC
{
    int i,ii,I,J;

    for(I=1;I<=N;I++)
    {
	for(J=1;J<=N;J++)
	{
	    C_GC[I][J]=0;
	}
	k_GC[I]=0;
    }

    // printf("\n");
    for(ii=1;ii<=GComp;ii++)
    {
	i=analyse_GC[ii];
	// 	printf("%d   ",analyse_GC[ii]);
	
	for(I=1;I<=N;I++)
	{
	    if(i==I)   //si el nodo forma parte de la GC
	    {
		for(J=1;J<=NN[I];J++)
		{
		    C_GC[I][J]=Phase[I][J];					
		}
		k_GC[I]=NN[I];
	    }
	}
	
    }
    
   

/*	printf("\n");  
      for(j=1;j<=N;j++)    
    {
	printf("%d (NN%d):",j,NN[j]);
	for(i=1;i<=N;i++)
	{
	    if(Phase[j][i]!=0)
	    {
	    printf("%d   ",Phase[j][i]);  
	    }
	}
	printf("\n");  
    }
    
  
       

	printf("\n");  

    for(j=1;j<=N;j++)    
    {
	printf("%d (k_GC%d):",j,k_GC[j]);
	for(i=1;i<=N;i++)
	{
	    if(C_GC[j][i]!=0)
	    {
		printf("%d   ",C_GC[j][i]);  
	    }
	}
	printf("\n");  
    }
    
 */
    


 
}



////////////////////////////////
//////////////////////////////////////////////////
/////////////////////////////////////////////////////////



void clustering_coef_topol()
{

    int i,j,n,h,vecino[N+1],conectividad,nodo1,nodo2,links,Nvalidos;
    double aux1,combina, doubleprec, coef;



    Nvalidos=0;
    cl_coef=0.0;

    for(i=1;i<=N;i++)      //recorro todos los nodos de la GC
    {
		
	for(j=1;j<=k[i];j++)
	{
	    vecino[j]=C[i][j];
	}
	
	
	links=0;
	coef=0.0;
	conectividad=k[i];
	
	if(conectividad>1)  // hace falta aki esto  ?????????????
	{
	    Nvalidos++;


	    for(j=1;j<=conectividad;j++)    //bucle a todos los vecinos de i
	    {
		
		nodo1=vecino[j];
		
		for(n=j+1;n<=conectividad;n++)
		{
		    nodo2=vecino[n]; 
		    
		    if(nodo1 != nodo2) // esto hace falta??????? (es solo por seguridad?)
		    {						
			for(h=1;h<=N_links_max;h++)
			{
			    if((D[h][1] !=0)   &&  (D[h][2] !=0))
			    {
				if( nodo1==D[h][1] && nodo2==D[h][2])
				{
				    links ++;	
				}
				else if (nodo1==D[h][2] && nodo2==D[h][1])
				{
				    links ++;				    
				}				
			    }			    
			}						
		    }		    		    		    		    
		}				
		
	    } //fin del bucle a todos los vecinos de i	 



	aux1=conectividad;	    
	combina=aux1*(aux1-1.0)/2.0;   //maximo de links que podria haber entre los vecinos de i
	doubleprec=links;	    
	coef=doubleprec/combina;


	cl_coef=cl_coef+coef;
	//printf("links:%d  combina:%lf  coef:%lf  clust_coef:%lf\n",links,combina,coef,cl_coef); 	


   
	} //fin de la condicional

    } //fin del bucle a todos los nodos de GC
    
    
    cl_coef=cl_coef/Nvalidos;
    //printf("clust_coef_top:%lf\n",cl_coef); 
    
    
    
}

////////////////////////////////
//////////////////////////////////////////////////
/////////////////////////////////////////////////////////



void clustering_coef_consenso()
{

    int i,j,n,ii,h,vecino[N+1],conectividad,nodo1,nodo2,links,Nvalidos;
    double aux1,aux2,combina, doubleprec, coef;



    Nvalidos=0;
    cl_coef=0.0;
    //printf("\nGC:%f    ",GComp); 

    /* for(i=1;i<=N;i++)
    {
	printf("%d:    ",i); 

	for(j=1;j<=k_GC[i];j++)
	{
	    if(C_GC[i][j]!=0)
	    {
		printf("%d    ",C_GC[i][j]); 
	    }
	    
	}
	printf("\n"); 
    }

    */

    for(ii=1;ii<=GComp;ii++)      //recorro todos los nodos de la GC
    {
	i=analyse_GC[ii];
	//printf("i:%d (k:%d)     ",i,k_GC[i]); 
	
	for(j=1;j<=k_GC[i];j++)
	{
	    vecino[j]=C_GC[i][j];
	}
	
	
	links=0;
	coef=0.0;
	conectividad=k_GC[i];
	
	if(conectividad>1)  // hace falta aki esto  ?????????????
	{
	    Nvalidos++;
	    
	    
	    for(j=1;j<=conectividad;j++)    //bucle a todos los vecinos de i
	    {
		
		nodo1=vecino[j];
		
		for(n=j+1;n<=conectividad;n++)
		{
		    nodo2=vecino[n]; 
		    
		    //printf("n1:%d   n2:%d \n",nodo1,nodo2); 	 
		    
		    if(nodo1 != nodo2) // esto hace falta???????'
		    {						
			for(h=1;h<=N_links_max;h++)
			{
			    if((D[h][1] !=0)   &&  (D[h][2] !=0))
			    {
				if( nodo1==D[h][1] && nodo2==D[h][2])
				{
				    links ++;
				    // printf("links:%d \n",links); 	
				}
				else if (nodo1==D[h][2] && nodo2==D[h][1])
				{
				    links ++;
				    //printf("links:%d \n",links); 	 		    
				}				
			    }			    
			}						
		    }		    		    		    		    
		}				
		
	    } //fin del bucle a todos los vecinos de i	 
	    
	    
	    
	    aux1=conectividad;	    
	    combina=aux1*(aux1-1.0)/2.0;   //maximo de links que podria haber entre los vecinos de i
	    doubleprec=links;	    
	    coef=doubleprec/combina;
	    
	    
	    cl_coef=cl_coef+coef;
	    //printf("links:%d  combina:%lf  conectividad:%d   coef:%lf\n",links,combina,conectividad,coef); 	
	    
	   
	    
	} //fin de la condicional
/*	else
	{
	    printf("conectividad:%d \n",conectividad); 	
	    }*/
	
    } //fin del bucle a todos los nodos de GC
    
    aux2=GComp;
    cl_coef=cl_coef/aux2;
    //  printf("clust_coef:%lf  \n",cl_coef); 
    
    
    
    
    
    
}






/////////////////////////////////////////
//////////////////////////////////////////////////
/////////////////////////////////////





void histograma_PK_GC(n_size)
{                               //si rasgo=0, estoy calculando la pk de la gc de consenso tot.

  int i,ii;
  double aux,aux1;

  
  for(i=0; i<=N; i++)   
    {
      PK_GC[i][rasgo][n_size] = 0.;
      pk_topol_GC[i][rasgo][n_size]=0.;
    }
  
  
  for(ii=1;ii<=GComp;ii++)
    {
      i=analyse_GC[ii];    //recorro los nodos que forman la GC
      
      
      PK_GC[k_GC[i]][rasgo][n_size]++;     //ojo!! utilizo no la k[i] ni la NN[], sino la k_GC[]!!!


      pk_topol_GC[k[i]][rasgo][n_size]++;
      
    }
  
  
  /* Normalizo PK */
  aux=GComp;
  aux1=N;
  for(i=1;i<=GComp;i++)
    {
      PK_GC[i][rasgo][n_size] = PK_GC[i][rasgo][n_size]/aux;
      
      if(PK[i]!=0)
	{
	  pk_topol_GC[i][rasgo][n_size]= pk_topol_GC[i][rasgo][n_size]/(PK[i]*aux1);  //normalizo por el numero de nodos con esa k  (notar que pk ya estaba normalizado por N, por eso ahora lo vuelvo a dividir)
	}
      
    }         
  
  //  printf("rasgo:%d (cont:%d)\n",rasgo,contador);


}









////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////



void construir_C_k()      //parto de la matriz de pares de links D[][]
{

    int i,j,d1,d2;

    for(i=1;i<=N;i++)
    {
	k[i]=0;
	for(j=1;j<=N;j++)
	{
	    C[i][j]=0;
	}
    }



    for(i=1;i<=filas;i++)
    {
//	printf("fila%d: ",i);

	d1=D[i][1];    //nodo 1 del link
	d2=D[i][2];    //nodo 2 del link

//	printf("%d-%d\n",d1,d2);


	k[d1]++;
	k[d2]++;
	
	/*printf("k1:%d,  k2:%d\n",k[d1],k[d2]);

	getchar();*/

	C[d1][k[d1]]=d2;
	C[d2][k[d2]]=d1;

    }
    
    /* for(i=1;i<=N;i++)   //comprobacion
    {

	printf("\n%d (k:%d):",i,k[i]);  
	for(j=1;j<=k[i];j++)
	{
	    printf("%d   ",C[i][j]);    
	}
	getchar();
	}*/
}










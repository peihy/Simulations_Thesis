//Implementacion de una dinamica de juego combinada con el crecimiento de la red por pref.-attach

// bucle de estadística y de barrido en b, ademas de computar para el p(k) solo las realizaciones no nulas   

//actualizacion paralela

//calculo de average path lenth y de clustering coeficient


//(basado en el growth&play-v2_validas.c)

// 30-enero: modificaciones para obtener Pk(t) en diferentes momentos del crecimiento 

// 11-marzo: añadido intervalo de tiempo final de juego una vez acabada de  crecer la red

// 17-6-08: en lugar de añadir intervalo de tiempo final de juego una vez acabada de  crecer la red, una vez crecida, la randomizo (manteniendo las estrategias) y  las dejo evolucionar, 
//para ver si el estado final de <c> al que se llega es igual al <c>inf o no.

// la randomizacion esta copiada del programa randomizar.c usado en el juego general (redes estáticas)

# include <stdio.h>
# include <stdlib.h>
# include <math.h>


# define N 1000 //tamaño de la red
# define m_o  2      //nodos inicialmente unidos
# define m   2      //links nuevos añadidos a cada paso de tiempo

# define Niter 1000   //estadistica
# define K_MAX   500      //para el tamaño de C[N][K_MAX]     ojo pq depende de N en SF!!!
                          //tambien calculable en la funcion matriz_Conect


# define  eps 0.99

# define  tauT 10       //(si tauT=10 y tauD=1, pongo 10 nodos y juego una vez)
# define  tauD 1


# define  ro 0.5 
# define  R 1.0
# define  Pu 0.0
# define  Su 0.0


# define b_min    1.0
# define b_max    3.1


# define delta_b 0.1

# define jugadas_extra   20000       //despues de acabar de crecer la red



//Generacion de numeros aleatorios
#define FNORM (2.3283063671E-10F)
#define RANDOM ((ira[ip++]=ira[ip1++]+ira[ip2++])^ira[ip3++])
//numero aleatorio flotante en el intervalo (0,1]
#define FRANDOM (FNORM*RANDOM)
unsigned char ip,ip1,ip2,ip3;
unsigned ira[256];


int k[N+1],k_PA[N+1],A[N+1];  //conectividad topológica y de Preferential Attachment (PA)
int   M[N+1][m+1], unido[m+1], Cuidado[N+1]; //  unido[] guarda a quién se ha unido, y cuidado[] quien le ha lanzadolinks
double P[N+1], P_prov[N+1];     //P_prov[] es la provisional para manipularla

double PK[N+1], PK_tot[N+1], PK_coop[N+1], PK_coop_tot[N+1];    //  para la funcion histograma_P(k)
double   PK_rand[N+1], PK_rand_tot[N+1];        //  para la distribucion P(k) despues de randomizar

double PK_tot_500[N+1], PK_coop_tot_500[N+1];    //para las p(k) durante el crecimiento
double PK_tot_1000[N+1], PK_coop_tot_1000[N+1];  
double PK_tot_2000[N+1], PK_coop_tot_2000[N+1];  
double PK_tot_4000[N+1], PK_coop_tot_4000[N+1]; 
double CC[K_MAX+1], norma_CC[K_MAX+1]; 
int kmax_500, kmax_1000, kmax_2000, kmax_4000;
double aux7,aux77,aux777;
int max_k_500,max_k_1000,max_k_2000,max_k_4000;

int NN[N+1],Phase[N+1][K_MAX+1],warning[N+1],analyse[N+1];  //para la GC de coop.
int NCLUSTERS,NCLUSTERSMED,GCMED,NonNullCP,GComp;

double r,v;                     //para guardar los aleatorios
int  II,i,j,jj,g,gg, d, q,y,z, x[N+1], C[N+1][K_MAX+1], steps,iter;
double norma,norma2;
int si[N+1], cont,cont2;

//Variables para el dilema

//double b,R,Pu,Su;
//int tauT,tauD;
int tiempo;

int  e[N+1],e_aux[N+1], t, w, k_m; 
double  ben[N+1],p;
int  n_coop; //numero de coop. instantaneos
int iter,counter,n,ii;

int s, pasos;


double fit[N+1],c_media;       
double b;
double aux;
int validas;


//para el av_path_l
double dist_tot,D_med;

int D[m*N+1][m+1],n_links,GC;   //matriz de pares de links

int D_prov[m*N+1][m+1];          //para el clust_coef
double clust_coef_tot,coop_media_inf;   


void inicia_rand(int semilla);
void nucleo_inicial();
void jugada();
void nuevo_nodo();
void histograma_pk();
void matriz_Conect(); //contiene todos los vecinos del nodo i (tamaño: N x k_max  )    //NO LA NECESITO????? BASTA CON LA M[i][j] para sacar D[i][j]???????
void av_path_length();
void clustering_coef();
int find_kmax();
void histograma_pk_coop();
void matriz_D(); //contiene todos los pares de vecinos (tamaño real: ((N-m_o)*m+3) x 2  )
void randomizar();
void check_GC();
void histograma1();


char nombre[256],nombre3[256],file8[256],file9[256], nombre4[256],nombre5[256];
char file7[256],file77[256],file777[256],file7777[256]; 
char file6[256],file66[256],file666[256],file6666[256]; 
char file[256], file1[256], file4[256],file22[256];  
char file2[256],file3[256];

FILE *fich7;     
FILE *fich77; 
FILE *fich777; 
FILE *fich7777;      
FILE *fich6;     
FILE *fich66; 
FILE *fich666; 
FILE *fich6666;              
FILE *fich; 
FILE *fich2; 
FILE *fich3; 
FILE *fich1; 
FILE *fich4;      
FILE *fich22;     
FILE *fich5;





int main()
{
    
    /*sprintf(file3,"%dC_med%d-%d-b_ini%.2lf-e%.2lf_%diter.dat",N,tauT,tauD,b_min,eps,Niter);   //un solo archivo     
      fich3=fopen(file3,"wt");
      fclose(fich3);*/
    
    
    sprintf(file4,"%dProp-top_%d-%d-b_ini%.2lf-e%.2lf_%diter_rand.dat",N,tauT,tauD,b_min,eps,Niter);   //un solo archivo     
    fich4=fopen(file4,"wt");
    fclose(fich4);
    
    
    /*sprintf(file2,"%dLength%d-%d-b%.2lf-e%.2lf_pay.dat",N,tauT,tauD,b_min,eps);        
      fich2=fopen(file2,"wt");
      fclose(fich2);*/
    
    inicia_rand(time(0));
    
    printf("\n\nPrograma Growth & Play con Prob-payoff Y Rand (%d iteraciones)\nN=%d  b_ini=%lf b_fin=%lf eps=%lf  TauT=%d  TauD=%d\n", Niter,N,b_min,b_max,eps,tauT,tauD);
    
    
    b=b_min;
    
    while(b<=b_max)     //bucle externo para barrer en b
    {
	printf("\nb=%lf\n",b);
	
	
/*	sprintf(file7,"%dPk%d-%d-b%.2lf-e%.2lf_size500_%diter.dat",N,tauT,tauD,b,eps,Niter); // para los P(k) durante el crecimiento
	fich7=fopen(file7,"wt");
	fclose(fich7);      */
	
	sprintf(file77,"%dPk%d-%d-b%.2lf-e%.2lf_size1000_%diter.dat",N,tauT,tauD,b,eps,Niter);      
	fich77=fopen(file77,"wt");
	fclose(fich77);      
	
/*	sprintf(file777,"%dPk%d-%d-b%.2lf-e%.2lf_size2000_%diter.dat",N,tauT,tauD,b,eps,Niter);      
	fich777=fopen(file777,"wt");
	fclose(fich777);
	
	sprintf(file7777,"%dPk%d-%d-b%.2lf-e%.2lf_size4000_%diter.dat",N,tauT,tauD,b,eps,Niter);      
	fich7777=fopen(file7777,"wt");
	fclose(fich7777);
	
*/	
	
	sprintf(file6,"%dPkc%d-%d-b%.2lf-e%.2lf_size500_%diter.dat",N,tauT,tauD,b,eps,Niter); // para los P(k)c  durante el crecimiento
	fich6=fopen(file6,"wt");
	fclose(fich6);      
	
	sprintf(file66,"%dPkc%d-%d-b%.2lf-e%.2lf_size1000_%diter.dat",N,tauT,tauD,b,eps,Niter);      
	fich66=fopen(file66,"wt");
	fclose(fich66);      
	
	sprintf(file666,"%dPkc%d-%d-b%.2lf-e%.2lf_size2000_%diter.dat",N,tauT,tauD,b,eps,Niter);      
	fich666=fopen(file666,"wt");
	fclose(fich666);
	
	sprintf(file6666,"%dPkc%d-%d-b%.2lf-e%.2lf_size4000_%diter.dat",N,tauT,tauD,b,eps,Niter);      
	fich6666=fopen(file6666,"wt");
	fclose(fich6666);
	
	
	
	sprintf(file,"%dEvolCoop%d-%d-b%.2lf-e%.2lf_%diter.dat",N,tauT,tauD,b,eps,Niter);    // id.    
	fich=fopen(file,"wt");
	fclose(fich);
	
	sprintf(file1,"%dnum_coop%d-%d-b%.2lf-e%.2lf_%diter.dat",N,tauT,tauD,b,eps,Niter);    // id.    
	fich1=fopen(file1,"wt");
	fclose(fich1);
	
	sprintf(file22,"%dCCk_%d-%d-b%.2lf-e%.2lf_%diter.dat",N,tauT,tauD,b,eps,Niter);    // id.    
	fich22=fopen(file22,"wt");
	fclose(fich22);
	

//Guarda la P(k) antes y despues de randomizar
          sprintf(nombre5,"PK_randomizar_b%.2lf_S%.2lf_P%.2lf_N%d_%diter_r.dat",b,Su,Pu,N,Niter);
          fich5=fopen(nombre5,"wt");
          fclose(fich5);



	
	
	c_media=0.;
	for (i=0;i<=N;i++)
	{
	    
	    PK_tot_500[i]=0.;	
	    PK_tot_1000[i]=0.;	
	    PK_tot_2000[i]=0.;	
	    PK_tot_4000[i]=0.;
	    
	    PK_coop_tot_500[i]=0.;	
	    PK_coop_tot_1000[i]=0.;	
	    PK_coop_tot_2000[i]=0.;	
	    PK_coop_tot_4000[i]=0.;
	    
	}
	
	for(i = 0; i <=N ; i++)
	{
	    PK_tot[i] = 0;
	    PK_rand_tot[i] = 0;
	}
	
	

	for (i=0;i<=K_MAX;i++)
	{
	    CC[i]=0.0;
	    norma_CC[i]=0.0;
	}
	
	
	
	max_k_500 = max_k_1000 = max_k_2000 = max_k_4000=0;
	
	clust_coef_tot=0.0;
	D_med=0.;
	validas=0;
	coop_media_inf=0.0;
	
	
	for(iter=1;iter<=Niter;iter++)         //inicio bucle de estadistica
	{
	    
	    printf("\niter:%d\n",iter);
	    
	    
	    
	    // INICIAliZO:
	    
	    steps=N-m_o;       //numero de nodos que se añadiran en total
	    s=m_o;         //tamagno actual de la red: s=N(t)
	    tiempo=1;       //contador de tiempos para tauT y tauD...	
	    n_coop= 0;  
	    
	    for(i=1;i<=N;i++)      
	    {
		ben[i]=0;
		e[i]=1;     //TODOS DEFECTORES      
	    }
	    
	    
	    
	    nucleo_inicial();
	    matriz_Conect();  //hay que actualizarla cada vez para poder jugar
	    //printf("num coop.: %d  \n",  n_coop);
	    
	    
	    do{
		
		for(II=1;II<=tauD;II++) 
		{
		    jugada();
		    
		    //printf("\n juego");
		    n_coop=0;
		    for(i=1;i<=s;i++)
		    {
			if(e[i]==0)	    
			    n_coop++;	      
		    }
		    
		    if(iter<=10)    //pq si no, el archivo se hace demasiado grande!!
		    {
			fich=fopen(file,"at");
			fprintf(fich,"%d   %d\n",s,n_coop);      
			fclose (fich);
		    }
		    
		}
		
		//Recalculo Probabilidad de attachment: (SOLO AQUI PARA OPTIMIZAR)
		
		for(i=1;i<=N;i++)
		{P[i]=0.;}
		
		for(i=1;i<=s;i++)
		{
		    counter=counter+k[i];
		    for(j=1;j<=i;j++)  
		    {
			P[i]=P[i]+fit[j]; 
		    }
		}
		norma=P[s];
		
		//ATTACHMENT: 
		
		for(II=1;II<=tauT;II++)
		{
		    if(s<N)    // parche para evitar que escriba en los vectores, más alla de s=N
		    {
			s++;	    
			nuevo_nodo();	      
			//printf("\n%i  ",s);
			
			if(s==500)        //Pk  durante el crecimiento:      
			{		  
			    kmax_500=find_kmax();	
			    
			    if(kmax_500>max_k_500)
			    {
				max_k_500=kmax_500;
			    }
			    
			    histograma_pk();     //ojo!  por ahora no la normalizo  dentro de la funcion!
			    histograma_pk_coop();
			    
			    for(i = 1; i <= s; i++)         //acumulo
			    {
				
				PK_tot_500[i] += PK[i]; 
				PK_coop_tot_500[i] += PK_coop[i]; 
				
			    }		  	
			    
			}       //fin del bucle de 500
			
			if(s==1000)        //Pk  durante el crecimiento:      
			{		  
			    kmax_1000=find_kmax(); 
			    
			    if(kmax_1000>max_k_1000)
			    {
				max_k_1000=kmax_1000;
			    }
			    
			    
			    histograma_pk();     //ojo!  por ahora no la normalizo  dentro de la funcion!
			    histograma_pk_coop();
			    
			    for(i = 1; i <= s; i++)         //acumulo
			    {				
				PK_tot_1000[i] += PK[i]; 
				PK_coop_tot_1000[i] += PK_coop[i]; 				
			    }		  	
			    
			}   //fin del bucle de 1000
			
			if(s==2000)        //Pk  durante el crecimiento:      
			{		  
			    kmax_2000 = find_kmax();
			    
			    if(kmax_2000>max_k_2000)
			    {
				max_k_2000=kmax_2000;
			    }
			    
			    histograma_pk();     //ojo!  por ahora no la normalizo  dentro de la funcion!
			    histograma_pk_coop();
			    
			    for(i = 1; i <= s; i++)         //normalizo
			    {				
				PK_tot_2000[i] += PK[i]; 
				PK_coop_tot_2000[i] += PK_coop[i]; 	      	
			    }		  
			    
			}   //fin del bucle de 2000
			
		    }	// cierre del parche
		    
		}       //fin del bucle de tau_t
		
		tiempo=tiempo+1; 
		
	    }while(s<N);        //acaba de crecer la red
	    
	    
	    
	    if(s==4000)        //Pk  final:
	    {		  
		kmax_4000=find_kmax(); 
		
		if(kmax_4000>max_k_4000)
		{
		    max_k_4000=kmax_4000;
		}
		
		
		histograma_pk();     //ojo!  por ahora no la normalizo  dentro de la funcion!
		histograma_pk_coop();
		
		for(i = 1; i <= s; i++)         //normalizo
		{		    		    
		    
		    PK_tot_4000[i] += PK[i]; 
		    PK_coop_tot_4000[i] += PK_coop[i]; 
		    
		}		  
		
	    }   //fin del bucle de 4000
	    
	    
	    
	    matriz_Conect();
	    matriz_D();                   //NO HACE FALTA CALCULAR LA MATRIZ C[i][j], basta con la M[i][j]?????
	    //printf("\nfuncion matriz_D  hecha");
	    av_path_length();
	    //printf("\nfuncion av_path_l  hecha");
	    
	    clustering_coef();
	    //printf("\nfuncion clust_coef hecha");
	    
	    c_media+=n_coop;
	    D_med+=dist_tot;
	    //printf("\nacumulo C y D");
	    if(n_coop>4*m) //para la norma del p(k)
	    {
		validas++;   
	    }
	    
	    
	    aux=n_coop/(double)N;
	    fich1=fopen(file1,"at");	  
	    fprintf(fich1,"%d  %lf\n", iter,aux);     //guardo la coop_media de cada iter	  	  
	    fclose (fich1);      
	     
	    
	    randomizar();
	    //printf("\nrandomizacion hecha");
	    histograma1();

	  
	    for(ii=1;ii<=jugadas_extra;ii++)     //bucle para continuar jugando con la red ya crecida
	    {
		jugada();
		n_coop=0;
		for(i=1;i<=s;i++)
		{
		    if(e[i]==0)	    
			n_coop++;	      
		}
		
		if(n_coop==0)   //stop
		    break;
		
		
	    }
	    
	    coop_media_inf=coop_media_inf+n_coop;    //solo guardo la cooperacion final tras las jugadas_extra
	    
	    //printf("\njugadas extra hechas");
	    
//	  printf("n_coop=%i\n",n_coop);
	    
	    
	    
	    
	    
	    
	    
	    
	    
	}                  //fin bucle estadistica
	
	
	c_media=c_media/(N*Niter);
	clust_coef_tot=clust_coef_tot/Niter;
	D_med=D_med/Niter;
	coop_media_inf=coop_media_inf/(N*Niter);
	
	
	for(ii=1;ii<=K_MAX;ii++)    
	{
	    if(norma_CC[ii]!=0.0)
	    {
		CC[ii]=CC[ii]/(Niter*norma_CC[ii]);   //////NO estoy segura de la normalizacion!!!!!
	    }
	    else
	    {
		CC[ii]=0.0;
	    }
	}
	
	
	printf("\n\nb:%lf   clust_coef:%lf   Av-path_l:%lf    c_med:%lf    c_med_inf:%lf   \n",b, clust_coef_tot, D_med, c_media,coop_media_inf);
	
/*	// Escribo y normalizo (por el numero de iteraciones) las PK 
	
fich7=fopen(file7,"at");        // Escribo PK  de s=500      
for(i = 1; i<N; i++)          // escribo la pk y la normalizo por el numero de iteraciones
{
aux7=i;
aux77=kmax_500;
aux777=aux7/aux77;
//printf("\n i=%d  kmax_500=%d  aux777=%lf  ",i,kmax_500,aux777);
fprintf(fich7,"%lf  %lf  %d\n", aux777, PK_tot_500[i]/(500*Niter),i);
}                  		
fclose (fich7);
*/	
    
    fich77=fopen(file77,"at");     // Escribo PK  de s=1000      
    for(i = 1; i<N; i++)          // escribo la pk y la normalizo por el numero de iteraciones
    {
	aux7=i;
	aux77=kmax_1000;
	aux777=aux7/aux77;
	fprintf(fich77,"%lf  %lf  %d\n", aux777, PK_tot_1000[i]/(1000*Niter),i);
    }                  		
    fclose (fich77);
    
    
/*
  fich7=fopen(file777,"at");        // Escribo PK  de s=500      
  for(i = 1; i<N; i++)          // escribo la pk y la normalizo por el numero de iteraciones
  {
  aux7=i;
  aux77=kmax_2000;
  aux777=aux7/aux77;
  fprintf(fich777,"%lf  %lf  %d\n", aux777, PK_tot_2000[i]/(2000*Niter),i);
  }                  		
  fclose (fich777);
  
  
  fich7777=fopen(file7777,"at");     // Escribo PK  de s=4000      
  for(i = 1; i<N; i++)          // escribo la pk y la normalizo por el numero de iteraciones
  {
  aux7=i;
  aux77=kmax_4000;
  aux777=aux7/aux77;
  fprintf(fich7777,"%lf  %lf  %d\n", aux777, PK_tot_4000[i]/(Niter*4000),i);
  }                  		
  fclose (fich7777);
*/	
    
    
// Escribo y normalizo (por el numero de iteraciones) las PK de coop 
    
    fich6=fopen(file6,"at");        // Escribo PKc  de s=500      
    for(i = 1; i<N; i++)          // escribo la pk y la normalizo por el numero de iteraciones
    {
	aux7=i;
	aux77=max_k_500;
	aux777=aux7/aux77;
	
	
	//printf("\n i=%d  kmax_500=%d  aux777=%lf  ",i,kmax_500,aux777);
	fprintf(fich6,"%lf  %lf  %d\n", aux777, PK_coop_tot_500[i]/PK_tot_500[i],i);
    }                  		
    fclose (fich6);
    
    
    fich66=fopen(file66,"at");     // Escribo PKc  de s=1000      
    for(i = 1; i<N; i++)          // escribo la pk y la normalizo por el numero de iteraciones
    {
	aux7=i;
	aux77=max_k_1000;
	aux777=aux7/aux77;
	fprintf(fich66,"%lf  %lf  %d\n", aux777, PK_coop_tot_1000[i]/PK_tot_1000[i],i);
    }                  		
    fclose (fich66);
    
    
    fich666=fopen(file666,"at");     // Escribo PKc  de s=2000      
    for(i = 1; i<N; i++)          // escribo la pk y la normalizo por el numero de iteraciones
    {
	aux7=i;
	aux77=max_k_2000;
	aux777=aux7/aux77;
	fprintf(fich666,"%lf  %lf  %d\n", aux777, PK_coop_tot_2000[i]/PK_tot_2000[i],i);
    }                  		
    fclose (fich666);
    
    
    fich6666=fopen(file6666,"at");     // Escribo PKc  de s=4000      
    for(i = 1; i<N; i++)          // escribo la pk y la normalizo por el numero de iteraciones
    {
	aux7=i;
	aux77=max_k_4000;
	aux777=aux7/aux77;
	fprintf(fich6666,"%lf  %lf  %d\n", aux777, PK_coop_tot_4000[i]/PK_tot_4000[i],i);
    }                  		
    fclose (fich6666);
    
    
    printf("K_max_500:%d  K_max_1000:%d  K_max_2000:%d K_max_4000:%d\n",max_k_500, max_k_1000, max_k_2000, max_k_4000);
    
    printf("Niter:%d   Validas:%d\n",Niter, validas);
    
    
    
    
//escribo  el clust-coef , el Av-path_length y la c_med
    
    fich4=fopen(file4,"at");
    fprintf(fich4,"%lf   %lf   %lf   %lf   %lf   %d   %d   %lf\n",b, clust_coef_tot, D_med, c_media, eps, tauT, tauD,coop_media_inf);      
    fclose (fich4);
    
    
    
// escribo el cluster coef en funcion de las clases de conectividad
    
    fich22=fopen(file22,"at");
    for(i = 1; i<K_MAX; i++)         
    {
	fprintf(fich22,"%d    %lf\n",i,CC[i]);      
    }
    fclose (fich22);
    
    
   
    
    
    fich5=fopen(nombre5,"at");    // guardo Pk antes y despues de randomizar (para comprobacion)
    for(i=1;i<=N;i++)
    {                              //normalizo la  pk de antes y de despues de randomizar
	fprintf(fich5,"%d   %lf   %lf\n",i, PK_tot_1000[i]/(1000.0*Niter), PK_rand_tot[i]/Niter);
    }
    fclose(fich5);
    
    
       
    
    b+=delta_b;    
    //fclose(escribe);
    //fclose(escribe2);
    
    }     //fin del bucle en b
    
    
    
    
}






/////////////////////////////////////////////////
/////////////////////////////////////////////////


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



/////////////////////////////////////////////
/////////////////////////////////////////////  


void nucleo_inicial()    //conecto los m_o nodos iniciales todos entre si
{
    
    
    for(i = 0; i<=N; i++)
    {
	k[i] = 0;     
	
	for(j = 0; j <= m_o; j++)
	{M[i][j] =0;}
	
	for(j=0;j<=K_MAX;j++)
	{C[i][j]=0;}
    }
    
    
    for(j = 1; j <=m_o ; j++) 
	k[j]=m_o-1;
    
    //  C[1][1]=2;
    //  C[2][1]=1;
    
    for(i = 1; i <=m_o ; i++)
    {
	counter=0;
	for(j = 1; j <=m_o ; j++)
	{
	    if(i!=j)
	    {
		counter++;
		C[i][counter]=j;
	    }
	}
    }
    
    
    for(i = 2; i <=m_o ; i++)
    {
	for(j = 1; j <=i-1 ; j++)
	{
	    M[i][j] = j;
	    //printf("M[%d][%d] = %d \n",i,j,M[i][j]);
	}
    }
    
    for(i=1;i<=m_o;i++)                 
    { 
	fit[i]=1.-eps;  
    }
    
    for(i = 0; i <= N; i++)
    {
	P[i] = 0;
	P_prov[i]=0;
    }
    
    for(i = 1; i<= m_o; i++)                    //prob de los nodos iniciales
    { 
	
	for(j=1; j<=i; j++)
	{                              
	    P[i] = P[i] + fit[j];        //para los m_o fit[i]=1
	}
	
	P_prov[i]=P[i];
    }
    
    norma=P[m_o];
    norma2=norma;
    
    // establezco como cooperadores a los  nodos iniciales:
    
    
    for(i=1;i<=m_o;i++)
    {     
	e[i]=0;       //cooperador
	n_coop++;
    }
    
    
}


//////////////////////////////////////
//////////////////////////////////////



void nuevo_nodo() 
{
    
// establezco el carcter del nuevo nodo:
    
    r=FRANDOM;
    
    if(r<ro)
    {                
	e[s]=0;       //cooperador
    }
    
    
    for(i=0; i<=m; i++)
    {unido[i]=0;}
    
    for(i=1; i<=N; i++)
    {P_prov[i]=P[i];}   //guardo la prob en un vector aux para manipularlo
    
    norma2=norma;
    P[s]=0.;  
    P_prov[s]=0.;  
    
    
    for(q = 1; q<=m; q++)        //bucle a los m links nuevos que voy a añadir
    {
	
	v=FRANDOM*norma2;
	
	for(j=1; j<s; j++)
	{
	    if(v<P_prov[j])        //elijo el nodo
	    {
		unido[q]=j;
		//printf(" %f   %d   %f nodo:%d\n",v,norma2,FRANDOM, j);
		break;
	    }
	}
	
	g=unido[q];
	P_prov[g]=0;    //lo guardo y anulo su prob para no volver a cogerlo
	
	for(j=g+1; j<s; j++)  
	{
	    P_prov[j]=P_prov[j]-fit[g] ; 
	}
	
	norma2=norma2-fit[g];
	
	//printf(" norma:   %d \n",norma2);
    }    //fin del bucle sobre los m links lanzados
    
    
    
    for(j=1;j<=m_o;j++)
    {
	g=unido[j];
	k[g]=k[g]+1;
	C[g][k[g]]=s;
	C[s][j]=g;
	M[s][j]=g;
        
        /*
	  for(i=g;i<s;i++)   
	  {
	  P[i]=P[i]+fit[g];
	  }
	*/
    }
    
    k[s]=m;
    fit[s]=(1.-eps);
    P[s]=P[s-1]+fit[s];
    norma=P[s];
    
    
}



void matriz_Conect()   //obtencion de la matriz C a partir de la M
{
    
    
    
    for(i=0;i<=N;i++)
    {
	x[i]=0;
	
	for(j=0;j<=K_MAX;j++)
	{
	    C[i][j]=0;
	}
    }
    
    for(i=1;i<=s;i++)     //ahora solo va de 1 a s
    {
	for(j=1;j<=m;j++)
	{ 
	    if(M[i][j]!=0)
	    {
		x[i]++;
		z=M[i][j];
		//printf("%i  %i\n",i,z);
		C[i][x[i]]=z;
		x[z]++;
		C[z][x[z]]=i;	 
	    }
	}
    }
    
}


//////////////////////////////////////////
////////////////////////////////////

void jugada()  //es un paso del bucle de tiempo de "juego()" 
{

    n_coop=0;

    for(i=1;i<=s;i++)         // ahora de 1 a s!!
    {
	ben[i]=0;
    }
    
    for(i=1;i<=s;i++) // cada nodo juega con cada vecino ahora de 1 a s!!
    {
	for(j=1;j<=k[i];j++)
	{
	    y=C[i][j];            
	    if(y>i)            //para no repetir vecinos en una partida
	    {  
		if(e[i]==0 && e[y]==0)
		{
		    ben[i]+=R;
		    ben[y]+=R;
		    //printf("%d : %d  empate coop \n", i,y);
		}
		
		if(e[i]==0 && e[y]==1)
		{
		    ben[i]+=Su;
		    ben[y]+=b;
		    //printf("%d : %d  coop contra defect\n", i,y);
		}

		if(e[i]==1 && e[y]==1)
		{
		    ben[i]+=Pu;
		    ben[y]+=Pu;
		    
		    //printf("%d : %d  empate defect \n", i,y);
		}
		
		if(e[i]==1 && e[y]==0)
		{
		    ben[i]+=b;
		    ben[y]+=Su;
		    //printf("%d : %d  empate defect \n", i,y);
		}
	    }
	  
	}
    }            //fin de la partida


for (i=1;i<=s;i++)
    {
	e_aux[i]=e[i];       //guardo la estrategia del ultimo paso (para el update paralelo)
    }
    

    
  for(i=1;i<=s;i++)        //comparo estrategias    ahora de 1 a s!!
     {
       r=FRANDOM;
       r=r*k[i];
       w=(int)r+1;

    
       t=C[i][w];    //el vecino elegido al azar
      
       if(k[i]>k[t])
       {k_m=k[i];}
       else
       {k_m=k[t];}

       //printf("juego1 ben[i]=%lf ben[t]=%lf\n",ben[i],ben[t]);//CONTROL


       if(ben[i]<ben[t])
       {
	   p=(ben[t]-ben[i]);
	   p=p/(b*(double)k_m);

          v=FRANDOM;
          if(v<p)
	  {
	      e_aux[i]=e[t];
	    
	  }
       }

     }



 for (i=1;i<=s;i++)      //actualizacion de estrategias en paralelo
      {
	e[i]=e_aux[i];
      }
    


  //calculo del fitness

  for (i=1;i<=s;i++)
    {
      fit[i]=1.0-eps+eps*ben[i];
    }
       
}

/////////////////////////////////////
///////////////////////////////////////////


void histograma_pk()            //costruccion de P(k)

{
    for(i = 0; i <= N; i++)           //inicializo
    {
	PK[i]=0.;
	
    }
    
    for(i =1; i <= s; i++)            //recuento
    {
	PK[k[i]]++;
	
    }
    
   
    
}        

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

  

void matriz_D()        //contiene todos los pares de vecinos (segun orden de creacion del link)
{
  int i,j,h;
  
  for(i=0;i<=2*N;i++)
    {
      for(j=0;j<=m;j++)
	{
	  D[i][j]=0;
	}
    }
  
  n_links=0;
  h=1;
  for(i=1;i<=N;i++)
    {
      for(j=1;j<=m;j++)
	{
	  if(M[i][j]!=0)
	    {
	      D[h][1]=i;
	      D[h][2]=M[i][j];
	      h++;
	    }
	}
      
    }
  n_links=h;   //guardo el numero total de links

  /* printf("\n");
  for(i=1;i<=2*N;i++)
    {
      printf("\nD[%d][]: ",i);
      for(j=1;j<=m;j++)
	{
	  printf("%d  ",D[i][j]);
	}
    }
  // getchar();
  
  printf("fin funcion matriz_D\n"); */
}  




//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////



void clustering_coef()
{
    int i,j,h,n,Nvalidos,links,nodo1,nodo2,vecino[K_MAX+1],conectividad;
    double coef, clust_coef,doubleprec, combina,aux1;
    
    for(i=1;i<=2*N;i++)
	for(j=1;j<=2;j++)
	    D_prov[i][j]=D[i][j];   //guardo la matriz de pares de links para manipularla
    
    
    
    Nvalidos=0;
    clust_coef=0.0;
    
    for(i=1;i<=N;i++)    //recorro la red
    {
	//printf("\nnodo %d  con conectividad %d\n",i,k[i]); 
	
	conectividad=k[i];
	
	if(conectividad>1)  
	{
	    Nvalidos++;
	    
	    
	    links=0;
	    coef=0.0;
	    for(j=1;j<=conectividad;j++) 
	    {
		vecino[j]=C[i][j];    //guardo los vecinos del nodo i
	    }
	    
	    
	    for(j=1;j<=conectividad;j++)     //recorro los vecinos de i
	    {	      
		nodo1=vecino[j];	   
		
		
		for(n=j+1;n<=conectividad;n++)   //busco vecinos de i que lo sean entre si
		{
		    // printf("j:%d n:%d\n",j,n);  
		    nodo2=vecino[n];
		    
		    if(nodo1 != nodo2)
		    {
			
			for(h=1;h<=m*N;h++)   //recorro la matriz de pares de links
			{
			    if((D_prov[h][1] !=0)   &&  (D_prov[h][2] !=0) )
			    {
				
				if( nodo1==D_prov[h][1] && nodo2==D_prov[h][2]) 
				{
				    links ++;				    
				}
				else if(nodo1==D_prov[h][2] && nodo2==D_prov[h][1])
				{
				    links ++;				    
				}
			    }
			}
			
		    }
		}	 
		
	    }      //recorridos todos los vecinos de i
	    // printf("\nrecorridos todos los vecinos de %i",i);
	    
	    aux1=conectividad;
	    
	    combina=aux1*(aux1-1.0)/2.0;   //maximo de links que podria haber entre los vecinos de i
	    
	    doubleprec=links;
	    
	    coef=doubleprec/combina;
	    
	    
	    
	    clust_coef=clust_coef+coef;
//	printf("links:%d  combina:%lf  coef:%lf  clust_coef:%lf\n",links,combina,coef,clust_coef); 	
	    
	    
	    
	    CC[conectividad] += coef;   //para representar la dependencia del clust coef con las clases de conectividad
	    //printf("nodo:%d CC[%d]=%lf\n",i,conectividad,CC[conectividad]); 
	    
	    norma_CC[conectividad]++;      //lo normalizaré al final del bucle de estadistica o no lo normalizo??????????????'
	    
	    
	}     //fin de la condicional k[i]>1 
	
    }//recorrida toda la red
    
    
    
     //printf("\nrecorrida toda la red");
    
    clust_coef=clust_coef/Nvalidos;
    printf("\nclust_coef:%lf",clust_coef); 
    
    
    clust_coef_tot+=clust_coef;	
  
    
}






/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


void av_path_length()
{


    int i,j,  h,cont,dist;
    int actual[N+1],di[N+1];  
      
      dist_tot=0.;
      
      for (i=1;i<=N;i++)     // para todos los nodos de la red
	{
	  dist=0;    //guardara la suma de las distancias de un nodo a todos los demas

	  for (j=1;j<=N;j++)
	    {
	      di[j]=-100;   //inicializo las banderas de los nodos como "no visitado"
	    }	               //guardara la distancia desde el nodo i a todos los demas


	  cont=1;
	  actual[cont]=i;     //guardo el nodo que estoy mirando en este momento (empiezo por mi)
	  di[i]=0;   //para que si parto de mi, no vuelva a mi mismo de nuevo

	  for(j=1;j<=cont;j++)
	    {	   

	      for(h = 1; h <= k[actual[j]]; h++)     //bucle sobre la conectividad del nodo que estoy mirando
		{
		  if(di[C[actual[j]][h]]==-100)   //si no he visitado el nodo
		    {
		      cont++;
		      actual[cont] = C[actual[j]][h]; 
		      di[C[actual[j]][h]]=di[actual[j]]+1;   //lo marco con 1+ la distancia del nodo actual
		    }
		}
	    }
	  


	  for (j=1;j<=N;j++)
	    {
	      if (di[j]>0)
		{
		  dist+=di[j];
		  //printf("dist:%d\n",dist);
		}
	    }
	  
	  dist_tot+=dist;
	  //printf("dist_tot:%lf\n",dist_tot);
	  
	}// fin del bucle a todos los nodos de la red
      


      dist_tot=dist_tot/((N-1)*N);




printf("\ndist_tot:%lf",dist_tot);

   



}


int find_kmax()
{
    int i, k_maxima;
    
    k_maxima=0;
    
    for(i=1;i<=s;i++)
    {
	if(k[i]>k_maxima)
	    k_maxima=k[i];
    }
    
    printf ("k_max=%d ",k_maxima);
    
    return (k_maxima);
}











void histograma_pk_coop()            //costruccion de P(k) de cooperadores (pero en su k[i] cuentan coop y defect!)
{
    
    int i;
    for(i = 0; i <= N; i++)           
    {
	PK_coop[i]=0.;
	
    }


    for(i=0; i<=s; i++)
    {
	if(e[i]==0)
	    
	    PK_coop[k[i]]++;    
    }
 


}





void randomizar()
{
  int i,w,v;
  double r, norma;
  int nodo1,nodo2,nodo3,nodo4,aux1,aux2;
  int cont,max;
  int ok1,ok2,ok3;
  int fila_de_C,x[N+1];




  norma=n_links; //numero total de links en la red

  cont=1;
  max=2*N;
  while(cont<=max)     //bucle de rewiring
    {
	//printf("cont: %d\n",cont);
	//printf("norma: %d\n",norma);
      ok1=ok2=ok3=1;   //flags de enlaces dobles y consigo mismo

      r=FRANDOM;    //elijo un link
      r=r*norma;
      w=(int)r+1;

      nodo1=D[w][1];
      nodo2=D[w][2];


      r=FRANDOM;     //elijo otro link
      r=r*norma;
      v=(int)r+1;

      nodo3=D[v][1];
      nodo4=D[v][2];

//printf("nodo1:%d nodo2:%d nodo3:%d nodo4:%d\n",nodo1,nodo2,nodo3,nodo4);

      if(nodo1==nodo4  ||   nodo2==nodo3)   //evito enlaces cosigo mismo
        {
          ok1=0;
          // printf("ok1 %d\n",ok1);
        }


      for(i=1;i<=K_MAX;i++)        //evito dobles enlaces
        {
          if (C[nodo1][i]==nodo4)
            {
              ok2=0;
              //  printf("ok2 %d\n",ok2);
              break;
            }
          if(C[nodo3][i]==nodo2)
            {
              ok3=0;
              //printf("ok3 %d\n",ok3);
              break;
            }

        }



      if(ok1==1 && ok2==1 && ok3==1)
        {
          aux1=nodo2;       //intercambio los links
          aux2=nodo4;

          nodo2=aux2;
          nodo4=aux1;


          D[w][2]=nodo2;
          D[v][2]=nodo4;

          cont++;

 }

      if (cont==max)
        {
          check_GC();

          if(GC<N)   //si red disconexa
           {
             max+=100;      //sigo con el rewiring
             printf("Red disconexa, continuo rewiring \n");
           }
        }

    } //fin del rewiring

  //printf("fin rewiring\n");

  //reconstruyo la matriz C (PERO USO PARA ELLO LA MATRIZ D, EN LUGAR DE LA  M!!!)

  for(i=1;i<=N;i++)
    x[i]=1;     //indice para saber en que posicion de la fila i de la matriz C me toca escribir

  for(i=1;i<=2*N;i++)
    {

      if(D[i][1]!=0 && D[i][2]!=0)
        {
          fila_de_C=D[i][1];
          C[fila_de_C][x[fila_de_C]]=D[i][2];
          x[fila_de_C]++;
        }

    }

  //no necesito reconstruir el vector k[i]  pq no se ve afectado




  //    printf("reconstruida matriz C\n");


}



void histograma1()            //construccion de P(k) despues de randomizar
{
    

for(i = 0; i <= N; i++)           //inicializo
     PK_rand[i]=0.;

 for(i =1; i <= N; i++)            //recuento
     PK_rand[k[i]]++;




 for(i = 1; i <= N; i++)         //normalizo
        {
          PK_rand[i] = PK_rand[i]/N;
          PK_rand_tot[i]+=PK_rand[i];      // acumulo        // NO ES +=PK_rand????????????? 
        }


   
/*    for(i =1; i <= N; i++)   
    {
	printf("P[%d]=%f\n",i,PK_rand[i]);   
	}*/
    
    
    
}


void check_GC()     //basado en el algoritmo del average path length: comprobar si es conexa
{
    
    int  label[N+1],j,h,cont,actual[N+1];
    
    for(j=1;j<=N;j++)
    {
	label[j]=0;   //inicializo las banderas de los nodos como "no visitado"
    }
    
    GC=0;
    cont=1;
    actual[1]=1;     //guardo el nodo que estoy mirando en este momento (empiezo por 1) (antes=i)
    
    
    for(j=1;j<=cont;j++)
    {
	
	for(h = 1; h <= k[actual[j]]; h++)     //bucle sobre la conectividad del nodo que estoy mirando
        {
	    if(label[C[actual[j]][h]]==0)   //si no he visitado el nodo
            {
		cont++;
		actual[cont] = C[actual[j]][h];
		label[C[actual[j]][h]]=1;   //lo marco como parte de la componente gigante
            }
        }
    }
    
    
    
    for (j=1;j<=N;j++)
    {
	GC+=label[j];
    }
    printf("\nGC:%d",GC);
    
    // getchar();
    
    /* if(GC==N)            //esto es correcto????
       {
       return (2*N);
       }
       else
       {
       return (2*N-100);
       }*/
    
}



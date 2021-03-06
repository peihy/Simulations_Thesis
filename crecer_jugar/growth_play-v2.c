//Implementacion de una dinamica de juego combinada con el crecimiento de la red por pref.-attach

//CORRECCION en la normalización: diferencia máxima de coef de la matriz de payoff*max k (antes: b*k_m siempre)


# include <stdio.h>
# include <stdlib.h>
# include <math.h>


# define N 5000 //tamaño de la red
# define m_o  2      //nodos inicialmente unidos
# define m   2      //links nuevos añadidos a cada paso de tiempo

# define Niter 100        //estadistica
# define K_MAX   500      //para el tamaño de C[N][K_MAX]     ojo pq depende de N en SF!!!
                          //tambien calculable en la funcion matriz_Conect

# define equilibrio 10000 //PASOS ADICIONALES

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
double PK[N+1], PK_tot[N+1], PKa[N+1];    //  para la distribucion P(k)
double r,v;                     //para guardar los aleatorios
int  II,i,j,jj,g,gg, d, q,y,z, x[N+1], C[N+1][K_MAX+1], steps;
double norma,norma2;
int si[N+1], cont,cont2;

//Variables para el dilema

double b,R,Pu,Su;
int tauT,tauD,tiempo;

int  e[N+1], t, w, k_m; 
double  ben[N+1],p,ro;
int  n_coop; //numero de coop. instantaneos
int iter,counter;

int s, pasos;

float D_med;          //para el average path length
int number,h,l,ultimo,label[N+1],analizar[N+1],D[N+1][N+1];  
double fit[N+1],eps;       

    	

void inicia_rand(int semilla);
void nucleo_inicial();
void jugada();
void nuevo_nodo();
void histograma_pk();
void matriz_Conect();
void av_path_length();

char nombre[256],nombre3[256],file7[256],file8[256],file9[256], nombre4[256];
char file7[256]; 
char file[256];  
char file2[256];

FILE *fich7;                   
FILE *fich; 
FILE *fich2; 


int main()
{
    eps=0.0;
    b=1.40;
    tauT=10;
    tauD=1;

    ro=0.5; 
    R=1.0;
    Pu=0.;
    Su=0.;
  
    sprintf(file7,"%dPk%d-%d-b%.2lf-e%.2lf.dat",N,tauT,tauD,b,eps);        
    fich7=fopen(file7,"wt");
    fclose(fich7);

    sprintf(file,"%dEvolCoop%d-%d-b%.2lf-e%.2lf.dat",N,tauT,tauD,b,eps);        
    fich=fopen(file,"wt");
    fclose(fich);
  
    sprintf(file2,"%dLength%d-%d-b%.2lf-e%.2lf.dat",N,tauT,tauD,b,eps);        
    fich2=fopen(file2,"wt");
    fclose(fich2);

// INICIAliZO:

    steps=N-m_o;       //numero de nodos que se añadiran en total
    s=m_o;         //tamagno actual de la red: s=N(t)
    tiempo=1;       //contador de tiempos para tauT y tauD...
    D_med=0.0;
    n_coop= 0;  
    
    for(i=1;i<=N;i++)      
    {
	ben[i]=0;
	e[i]=1;     //TODOS DEFECTORES      
    }
  
    inicia_rand(time(0));
  
    nucleo_inicial();
//    matriz_Conect();  //hay que actualizarla cada vez para poder jugar
    printf("num coop.: %d  \n",  n_coop);
  

    do{
    
	for(II=1;II<=tauD;II++) 
	{
	    jugada();
	
	    n_coop=0;
	    for(i=1;i<=s;i++)
	    {
		if(e[i]==0)	    
		    n_coop++;	      
	    }
	    fich=fopen(file,"at");
	    fprintf(fich,"%d   %d\n",s,n_coop);      
	    fclose (fich);
      
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
	    
	    s++;	    
	    nuevo_nodo();	      
	    printf("%i   %lf\n",s,norma);	
//	    matriz_Conect();
	}
	
	//printf("num coop.: %d  \n",  n_coop);

	tiempo=tiempo+1; 
 
    }while(s<N);
  
  
    histograma_pk();
    //av_path_length();
  
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

	  //CORRECCION en la normalización: diferencia máxima de coef de la matriz de payoff*max k (antes: b*k_m siempre)

	   if(Su>=0)
	   {
	       p=p/(b*(double)k_m);
	   }
	   else
	   {
	       p=p/((b-Su)*(double)k_m);
	   }
	   

          v=FRANDOM;
          if(v<p)
	  {
	      e[i]=e[t];
	    
	  }
       }

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
	PKa[j]=0.;
    }
  
  for(i =1; i <= N; i++)            //recuento
  {
      PK[k[i]]++;

      for(j=1;j<=k[i];j++)
      {
	  PKa[j]++;
      }
  }

  for(i = 1; i <= N; i++)         //normalizo
    {
      PK[i] = PK[i]/N;    
      PKa[i] = PKa[i]/N;      
     
    }
  
  
  // Escribo k y PK en un archivo 

  fich7=fopen(file7,"at");
  
  for(i = 1; i<N; i++)
    {
      fprintf(fich7,"%d  %lf  %lf\n", i, PK[i],PKa[i]);
    }
  
  fclose (fich7);
}        

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void av_path_length()          	/* Calculo el average path length */
{


  for(i = 1; i <= N; i++)
    {
      for(j = 1; j <= N; j++)
	label[j] = 2*N;
      
      label[i] = 0;
      number = 1;
      analizar[1] = i;
      
      for(j = 1; j <= number; j++)
	{
	  for(h = 1; h <= k[analizar[j]]; h++)
	    {
	      if(label[C[analizar[j]][h]] > N+1)
		{
		  number++;
		  analizar[number] = C[analizar[j]][h];
		  label[C[analizar[j]][h]] = label[analizar[j]] + 1;
		}
	    }
	}
      
      for(j = 1; j <= N; j++)
	{
	  D[i][j] = label[j];
	}
    }
  
  
  
  
  
  
  for(i = 1; i <= N; i++)
    {
      for(j = i+1; j <=N; j++)
	D_med+= D[i][j];
    }
  
  //fin del bucle en iter
  
  
  
  
  //D_med = (2.*D_med)/((N+1)*N*iter);          //????????
  
  
  D_med = (2.*D_med)/((N+1)*N); 
  
  printf("D: %lf\n", D_med); 
  
  
  fich2=fopen(file2,"at");
  fprintf(fich2,"%lf\n", D_med);      
  fclose (fich2);
  
  
  
}

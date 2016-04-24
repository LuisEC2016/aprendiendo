#include <stdio.h>
#include <stdlib.h>  
#include <pthread.h> //libreria de hilos
#include <unistd.h> // para la funcion usleep y sched_yield()
#define Threadmaximo 5 //numero maximo de hilos
#define max_comidas 20 //numero de comidas 
int i, j;
/*estado 1 disponible*/
/*estado 0 no disponible*/
int tenedor[Threadmaximo]={1,1,1,1,1}; 
/* Bloqueos mutex para cada cuchara */
pthread_mutex_t m[Threadmaximo];
/* ID de cada filosofo */ 
int p[Threadmaximo]={0,1,2,3,4}; 
/* Número de comidas consumidas por cada filósofo */
int numComidas[Threadmaximo]={0,0,0,0,0};
/* Contador para el número de comidas */
int comidaCont = 0;
void *filosofos(void *); /* Prototipo de rutina filósofo */
int main(){
	pthread_t tid[Threadmaximo];
	srand((long)time(NULL));
	/* Crear 5 hilos que representan el 5 filósofo comedor */
	for(i=0; i<Threadmaximo; i++){
		if(pthread_create( tid+i, 0,filosofos, p + i ) != 0){ 
	        perror("pthread_create() error al Crear hilo."); 
	        exit(1); 
	    } 
	}
	/*esperar unión de 5 hilos*/
	for (i=0;i<Threadmaximo;i++){ 
        if(!pthread_join(tid[i], NULL)==0){ 
        	perror("falla en el pthread_join() ."); 
            exit(1); 
        } 
    } 
    printf("\n"); 
    /*finalizando el programa*/
    for(i=0;i<Threadmaximo;i++) 
        printf("El Filosofo %d comio %d veces.\n", i, numComidas[i]); 
    printf("\nprincipal(): Los filósofos se han ido. Voy a salir!\n\n"); 
    return (0); 
}
void *filosofos(void  *arg){
	int  sub = *(int*)arg;
	while(comidaCont < max_comidas){
		printf("Filosofo %d: ¡Voy a comer!\n", sub);
		/* Bloquear el tenedor de la izquierda */
		pthread_mutex_lock( m + sub );
		if( tenedor[sub] == 1 ){
			/* La izquierda está disponible */
			printf("Filosofo %d: izquierda=%d\n",sub,tenedor[sub]);  
            printf("Filosofo %d: tengo tenedor!\n", sub);
            tenedor[sub]=0; /**Establecer el tenedor izquierdo no disponible */
            pthread_mutex_unlock( m + sub ); /* Desbloquear el tenedor de la izquierda */
            /* Bloquear el la cuchara de la derecha */
            pthread_mutex_lock(m+((sub+1)%Threadmaximo));
            if( tenedor[(sub+1)%Threadmaximo]==1){
				/* La derecha está disponible */
				printf("filosofo %d: derecho=%d\n", sub,tenedor[(sub+1)%Threadmaximo]);
				/**Establecer la cuchara derecha no disponible */
				tenedor[(sub+1)%Threadmaximo]=0;
				/*Desbloquear la cuchara derecha */
				pthread_mutex_unlock(m+((sub+1)%Threadmaximo));
				printf("Filosofo %d: Tengo un tenedor y una cuchara!\n", sub); 
                printf("Filosofo %d: ¡Estoy comiendo!\n\n", sub);
                numComidas[sub]++;
                comidaCont++;
                usleep(rand() % 3000000); /* tiempo de comer */
                /* Bloquear izquierda y derecha */
                pthread_mutex_lock(m+sub);  
                pthread_mutex_lock(m+((sub+1)%Threadmaximo));
                tenedor[sub]=1; /* Establece el tenedor izquierdo disponible */
                /* Establece la cuchara derecha disponible */
                tenedor[(sub+1)%Threadmaximo]=1;
                /* Desbloquear izquierda y derecha */
                pthread_mutex_unlock ( &m[sub]); 
                pthread_mutex_unlock (&m[(sub+1)%Threadmaximo]);
                usleep(rand() % 3000000); /* Tiempo de establecimiento de alimentos */
            }
            else{/* La derecha no está disponible */
            	printf("Filosofo %d: derecha=%d\n", 
                sub, tenedor[(sub+1)%Threadmaximo]);	 
                printf("Filosofo %d: cuchara ocupada!\n\n", sub);
                /* Desbloquear la cuchara derecha */
                pthread_mutex_unlock(&m[(sub+1)%Threadmaximo]);
                /* Bloquear el tenedor izquierda */
                pthread_mutex_lock(&m[sub]);
                /* Establece el tenedor izquierdo disponible*/
                tenedor[sub]=1;
                /* Desbloquear el tenedor de la izquierda */
                pthread_mutex_unlock(&m[sub]);
                /* Esperar un tiempo y vuelve a intentarlo más tarde */
                usleep(rand() % 3000000); 
            }
		}
		else{
			/* La cuchara no está disponible */
			printf("filosofo %d: derecho=%d\n",sub,tenedor[sub]);
			printf("filosofo %d: No puedo conseguir la cuchara!\n\n",sub);
			/* Desbloquear el tenedor izquierdo */
			pthread_mutex_unlock(&m[sub]);
			/* Esperar un tiempo y vuelve a intentarlo más tarde */
			usleep(rand() % 3000000);
		}
		sched_yield(); /* Para LINUX también puede utilizar pthread_yield (); */
		/*sched_yield () hace que el subproceso de llamada a renunciar a la CPU. los
        hilo se mueve a la final de la cola para su prioridad estática y una
        nuevo hilo se pone a correr*/
	}
	printf("Filosofo %d ha terminado de cenar y se está despidiendo!\n", sub);
	pthread_exit(0);
	//FIN
}
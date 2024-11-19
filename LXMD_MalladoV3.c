/*  Mallado2D con Grafo Dirigido usando GL/glut y Triangulacion de Delaunay

 *  'Grafo de vertices'
 *  Graficación por computadora
 *  03/10/24
 */


/* Fuentes.
    Center and Radius of a Circle from Three Points
https://web.archive.org/web/20161011113446/http://www.abecedarical.com/zenosamples/zs_circle3pts.html

    Equation of a circle passing through 3 points
http://ambrnet.com/TrigoCalc/Circle3D.htm

    Get the equation of a circle when given 3 points
https://math.stackexchange.com/questions/213658/get-the-equation-of-a-circle-when-given-3-points

    Circuncentro de un triángulo
https://www.universoformulas.com/matematicas/geometria/circuncentro-triangulo/

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#define P 40 //percentil default

enum _bool{FALSE,TRUE}BOOL;

/* DEFINICIÓN DE ESTRUCTURAS BASE PARA LOS NODOS */
    typedef struct nodo_vertice
    {   
        int V;
        float x,y;
    }NODO_VERTICE;

    typedef struct lista_adyacencia
    {
        struct nodo_vertice *vertice;
        struct lista_adyacencia *next;
    }L_V;

    typedef struct grafo
    {
        struct nodo_vertice *vertice;
        struct lista_adyacencia *l_v;
        struct grafo *next;
        int T;
    }GRAFO;

    typedef struct lista_criterio
    {
        float d;
        int Vi,Vf;
        struct lista_criterio *next;
    }DISTANCIAS;

    typedef struct lista_criterios2
    {
        float d;
        int Vi,Vf;
        struct lista_criterios2 *next;
    }
    Distancias2;

    /* Estructuras globales para display */
    GRAFO *grafo_head = NULL;
    Distancias2 *distancias2 = NULL;
    int window = 0; //no hay ventana abierta

    /*__________CREACIÓN DE NODOS__________*/
    NODO_VERTICE *crear_vertices(float x, float y, int ID)
    {
        NODO_VERTICE *nodito = malloc(sizeof(NODO_VERTICE));
        if(!nodito) exit(1);
        nodito->V = ID;
        nodito->x=x;
        nodito->y=y;
        return nodito;
    }

    L_V *crear_arista(NODO_VERTICE *nodito)
    {
        L_V *new = malloc(sizeof(L_V));
        if(!new) exit(1);
        new->vertice=nodito;
        new->next=NULL;
        return new;
    }

    GRAFO *lista_adyacencia(NODO_VERTICE *nodito)
    {
        GRAFO *new = malloc(sizeof(GRAFO));
        if(!new) exit(1);
        new->vertice=nodito;
        new->l_v=NULL;
        new->next=NULL;
        new->T++;
        if(nodito->V == 0) new->T = 0;
        return new;
    }

    DISTANCIAS *lista_criterios(float d, int Vf, int Vi)
    {
        DISTANCIAS *nodito = malloc(sizeof(DISTANCIAS));
        if(!nodito) exit(1);
        nodito->d = d;
        nodito->Vf = Vf;
        nodito->Vi = Vi;
        nodito->next = NULL;
        return nodito;
    }

    Distancias2 *criterios2(float d, int Vf, int Vi)
    {
        Distancias2 *nodito = malloc(sizeof(Distancias2));
        if(!nodito) exit(1);
        nodito->d = d;
        nodito->Vf = Vf;
        nodito->Vi = Vi;
        nodito->next = NULL;
        return nodito;
    }

    /*__________CALCULAR DISTANCIA ENTRE DOS PUNTOS__________*/
    float calcular_distancia(NODO_VERTICE *V2 , NODO_VERTICE *V1)
    {
        float d = sqrt( pow((V2->x - V1->x), 2) + pow((V2->y - V1->y),2));
        return d;
    }
    
    /*__________INSERTAR EN LA LISTA 'PRINCIPAL' DE GRAFO__________*/
    void *crear_grafo(NODO_VERTICE *nodito, GRAFO **grafo_head, int ID)
    {
        GRAFO *new = lista_adyacencia(nodito),
              *aux;
        new->T= ID;
        new->next= *grafo_head;
        *grafo_head=new;
    }

    /*__________BUSCAR DISTANCIA ENTRE 2 PUNTOS__________ */
    float buscardistanciaAB(int vA, int vB, DISTANCIAS **head)
    {
        if(!(*head)) return 0;
        else if(!(*head)->d) return 0;
        else if(((*head)->Vi == vA && (*head)->Vf == vB) || ( (*head)->Vi == vB && (*head)->Vf == vA)) return (*head)->d;
        return buscardistanciaAB(vA,vB, &(*head)->next);
    }

    /*__________BUSCAR EN LISTA DE DISTANCIAS__________ */
    int buscar_distancia(DISTANCIAS **head, float d)
    {
        if(!(*head)) return 0;
        else if((*head)->d == d) return 1;
        return buscar_distancia(&(*head)->next, d);
    }

    /*__________CREAR LISTA DE DISTANCIAS__________*/
    void *generar_distancias(GRAFO **head_grafo, DISTANCIAS **head_distancias)
    {
        if((*head_grafo)->next == NULL) return NULL; 
        GRAFO *main = *head_grafo;

        while(main)
        {
            GRAFO *nxt = main->next;
            while(nxt)
            {
                float d = calcular_distancia(main->vertice, nxt->vertice);
                //si no encuentra una distancia igual en la lista
                if(!buscar_distancia(&(*head_distancias), d))
                {
                    DISTANCIAS *new = lista_criterios(d, main->T, nxt->T);
                    new->next = *head_distancias;
                    *head_distancias = new;
                }
                nxt = nxt->next;
            }
            main = main->next;
        }
    }
    
    /*__________ORDENAR LISTA DE DISTANCIAS__________*/
    void ordenar_lista(DISTANCIAS **head_distancias)
    {
        DISTANCIAS *i = *head_distancias, *j, *aux = malloc(sizeof(DISTANCIAS));
        while(i != NULL)
        {
            j = i->next;
            while(j != NULL)
            {
                if(i->d > j->d)
                {   //guardamos i en aux
                    aux->d = i->d;
                    aux->Vf = i->Vf;
                    aux->Vi = i->Vi;
                    //se pasa j a i
                    i->d = j->d;
                    i->Vf = j->Vf;
                    i->Vi = j->Vi;
                    //se pasa aux a j
                    j->d = aux->d;
                    j->Vf = aux->Vf;
                    j->Vi = aux->Vi;
                }
                j = j->next;
            }
            i = i->next;
        }
    }

    /*__CÁLCULO DE PERCENTILES (GRADO DE APROXIMACIÓN DE FORMA 'DINÁMICA')__*/
    float percentil(DISTANCIAS **head_distancias)
    {
        DISTANCIAS *aux; int i=1,j;
        //cuenta la cantidad de distancias que hay en la lista vv
        for(aux = *head_distancias; aux->next != NULL; aux = aux->next) i++;

        float Perc = (P * i)/100,
              valor1,valor2;

        /* saber si tiene punto flotante */
        double redondeov = floor(Perc);
        //si es diferente, tiene punto decimal
        if(Perc != redondeov)
        {
            int c1 = Perc, c2 = Perc + 1;
            //identificar las distancias de esas dos posiciones para el promedio
            for(j=1, aux = *head_distancias; j<c1; j++) aux = aux->next;
            valor1 = aux->d;
            for(j=1, aux = *head_distancias; j<c2; j++) aux = aux->next;
            valor2 = aux->d;

            return (valor1+valor2)/2;
        }
        else //si no es diferente, numero entero 
        {
            for(j=1, aux = *head_distancias; j<Perc; j++) aux = aux->next; 
            return aux->d;
        }
    }

    /*__________BUSCAR VÉRTICE EN EL GRAFO__________*/
    GRAFO *buscar_vertice(GRAFO **head, int ID )
    {
        if(!*head) return NULL;
        else if((*head)->vertice->V == ID) return *head;
        return buscar_vertice(&(*head)->next, ID);
    }

    /*__________BUSCAR NODO EN LISTA DE ADYACENCIA__________*/
    int es_adyacente(L_V **lista, NODO_VERTICE *nodito)
    {
        if(!*lista) return 0;
        else if((*lista)->vertice == nodito) return 1;
        return es_adyacente(&(*lista)->next, nodito);
    }

    /*__________INSERTAR VÉRTICE EN LA LISTA DE ARISTAS DE CADA VÉRTICE__________*/
    void insertar_arista(GRAFO **nodo, L_V *nodo_arista)
    {
        L_V *aux = (*nodo)->l_v;
        if((*nodo)->l_v)
        {
            while(aux)
            {
                if(aux->vertice == nodo_arista->vertice) return;
                aux = aux->next;
            }
            aux = (*nodo)->l_v;
            while(aux->next) aux = aux->next;
            aux->next = nodo_arista;
        }
        else (*nodo)->l_v = nodo_arista;
    }

    /*__________AGREGAR ARISTA SI CUMPLE CON EL CRITERIO__________*/
    void vertice_adyacente(DISTANCIAS **head_distancias, float umbral, GRAFO **head_grafo)
    {
        DISTANCIAS *aux;  NODO_VERTICE *nodito;
        for(aux = *head_distancias; aux->next != NULL; aux = aux->next)
        {
            //¿cumple con el criterio predefinido?
            if(aux->d <= umbral)
            {
                //obtiene la posición de cada nodito :)
                GRAFO *noditoA = buscar_vertice(&(*head_grafo), aux->Vf),
                      *noditoB = buscar_vertice(&(*head_grafo), aux->Vi);
                L_V *Adyacencia_Node = crear_arista(noditoB->vertice),
                    *Adyacencia_Node2 = crear_arista(noditoA->vertice);

                insertar_arista(&noditoA, Adyacencia_Node);
                insertar_arista(&noditoB, Adyacencia_Node2);
            }
        }
    }

    /*___BORRAR NODO ARISTA___*/
    void borrar_nodoarista(L_V **head, NODO_VERTICE *nodito)
    {
        if(!*head) return NULL;
        else if((*head)->vertice == nodito)
        {
            L_V *aux = (*head)->next;
            free(*head);
            *head = aux;
            return;
        }
        return borrar_nodoarista(&(*head)->next, nodito);
    }

    /*___BORRAR NODO GRAFO___*/
    void borrar_nodografo(NODO_VERTICE *nodito, GRAFO **head)
    {
        if(!*head) return NULL;
        else if(!(*head)->vertice) return NULL;
        else if((*head)->vertice == nodito)
        {
            GRAFO *aux = (*head)->next;
            free((*head)->l_v);
            free(*head);
            *head = aux;
            return;
        }
        return borrar_nodografo(&(*head)->next, nodito);
    }

    /*___CHECAR SI EXISTE UN VERTICE EN LA CIRCUNFERENCIA___*/
    int distanciacheck(NODO_VERTICE *A, NODO_VERTICE *B, NODO_VERTICE *C, NODO_VERTICE *O, float radio)
    {
        GRAFO *aux;
        for(aux = grafo_head; aux->next != NULL; aux = aux->next)
        {
            //si no es un vertice del triangulo, checar su distancia
            if(aux->vertice != A && aux->vertice != B && aux->vertice != C && aux->vertice != O)
            {
                //¿la distancia es menor que el radio del circulo?
                //si es menor, está adentro del circulo, hacer flip
                if(calcular_distancia(aux->vertice, O) < radio)
                {
                    GRAFO *p = aux;
                    GRAFO *a = buscar_vertice(&grafo_head, A->V);
                    GRAFO *b = buscar_vertice(&grafo_head, B->V);
                    GRAFO *c = buscar_vertice(&grafo_head, C->V);
                    borrar_conexiones(p, a,b,c);
                    return 1;
                }
            }
        } 
        return 0;
    }

    /* hace todo el 'flip' de cada triangulo, vuelve a unir si cumple el criterio */
    void flip(float criterio, GRAFO *p, GRAFO *A, GRAFO *B, GRAFO *C)
    {
        DISTANCIAS *aux;
        for(aux = distancias2; aux->next != NULL; aux = aux->next)
        {
            //cumple con el criterio?
            if(aux->d <= criterio)
            {
                //de que puntos estamos hablando?
                if(aux->Vi == p->T)
                {
                    if(aux->Vf == A->T)
                    {
                        L_V *ad_node = crear_arista(A->vertice);
                        L_V *ad_node2 = crear_arista(p->vertice);
                        insertar_arista(&p, ad_node);
                        insertar_arista(&A, ad_node2);
                    }
                    else if(aux->Vf == B->T)
                    {
                        L_V *ad_node = crear_arista(B->vertice);
                        L_V *ad_node2 = crear_arista(p->vertice);
                        insertar_arista(&p, ad_node);
                        insertar_arista(&B, ad_node2);
                    }
                    else if(aux->Vf == C->T)
                    {
                        L_V *ad_node = crear_arista(C->vertice);
                        L_V *ad_node2 = crear_arista(p->vertice);
                        insertar_arista(&p, ad_node);
                        insertar_arista(&C, ad_node2);
                    }
                }
                else if(aux->Vi == A->T)
                {
                    if(aux->Vf == B->T)
                    {
                        L_V *ad_node = crear_arista(B->vertice);
                        L_V *ad_node2 = crear_arista(A->vertice);
                        insertar_arista(&A, ad_node);
                        insertar_arista(&B, ad_node2);
                    }
                    else if(aux->Vf == C->T)
                    {
                        L_V *ad_node = crear_arista(C->vertice);
                        L_V *ad_node2 = crear_arista(A->vertice);
                        insertar_arista(&A, ad_node);
                        insertar_arista(&C, ad_node2);
                    }
                }
                else if(aux->Vi == B->T)
                {
                    L_V *ad_node = crear_arista(C->vertice);
                    L_V *ad_node2 = crear_arista(B->vertice);
                    insertar_arista(&B, ad_node);
                    insertar_arista(&C, ad_node2);
                }
            }
        }
    }

    /*___reunir los puntos de acuerdo al nuevo criterio___ */
    void reunir(GRAFO *p, GRAFO *A, GRAFO *B, GRAFO *C)
    {
        float d = calcular_distancia(p->vertice, A->vertice);
        if(!buscar_distancia(&distancias2, d))
        {
           Distancias2 *new = criterios2(d, p->T, A->T);
           new->next = distancias2;
           distancias2 = new;
        }

        d = calcular_distancia(p->vertice, B->vertice);
        if(!buscar_distancia(&distancias2, d))
        {
           Distancias2 *new = criterios2(d, p->T, B->T);
           new->next = distancias2;
           distancias2 = new;
        }

        d = calcular_distancia(p->vertice, C->vertice);
        if(!buscar_distancia(&distancias2, d))
        {
           Distancias2 *new = criterios2(d, p->T, C->T);
           new->next = distancias2;
           distancias2 = new;
        }

        d = calcular_distancia(A->vertice, B->vertice);
        if(!buscar_distancia(&distancias2, d))
        {
           Distancias2 *new = criterios2(d, A->T, B->T);
           new->next = distancias2;
           distancias2 = new;
        }

        d = calcular_distancia(A->vertice, C->vertice);
        if(!buscar_distancia(&distancias2, d))
        {
           Distancias2 *new = criterios2(d, A->T, C->T);
           new->next = distancias2;
           distancias2 = new;
        }

        d = calcular_distancia(B->vertice, C->vertice);
        if(!buscar_distancia(&distancias2, d))
        {
           Distancias2 *new = criterios2(d, B->T, C->T);
           new->next = distancias2;
           distancias2 = new;
        }

        ordenar_lista(&distancias2);
        float c = percentil(&distancias2);
        flip(c,p,A,B,C);
    }

    /*___borrar todas las conexiones si hay un vertice en circunferencia___*/
    void borrar_conexiones(GRAFO *p, GRAFO *A, GRAFO *B, GRAFO *C)
    {
        //conexiones con A
        borrar_nodoarista(&A->l_v, p->vertice);
        borrar_nodoarista(&A->l_v, B->vertice);
        borrar_nodoarista(&A->l_v, C->vertice);

        //conexiones con B
        borrar_nodoarista(&B->l_v, p->vertice);
        borrar_nodoarista(&B->l_v, A->vertice);
        borrar_nodoarista(&B->l_v, C->vertice);

        //conexiones con C
        borrar_nodoarista(&C->l_v, p->vertice);
        borrar_nodoarista(&C->l_v, A->vertice);
        borrar_nodoarista(&C->l_v, B->vertice);

        //conexiones con P
        borrar_nodoarista(&p->l_v, C->vertice);
        borrar_nodoarista(&p->l_v, A->vertice);
        borrar_nodoarista(&p->l_v, B->vertice);

        /* sacar nuevo 'criterio' y volver a unir pero en ese rango y vertices*/

        reunir(p,A,B,C); //reunir todos los puntos

    }
    
    /*__________CONTAR TRIANGULOS (SI HAY CONEXIÓN)__________*/
    int contar_triangulos(GRAFO **head, DISTANCIAS **head_distancia) 
    {
        int tri = 0; float A,B,C,s,R, M11,M12,M13, x0, y0;
        GRAFO *main = *head; L_V *ady1, *ady2, *ady3;
        //si existe el vertice, buscar en los siguientes vertices quienes dan la "vuelta"
        while(main) //porque será el vertice "principal que recorrerá"
        {
            if(main->l_v) ady1 = main->l_v;

            while(ady1)
            {
                if(!ady1->vertice) break;
                GRAFO *v1 = buscar_vertice(head, ady1->vertice->V);

                if(v1->l_v) ady2 = v1->l_v;

                while(ady2)
                {
                    if(!ady2->vertice) break;
                    if(ady2->vertice->V != main->vertice->V)
                    {
                        GRAFO *v2 = buscar_vertice(head, ady2->vertice->V);
                        if(v2->l_v) ady3 = v2->l_v;
                        while(ady3)
                        {
                            if(!ady3->vertice) break;
                            if(ady3->vertice->V == main->vertice->V)
                            {
                                //existe un triangulo porque dió la "vuelta"
                                // saca la distancia de cada lado del triangulo :)
                                A = buscardistanciaAB(main->T, v1->T, &(*head_distancia));
                                B = buscardistanciaAB(v1->T, v2->T ,&(*head_distancia));
                                C = buscardistanciaAB(v2->T, main->T,&(*head_distancia));
                                s = (A+B+C)/2; //semiperimetro
                                R = (A*B*C) / (4 * sqrt(s * (s-A) * (s-B) * (s-C))); //radio
                                //radio será nuestro criterio para averiguar los puntos que están dentro de la circunferencia :)

                                // para las coordenadas del circucentro:
                                M11 = (main->vertice->x * (v1->vertice->y - v2->vertice->y)) -   (main->vertice->y * (v1 ->vertice->x - v2->vertice->x)) + (v1->vertice->x * v2->vertice->y) - (v2->vertice->x * v1->vertice->y);

                                M12 = ( (pow(main->vertice->x,2) + pow(main->vertice->y,2) ) * (v1->vertice->y - v2->vertice->y) +  ( pow(v1->vertice->x,2) + pow(v1->vertice->y,2)) * (v2->vertice->y - main->vertice->y) + ( pow(v2->vertice->x,2) + pow(v2->vertice->y,2) ) * (main->vertice->y - v1->vertice->y) ) ;

                                M13 = ( (pow(main->vertice->x,2) + pow(main->vertice->y,2) ) * (v2->vertice->x - v1->vertice->x) +  ( pow(v1->vertice->x,2) + pow(v1->vertice->y,2)) * (main->vertice->x - v2->vertice->x) + ( pow(v2->vertice->x,2) + pow(v2->vertice->y,2) ) * (v1->vertice->x - main->vertice->x) ) ;

                                // coordenadas de circucentro
                                x0 = 0.5 * (M12/M11);
                                y0 = 0.5 * (M13/M11);

                                // inserta circucentro en el grafo
                                NODO_VERTICE *node = crear_vertices(x0,y0, 0);
                                GRAFO *new = lista_adyacencia(node);
                                //insertar al final con ID 0
                                GRAFO *aux = *head;
                                while(aux)
                                {
                                    if(!aux->vertice->V == 0)
                                    {
                                            aux = new;
                                            break;
                                    }
                                    aux = aux->next;
                                }
                                //calcular todas las distancias desde el circucentro excepto los vertices que forman el triangulo, si una distancia es menor o igual a el radio, está dentro del circulo

                                //checa si existe un vertice en la circunferencia, si hay, hace 'flip'
                                NODO_VERTICE *aa = main->vertice, *bb = v1->vertice, *cc = v2->vertice;
                                if(distanciacheck(aa, bb, cc, node, R))
                                {
                                    ady3 = v2->l_v;
                                    ady2 = v1->l_v;
                                    ady1 = main->l_v;
                                    
                                    continue;

                                    /* NOTA
                                     * por ahora se descartó el uso de recursión
                                     * ya que se sigue testeando debido a
                                     * errores de segmentation fault.
                                     * 
                                     * se sigue trabajando en ello ;)
                                     * - LXMD
                                     */

                                    //contar_triangulos(&grafo_head, &(*head_distancia));vuelve a checar entre todo con todos otra vez hasta que todo esté bien :)2
                                }
                                tri++; //si lo cuenta como triangulo
                                break;
                            }
                            if(ady3) ady3 = ady3->next;
                        }
                    }
                    if(ady2) ady2 = ady2->next;
                }
                if(ady1) ady1 = ady1 ->next;
            }     
            main = main->next;
        }
        return tri/6;
    }

    void imprimir_grafo(GRAFO *grafo_head)
    {
        GRAFO *actuel = grafo_head;
        system("cls || clear");
        puts("~*============={            }===============*~");
        while(actuel)
        {
            printf("[v%d]: ", actuel->vertice->V, actuel->vertice->x, actuel->vertice->y);
            //imprimir la vista de adyacencia  [aristas] de ese nodo
            L_V *aristas = actuel->l_v;
            while(aristas)
            {
                printf(" v%d (%g,%g) ->", aristas->vertice->V, aristas->vertice->x, aristas->vertice->y);
                aristas = aristas->next;
            }
            printf("NULL\n");
            actuel = actuel->next; //avanzar
        }
        puts("~*<>><<>><<>><<>><<>><<>><<>><<>><<>><<><<<>*~");
    }
  
    static void init()
    {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glViewport(0,0,500,500);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 1.0, 0.0, 0.0, 0.0, 0.0);
    }

    /*__________DIBUJAR__________ */
    static void lines()
    {   //limpiar el buffer de color y profundidad
        glClear(GL_COLOR_BUFFER_BIT);
        glScalef(0.3, 0.3, 0.3);
        
        // para el plano :)
        glLineWidth(3.0);
        glColor3f(0.8, 0.8, 0.8);

        glBegin(GL_LINES);
        {
            glVertex2f(0.0, 15.0);
            glVertex2f(0.0, -15.0);
            glVertex2f(15.0, 0.0);
            glVertex2f(-15.0, 0.0);
        }
        glEnd();
        
        // para mostrar guias de cada eje 
        // just pa' que se vea bonito :)
        glLineWidth(3.0);
        glColor3f(0.8, 0.8, 0.8);
        glBegin(GL_LINES);
        int J=0, I=0;
            for(;I<=15 && J<=0; I++, J--)
            {
                glVertex2f(I, 0.5);
                glVertex2f(I, -0.5);

                glVertex2f(J, 0.5);
                glVertex2f(J, -0.5);

                glVertex2f(0.5, I);
                glVertex2f(-0.5, I);

                glVertex2f(0.5, J);
                glVertex2f(-0.5, J);
            }
        glEnd();

        //dibujar puntos
        glColor3f(1.0, 0.0, 0.0);
        glPointSize(5.0);
        

        glBegin(GL_POINTS);
            GRAFO *k = grafo_head;
            while(k)
            {
                glVertex2f(k->vertice->x, k->vertice->y);
                k = k->next;
            }
        glEnd();

        //dibjujar lineas
        glColor3f(0.5, 0.2, 0.5);
        glLineWidth(3.0); //lineas conectadas
        //mostrar la unión de los vértices
        glBegin(GL_LINES);  //para lineas conectadas

            GRAFO *i = grafo_head;
            while(i)
            {
                L_V *j = i->l_v;
                while(j)
                {
                    glVertex2f(i->vertice->x, i->vertice->y);
                    glVertex2f(j->vertice->x, j->vertice->y); 
                    j = j->next;
                }
                i = i->next;
            }
        glEnd(); 
        //glFlush();
        glutSwapBuffers();
    }

    /* ___MANEJAR ENTRADA POR TECLADO___ */
    static void control_teclado(unsigned char key, int x, int y)
    {
        if(key == 13) //para el enter :)
        {
            glutDestroyWindow(glutGetWindow());
            window = 0; // Marcar que la ventana gráfica ya no está activa
        }
    }
    
    /*Menú interactivo ^^ */
    void menu(enum _bool BOOL)
    {   
        system("cls || clear");
        puts("╔═.✵.════════════════════════════════════════╗");
        puts("\t          -ˋˏ ༻✿༺ ˎˊ-");
        puts("\t         .Mallado2D v3.");
        if(!BOOL){//<- si NO hay entrada de vértices de antemano
            puts("⤷ [1]. Crear Vertices");
            puts("⤷ [5]. About");
            puts("⤷ [0]. Salir del programa");
            puts("╚════════════════════════════════════════.✵.═╝");
            printf("\tOpcion: \n>");
        } 
        else{  //no mostrar si no hay triángulos creados todavía  ;))
            puts("[1]. Crear mas vertices");
            puts("[2]. Triangulos Creados");
            puts("[3]. Ver adyacencias y coordenadas");
            puts("[4]. Mostrar graficamente");
            puts("[5]. About");
            puts("[0]. Salir del programa");
            //puts("~*<>><<>><<>><<>><<>><<>><<>><<>><<>><<><<<>*~");
            puts("╚════════════════════════════════════════.✵.═╝");
            printf("\tOpcion: \n>");
        }   
    }

    void pause()
    {
        puts("Pulse ENTER para regresar al menu");
        getchar();
        getchar();
    }

    void about()
    {
        system("cls || clear");
        puts("╔═.✵.════════════════════════════════════════╗");
        puts("\t          -ˋˏ ༻✿༺ ˎˊ-");
        puts("\t         .Mallado2D v3.");
        puts("\t©       Denisse ®");
        puts("\t       █║▌│█│║▌║││█║▌║▌║");
        puts("\t\"Graficacion por Computadora\"");
        puts("╚════════════════════════════════════════.✵.═╝");
    }

    void warning()
    {
        puts("\n[W A R N I N G]\nNo se pueden generar triangulos ya que NO hay vertices suficientes");
        puts("CONSEJO: Agregar otros vertices :)\n");   
    }

int main(int argc, char **argv)
{   
    DISTANCIAS *distancias_head = NULL;
    int op, vertices, ID = 0, j = 0, triangle= 0;
    float x,y, c;
    BOOL = FALSE;
    glutInit(&argc, argv);
    do
    {
        do
        {   menu(BOOL);
            scanf("%d", &op);
        }while(op != 1 && op != 2 && op != 3 && op != 4 && op !=5 && op != 0);

        switch(op)
        {     
            case 1:
                system("cls || clear");
                puts("~*============={            }===============*~");
                printf("       ¿Cuantos vertices desea crear?\n>");
                scanf("%d", &vertices);

                for(int i=0; i<vertices; i++, j++)
                {   system("cls || clear");
                    puts("~*==============={            }=================*~");
                    printf("Ingrese las coordenadas (x,y) para su Vertice %d\n", j+1);
                    printf("x = "); scanf("%f", &x);
                    printf("y = "); scanf("%f", &y);
                    //agregar todos los vertices en la lista para el grafo 
                    NODO_VERTICE *node = crear_vertices(x,y, ++ID);
                    crear_grafo(node, &grafo_head, ID);
                }
                puts("Todos los vertices han sido agregados en el plano de forma exitosa!");
                
                //crear lista de distancias, después lo ordena  y crea criterio
                generar_distancias(&grafo_head, &distancias_head);

                if(ID>=3) BOOL = TRUE; // hay vertices suficientes
                else warning(); // no hay :c
                
                if(BOOL) //<- si existe entrada exitosa de coordenadas sin warning
                {
                    ordenar_lista(&distancias_head);
                    c = percentil(&distancias_head);

                    /* checar cuales vértices son los que son <= que el criterio,
                    agregarlos a lista de vertices y crear lista de adyacencia */
                    vertice_adyacente(&distancias_head, c, &grafo_head);
                    //hay triangulos?
                    triangle = contar_triangulos(&grafo_head, &distancias_head);
                }
                pause();
                break;
            case 2:
                    system("cls || clear");
                    puts("~*============={            }===============*~");
                    printf("HAY %d TRIANGULO(S)\n", triangle);
                    puts("~*<>><<>><<>><<>><<>><<>><<>><<>><<>><<><<<>*~");
                    pause();
                break;
            case 3: imprimir_grafo(grafo_head);
                    pause();
                break;
            case 4: 
                    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
                    glutInitWindowSize(600, 600); 
                    glutInitWindowPosition(10,50);
                    glutCreateWindow("mes triangles");
                    init();
                   
                    window = 1;
                    glutDisplayFunc(lines);
                    glutKeyboardFunc(control_teclado);
                    while(window)
                    {
                        glutMainLoopEvent();
                    }
                    //glutMainLoop();
                    
                break;
            case 5: about();
                    pause();
                break;
            default: puts("Au revoir c:");
        }
    }while(op);

    return 0;
}
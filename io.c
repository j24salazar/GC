#include "definitions.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>

extern object3d * _first_object;
extern object3d * _selected_object;

extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;
elm_matriz *auxi;
int ToRoE,LoG,camara,modo_camara;


/**
 * @brief This function just prints information about the use
 * of the keys
 */
void print_Matrix(double *m)
{
    int i; 
    for (i=0;i<4;i++)printf("%f, %f, %f, %f\n",m[i],m[i+4],m[i+8],m[i+12]);

}

void print_help(){
    printf("KbG Irakasgaiaren Praktika. Programa honek 3D objektuak \n");
    printf("aldatzen eta bistaratzen ditu.  \n\n");
    printf("\n\n");
    printf("FUNTZIO NAGUSIAK \n");
    printf("<?>\t\t Laguntza hau bistaratu \n");
    printf("<ESC>\t\t Programatik irten \n");
    printf("<F>\t\t Objektua bat kargatu\n");
    printf("<TAB>\t\t Kargaturiko objektuen artean bat hautatu\n");
    printf("<DEL>\t\t Hautatutako objektua ezabatu\n");
    printf("<CTRL + ->\t Bistaratze-eremua handitu\n");
    printf("<CTRL + +>\t Bistaratze-eremua txikitu\n");
    printf("Todas estas funciones se direjen atraves de las flechas <-^->:\n");
    printf("<l,L>\tMover,Escalar,Rotar sobre el propio objeto\n");
    printf("<W,w>\tMover,Escalar,Rotar sobre los ejes\n");
    printf("<av pag>\tMover,Escalar,Rotar sobre el eje z a menos\n");
    printf("<re pag>\tMover,Escalar,Rotar sobre el eje z a más\n");
    printf("<m,M>\tMover(trasladar) el odjeto(l,L) o sobre el eje(g,G)\n");
    printf("<b,B>\t`Biraje´ sobre el propio objeto(l,L) o ejes (g,G)\n");
    printf("<t,T>\t Tamaño sobre el propio objeto(l,L) o sobre los ejes(g,G)\n");
    printf("<+>\t Escala  en todo el objeto + \n");
    printf("<->\t Escala en todo el objeto - \n");

    printf("\n\n");
}

/**
 * deletea el objeto eliminando toda la memoria reservada
 * pasando por la dos tablas la de vertices y la de caras
 * */
void destroy_obj(object3d *obj)
{
    int i; 
    elm_matriz *matroz;
    for (i = 0; i+1 < obj->num_faces; i++)
    {
       free(obj->face_table[i].vertex_table); 
    }  
    while (obj->pMptr!= 0)
    {

        matroz=obj->pMptr;

        obj->pMptr=obj->pMptr->mptr;
    
        free(matroz);
    }
    free (obj->face_table);
    free(obj->vertex_table);
}
void matrix_inv(elm_matriz *a,elm_matriz *b) // a->Matriz[16] = Inverse(b16])
{
    GLdouble x,y,z;
    // transpose of rotation matrix
    a->Matriz[ 0]=b->Matriz[ 0];
    a->Matriz[ 5]=b->Matriz[ 5];
    a->Matriz[10]=b->Matriz[10];
    x=b->Matriz[1]; a->Matriz[1]=b->Matriz[4]; a->Matriz[4]=x;
    x=b->Matriz[2]; a->Matriz[2]=b->Matriz[8]; a->Matriz[8]=x;
    x=b->Matriz[6]; a->Matriz[6]=b->Matriz[9]; a->Matriz[9]=x;
    // copy projection part
    a->Matriz[ 3]=b->Matriz[ 3];
    a->Matriz[ 7]=b->Matriz[ 7];
    a->Matriz[11]=b->Matriz[11];
    a->Matriz[15]=b->Matriz[15];
    // convert origin: new_pos = - new_rotation_matrix * old_pos
    x=(a->Matriz[ 0]*b->Matriz[12])+(a->Matriz[ 4]*b->Matriz[13])+(a->Matriz[ 8]*b->Matriz[14]);
    y=(a->Matriz[ 1]*b->Matriz[12])+(a->Matriz[ 5]*b->Matriz[13])+(a->Matriz[ 9]*b->Matriz[14]);
    z=(a->Matriz[ 2]*b->Matriz[12])+(a->Matriz[ 6]*b->Matriz[13])+(a->Matriz[10]*b->Matriz[14]);
    a->Matriz[12]=-x;
    a->Matriz[13]=-y;
    a->Matriz[14]=-z;
    print_Matrix(b->Matriz);
    printf("la inversa:\n\n");
    print_Matrix(a->Matriz);
}
/**
 * @brief Callback function to control the basic keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */

//Meter codigo y corregir 2


//Al principio a cada objeto le asociamos la matriz I, tras realizar una transformacion la matriz asociada al objeto pasara a ser M
//y asi sucesivamente por cada transformacion que hagamos, todas estas matrices de transformacion las guardaremos en una lista de matrices
//y gracias a esto si queremos desacer una transformacion simplemente eliminamos la ultima matriz de asociacion.
void keyboard(unsigned char key, int x, int y) {

    char* fname = malloc(sizeof (char)*128); /* Note that scanf adds a null character at the end of the vector*/
    int read = 0, i;
    object3d *auxiliar_object = 0;
    GLdouble wd,he,midx,midy;
    elm_matriz *mptrr;

    switch (key) {
    case 'f':
    case 'F':
        /*Ask for file*/
        printf("%s", KG_MSSG_SELECT_FILE);
        scanf("%s", fname);
        /*Allocate memory for the structure and read the file*/
        auxiliar_object = (object3d *) malloc(sizeof (object3d));
        read = read_wavefront(fname, auxiliar_object);
        switch (read) {
        /*Errors in the reading*/
        case 1:
            printf("%s: %s\n", fname, KG_MSSG_FILENOTFOUND);
            break;
        case 2:
            printf("%s: %s\n", fname, KG_MSSG_INVALIDFILE);
            break;
        case 3:
            printf("%s: %s\n", fname, KG_MSSG_EMPTYFILE);
            break;
        /*Read OK*/
        case 0:
            /*Insert the new object in the list*/
            auxiliar_object->next = _first_object;
            _first_object = auxiliar_object;
            _selected_object = _first_object;
            mptrr= (elm_matriz*) malloc(sizeof(elm_matriz));
            mptrr->mptr = 0;
            for (i = 1; i < 15; i++)
            {
                mptrr->Matriz[i]=0;
            }
                mptrr->Matriz[0]= 1;
                mptrr->Matriz[5]= 1;
                mptrr->Matriz[10]= 1;
                mptrr->Matriz[15]= 1;
                
                
                _selected_object->pMptr = mptrr;
                _selected_object->inv = mptrr;
                ToRoE=0;
                LoG= 0;            
            printf("%s\n",KG_MSSG_FILEREAD);
            break;
        }
        break;
    //Corregir
    
    case 9: /* <TAB> */
        //Cuidado por que en la primera inicializacion no hay ningun next por lo que no tiene nada donde apuntar 
        if(_selected_object !=0) _selected_object = _selected_object->next;
        /*The selection is circular, thus if we move out of the list we go back to the first element*/
        
        if(_selected_object == 0) _selected_object = _first_object;
        break;

    case 127: /* <SUPR> */
        /*Erasing an object depends on whether it is the first one or not*/
          if(_selected_object ==0);
        else if (_selected_object == _first_object)
        {
            /*To remove the first object we just set the first as the current's next*/
            _first_object = _first_object->next;
            /*Once updated the pointer to the first object it is save to free the memory*/
            destroy_obj(_selected_object);
            free(_selected_object);
            
            /*Finally, set the selected to the new first one*/
            _selected_object = _first_object;
        } else {
            /*In this case we need to get the previous element to the one we want to erase*/
            auxiliar_object = _first_object;
            while (auxiliar_object->next != _selected_object)
                auxiliar_object = auxiliar_object->next;
            /*Now we bypass the element to erase*/
            auxiliar_object->next = _selected_object->next;
            /*free the memory*/
            destroy_obj(_selected_object);
            free(_selected_object);
            /*and update the selection*/
            _selected_object = auxiliar_object;
        }

    break;
    case '+':
        if (glutGetModifiers() == GLUT_ACTIVE_CTRL){

            /*Increase the projection plane; compute the new dimensions*/
            wd=(_ortho_x_max-_ortho_x_min)/KG_STEP_ZOOM;
            he=(_ortho_y_max-_ortho_y_min)/KG_STEP_ZOOM;
            /*In order to avoid moving the center of the plane, we get its coordinates*/
            midx = (_ortho_x_max+_ortho_x_min)*2;
            midy = (_ortho_y_max+_ortho_y_min)*2;
            /*The the new limits are set, keeping the center of the plane*/
            _ortho_x_max = midx + wd/2;
            _ortho_x_min = midx - wd/2;
            _ortho_y_max = midy + he/2;
            _ortho_y_min = midy - he/2;
        }
        else
        {
        glLoadMatrixd(_selected_object->pMptr->Matriz);
        glScaled(1.1,1.1,1.1);
        glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->pMptr->Matriz);    
        }        
        break;

    case '-':
        //INPLEMENTA EZAZU CTRL + + KONBINAZIOAREN FUNTZIOANLITATEA
        if (glutGetModifiers() == GLUT_ACTIVE_CTRL){

            /*Increase the projection plane; compute the new dimensions*/
            wd=(_ortho_x_max-_ortho_x_min)*KG_STEP_ZOOM;
            he=(_ortho_y_max-_ortho_y_min)*KG_STEP_ZOOM;
            /*In order to avoid moving the center of the plane, we get its coordinates*/
            midx = (_ortho_x_max+_ortho_x_min)/2;
            midy = (_ortho_y_max+_ortho_y_min)/2;
            /*The the new limits are set, keeping the center of the plane*/
            _ortho_x_max = midx + wd/2;
            _ortho_x_min = midx - wd/2;
            _ortho_y_max = midy + he/2;
            _ortho_y_min = midy - he/2;
        } 
        else
        {
        glLoadMatrixd(_selected_object->pMptr->Matriz);
        glScaled(0.9,0.9,0.9);
        glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->pMptr->Matriz);
        }
            
        break;

    case '?':
        print_help();
        break;

    case 27: /* <ESC> */
        exit(0);
        break;
    case 26:
        if(_first_object!=0){
        if (_selected_object->pMptr->mptr != 0)
        {
        free(_selected_object->pMptr->Matriz);
        _selected_object->pMptr=_selected_object->pMptr->mptr;
        }
        else
        {
            printf("Es más atrras y rompes el propio espacio-tiempo\n");
        }
        }
        
        break;
    case 'k':
    case 'K':
    if(modo_camara==0)modo_camara=1;
    else modo_camara=0;
    break;
    case 'M':
    case 'm':
        ToRoE = 0;
        break;
    case 'g':
    case 'G':
        if (camara==0)camara=1;
        else camara=0;
    break;
    case 'B':
    case 'b':
        ToRoE = 1;
    break;
    case 'T':
    case 't':
        ToRoE = 2;
    break;
    case 'L':
    case 'l':
        LoG = 0;
    break;
    case 'W':
    case 'w':
        LoG = 1;
    break;
    default:
        /*In the default case we just print the code of the key. This is usefull to define new cases*/
        printf("%d %c\n", key, key);
    }
    /*In case we have do any modification affecting the displaying of the object, we redraw them*/
    
    glutPostRedisplay();
    free(fname);
    
}
void guardar()//esta funcion guarda la matriz en mptr
{
    elm_matriz * aux;
    elm_matriz *guardado;
    guardado = (elm_matriz *)malloc(sizeof(elm_matriz));
    glGetDoublev(GL_MODELVIEW_MATRIX, guardado->Matriz);
    aux =_selected_object->pMptr;
    _selected_object->pMptr = guardado;
    guardado->mptr = aux;
}
void global(double x, double y)//traslada,rota,escala respecto a los ejes 
{
    glLoadMatrixd(_selected_object->pMptr->Matriz);
    guardar();
    glLoadIdentity();
    switch (ToRoE)
    {
    case 0:
        glTranslated(x, y,0);
        break;
    case 1:
        glRotated(10.0,x,y,0);
        break;

    case 2:
        glScaled(x, y,1);
        break;
    }

        glMultMatrixd(_selected_object->pMptr->Matriz);
        glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->pMptr->Matriz);
        glLoadMatrixd(_selected_object->pMptr->Matriz);
}
void modoObj(int Tecla,int camara){//esta funcion sibe para rotar trasladar.. objetos y camaras
    int x,y,z;
    if(camara==0)glLoadMatrixd(_selected_object->pMptr->Matriz);
   //else glLoadMatrixd("camara")
    guardar();
    if(LoG==1 && camara==0)glLoadIdentity();
    
    switch (Tecla) {
        case GLUT_KEY_UP :
        printf("hola\n");
        x=0;y=1;z=0;
        break;
        case GLUT_KEY_DOWN :
        x=0;y=-1;z=0;
        break;
        case GLUT_KEY_LEFT :
        x=-1;y=0;z=0;
        break;
        case GLUT_KEY_RIGHT :
        x=1;y=0;z=0;
        break;
        case GLUT_KEY_PAGE_DOWN:
        x=0;y=0;z=-1;
        break;
        case GLUT_KEY_PAGE_UP:
        x=0;y=0;z=1;
        break;
        }
    switch (ToRoE)
    {
        case 0: //Translacion
            glTranslated(x,y,z);
        break;
        case 1: //Rotacion
            glRotated(10,-y,-x,z);
        break;
        case 2: //Escalado
            glScaled(1+x*0.1,1+y*0.1,1+z*0.1);
        break;
        
    }
    if(camara==0){
    if(LoG==1)glMultMatrixd(_selected_object->pMptr->Matriz);
    glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->pMptr->Matriz);
    glLoadMatrixd(_selected_object->pMptr->Matriz);
    }
    else{//guardas esta matriz en la camara
    }
}
void modoCamara(int Tecla){


}

void special(int k, int x, int y) {
    if(_first_object != 0){
    if(k == 114);
    else{
   switch (modo_camara)
    {
    case 0:
        modoObj(k,0);
    break;
    case 1:
        modoCamara(k);
    break;
    }
    }
   }
   glutPostRedisplay();
    
    
    free(_selected_object->inv);   
   auxi = (elm_matriz *)malloc(sizeof(elm_matriz));
    matrix_inv(auxi,_selected_object->pMptr);
    _selected_object->inv=auxi;

}

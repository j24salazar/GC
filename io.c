#include "definitions.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>

extern object3d * _first_object;
extern object3d * _selected_object;

extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;
int ToRoE,LoG;


/**
 * @brief This function just prints information about the use
 * of the keys
 */
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
    printf("\n\n");
}
/**
 * deletea el objeto eliminando toda la memoria reservada
 * pasando por la dos tablas la de vertices y la de caras
 * */
void destroy_obj(object3d *obj)
{
    int i; 
    for (i = 0; i < obj->num_faces; i++)
    {
       free(obj->face_table[i].vertex_table); 
    }
    free (obj->face_table);
    free(obj->vertex_table);    
   
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
    elm_matriz *mptr;

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
            mptr= (elm_matriz*) malloc(sizeof(elm_matriz));
            for (i = 1; i < 15; i++)
            {
                mptr->Matriz[i]=0;
            }
                mptr->Matriz[0]= 1;
                mptr->Matriz[5]= 1;
                mptr->Matriz[10]= 1;
                mptr->Matriz[15]= 1;
                
                
                _selected_object->pMptr = mptr;
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
            free(_selected_object->pMptr->Matriz);
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
            free(_selected_object->pMptr->Matriz);
            free(_selected_object);
            /*and update the selection*/
            _selected_object = auxiliar_object;
        }
        break;

    break;
    case '+':
        if (glutGetModifiers() == GLUT_ACTIVE_CTRL){

            /*Increase the projection plane; compute the new dimensions*/
            wd=(_ortho_x_max-_ortho_x_min)/KG_STEP_ZOOM;
            he=(_ortho_y_max-_ortho_y_min)/KG_STEP_ZOOM;
            /*In order to avoid moving the center of the plane, we get its coordinates*/
            midx = (_ortho_x_max+_ortho_x_min)/2;
            midy = (_ortho_y_max+_ortho_y_min)/2;
            /*The the new limits are set, keeping the center of the plane*/
            _ortho_x_max = midx + wd/2;
            _ortho_x_min = midx - wd/2;
            _ortho_y_max = midy + he/2;
            _ortho_y_min = midy - he/2;
        }
        break;

    case '-':
        //INPLEMENTA EZAZU CTRL + + KONBINAZIOAREN FUNTZIOANLITATEA
        if (glutGetModifiers() == GLUT_ACTIVE_CTRL){

            /*Increase the projection plane; compute the new dimensions*/
            wd=(_ortho_x_max-_ortho_x_min)*KG_STEP_ZOOM;
            he=(_ortho_y_max-_ortho_y_min)*KG_STEP_ZOOM;
            /*In order to avoid moving the center of the plane, we get its coordinates*/
            midx = (_ortho_x_max+_ortho_x_min)*2;
            midy = (_ortho_y_max+_ortho_y_min)*2;
            /*The the new limits are set, keeping the center of the plane*/
            _ortho_x_max = midx + wd/2;
            _ortho_x_min = midx - wd/2;
            _ortho_y_max = midy + he/2;
            _ortho_y_min = midy - he/2;
        }
        break;

    case '?':
        print_help();
        break;

    case 27: /* <ESC> */
        exit(0);
        break;
    case 26:
        if (_selected_object->pMptr->mptr != 0)
        {
        free(_selected_object->pMptr->Matriz);
        _selected_object->pMptr=_selected_object->pMptr->mptr;
        }
        else
        {
            printf("tranqui \n");
        }
        
        break;
    case 'T':
    case 't':
        ToRoE = 0;
        break;
    case 'R':
    case 'r':
        ToRoE = 1;
        break;
    case 'E':
    case 'e':
        ToRoE = 2;
        break;
    case 'L':
    case 'l':
        LoG = 0;
        break;
    case 'G':
    case 'g':
        LoG = 1;
        break;
    default:
        /*In the default case we just print the code of the key. This is usefull to define new cases*/
        printf("%d %c\n", key, key);
    }
    /*In case we have do any modification affecting the displaying of the object, we redraw them*/
    glutPostRedisplay();
}
void guardar(){
    elm_matriz * aux;
    elm_matriz *guardado;
    guardado = (elm_matriz *)malloc(sizeof(elm_matriz));
    printf("guardado\n");
    glGetDoublev(GL_MODELVIEW_MATRIX, guardado->Matriz);
    aux =_selected_object->pMptr;
    _selected_object->pMptr = guardado;
    _selected_object->pMptr->mptr = aux;
}
void global(double x, double y)
{

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

        glutPostRedisplay();
}

void special(int k, int x, int y) {
   switch (k) {
    case GLUT_KEY_UP :
    switch (LoG)
        {
        case 0:
            glLoadMatrixd(_selected_object->pMptr->Matriz);
            guardar();
            switch (ToRoE)
            {
            case 0: //Translacion
                glTranslated(0,1,0);
                break;
            case 1: //Rotacion
                glRotated(10,-1,0,0);
                break;
            case 2: //Escalado
                glScaled(1,1.1,1);
                break;
            }
            glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->pMptr->Matriz);
            break;
        case 1:

            switch (ToRoE)
            {
            case 0: //Translacion
                global(0,1);
                break;
            case 1: //Rotacion
                global(-1,0);
                break;
            case 2: //Escalado
                global(1,1.1);
                break;
            }
            break;
        }
    break;
    case GLUT_KEY_DOWN :
       switch (LoG)
        {
        case 0: 
            glLoadMatrixd(_selected_object->pMptr->Matriz);
            guardar();
            switch (ToRoE)
            {
            case 0: //Translacion
                glTranslated(0,-1,0);
                break;
            case 1: //Rotacion
                glRotated(10,1,0,0);
                break;
            case 2: //Escalado
                glScaled(1,0.9,1);
                break;
            }
            glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->pMptr->Matriz);
            break;
        case 1:

            switch (ToRoE)
            {
            case 0: //Translacion
                global(0,-1);
                break;
            case 1: //Rotacion
                global(1,0);
                break;
            case 2: //Escalado
                global(1,0.9);
                break;
            }
            break;
        }
        
        
    break;
    case GLUT_KEY_LEFT :
       switch (LoG)
        {
        case 0:
            glLoadMatrixd(_selected_object->pMptr->Matriz);
            guardar();
            switch (ToRoE)
            {
            case 0: //Translacion
                glTranslated(-1,0,0);
                break;
            case 1: //Rotacion
                glRotated(10,0,-1,0);
                break;
            case 2: //Escalado
                glScaled(0.9,1,1);
                break;
            }
            glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->pMptr->Matriz);
            break;
        case 1:

            switch (ToRoE)
            {
            case 0: //Translacion
                global(-1,0);
                break;
            case 1: //Rotacion
                global(0,1);
                break;
            case 2: //Escalado
                global(0.9,1);
                break;
            }
            break;
        }
        
        

    break;
    case GLUT_KEY_RIGHT :
    switch (LoG)
        {
        case 0:
            glLoadMatrixd(_selected_object->pMptr->Matriz);
            guardar();
            switch (ToRoE)
            {
            case 0: //Translacion
                glTranslated(1,0,0);
                break;
            case 1: //Rotacion
                glRotated(10,0,1,0);
                break;
            case 2: //Escalado
                glScaled(1.1,1,1);
                break;
            }
            glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->pMptr->Matriz);
            break;
        case 1:

            switch (ToRoE)
            {
            case 0: //Translacion
                global(1,0);
                break;
            case 1: //Rotacion
                global(0,1);
                break;
            case 2: //Escalado
                global(1.1,1);
                break;
            }
            break;
        }
    break;
    default:
        /*In the default case we just print the code of the key. This is usefull to define new cases*/
        printf("%d %c\n", k, k);
   }
   glutPostRedisplay();
}

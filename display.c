#include "definitions.h"
#include "io.h"
#include <GL/glut.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

/** EXTERNAL VARIABLES **/

extern GLdouble _window_ratio;
extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

extern object3d *_first_object;
extern object3d *_selected_object;
extern elm_matriz *_selected_camara;
extern elm_matriz *_selected_camara_inv;
extern elm_matriz *_first_camara;
extern int camara,modo_camara;

/**
 * @brief Function to draw the axes
 */

//Funcion que se dedica a dibujar los ejes:

void draw_axes()
{
    glLoadIdentity();
    /*Draw X axis*/
    glColor3f(KG_COL_X_AXIS_R,KG_COL_X_AXIS_G,KG_COL_X_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(KG_MAX_DOUBLE,0,0);
    glVertex3d(-1*KG_MAX_DOUBLE,0,0);
    glEnd();
    /*Draw Y axis*/
    glColor3f(KG_COL_Y_AXIS_R,KG_COL_Y_AXIS_G,KG_COL_Y_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0,KG_MAX_DOUBLE,0);
    glVertex3d(0,-1*KG_MAX_DOUBLE,0);
    glEnd();
    /*Draw Z axis*/
    glColor3f(KG_COL_Z_AXIS_R,KG_COL_Z_AXIS_G,KG_COL_Z_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0,0,KG_MAX_DOUBLE);
    glVertex3d(0,0,-1*KG_MAX_DOUBLE);
    glEnd();
}


/**
 * @brief Callback reshape function. We just store the new proportions of the window
 * @param width New width of the window
 * @param height New height of the window
 */
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    /*  Take care, the width and height are integer numbers, but the ratio is a GLdouble so, in order to avoid
     *  rounding the ratio to integer values we need to cast width and height before computing the ratio */
    _window_ratio = (GLdouble) width / (GLdouble) height;
}


/**
 * @brief Callback display function
 */


//NO MODIFICA NADA DEL MUNDO, solo dibuja
void cabinacamara(){
    elm_matriz *auxx,*auxxi;
    auxx = (elm_matriz *)malloc(sizeof(elm_matriz));
    auxxi = (elm_matriz *)malloc(sizeof(elm_matriz));
    double At[3],E[3],Vup[3],z[3],x[3],y[3];
    glGetDoublev(GL_MODELVIEW_MATRIX,auxx->Matriz);
    

    for (int i = 0; i < 3;i++){
        x[i] = _selected_object->pMptr->Matriz[i];
        y[i] = _selected_object->pMptr->Matriz[4+i];
        z[i] = _selected_object->pMptr->Matriz[8+i];
        E[i] = _selected_object->pMptr->Matriz[12+i];
    }
    for (int i = 0; i < 3; i++){
        E[i] = E[i] + z[i]*_selected_object->max.z*0.51+y[i]*_selected_object->max.y*0.77;
        x[i] = -x[i];
        z[i] = -z[i];
        
    }
    for (int i = 0; i < 3;i++){
        auxx->Matriz[i] = x[i];
        auxx->Matriz[4+i] = y[i];
        auxx->Matriz[8+i] = z[i];
        auxx->Matriz[12+i] = E[i];
    }
    auxx->Matriz[15] = 1;
    matrix_inv(auxxi, auxx);
    glLoadIdentity();
    glLoadMatrixd(auxxi->Matriz);
    glGetDoublev(GL_MODELVIEW_MATRIX,auxx->Matriz);

}

void display(void) {
    GLint v_index, v, f;
    //Aux_obj es un puntero tri-dimensional que apunta a el primer objeto de nuestra lista de objetos (_first_object)
    object3d *aux_obj = _first_object;

    /* Clear the screen */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Define the projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //Establece una matriz de proyeccion que determina que lo que este dentro del volumen de vision lo mandara a pantalla.

    /*When the window is wider than our original projection plane we extend the plane in the X axis
      if ((_ortho_x_max - _ortho_x_min) / (_ortho_y_max - _ortho_y_min) < _window_ratio) {
        /* New widt
         GLdouble wd = (_ortho_y_max - _ortho_y_min) * _window_ratio;
        /* Midpoint in the X axis 
        GLdouble midpt = (_ortho_x_min + _ortho_x_max) / 2;
        /*Definition of the projection
        glOrtho(midpt - (wd / 2), midpt + (wd / 2), _ortho_y_min, _ortho_y_max, _ortho_z_min, _ortho_z_max);
    } else {/* In the opposite situation we extend the Y axis 
        /* New height 
        GLdouble he = (_ortho_x_max - _ortho_x_min) / _window_ratio;
        /* Midpoint in the Y axis 
        GLdouble midpt = (_ortho_y_min + _ortho_y_max) / 2;
        /*Definition of the projection
        glOrtho(_ortho_x_min, _ortho_x_max, midpt - (he / 2), midpt + (he / 2), _ortho_z_min, _ortho_z_max);
    }
    */
    glFrustum(-0.1,0.1,-0.1,0.1,0.1,1000.0);
    /* Now we start drawing the object */
    glMatrixMode(GL_MODELVIEW);
    draw_axes();
     if ( _selected_object!=0)
    {
       if (modo_camara==0)//centrado en el objeto
       {
            printf("entro aqui\n");
            glLoadMatrixd(_selected_object->inv->Matriz);
       }
       else if(modo_camara==2)
       {
            cabinacamara();
       }
       else//camara
       {
           glLoadMatrixd(_selected_camara_inv->Matriz);
       }
    } 
    
    

    /*First, we draw the axes*/
    

    /*Now each of the objects in the list*/

    //Mientras tengamos objectos restantes los dibujara:
    while (aux_obj != 0) {
        glPushMatrix();
        
        /* Select the color, depending on whether the current object is the selected one or not */
        
        if (aux_obj == _selected_object){
            glColor3f(KG_COL_SELECTED_R,KG_COL_SELECTED_G,KG_COL_SELECTED_B);
        }else{
            glColor3f(KG_COL_NONSELECTED_R,KG_COL_NONSELECTED_G,KG_COL_NONSELECTED_B);
        }

        /* Draw the object; for each face create a new polygon with the corresponding vertices */
        glMultMatrixd(aux_obj->pMptr->Matriz);
     

        //Como dibujara? De eso se encarga OpenGL simplemente.
        for (f = 0; f < aux_obj->num_faces; f++) {
            glBegin(GL_POLYGON);
            for (v = 0; v < aux_obj->face_table[f].num_vertices; v++) {
                v_index = aux_obj->face_table[f].vertex_table[v];
                glVertex3d(aux_obj->vertex_table[v_index].coord.x,
                        aux_obj->vertex_table[v_index].coord.y,
                        aux_obj->vertex_table[v_index].coord.z);

            }
            glEnd();
        }
        aux_obj = aux_obj->next;
        glPopMatrix();
    }
    /*Do the actual drawing*/
    
    glFlush();
}

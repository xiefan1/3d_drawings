/*
  CSC418 - RayTracer code - Winter 2017 - Assignment 3&4

  Written Dec. 9 2010 - Jan 20, 2011 by F. J. Estrada
  Freely distributable for adacemic purposes only.

  Uses Tom F. El-Maraghi's code for computing inverse
  matrices. You will need to compile together with
  svdDynamic.c

  You need to understand the code provided in
  this file, the corresponding header file, and the
  utils.c and utils.h files. Do not worry about
  svdDynamic.c, we need it only to compute
  inverse matrices.

  You only need to modify or add code in sections
  clearly marked "TO DO"
*/

#include "utils.h"
#include "assert.h"
#define maxlight 10

// A couple of global structures and data: An object list, a light list, and the
// maximum recursion depth
struct object3D *object_list;
struct object3D *light_list;
double light_radius[maxlight];
int MAX_DEPTH;
int antialiasing;	// Flag to determine whether antialiaing is enabled or disabled

//generate weights from Gaussian normal function
//size is always odd
void gen_Gaussian_weight(double *table,int center){
    if(!table) return;
    double coe = 1/(2*PI);
    int size = center*2+1;
    for(int i=0;i<size;++i){
	for(int j=0;j<size;++j){
	    double x = j - center;
	    double y = i - center;
	    double p = -(x*x+y*y)/2;
	    *(table+i*size+j) = coe*pow(E,p);
	}
    }
}
void buildScene(void)
{
 // Sets up all objects in the scene. This involves creating each object,
 // defining the transformations needed to shape and position it as
 // desired, specifying the reflectance properties (albedos and colours)
 // and setting up textures where needed.
 // Light sources must be defined, positioned, and their colour defined.
 // All objects must be inserted in the object_list. All light sources
 // must be inserted in the light_list.
 //
 // To create hierarchical objects:
 //   Copy the transform matrix from the parent node to the child, and
 //   apply any required transformations afterwards.
 //
 // NOTE: After setting up the transformations for each object, don't
 //       forget to set up the inverse transform matrix!

 struct object3D *o;

 ///////////////////////////////////////
 // TO DO: For Assignment 3 you have to use
 //        the simple scene provided
 //        here, but for Assignment 4 you
 //        *MUST* define your own scene.
 //        Part of your mark will depend
 //        on how nice a scene you
 //        create. Use the simple scene
 //        provided as a sample of how to
 //        define and position objects.
 ///////////////////////////////////////

 // Simple scene for Assignment 3:
 // Insert a couple of objects. A plane and two spheres
 // with some transformations.

 // Let's add a plane
 // Note the parameters: ra, rd, rs, rg, R, G, B, alpha, r_index, and shinyness)

 o=newPlane(.1,.75,.05,.35,.55,.8,.75,1,1,2);	// Note the plane is highly-reflective (rs=rg=.75) so we
						// should see some reflections if all is done properly.
						// Colour is close to cyan, and currently the plane is
						// completely opaque (alpha=1). The refraction index is
						// meaningless since alpha=1
 Scale(o,16,16,1);				// Do a few transforms...
 RotateZ(o,PI/1.20);
 RotateX(o,PI/2.25);
 Translate(o,0,-3,10);
 invert(&o->T[0][0],&o->Tinv[0][0]);		// Very important! compute
						// and store the inverse
						// transform for this object!
 insertObject(o,&object_list);			// Insert into object list

 //an opague ellipse
 o=newSphere(.05,.95,.35,.35,.5,1,.83,1,1,10);
 Scale(o,.75,.5,1.5);
 RotateY(o,PI/3);
 Translate(o,-4,1.1,5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 //a tranparent ellipse
 o=newSphere(.05,.95,.95,1,.8,.5,.3,0.8,1.52,10);
 Scale(o,.5,2.0,1.0);
 RotateZ(o,PI/1.5);
 Translate(o,-4.5,-2,1.5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 //a mirror
 o=newPlane(.05,.75,.05,1,1,1,1,1,1,2);
 o->isMirror = 1; 			//for mirror, specify all rgb to 1,1,1
 Scale(o,2.2,1.1,1);
 RotateX(o,-PI/12);
 RotateY(o,PI/4);
 Translate(o,2,.5,2.8);
 invert(&o->T[0][0],&o->Tinv[0][0]);	
 insertObject(o,&object_list);		

 //an opague refractive sphere
 o=newSphere(.3,.95,.95,1,.94,.5,.5,1,1.52,10);
 Translate(o,0.5,1.7,0.75);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 //an transparent sphere
 o=newSphere(.1,0,.95,.5,.94,1,.5,0,1.42,10);
 Scale(o,1,1,0.5);
 Translate(o,-1,-3.0,-0);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);


 // Insert a single point light source as sphere
 double r1=3;
 light_radius[0]=r1;
 o=newSphere(0,0,0,0,.95,.95,.95,1,0,0);
 o->isLightSource=1;
 Scale(o,r1,r1,r1);
 Translate(o,0,14.5,-9.5);
 insertObject(o,&light_list);

 // End of simple scene for Assignment 3
 // Keep in mind that you can define new types of objects such as cylinders and parametric surfaces,
 // or, you can create code to handle arbitrary triangles and then define objects as surface meshes.
 //
 // Remember: A lot of the quality of your scene will depend on how much care you have put into defining
 //           the relflectance properties of your objects, and the number and type of light sources
 //           in the scene.
}





int main(int argc, char *argv[])
{
 // Main function for the raytracer. Parses input parameters,
 // sets up the initial blank image, and calls the functions
 // that set up the scene and do the raytracing.
 struct image *im;	// Will hold the raytraced image
 struct view *cam;	// Camera and view for this scene
 int sx;		// Size of the raytraced image
 char output_name[1024];	// Name of the output file for the raytraced .ppm image
 struct point3D e;		// Camera view parameters 'e', 'g', and 'up'
 struct point3D g;
 struct point3D up;
 double du, dv;			// Increase along u and v directions for pixel coordinates
 struct colourRGB background;   // Background colour
 unsigned char *rgbIm;
 srand(1522);

 if (argc<5)
 {
  fprintf(stderr,"RayTracer: Can not parse input parameters\n");
  fprintf(stderr,"USAGE: RayTracer size rec_depth antialias output_name\n");
  fprintf(stderr,"   size = Image size (both along x and y)\n");
  fprintf(stderr,"   rec_depth = Recursion depth\n");
  fprintf(stderr,"   antialias = A single digit, 0 disables antialiasing. Anything else enables antialiasing\n");
  fprintf(stderr,"   output_name = Name of the output file, e.g. MyRender.ppm\n");
  exit(0);
 }
 sx=atoi(argv[1]);
 MAX_DEPTH=atoi(argv[2]);
 if (atoi(argv[3])==0) antialiasing=0; else antialiasing=1;
 strcpy(&output_name[0],argv[4]);

 fprintf(stderr,"Rendering image at %d x %d\n",sx,sx);
 fprintf(stderr,"Recursion depth = %d\n",MAX_DEPTH);
 if (!antialiasing) fprintf(stderr,"Antialising is off\n");
 else fprintf(stderr,"Antialising is on\n");
 fprintf(stderr,"Output file name: %s\n",output_name);

 object_list=NULL;
 light_list=NULL;

 // Allocate memory for the new image
 im=newImage(sx, sx);
 if (!im)
 {
  fprintf(stderr,"Unable to allocate memory for raytraced image\n");
  exit(0);
 }
 else rgbIm=(unsigned char *)im->rgbdata; //Fan: char *rgb[im->sx][im->sy][3]

 ///////////////////////////////////////////////////
 // TO DO: You will need to implement several of the
 //        functions below. For Assignment 3, you can use
 //        the simple scene already provided. But
 //        for Assignment 4 you need to create your own
 //        *interesting* scene.
 ///////////////////////////////////////////////////
 buildScene();		// Create a scene. This defines all the
			// objects in the world of the raytracer

 //////////////////////////////////////////
 // TO DO: For Assignment 3 you can use the setup
 //        already provided here. For Assignment 4
 //        you may want to move the camera
 //        and change the view parameters
 //        to suit your scene.
 //////////////////////////////////////////

 // Mind the homogeneous coordinate w of all vectors below. DO NOT
 // forget to set it to 1, or you'll get junk out of the
 // geometric transformations later on.

 // Camera center is at (0,0,-1)
 e.px=0;
 e.py=0;
 e.pz=-6;
 e.pw=1;

 // To define the gaze vector, we choose a point 'pc' in the scene that
 // the camera is looking at, and do the vector subtraction pc-e.
 // Here we set up the camera to be looking at the origin, so g=(0,0,0)-(0,0,-1)
 g.px=0;
 g.py=-0.1;
 g.pz=1;
 g.pw=0;
 normalize(&g);

 // Define the 'up' vector to be the Y axis
 up.px=0;
 up.py=1;
 up.pz=0;
 up.pw=0;

 // Set up view with given the above vectors, a 4x4 window. Fan: size 4 is in distance units, not pixels
 // and a focal length of -1 (why? where is the image plane?)
 // Note that the top-left corner of the window is at (-2, 2)
 // in camera coordinates.
 cam=setupView(&e, &g, &up, -2, -2, 2, 4);

 if (cam==NULL)
 {
  fprintf(stderr,"Unable to set up the view and camera parameters. Our of memory!\n");
  cleanup(object_list);
  cleanup(light_list);
  deleteImage(im);
  exit(0);
 }

 // Set up background colour here
 background.R=0;
 background.G=0;
 background.B=0;
 du=cam->wsize/(sx-1);		// dv is negative since y increases downward in pixel
 dv=-cam->wsize/(sx-1);		// coordinates and upward in camera coordinates.
				//Fan: cam->wsize is in distance unit, sx is the resolution
				
 fprintf(stderr,"View parameters:\n");
 fprintf(stderr,"Left=%f, Top=%f, Width=%f, f=%f\n",cam->wl,cam->wt,cam->wsize,cam->f);
 fprintf(stderr,"Camera to world conversion matrix (make sure it makes sense!):\n");
 printmatrix(cam->C2W);
 fprintf(stderr,"World to camera conversion matrix\n");
 printmatrix(cam->W2C);
 fprintf(stderr,"\n");

 fprintf(stderr,"Rendering rows ");
 
 int center = 1;
 int ns=2*center+1; //[ns x ns] subcells per pixel
 double num = ns*ns;
 double dsu = du/(ns-1);
 double dsv = dv/(ns-1); //note dsy is negative
 double weightG[ns][ns];
 //compute weight from Gaussian function (low-pass filter)
 gen_Gaussian_weight(&weightG[0][0],center);

 //initialize points and vectors in the camera space
 struct point3D origin;
 origin.px=0;
 origin.py=0;
 origin.pz=0;
 origin.pw=1;

 //openmp multi-threaded
 #pragma omp parallel for
 for (int j=0;j<sx;j++)		// For each of the pixels in the image
 {
  //direction vector: pixel coordinate-origin
  struct point3D ps;
  ps.py=cam->wt+j*dv; //note: dv is negative
  ps.pz=cam->f;
  ps.pw=0;

   for (int i=0;i<sx;i++)
  {
    //update to the current pixel position
    ps.px=cam->wl+i*du;

    struct colourRGB col_avg={0,0,0};
    struct point3D copyP;
    copyPoint(&ps,&copyP);

    //anti-aliasing by supersampling
    //divide per pixel into nsxns cells and randomly shoot rays
    for(int su=0;su<ns;++su){
	for(int sv=0;sv<ns;++sv){
	    struct colourRGB col={0,0,0};

	    //for each subcell
	    //construct the primary ray
	    struct ray3D *ray = newRay(&origin,&copyP);

	    //transform the ray into the world space
	    matRayMult(cam->C2W,ray);
	    rayTrace(ray,0,&col,NULL);

	    //average the col with Gaussian weight
	    mult_col(weightG[su][sv],&col);
	    add_col(&col,&col_avg);

	    //update to the next subcell position
	    copyP.px+=dsu;
	    free(ray);
	    ray = NULL;
	}
	copyP.px=ps.px;
	copyP.py+=dsv;
    }

    //set color of this pixel
    //printf("(%f, %f, %f)",col.R,col.G,col.B);
    *(rgbIm+j*sx*3+i*3+0) = col_avg.R*255;
    *(rgbIm+j*sx*3+i*3+1) = col_avg.G*255;
    *(rgbIm+j*sx*3+i*3+2) = col_avg.B*255;

  } // end of this row
 } // end for j

 fprintf(stderr,"\nDone!\n");

 // Output rendered image
 imageOutput(im,output_name);

 // Exit section. Clean up and return.
 cleanup(object_list);		// Object and light lists
 cleanup(light_list);
 deleteImage(im);				// Rendered image
 free(cam);					// camera view
 exit(0);
}


// Find the closest intersection between the ray and any objects in the scene.
// It returns:
//   - The lambda at the intersection (or < 0 if no intersection)
//   - The pointer to the object at the intersection (so we can evaluate the colour in the shading function)
//   - The location of the intersection point (in p)
//   - The normal at the intersection point (in n)
//
// Os is the 'source' object for the ray we are processing, can be NULL, and is used to ensure we don't 
// return a self-intersection due to numerical errors for recursive raytrace calls.
// note: ray is in the world coords
void findFirstHit(struct ray3D *ray, double *lambda, struct object3D *Os,
		  	struct object3D **obj, struct point3D *p, 
			struct point3D *n, double *a, double *b, int depth){
    *lambda = -1;
    *obj = NULL;
    int initial=1;
    struct object3D *cur_obj=object_list;
    while(cur_obj!=NULL){
	//make one object can not intersect with itself
	//to avoid errors caused by rounding precision errors etc.
	if(cur_obj == Os){
	    cur_obj=cur_obj->next;
	    continue;
	}

	double temp=0; //temporary lambda
	cur_obj->intersect(cur_obj,ray,&temp,p,n,a,b);

	//Q1:should it compare with 1 instead??
	if(temp>0){
    	    if(initial==1 || *lambda>temp)
    	    {
		initial = 0;
		*lambda = temp;
		*obj = cur_obj;

		/* Transform n and p back to the world coords */
		// transform normal vectors n
		double Tinv_trans[4][4]={0};
		transpose(&(cur_obj->Tinv[0][0]),&(Tinv_trans[0][0]));
		matVecMult(Tinv_trans,n);
		normalize(n);

		// transform the intersect point p
		matVecMult(cur_obj->T,p);
	    }
	}

	cur_obj=cur_obj->next;
    }
}


// generate unit refraction ray
// Warning: this function assumes air-object or object-air interface
// n - normal unit vector
// b - intersection to eye unit vector
// p - intersection point
struct ray3D* gen_refractionRay(struct object3D* obj, struct point3D* n, struct point3D* b, struct point3D* p){
    double nt = obj->r_index;
    struct point3D d;
    d.px=-(b->px);
    d.py=-(b->py);
    d.pz=-(b->pz);
    d.pw=1;

    //if rays goes from air to object, cosTheta < 0
    double cosTheta = dot(n,&d);
    struct point3D temp;
    if(cosTheta<0){
	//air-object interface
	double coef1 = (1-cosTheta*cosTheta)/(nt*nt);
	assert(coef1<=1);
	coef1 = sqrt(1-coef1);
	temp.px = ((d.px-n->px*cosTheta)/nt) - n->px*coef1;
	temp.py = ((d.py-n->py*cosTheta)/nt) - n->py*coef1;
	temp.pz = ((d.pz-n->pz*cosTheta)/nt) - n->pz*coef1;
	temp.pw = 0;
	normalize(&temp);
    }else{
	//object-air interface
	double cosPhi = 1- nt*nt*(1-cosTheta*cosTheta);
	assert(cosPhi<=1);
	cosPhi = sqrt(1-cosPhi);
	temp.px = ((d.px-n->px*cosTheta)*nt) + n->px*cosPhi;
	temp.py = ((d.py-n->py*cosTheta)*nt) + n->py*cosPhi;
	temp.pz = ((d.pz-n->pz*cosTheta)*nt) + n->pz*cosPhi;
	temp.pw = 0;
	normalize(&temp);
    }

    return(newRay(p,&temp));
}


// generate unit reflection ray
// n - normal unit vector
// b - intersection to eye unit vector
// p - intersection point
struct ray3D* gen_reflectionRay(struct point3D* n, struct point3D* b, struct point3D* p){
    struct point3D r;
    copyPoint(n,&r);
    double up=2*dot(n,b);
    multVector(up,&r);
    subVectors(b,&r);
    normalize(&r);
    r.pw=0;

    return(newRay(p,&r)); //r is normalized
}


// Ray-Tracing function. It finds the closest intersection between
// the ray and any scene objects, calls the shading function to
// determine the colour at this intersection, and returns the
// colour.
//
// Os is needed for recursive calls to ensure that findFirstHit will
// not simply return a self-intersection due to numerical
// errors. For the top level call, Os should be NULL. And thereafter
// it will correspond to the object from which the recursive
// ray originates.
void rayTrace(struct ray3D *ray, int depth, struct colourRGB *col,
			struct object3D *Os)
{
	assert(ray);
	if (depth>MAX_DEPTH)	// Max recursion depth reached
	    return;

	double lambda=0, a=0,b=0; //a,b are texture coords
    	struct object3D* hitObj=NULL;
        struct point3D p,n;
        //find the first intersection
        //return lambda, hit object(next object source), hit point and normal
        findFirstHit(ray,&lambda,Os,&hitObj,&p,&n,&a,&b,depth);

	//color shading
        if(hitObj){
            //if hit an object
	    //Phong illumination
	    rtShade(hitObj,&p,&n,ray,depth,a,b,col);
	    //if(hitObj->frontAndBack==1) printf("plane normal (%f,%f,%f)\n",n.px,n.py,n.pz);
	}
}


// This function implements the shading model as described in lecture. It takes
// - A pointer to the first object intersected by the ray (to get the colour properties)
// - The coordinates of the intersection point (in world coordinates)
// - The normal at the point
// - The ray (needed to determine the reflection direction to use for the global component, as well as for
//   the Phong specular component)
// - The current racursion depth
// - The (a,b) texture coordinates (meaningless unless texture is enabled)
//
// Returns:
// - The colour for this ray (using the col pointer)
void rtShade(struct object3D *obj, struct point3D *p, struct point3D *n, struct ray3D *ray,
				int depth, double _a, double _b, struct colourRGB *col)
{
 double R,G,B;			// Colour for the object in R G and B

 if (obj->texImg==NULL)		// Not textured, use object colour
 {
  R=obj->col.R;
  G=obj->col.G;
  B=obj->col.B;
 }
 else
 {
  // Get object colour from the texture given the texture coordinates (a,b), and the texturing function
  // for the object. Note that we will use textures also for Photon Mapping.
  obj->textureMap(obj->texImg,_a,_b,&R,&G,&B);
 }

 double ra,rd,rs,rg;
 ra=obj->alb.ra;
 rd=obj->alb.rd;
 rs=obj->alb.rs;
 rg=obj->alb.rg;

 //compute the unit p->OS(eye) vector
 struct point3D b;
 copyPoint(&(ray->d),&b);
 normalize(&b);
 multVector(-1,&b);
 b.pw=0;

 //if it's mirror, only have reflection component
 if(obj->isMirror==0){
    
     //for all the light sources
     struct object3D *cur;
     int num_light=0;
     cur=light_list;
     while(cur!=NULL){
        double lr,lg,lb;
        lr=cur->col.R;
        lg=cur->col.G;
        lb=cur->col.B;
     
        //compute the unit p->light vector
        struct point3D s={0,0,0,1}; //the p->light vector
        matVecMult(cur->T,&s);
        subVectors(p,&s);
        //normalize the p->light vector
        normalize(&s);
        s.pw=0;
 
        //compute the unit reflection vector
        struct point3D r;
        copyPoint(n,&r);
        double up=2*dot(n,&s);
        multVector(up,&r);
        subVectors(&s,&r);
        normalize(&r);
        r.pw=0;
    
    
        /* ambient */
        add_col(ra*lr*R,ra*lg*G,ra*lb*B,col);
    
        /* shadow (diffuse and specular) */

	//shoot multiple rays from p to the light source
	int numRays=1;
	if(antialiasing==1) numRays = 10;//turn on anti-aliasing

	for(int light_i=0;light_i<numRays;++light_i){
            //create ray from hitObj to a random point on light source
    	    struct point3D shadowRay={0,0,0,1}; //it's still a point for now
	    double theta = 2*PI*drand48();
	    double phi = 2*PI*drand48();
      	    double rxyz = light_radius[num_light]*drand48();
	    double rxy = rxyz*sin(theta);
	    shadowRay.px = rxy*cos(phi);
	    shadowRay.py = rxy*sin(phi);
            shadowRay.pz = rxyz*cos(theta);
            matVecMult(cur->T,&shadowRay); //transform to object world
            subVectors(p,&shadowRay);
            shadowRay.pw=0; //now it's a vector
        
	    //note shadow shall not be normalized
            struct ray3D *ray_to_light = newRay(p,&shadowRay);
            double shadow_t=0;
            struct object3D* hitObj=NULL;
            struct point3D _p,_n;
            findFirstHit(ray_to_light,&shadow_t,obj,&hitObj,&_p,&_n,NULL,NULL,depth);
            free(ray_to_light);
            ray_to_light=NULL;
           
        
            //if any object blocks the light,
            //exclude diffuse and specular components
            if(hitObj == NULL || shadow_t>=1 || shadow_t <=0){
		struct colourRGB col_ds={0,0,0};
                /* diffuse */
                double dim = dot(n,&s);
                if(dim<0){
                	if(obj->frontAndBack) dim=-dim;
            	else dim=0;
                }
                add_col(rd*lr*R*dim,rd*lg*G*dim,rd*lb*B*dim,&col_ds);
            
            
                /* specular */
                dim = dot(&b,&r);
                if(dim<0){
                	if(obj->frontAndBack) dim=-dim;
            	else dim=0;
                }
                dim = pow(dim,obj->shinyness);
                add_col(rs*lr*dim,rs*lg*dim,rs*lb*dim,&col_ds);

		mult_col(((double)1/numRays),&col_ds);
		add_col(&col_ds,col);
        
            }
	}//end of shadow
    
        //next light source
        cur=cur->next;
	num_light+=1;
     }
    
     if(col->R>1) col->R=1;
     if(col->G>1) col->G=1;
     if(col->B>1) col->B=1;
     if(col->R==1 && col->G==1 && col->B==1) return;
 } 

 if(depth<MAX_DEPTH){
    struct colourRGB col_ref={0,0,0};
    double alpha = obj->alpha;

    /* reflection */
    //generate the reflection ray
    struct ray3D* rRay = gen_reflectionRay(n,&b,p);
    //recursive call of rayTrace
    rayTrace(rRay,depth+1,&col_ref,obj);
    free(rRay);
    rRay=NULL;
    col_ref.R*=rg*R*alpha;
    col_ref.G*=rg*G*alpha;
    col_ref.B*=rg*B*alpha;
    add_col(&col_ref,col);


    /*refraction*/
    if(alpha<1){
	rRay = gen_refractionRay(obj,n,&b,p);
	struct colourRGB col_refract={0,0,0};
	rayTrace(rRay,depth+1,&col_refract,obj);
    	free(rRay);
      	rRay=NULL;
	col_ref.R*=(1-alpha)*R;
	col_ref.G*=(1-alpha)*G;
	col_ref.B*=(1-alpha)*B;
	add_col(&col_refract,col);
   }

 }
 if(col->R>1) col->R=1;
 if(col->G>1) col->G=1;
 if(col->B>1) col->B=1;
}



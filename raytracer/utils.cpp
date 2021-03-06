/*
   utils.c - F.J. Estrada, Dec. 9, 2010

   Utilities for the ray tracer. You will need to complete
   some of the functions in this file. Look for the sections
   marked "TO DO". Be sure to read the rest of the file and
   understand how the entire code works.
*/

#include "utils.h"
#include "cmath"
#include "assert.h"

// A useful 4x4 identity matrix which can be used at any point to
// initialize or reset object transformations
double eye4x4[4][4]={{1.0, 0.0, 0.0, 0.0},
                    {0.0, 1.0, 0.0, 0.0},
                    {0.0, 0.0, 1.0, 0.0},
                    {0.0, 0.0, 0.0, 1.0}};

/////////////////////////////////////////////
// Primitive data structure section
/////////////////////////////////////////////
struct point3D *newPoint(double px, double py, double pz)
{
 // Allocate a new point structure, initialize it to
 // the specified coordinates, and return a pointer
 // to it.

 struct point3D *pt=(struct point3D *)calloc(1,sizeof(struct point3D));
 if (!pt) fprintf(stderr,"Out of memory allocating point structure!\n");
 else
 {
  pt->px=px;
  pt->py=py;
  pt->pz=pz;
  pt->pw=1.0;
 }
 return(pt);
}


/////////////////////////////////////////////
// Ray and normal transforms
/////////////////////////////////////////////

// to transform the ray into the model world
// simply call matRayMult(obj->Tinv,ray);

// to transfrom normals
// call transpose then matRayMult
// (see in FindFirstHit)


/////////////////////////////////////////////
// Object management section
/////////////////////////////////////////////
struct object3D *newPlane(double ra, double rd, double rs, double rg, double r, double g, double b, double alpha, double r_index, double shiny)
{
 // Intialize a new plane with the specified parameters:
 // ra, rd, rs, rg - Albedos for the components of the Phong model
 // r, g, b, - Colour for this plane
 // alpha - Transparency, must be set to 1 unless you are doing refraction
 // r_index - Refraction index if you are doing refraction.
 // shiny - Exponent for the specular component of the Phong model
 //
 // The plane is defined by the following vertices (CCW)
 // (1,1,0), (-1,1,0), (-1,-1,0), (1,-1,0)
 // With normal vector (0,0,1) (i.e. parallel to the XY plane)

 struct object3D *plane=(struct object3D *)calloc(1,sizeof(struct object3D));

 if (!plane) fprintf(stderr,"Unable to allocate new plane, out of memory!\n");
 else
 {
  plane->alb.ra=ra;
  plane->alb.rd=rd;
  plane->alb.rs=rs;
  plane->alb.rg=rg; //what is rg?? depth factor?
  plane->col.R=r;
  plane->col.G=g;
  plane->col.B=b;
  plane->alpha=alpha;
  plane->r_index=r_index;
  plane->shinyness=shiny;
  plane->intersect=&planeIntersect;
  plane->texImg=NULL;
  memcpy(&plane->T[0][0],&eye4x4[0][0],16*sizeof(double));
  memcpy(&plane->Tinv[0][0],&eye4x4[0][0],16*sizeof(double));
  plane->textureMap=&texMap;
  plane->frontAndBack=1;
  plane->isLightSource=0;
  plane->isMirror=0;
  plane->goingOut=0;
 }
 return(plane);
}



struct object3D *newSphere(double ra, double rd, double rs, double rg, double r, double g,
				double b, double alpha, double r_index, double shiny)
{
 // Intialize a new sphere with the specified parameters:
 // ra, rd, rs, rg - Albedos for the components of the Phong model
 // r, g, b, - Colour for this plane
 // alpha - Transparency, must be set to 1 unless you are doing refraction
 // r_index - Refraction index if you are doing refraction.
 // shiny -Exponent for the specular component of the Phong model
 //
 // This is assumed to represent a unit sphere centered at the origin.

 struct object3D *sphere=(struct object3D *)calloc(1,sizeof(struct object3D));

 if (!sphere) fprintf(stderr,"Unable to allocate new sphere, out of memory!\n");
 else
 {
  sphere->alb.ra=ra;
  sphere->alb.rd=rd;
  sphere->alb.rs=rs;
  sphere->alb.rg=rg;
  sphere->col.R=r;
  sphere->col.G=g;
  sphere->col.B=b;
  sphere->alpha=alpha;
  sphere->r_index=r_index;
  sphere->shinyness=shiny;
  sphere->intersect=&sphereIntersect;
  sphere->texImg=NULL;
  memcpy(&sphere->T[0][0],&eye4x4[0][0],16*sizeof(double));
  memcpy(&sphere->Tinv[0][0],&eye4x4[0][0],16*sizeof(double));
  sphere->textureMap=&texMap;
  sphere->frontAndBack=0;
  sphere->isLightSource=0;
  sphere->isMirror=0;
  sphere->goingOut=0;
 }
 return(sphere);
}

struct object3D *newCone(double ra, double rd, double rs, double rg, double r, double g,
				double b, double alpha, double r_index, double shiny)
{
 // This is assumed to represent a unit cone with vertex at the origin.
 // x^2+z^2-y^2=0
 struct object3D *cone=(struct object3D *)calloc(1,sizeof(struct object3D));

 if (!cone) fprintf(stderr,"Unable to allocate new cone, out of memory!\n");
 else
 {
  cone->alb.ra=ra;
  cone->alb.rd=rd;
  cone->alb.rs=rs;
  cone->alb.rg=rg;
  cone->col.R=r;
  cone->col.G=g;
  cone->col.B=b;
  cone->alpha=alpha;
  cone->r_index=r_index;
  cone->shinyness=shiny;
  cone->intersect=&coneIntersect;
  cone->texImg=NULL;
  memcpy(&cone->T[0][0],&eye4x4[0][0],16*sizeof(double));
  memcpy(&cone->Tinv[0][0],&eye4x4[0][0],16*sizeof(double));
  cone->textureMap=&texMap;
  cone->frontAndBack=1;
  cone->isLightSource=0;
  cone->isMirror=0;
  cone->goingOut=0;
 }
 return(cone);
}

struct object3D *newParaboloid(double ra, double rd, double rs, double rg, double r, double g,
				double b, double alpha, double r_index, double shiny)
{
 // This is assumed to represent a unit paraboloid with vertex at the origin.
 // x^2+z^2+y=0
 struct object3D *paraboloid=(struct object3D *)calloc(1,sizeof(struct object3D));

 if (!paraboloid) fprintf(stderr,"Unable to allocate new paraboloid, out of memory!\n");
 else
 {
  paraboloid->alb.ra=ra;
  paraboloid->alb.rd=rd;
  paraboloid->alb.rs=rs;
  paraboloid->alb.rg=rg;
  paraboloid->col.R=r;
  paraboloid->col.G=g;
  paraboloid->col.B=b;
  paraboloid->alpha=alpha;
  paraboloid->r_index=r_index;
  paraboloid->shinyness=shiny;
  paraboloid->intersect=&paraboloidIntersect;
  paraboloid->texImg=NULL;
  memcpy(&paraboloid->T[0][0],&eye4x4[0][0],16*sizeof(double));
  memcpy(&paraboloid->Tinv[0][0],&eye4x4[0][0],16*sizeof(double));
  paraboloid->textureMap=&texMap;
  paraboloid->frontAndBack=1;
  paraboloid->isLightSource=0;
  paraboloid->isMirror=0;
  paraboloid->goingOut=0;
}
 return(paraboloid);
}




struct object3D *newBox(double ra, double rd, double rs, double rg, double r, double g,
				double b, double alpha, double r_index, double shiny)
{
 // This is assumed to represent a unit box with center at the origin.
 // x=-1, x=1, y=-1, y=1, z=-1, z=1
 struct object3D *box=(struct object3D *)calloc(1,sizeof(struct object3D));

 if (!box) fprintf(stderr,"Unable to allocate new box, out of memory!\n");
 else
 {
  box->alb.ra=ra;
  box->alb.rd=rd;
  box->alb.rs=rs;
  box->alb.rg=rg;
  box->col.R=r;
  box->col.G=g;
  box->col.B=b;
  box->alpha=alpha;
  box->r_index=r_index;
  box->shinyness=shiny;
  box->intersect=&boxIntersect;
  box->texImg=NULL;
  memcpy(&box->T[0][0],&eye4x4[0][0],16*sizeof(double));
  memcpy(&box->Tinv[0][0],&eye4x4[0][0],16*sizeof(double));
  box->textureMap=&texMap;
  box->frontAndBack=0;
  box->isLightSource=0;
  box->isMirror=0;
  box->goingOut=0;
}
 return(box);
}


///////////////////////////////////////////////////////////////////////////////////////
//	Complete the functions that compute intersections for the canonical plane (Model world)
//      and canonical sphere with a given ray. This is the most fundamental component
//      of the raytracer.
///////////////////////////////////////////////////////////////////////////////////////
void planeIntersect(struct object3D *plane, struct ray3D *ray, double *lambda,
			struct point3D *_p, struct point3D *_n, double *a, double *b)
{
    //transform the ray into Model world
    matRayMult(plane->Tinv,ray);

    double x,y,t;
    struct point3D *p,*d;
    p=&(ray->p0);
    d=&(ray->d);
    *lambda=-1;
    
    if(d->pz==0){
    	matRayMult(plane->T,ray);
	return;
    }

    t = -(p->pz/d->pz);
    if(t<0){
	matRayMult(plane->T,ray);
	return;
    }

    x = p->px+t*d->px;
    y = p->py+t*d->py;

    //check the boundaries
    if(x>=-1 && x<=1 && y>=-1 && y<=1){
	//assign vectors in the model world
	*lambda=t;
	ray->rayPos(ray, t, _p);
	_n->px=0;
	_n->py=0;
	_n->pz=-1;
	_n->pw=0;

	if( a && b && plane->texImg!=NULL){
	    *a = (_p->px+1.0)/2.0;
	    *b = (_p->py+1.0)/2.0;
	    //printf("%.3f %.3f   ",*a,*b);
	}
    }
    //convert back the ray into the object world
    matRayMult(plane->T,ray);
}

void sphereIntersect(struct object3D *sphere, struct ray3D *ray, double *lambda, struct point3D *_p,
					struct point3D *_n, double *u, double *v)
{
    //transform the ray into Model world
    matRayMult(sphere->Tinv,ray);

    double A,B,C;
    double px,py,pz,dx,dy,dz;
    *lambda=-1;
    px=ray->p0.px;
    py=ray->p0.py;
    pz=ray->p0.pz;
    dx=ray->d.px;
    dy=ray->d.py;
    dz=ray->d.pz;
 
    A = dx*dx+dy*dy+dz*dz;
    B = (px*dx+py*dy+pz*dz)*2;
    C = px*px+py*py+pz*pz-1;
    double delta = B*B-4*A*C,t;
    if(A>0 && delta>=0){
    	if(delta==0){
	    //there is one root
	    t = -B/(2*A);
	}else{
	    //2 roots
	    //let t be the smaller root
	    double t1,t2;
	    delta = sqrt(delta);
	    t1 = (-B-delta)/A;
	    t2 = (-B+delta)/A;
	    if(t1>0) t=t1/2;
	    else t=t2/2;
	}
	if(t>0){
	    //t is a positive (valid) root
	    *lambda=t;
	    ray->rayPos(ray, t, _p);
	    _n->px=_p->px;
	    _n->py=_p->py;
	    _n->pz=_p->pz;
	    _n->pw=0;

	    //determine normal vector direction, i.e. towards the center or outwards
	    if(dot(_n,&(ray->d))>0){
		//the ray is shooting from inside the sphere to the world
		multVector(-1.0,_n);
		sphere->goingOut=1;
	    }else
		sphere->goingOut=0;

	    //compute the texture (u,v) coordinates
	    if(u && v && sphere->texImg != NULL && sphere->textureMap != NULL){
		    //compute the radius
		    double r = length(_p);
		    *v = 1.0 - std::acos(_p->py/r)/PI;
		    *u = 0.5 + std::atan2(_p->px,_p->pz)/(2.0*PI);
	    }
	}
    }

    //convert back the ray into the object world
    matRayMult(sphere->T,ray);

}



void coneIntersect(struct object3D *cone, struct ray3D *ray, double *lambda, struct point3D *_p,
					struct point3D *_n, double *u, double *v)
{
    //transform the ray into Model world
    matRayMult(cone->Tinv,ray);

    double A,B,C;
    double px,py,pz,dx,dy,dz;
    *lambda=-1;
    px=ray->p0.px;
    py=ray->p0.py;
    pz=ray->p0.pz;
    dx=ray->d.px;
    dy=ray->d.py;
    dz=ray->d.pz;
 
    A = dx*dx-dy*dy+dz*dz;
    B = (px*dx-py*dy+pz*dz)*2;
    C = px*px-py*py+pz*pz;
    double delta = B*B-4*A*C,t;
    if(A!=0 && delta>=0){
    	if(delta==0){
	    //there is one root
	    t = -B/(2*A);
	}else{
	    //2 roots
	    //let t be the smaller root
	    double t1,t2;
	    delta = sqrt(delta);
	    t1 = (-B-delta)/A;
	    t2 = (-B+delta)/A;
	    if(t1>0) t=t1/2;
	    else t=t2/2;
	}
	if(t>0){
	    //t is a positive (valid) root
	    //now check the bound of y
	    ray->rayPos(ray, t, _p);
	    if(_p->py>=-1 && _p->py<=0){
		    *lambda=t;
		    _n->px=_p->px;
		    _n->py=-_p->py;
		    _n->pz=_p->pz;
		    _n->pw=0;
	
		    if(dot(_n,&(ray->d))>0){
			//the ray is shooting from inside the sphere to the world
			multVector(-1.0,_n);
			cone->goingOut=1;
		    }else
			cone->goingOut=0;

		    //compute the texture (u,v) coordinates
		    if( u && v && cone->texImg != NULL && cone->textureMap != NULL){
			    //compute the radius
			    double r = sqrt(_p->px*_p->px+_p->pz*_p->pz);
			    *v = 1.0 + _p->py;
			    *u = 0.5 + std::atan2(_p->px,_p->pz)/(2.0*PI);
		    }
	    }
	}
    }

    //convert back the ray into the object world
    matRayMult(cone->T,ray);
}




void paraboloidIntersect(struct object3D *paraboloid, struct ray3D *ray, double *lambda, struct point3D *_p,
					struct point3D *_n, double *u, double *v)
{
    //transform the ray into Model world
    matRayMult(paraboloid->Tinv,ray);

    double A,B,C;
    double px,py,pz,dx,dy,dz;
    *lambda=-1;
    px=ray->p0.px;
    py=ray->p0.py;
    pz=ray->p0.pz;
    dx=ray->d.px;
    dy=ray->d.py;
    dz=ray->d.pz;
 
    A = dx*dx+dz*dz;
    B = (px*dx+pz*dz)*2+dy;
    C = px*px+pz*pz+py;
    double delta = B*B-4*A*C,t;
    if(A!=0 && delta>=0){
    	if(delta==0){
	    //there is one root
	    t = -B/(2*A);
	}else{
	    //2 roots
	    //let t be the smaller root
	    double t1,t2;
	    delta = sqrt(delta);
	    t1 = (-B-delta)/A;
	    t2 = (-B+delta)/A;
	    if(t1>0) t=t1/2;
	    else t=t2/2;
	}
	if(t>0){
	    //t is a positive (valid) root
	    //now check the bound of y
	    ray->rayPos(ray, t, _p);
	    if(_p->py>=-1 && _p->py<=0){
		    *lambda=t;
		    _n->px=_p->px*2;
		    _n->py=1.0;
		    _n->pz=_p->pz*2;
		    _n->pw=0;
	
		    if(dot(_n,&(ray->d))>0){
			//the ray is shooting from inside the sphere to the world
			multVector(-1.0,_n);
			paraboloid->goingOut=1;
		    }else
			paraboloid->goingOut=0;

		    //compute the texture (u,v) coordinates
/*		    if(paraboloid->texImg != NULL && paraboloid->textureMap != NULL){
			    //compute the radius
			    double r = sqrt(_p->px*_p->px+_p->pz*_p->pz);
			    *v = 1.0 + _p->py;
			    *u = 0.5 + std::atan2(_p->px,_p->pz)/(2.0*PI);
		    }
*/
	    }
	}
    }

    //convert back the ray into the object world
    matRayMult(paraboloid->T,ray);
}




void boxIntersect(struct object3D *box, struct ray3D *ray, double *lambda, struct point3D *_p,
					struct point3D *_n, double *u, double *v)
{
    //transform the ray into Model world
    matRayMult(box->Tinv,ray);

    double px,py,pz,dx,dy,dz,txy,tzy,tzx;
    *lambda=-1;
    txy=-1;
    tzy=-1;
    tzx=-1;
    px=ray->p0.px;
    py=ray->p0.py;
    pz=ray->p0.pz;
    dx=ray->d.px;
    dy=ray->d.py;
    dz=ray->d.pz;
    memset(_n,0,sizeof(struct point3D));
 
    //xy plane
    //if no valid txy, txy<=0
    if(dz!=0){
	struct point3D p1,p2;
	double t1,t2;
	t1=(1-pz)/dz;
	t2=(-1-pz)/dz; 
	//note t2 < t1 always
	if(t2<=0) txy=t1;
	else{
    	    ray->rayPos(ray,t2,&p2);
	    if(p2.px<=1 && p2.px>=-1 &&
		p2.py<=1 && p2.py>=-1)
		    txy=t2;
	    else
		txy=t1;
	}
	if(txy==t1){
	    if(t1>0){
    		ray->rayPos(ray,t1,&p1);
		if(p1.px>1 || p1.px<-1 ||
		    p1.py>1 || p1.py<-1)
			txy=-1;
	    }
	}
    }
    //zy plane
    //if no valid tzy, tzy<=0
    if(dx!=0){
	struct point3D p1,p2;
	double t1,t2;
	t1=(1-px)/dx;
	t2=(-1-px)/dx;
	if(t2<0) tzy=t1;
	else{
	    ray->rayPos(ray,t2,&p2);
	    if(p2.py<=1 && p2.py>=-1 &&
		p2.pz<=1 && p2.pz>=-1)
		    tzy=t2;
	    else tzy=t1;

	}
	if(tzy==t1){
	    if(t1>0){
    		ray->rayPos(ray,t1,&p1);
		if(p1.pz>1 || p1.pz<-1 ||
		    p1.py>1 || p1.py<-1)
			tzy=-1;
	    }
	}
    }
    //zx plane
    //if no valid tzx, tzx<=0
    if(dy!=0){
	struct point3D p1,p2;
	double t1,t2;
	t1=(1-py)/dy;
	t2=(-1-py)/dy; 
	//note t2 < t1 always
	if(t2<=0) tzx=t1;
	else{
    	    ray->rayPos(ray,t2,&p2);
	    if(p2.px<=1 && p2.px>=-1 &&
		p2.pz<=1 && p2.pz>=-1)
		    tzx=t2;
	    else
		tzx=t1;
	}
	if(tzx==t1){
	    if(t1>0){
    		ray->rayPos(ray,t1,&p1);
		if(p1.px>1 || p1.px<-1 ||
		    p1.pz>1 || p1.pz<-1)
			tzx=-1;
	    }
	}
    }

    double tmin=-1;
    if(txy>0) tmin=txy;
    else if(tzy>0) tmin=tzy;
    else if(tzx>0) tmin=tzx;
    else{
	matRayMult(box->T,ray);
	return;
    }

    if(tzy>0 && tzy<tmin) tmin=tzy;
    if(tzx>0 && tzx<tmin) tmin=tzx;


    //if reach here, we have a valid min root
    assert(tmin>0);
    *lambda=tmin;
    ray->rayPos(ray,tmin,_p);

    //set normal vectors
    if(tmin==txy){
	if(_p->pz>0) _n->pz=1;
	else _n->pz=-1;
	if(u && v && box->texImg!=NULL){
	    *u = _p->px/2+0.5;
	    *v = _p->py/2+0.5;
	}
    }else if(tmin==tzy){
	if(_p->px>0) _n->px=1;
	else _n->px=-1;

	if(u && v && box->texImg!=NULL){
	    *u = _p->pz/2+0.5;
	    *v = _p->py/2+0.5;
	}
    }else{
	assert(tmin==tzx);
	if(_p->py>0) _n->py=1;
	else _n->py=-1;

    	if(u && v && box->texImg!=NULL){
	    *u = _p->px/2+0.5;
	    *v = _p->pz/2+0.5;
	}
}

    if(dot(_n,&(ray->d))>0){
	//the ray is shooting from inside the sphere to the world
	multVector(-1.0,_n);
	box->goingOut=1;
    }else
	box->goingOut=0;

    //convert back the ray into the object world
    matRayMult(box->T,ray);
}




void loadTexture(struct object3D *o, const char *filename)
{
 // Load a texture image from file and assign it to the
 // specified object
 if (o!=NULL)
 {
  if (o->texImg!=NULL)	// We have previously loaded a texture
  {			// for this object, need to de-allocate it
   if (o->texImg->rgbdata!=NULL) free(o->texImg->rgbdata);
   free(o->texImg);
  }
  o->texImg=readPPMimage(filename);	// Allocate new texture
 }
}


/*
 Function to determine the colour of a textured object at
 the normalized texture coordinates (u,v).

 u and v are texture coordinates in [0 1].
 img is a pointer to the image structure holding the texture for
  a given object.

 The colour is returned in R, G, B. Uses bi-linear interpolation
 to determine texture colour.
*/
void texMap(struct image *img, double u, double v, double *R, double *G, double *B)
{
    assert(u<=1 && v<=1 && u>=0 && v>=0);
    if(!img || !(img->rgbdata)) return;
    double* rgb = (double *)img->rgbdata;
    double nx=img->sx;
    double ny=img->sy;
    double _u, _v, i, j;
    double *c00, *c01, *c10, *c11;

    i = floor(u*nx);
    j = floor(v*ny);
    _u = u*nx -i;
    _v = v*ny -j;

    c00 = (rgb+(int)i*3+(int)(j*nx)*3);
    c10 = (rgb+(int)i*3+3+(int)(j*nx)*3);
    //in case the last row
    if(j==(ny-1)){
	c01=c00;
	c11=c10;
    }else{
    	c01 = (rgb+(int)i*3+(int)(j*nx)*3+(int)nx*3);
       	c11 = (rgb+(int)i*3+3+(int)(j*nx)*3+(int)nx*3);
    }

    *R = (1-_u)*(1-_v)*(*c00) + _u*(1-_v)*(*c10)
	+(1-_u)*_v*(*c01) + _u*_v*(*c11);;
    *G = (1-_u)*(1-_v)*(*(c00+1)) + _u*(1-_v)*(*(c10+1))
	+(1-_u)*_v*(*(c01+1)) + _u*_v*(*(c11+1));;
    *B = (1-_u)*(1-_v)*(*(c00+2)) + _u*(1-_v)*(*(c10+2))
	+(1-_u)*_v*(*(c01+2)) + _u*_v*(*(c11+2));;
}

void insertObject(struct object3D *o, struct object3D **list)
{
 if (o==NULL) return;
 // Inserts an object into the object list.
 if (*(list)==NULL)
 {
  *(list)=o;
  (*(list))->next=NULL;
 }
 else
 {
  o->next=(*(list))->next;
  (*(list))->next=o;
 }
}


void addAreaLight(float sx, float sy, float nx, float ny, float nz,\
                  float tx, float ty, float tz, int lx, int ly,\
                  float r, float g, float b, struct object3D **o_list, struct object3D **l_list)
{
 /*
   This function sets up and inserts a rectangular area light source
   with size (sx, sy)
   orientation given by the normal vector (nx, ny, nz)
   centered at (tx, ty, tz)
   consisting of (lx x ly) point light sources (uniformly sampled)
   and with colour (r,g,b) - which also determines intensity

   Note that the light source is visible as a uniformly colored rectangle and
   casts no shadow. If you require a lightsource to shade another, you must
   make it into a proper solid box with backing and sides of non-light-emitting
   material
 */

  /////////////////////////////////////////////////////
  // TO DO: (Assignment 4!)
  // Implement this function to enable area light sources
  /////////////////////////////////////////////////////

}

///////////////////////////////////
// Geometric transformation section
///////////////////////////////////
void transpose(double *T, double *Ttrans){
    int i, j;
    for(i=0;i<4;i++){
	for(j=0;j<4;j++){
	    *(Ttrans+4*j+i) = *(T+i*4+j);
	}
    }
}


// Computes the inverse of transformation matrix T.
// the result is returned in Tinv.
void invert(double *T, double *Tinv)
{
 float *U, *s, *V, *rv1;
 int singFlag, i;
 float T3x3[3][3],Tinv3x3[3][3];
 double tx,ty,tz;

 // Because of the fact we're using homogeneous coordinates, we must be careful how
 // we invert the transformation matrix. What we need is the inverse of the
 // 3x3 Affine transform, and -1 * the translation component. If we just invert
 // the entire matrix, junk happens.
 // So, we need a 3x3 matrix for inversion:
 T3x3[0][0]=(float)*(T+(0*4)+0);
 T3x3[0][1]=(float)*(T+(0*4)+1);
 T3x3[0][2]=(float)*(T+(0*4)+2);
 T3x3[1][0]=(float)*(T+(1*4)+0);
 T3x3[1][1]=(float)*(T+(1*4)+1);
 T3x3[1][2]=(float)*(T+(1*4)+2);
 T3x3[2][0]=(float)*(T+(2*4)+0);
 T3x3[2][1]=(float)*(T+(2*4)+1);
 T3x3[2][2]=(float)*(T+(2*4)+2);
 // Happily, we don't need to do this often.
 // Now for the translation component:
 tx=-(*(T+(0*4)+3));
 ty=-(*(T+(1*4)+3));
 tz=-(*(T+(2*4)+3));

 // Invert the affine transform
 U=NULL;
 s=NULL;
 V=NULL;
 rv1=NULL;
 singFlag=0;

 SVD(&T3x3[0][0],3,3,&U,&s,&V,&rv1);
 if (U==NULL||s==NULL||V==NULL)
 {
  fprintf(stderr,"Error: Matrix not invertible for this object, returning identity\n");
  memcpy(Tinv,eye4x4,16*sizeof(double));
  return;
 }

 // Check for singular matrices...
 for (i=0;i<3;i++) if (*(s+i)<1e-9) singFlag=1;
 if (singFlag)
 {
  fprintf(stderr,"Error: Transformation matrix is singular, returning identity\n");
  memcpy(Tinv,eye4x4,16*sizeof(double));
  return;
 }

 // Compute and store inverse matrix
 InvertMatrix(U,s,V,3,&Tinv3x3[0][0]);

 // Now stuff the transform into Tinv
 *(Tinv+(0*4)+0)=(double)Tinv3x3[0][0];
 *(Tinv+(0*4)+1)=(double)Tinv3x3[0][1];
 *(Tinv+(0*4)+2)=(double)Tinv3x3[0][2];
 *(Tinv+(1*4)+0)=(double)Tinv3x3[1][0];
 *(Tinv+(1*4)+1)=(double)Tinv3x3[1][1];
 *(Tinv+(1*4)+2)=(double)Tinv3x3[1][2];
 *(Tinv+(2*4)+0)=(double)Tinv3x3[2][0];
 *(Tinv+(2*4)+1)=(double)Tinv3x3[2][1];
 *(Tinv+(2*4)+2)=(double)Tinv3x3[2][2];
 *(Tinv+(0*4)+3)=Tinv3x3[0][0]*tx + Tinv3x3[0][1]*ty + Tinv3x3[0][2]*tz;
 *(Tinv+(1*4)+3)=Tinv3x3[1][0]*tx + Tinv3x3[1][1]*ty + Tinv3x3[1][2]*tz;
 *(Tinv+(2*4)+3)=Tinv3x3[2][0]*tx + Tinv3x3[2][1]*ty + Tinv3x3[2][2]*tz;
 *(Tinv+(3*4)+3)=1;

 free(U);
 free(s);
 free(V);
}

void RotateX(struct object3D *o, double theta)
{
 // Multiply the current object transformation matrix T in object o
 // by a matrix that rotates the object theta *RADIANS* around the
 // X axis.

 double R[4][4];
 memset(&R[0][0],0,16*sizeof(double));

 R[0][0]=1.0;
 R[1][1]=cos(theta);
 R[1][2]=-sin(theta);
 R[2][1]=sin(theta);
 R[2][2]=cos(theta);
 R[3][3]=1.0;

 matMult(R,o->T);
}

void RotateY(struct object3D *o, double theta)
{
 // Multiply the current object transformation matrix T in object o
 // by a matrix that rotates the object theta *RADIANS* around the
 // Y axis.

 double R[4][4];
 memset(&R[0][0],0,16*sizeof(double));

 R[0][0]=cos(theta);
 R[0][2]=sin(theta);
 R[1][1]=1.0;
 R[2][0]=-sin(theta);
 R[2][2]=cos(theta);
 R[3][3]=1.0;

 matMult(R,o->T);
}

void RotateZ(struct object3D *o, double theta)
{
 // Multiply the current object transformation matrix T in object o
 // by a matrix that rotates the object theta *RADIANS* around the
 // Z axis.

 double R[4][4];
 memset(&R[0][0],0,16*sizeof(double));

 R[0][0]=cos(theta);
 R[0][1]=-sin(theta);
 R[1][0]=sin(theta);
 R[1][1]=cos(theta);
 R[2][2]=1.0;
 R[3][3]=1.0;

 matMult(R,o->T);
}

void Translate(struct object3D *o, double tx, double ty, double tz)
{
 // Multiply the current object transformation matrix T in object o
 // by a matrix that translates the object by the specified amounts.

 double tr[4][4];
 memset(&tr[0][0],0,16*sizeof(double));

 tr[0][0]=1.0;
 tr[1][1]=1.0;
 tr[2][2]=1.0;
 tr[0][3]=tx;
 tr[1][3]=ty;
 tr[2][3]=tz;
 tr[3][3]=1.0;

 matMult(tr,o->T);
}

void Scale(struct object3D *o, double sx, double sy, double sz)
{
 // Multiply the current object transformation matrix T in object o
 // by a matrix that scales the object as indicated.

 double S[4][4];
 memset(&S[0][0],0,16*sizeof(double));

 S[0][0]=sx;
 S[1][1]=sy;
 S[2][2]=sz;
 S[3][3]=1.0;

 matMult(S,o->T);
}

void printmatrix(double mat[4][4])
{
 fprintf(stderr,"Matrix contains:\n");
 fprintf(stderr,"%f %f %f %f\n",mat[0][0],mat[0][1],mat[0][2],mat[0][3]);
 fprintf(stderr,"%f %f %f %f\n",mat[1][0],mat[1][1],mat[1][2],mat[1][3]);
 fprintf(stderr,"%f %f %f %f\n",mat[2][0],mat[2][1],mat[2][2],mat[2][3]);
 fprintf(stderr,"%f %f %f %f\n",mat[3][0],mat[3][1],mat[3][2],mat[3][3]);
}

/////////////////////////////////////////
// Camera and view setup
/////////////////////////////////////////
struct view *setupView(struct point3D *e, struct point3D *g, struct point3D *up, double f, double wl, double wt, double wsize)
{
 /*
   This function sets up the camera axes and viewing direction as discussed in the
   lecture notes.
   e - Camera center
   g - Gaze direction
   up - Up vector
   fov - Fild of view in degrees
   f - focal length
 */
 struct view *c;
 struct point3D *u, *v;

 u=v=NULL;

 // Allocate space for the camera structure
 c=(struct view *)calloc(1,sizeof(struct view));
 if (c==NULL)
 {
  fprintf(stderr,"Out of memory setting up camera model!\n");
  return(NULL);
 }

 // Set up camera center and axes
 c->e.px=e->px;		// Copy camera center location, note we must make sure
 c->e.py=e->py;		// the camera center provided to this function has pw=1
 c->e.pz=e->pz;
 c->e.pw=1;

 // Set up w vector (camera's Z axis). w=-g/||g||
 c->w.px=-g->px;
 c->w.py=-g->py;
 c->w.pz=-g->pz;
 c->w.pw=0;
 normalize(&c->w);

 // Set up the horizontal direction, which must be perpenticular to w and up
 u=cross(&c->w, up);
 normalize(u);
 c->u.px=u->px;
 c->u.py=u->py;
 c->u.pz=u->pz;
 c->u.pw=0;

 // Set up the remaining direction, v=(u x w)  - Mind the signs
 v=cross(&c->u, &c->w);
 normalize(v);
 c->v.px=v->px;
 c->v.py=v->py;
 c->v.pz=v->pz;
 c->v.pw=0;

 // Copy focal length and window size parameters
 c->f=f;
 c->wl=wl;
 c->wt=wt;
 c->wsize=wsize;

 // Set up coordinate conversion matrices
 // Camera2World matrix (M_cw in the notes)
 // Mind the indexing convention [row][col]
 c->C2W[0][0]=c->u.px;
 c->C2W[1][0]=c->u.py;
 c->C2W[2][0]=c->u.pz;
 c->C2W[3][0]=0;

 c->C2W[0][1]=c->v.px;
 c->C2W[1][1]=c->v.py;
 c->C2W[2][1]=c->v.pz;
 c->C2W[3][1]=0;

 c->C2W[0][2]=c->w.px;
 c->C2W[1][2]=c->w.py;
 c->C2W[2][2]=c->w.pz;
 c->C2W[3][2]=0;

 c->C2W[0][3]=c->e.px;
 c->C2W[1][3]=c->e.py;
 c->C2W[2][3]=c->e.pz;
 c->C2W[3][3]=1;

 // World2Camera matrix (M_wc in the notes)
 // Mind the indexing convention [row][col]
 c->W2C[0][0]=c->u.px;
 c->W2C[1][0]=c->v.px;
 c->W2C[2][0]=c->w.px;
 c->W2C[3][0]=0;

 c->W2C[0][1]=c->u.py;
 c->W2C[1][1]=c->v.py;
 c->W2C[2][1]=c->w.py;
 c->W2C[3][1]=0;

 c->W2C[0][2]=c->u.pz;
 c->W2C[1][2]=c->v.pz;
 c->W2C[2][2]=c->w.pz;
 c->W2C[3][2]=0;

 c->W2C[0][3]=-dot(&c->u,&c->e);
 c->W2C[1][3]=-dot(&c->v,&c->e);
 c->W2C[2][3]=-dot(&c->w,&c->e);
 c->W2C[3][3]=1;

 free(u);
 free(v);
 return(c);
}

/////////////////////////////////////////
// Image I/O section
/////////////////////////////////////////
struct image *readPPMimage(const char *filename)
{
 // Reads an image from a .ppm file. A .ppm file is a very simple image representation
 // format with a text header followed by the binary RGB data at 24bits per pixel.
 // The header has the following form:
 //
 // P6
 // # One or more comment lines preceded by '#'
 // 340 200
 // 255
 //
 // The first line 'P6' is the .ppm format identifier, this is followed by one or more
 // lines with comments, typically used to inidicate which program generated the
 // .ppm file.
 // After the comments, a line with two integer values specifies the image resolution
 // as number of pixels in x and number of pixels in y.
 // The final line of the header stores the maximum value for pixels in the image,
 // usually 255.
 // After this last header line, binary data stores the RGB values for each pixel
 // in row-major order. Each pixel requires 3 bytes ordered R, G, and B.
 //
 // NOTE: Windows file handling is rather crotchetty. You may have to change the
 //       way this file is accessed if the images are being corrupted on read
 //       on Windows.
 //
 // readPPMdata converts the image colour information to floating point. This is so that
 // the texture mapping function doesn't have to do the conversion every time
 // it is asked to return the colour at a specific location.
 //

 FILE *f;
 struct image *im;
 char line[1024];
 int sizx,sizy;
 int i;
 unsigned char *tmp;
 double *fRGB;

 im=(struct image *)calloc(1,sizeof(struct image));
 if (im!=NULL)
 {
  im->rgbdata=NULL;
  f=fopen(filename,"rb+");
  if (f==NULL)
  {
   fprintf(stderr,"Unable to open file %s for reading, please check name and path\n",filename);
   free(im);
   return(NULL);
  }
  fgets(&line[0],1000,f);
  if (strcmp(&line[0],"P6\n")!=0)
  {
   fprintf(stderr,"Wrong file format, not a .ppm file or header end-of-line characters missing\n");
   free(im);
   fclose(f);
   return(NULL);
  }
  fprintf(stderr,"%s\n",line);
  // Skip over comments
  fgets(&line[0],511,f);
  while (line[0]=='#')
  {
   fprintf(stderr,"%s",line);
   fgets(&line[0],511,f);
  }
  sscanf(&line[0],"%d %d\n",&sizx,&sizy);           // Read file size
  fprintf(stderr,"nx=%d, ny=%d\n\n",sizx,sizy);
  im->sx=sizx;
  im->sy=sizy;

  fgets(&line[0],9,f);  	                // Read the remaining header line
  fprintf(stderr,"%s\n",line);
  tmp=(unsigned char *)calloc(sizx*sizy*3,sizeof(unsigned char));
  fRGB=(double *)calloc(sizx*sizy*3,sizeof(double));
  if (tmp==NULL||fRGB==NULL)
  {
   fprintf(stderr,"Out of memory allocating space for image\n");
   free(im);
   fclose(f);
   return(NULL);
  }

  fread(tmp,sizx*sizy*3*sizeof(unsigned char),1,f);
  fclose(f);

  // Conversion to floating point
  for (i=0; i<sizx*sizy*3; i++) *(fRGB+i)=((double)*(tmp+i))/255.0;
  free(tmp);
  im->rgbdata=(void *)fRGB;

  return(im);
 }

 fprintf(stderr,"Unable to allocate memory for image structure\n");
 return(NULL);
}

struct image *newImage(int size_x, int size_y)
{
 // Allocates and returns a new image with all zeros. Assumes 24 bit per pixel,
 // unsigned char array.
 struct image *im;

 im=(struct image *)calloc(1,sizeof(struct image));
 if (im!=NULL)
 {
  im->rgbdata=NULL;
  im->sx=size_x;
  im->sy=size_y;
  im->rgbdata=(void *)calloc(size_x*size_y*3,sizeof(unsigned char));
  if (im->rgbdata!=NULL) return(im);
 }
 fprintf(stderr,"Unable to allocate memory for new image\n");
 return(NULL);
}

void imageOutput(struct image *im, const char *filename)
{
 // Writes out a .ppm file from the image data contained in 'im'.
 // Note that Windows typically doesn't know how to open .ppm
 // images. Use Gimp or any other seious image processing
 // software to display .ppm images.
 // Also, note that because of Windows file format management,
 // you may have to modify this file to get image output on
 // Windows machines to work properly.
 //
 // Assumes a 24 bit per pixel image stored as unsigned chars
 //

 FILE *f;
 if (im!=NULL)
  if (im->rgbdata!=NULL)
  {
   f=fopen(filename,"wb+");
   if (f==NULL)
   {
    fprintf(stderr,"Unable to open file %s for output! No image written\n",filename);
    return;
   }
   fprintf(f,"P6\n");
   fprintf(f,"# Output from RayTracer.c\n");
   fprintf(f,"%d %d\n",im->sx,im->sy);
   fprintf(f,"255\n");
   fwrite((unsigned char *)im->rgbdata,im->sx*im->sy*3*sizeof(unsigned char),1,f);
   fclose(f);
   return;
  }
 fprintf(stderr,"imageOutput(): Specified image is empty. Nothing output\n");
}

void deleteImage(struct image *im)
{
 // De-allocates memory reserved for the image stored in 'im'
 if (im!=NULL)
 {
  if (im->rgbdata!=NULL) free(im->rgbdata);
  free(im);
 }
}

void cleanup(struct object3D *o_list)
{
 // Everything is stored as object3D including light sources
 // De-allocates memory reserved for the object list
 // list. Note that *YOU* must de-allocate any memory reserved for images
 // rendered by the raytracer.
 struct object3D *p, *q;

 p=o_list;		// De-allocate all memory from objects in the list
 while(p!=NULL)
 {
  q=p->next;
  if (p->texImg!=NULL)
  {
   if (p->texImg->rgbdata!=NULL) free(p->texImg->rgbdata);
   free(p->texImg);
  }
  if(p->children!=NULL){
    cleanup(p->children);
  }
  free(p);
  p=q;
 }
}



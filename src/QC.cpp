#include<iostream>
#include<cmath>
#include<cstdlib>
#include<cstdio>
#include<vector>
#include<ctime>
#include<windows.h>
#include "GL/glut.h"
#include<sstream>
using namespace std;

#define FRAMES1PERQUANT 1
#define FRAMES2PERQUANT 1
#define FRAMES3 30
#define FRAMES4 15
#define FRAMES5 15
#define FRAMES6 20

GLint winw=700,winh=500;
int interval,level,quant,start,intro,guessyet,guess,remain,groupnum,timestart,run,subn;
string startat="Start at ",difficultylevel="Difficulty level: ",quantity="Quantity: ",strcont="Continue";

struct buttontype
{
	GLint x;
	GLint xend;
	GLint y;
	GLint yend;
};

buttontype startbutton={220,310,300,330};
buttontype contbutton={winw-100,winw,winh-30,winh};
buttontype nextstep={155,310,300,330};

struct incrementboxtype
{
	GLint x;
	GLint y;
	GLint arrowwidth;
	GLint boxwidth;
	GLint height;
};
incrementboxtype guessbox={winw-150,winh-80,20,30,20};
incrementboxtype diffbox={230,197,20,30,20};
incrementboxtype startatbox={230,227,20,30,20};

struct pointtype
{
	GLint x;
	GLint y;
	GLfloat size;
	GLfloat c[3];
};
vector<pointtype> point;

bool inrange(int testvar,int lower,int upper)
{
	if(lower<=testvar && testvar<=upper)
		return 1;
	else return 0;
}

void dispstring(GLfloat r,GLfloat g,GLfloat b,int x,int y,string text)
{
	int n;
	
	glPushMatrix();
	glColor3f(r,g,b);
	glRasterPos2i(x,y);
	for(n=0;n<text.size();n++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,text[n]);
	glPopMatrix();
}

void dispbox(incrementboxtype *box,int value)
{
	char tempchar[3];
	string tempstring;
	
	glColor3f(.7,.7,.7);
	glRecti(box->x,box->y,box->x+box->arrowwidth,box->y+box->height);
	glRecti(box->x+box->arrowwidth+4+box->boxwidth,box->y,box->x+2*box->arrowwidth+4+box->boxwidth,box->y+box->height);
	glColor3f(.9,.9,.9);
	glRecti(box->x+box->arrowwidth+2,box->y,box->x+box->arrowwidth+2+box->boxwidth,box->y+box->height);
	
	glColor3f(0,0,0);
	glBegin(GL_POLYGON);
		glVertex2i(box->x+box->arrowwidth-3,box->y+3);
		glVertex2i(box->x+3,box->y+(box->height/2));
		glVertex2i(box->x+box->arrowwidth-3,box->y+box->height-3);
	glEnd();
	glBegin(GL_POLYGON);
		glVertex2i(box->x+box->arrowwidth+4+box->boxwidth+3,box->y+3);
		glVertex2i(box->x+2*box->arrowwidth+4+box->boxwidth-3,box->y+(box->height/2));
		glVertex2i(box->x+box->arrowwidth+4+box->boxwidth+3,box->y+box->height-3);
	glEnd();
	
	sprintf(tempchar,"%02d",value);
	tempstring=tempchar;
	dispstring(0,0,0,box->x+box->arrowwidth+5,box->y+box->height-3,tempstring);
}

void resize (int w, int h)
{
	winw=w; 
	winh=h;
	contbutton.x=winw-100;
	contbutton.xend=winw;
	contbutton.y=winh-30;
	contbutton.yend=winh;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,winw,winh,0);
	glViewport(0,0,winw,winh);
}

void init(void)
{
	glClearColor(.2,.2,.2,1);
	glShadeModel(GL_SMOOTH);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,winw,winh,0);
	
	glMatrixMode(GL_MODELVIEW);
	glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);

	start=intro=0;
	intro=1;
	level=1;
	quant=3;
	srand(time(NULL));
	
	//create noise image
	int n,m;
	glNewList(2,GL_COMPILE);
		glClear(GL_COLOR_BUFFER_BIT);
		glPointSize(10);
		glBegin(GL_POINTS);
			for(n=0;n<winw;n+=10)
			{
				for(m=0;m<winh;m+=10)
				{
					glColor3i(rand(),rand(),rand());
					glVertex2i(n,m);
				}
			}
		glEnd();
		glPointSize(1);
	glEndList();
	
	glFlush();
}

void mouseaction(int button, int state, int x, int y)
{
	int n;
	
	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
	{
		if(intro==0)
		{
			if(inrange(x,diffbox.x,diffbox.x+diffbox.arrowwidth)&&inrange(y,diffbox.y,diffbox.y+diffbox.height)&&level>1)
				level--;
			else if(inrange(x,diffbox.x+diffbox.arrowwidth+diffbox.boxwidth+4,diffbox.x+2*diffbox.arrowwidth+diffbox.boxwidth+4)&&inrange(y,diffbox.y,diffbox.y+diffbox.height)&&level<8)
				level++;
			else if(inrange(x,startatbox.x,startatbox.x+startatbox.arrowwidth)&&inrange(y,startatbox.y,startatbox.y+startatbox.height)&&quant>3)
				quant--;
			else if(inrange(x,startatbox.x+startatbox.arrowwidth+startatbox.boxwidth+4,startatbox.x+2*startatbox.arrowwidth+startatbox.boxwidth+4)&&inrange(y,startatbox.y,startatbox.y+startatbox.height)&&quant<20)
				quant++;
			else if(inrange(x,startbutton.x,startbutton.xend)&&inrange(y,startbutton.y,startbutton.yend))
			{
				intro=guessyet=1;
				subn=0;
				interval=(13-level)*3000*((float)CLOCKS_PER_SEC/200000.0);
				start=0;
				guessyet=-1;
			}
		}
		else if(subn==6)
		{
			if(inrange(x,nextstep.x,nextstep.xend)&&inrange(y,nextstep.y,nextstep.yend))
			{
				level++;
				interval=(13-level)*3000*((float)CLOCKS_PER_SEC/200000.0);
				subn=0;
				start=0;
			}
			else if(inrange(x,nextstep.x,nextstep.xend)&&inrange(y,nextstep.y+50,nextstep.yend+50))
			{
				quant++;
				subn=0;
				start=0;
			}
			else if(inrange(x,nextstep.x,nextstep.xend)&&inrange(y,nextstep.y+100,nextstep.yend+100))
			{
				exit(0);
			}
		}
		else if(intro==1&&start==0&&inrange(x,contbutton.x,contbutton.xend)&&inrange(y,contbutton.y,contbutton.yend))
		{
			start=1;
			run=0;
			guessyet=-1;
		}
		else if(start==1&&guessyet==0&&subn>1)
		{
			if(inrange(x,contbutton.x,contbutton.xend)&&inrange(y,contbutton.y,contbutton.yend))
				guessyet=1;
			else if(inrange(x,guessbox.x,guessbox.x+guessbox.arrowwidth)&&inrange(y,guessbox.y,guessbox.y+guessbox.height)&&guess>0)
				guess--;
			else if(inrange(x,guessbox.x+guessbox.arrowwidth+guessbox.boxwidth+4,guessbox.x+2*guessbox.arrowwidth+guessbox.boxwidth+4)&&inrange(y,guessbox.y,guessbox.y+guessbox.height)&&guess<200)
				guess++;
		}
	}
}

void controls(unsigned char key, int x, int y)
{
	
}

void dispfooter(bool cont)
{
	char tempchar[100];
	string footertext;
	
	glColor3f(.7,.7,.7);
	glRecti(0,winh-30,winw,winh);
	
	if(intro==0)
		sprintf(tempchar,"Quantity: %d    Level: %d",quant,level);
	else
		sprintf(tempchar,"Quantity: %d       Level: %d       Part: %d of 6",quant,level,subn+1);
	footertext=tempchar;
	dispstring(0,0,0,10,winh-7,footertext);
	
	if(cont)
	{
		glColor3f(0,1,0);
		glRecti(contbutton.x,contbutton.y,contbutton.xend,contbutton.yend);
		dispstring(0,0,0,contbutton.x+7,contbutton.yend-7,"Continue");
	}
}

void points(int num)
{
	int n,m,good=0,color;
	pointtype temppoint;
	
	while(good==0)
	{
		point.clear();
		for(n=0;n<num;n++)
		{
			temppoint.x=(rand()%(winw-winw/7))+winw/14;
			temppoint.y=(rand()%(winh-winh/7-30))+winh/14;
			temppoint.size=(float)(rand()%5)+(winw/70);
			point.push_back(temppoint);
		}
		good=1;
		for(n=0;n<num;n++)
		{
			for(m=0;m<num;m++)
			{
				if(n==m)
					continue;
				else if(sqrt(pow((double)point[n].x-(double)point[m].x,2)+pow((double)point[n].y-(double)point[m].y,2))<=(point[n].size+point[m].size)+4)
				{
					good=0;
					break;
				}
			}
			if(good==0)
				break;
		}
	}
	for(n=0;n<num;n++)
	{
		color=rand()%3;
		point[n].c[color]=1;
		point[n].c[(color+1)%3]=(GLfloat)((rand()%100)/99.0);
		point[n].c[(color+2)%3]=(GLfloat)((rand()%100)/99.0);
		point[n].c[(color+((rand()%2)+1))%3]=0;
	}
	
	glNewList(1,GL_COMPILE);
		glClear(GL_COLOR_BUFFER_BIT);
		for(n=0;n<point.size();n++)
		{
			glColor3f(point[n].c[0],point[n].c[1],point[n].c[2]);
			glBegin(GL_POLYGON);
				for(m=0;m<50;m++)
					glVertex2f(point[n].x+(sin((float)m*6.283/50.0)*point[n].size),point[n].y+(cos((float)m*6.283/50.0)*point[n].size));
			glEnd();
		}
		dispfooter(0);
	glEndList();	
}

void disppoints(int pause,bool noise)
{
	int timeend=clock()+pause;
	
	glCallList(1);
	glFlush();
	while(clock()<timeend);
	
	if(noise)
	{
		glCallList(2);
		dispfooter(0);
		glFlush();
		while(clock()-timeend<200000);
	}
}

void introscreen()
{
	dispstring(.9,.9,.9,100,213,"Difficulty Level ");
	dispstring(.9,.9,.9,100,243,"           Start at                    points");
	
	dispbox(&diffbox,level);
	dispbox(&startatbox,quant);
	
	glColor3f(0,1,0);
	glRecti(startbutton.x,startbutton.y,startbutton.xend,startbutton.yend);
	dispstring(0,0,0,startbutton.x+23,startbutton.yend-8,"Start");
}

void subscreen()
{
	char temptext[100];
	string title;
	int n;
	
	switch(subn)
	{
		case 0:
			if(start==0)
			{
				sprintf(temptext,"Part 1: Learn what %d looks like by",quant);
				title=temptext;
				dispstring(.9,.9,.9,150,200,title);
				sprintf(temptext,"observing various configurations of %d.",quant);
				title=temptext;
				dispstring(.9,.9,.9,150,220,title);
				dispfooter(1);
			}
			else
			{
				if(run<=FRAMES1PERQUANT*quant)
				{
					points(quant);
					disppoints(quant*interval,0);
					run++;
				}
				else
				{
					start=0;
					run=0;
					subn++;
				}
			}
			break;
		case 1:
			if(start==0)
			{
				sprintf(temptext,"Part 2:  Pick out any group of %d in",quant);
				title=temptext;
				dispstring(.9,.9,.9,150,200,title);
				sprintf(temptext,"these sets and ignore all other points.");
				title=temptext;
				dispstring(.9,.9,.9,150,220,title);
				dispfooter(1);
			}
			else
			{
				//dispdelay=700*700/(winw*winh);
				if(run<=FRAMES2PERQUANT*quant)
				{
					points(2*quant+(rand()%(2*quant)));
					disppoints(quant*interval,0);
					run++;
				}
				else
				{
					start=0;
					run=0;
					subn++;
				}
			}
			break;
		case 2:
			if(start==0)
			{
				sprintf(temptext,"Part 3:  The following sets contain only ONE");
				title=temptext;
				dispstring(.9,.9,.9,150,200,title);
				sprintf(temptext,"group of %d.  Pick out a group of %d, exclude",quant,quant);
				title=temptext;
				dispstring(.9,.9,.9,150,220,title);
				sprintf(temptext,"it, and count the remainder.");
				title=temptext;
				dispstring(.9,.9,.9,150,240,title);
				dispfooter(1);
			}
			else if(run<=FRAMES3)
				{
					if(guessyet==0)
					{
						dispstring(.9,.9,.9,guessbox.x-130,guessbox.y+16,"Remainder ");	
						dispbox(&guessbox,guess);
						dispfooter(1);
					}
					else if(guessyet==1)
					{
						if(guess!=remain) run=0;
						else run++;
						guessyet=-1;
					}
					else
					{
						remain=rand()%quant;
						points(quant+remain);
						disppoints(quant*interval,1);
						guessyet=guess=0;
					}
				}
			else
			{
				start=0;
				run=0;
				subn++;
			}
			break;
		case 3:
			if(start==0)
			{
				sprintf(temptext,"Part 4:  Count groups of %d.",quant);
				title=temptext;
				dispstring(.9,.9,.9,150,200,title);
				sprintf(temptext,"There will be no remainders.");
				title=temptext;
				dispstring(.9,.9,.9,150,220,title);
				dispfooter(1);
			}
			else if(run<=FRAMES4)
			{
				if(guessyet==0)
				{
					dispstring(.9,.9,.9,guessbox.x-130,guessbox.y+16,"Remainder ");	
					dispbox(&guessbox,guess);
					dispfooter(1);
				}
				else if(guessyet==1)
				{
					if(guess!=groupnum) run=0;
					else run++;
					guessyet=-1;
				}
				else
				{
					groupnum=(rand()%3)+2;
					points(quant*groupnum);
					disppoints(quant*interval*((float)groupnum*.9),1);
					guessyet=guess=0;
				}
			}
			else
			{
				start=0;
				run=0;
				subn++;
			}
			break;
		case 4:
			if(start==0)
			{
				sprintf(temptext,"Part 5:  Exclude multiple groups of %d",quant);
				title=temptext;
				dispstring(.9,.9,.9,150,200,title);
				sprintf(temptext,"and count the remainder.");
				title=temptext;
				dispstring(.9,.9,.9,150,220,title);
				dispfooter(1);
			}
			else if(run<=FRAMES5)
			{
				if(guessyet==0)
				{
					dispstring(.9,.9,.9,guessbox.x-130,guessbox.y+16,"Remainder ");	
					dispbox(&guessbox,guess);
					dispfooter(1);
				}
				else if(guessyet==1)
				{
					if(guess!=remain) run=0;
					else run++;
					guessyet=-1;
				}
				else
				{
						groupnum=(3.0*run/15.0)+2;
						remain=rand()%quant;
						guessyet=guess=0;
						points(quant*groupnum+remain);
						disppoints(quant*interval*((float)groupnum*.9),1);
				}
			}
			else
			{
				start=0;
				run=0;
				subn++;
			}
			break;
		case 5:
			if(start==0)
			{
				sprintf(temptext,"Part 6:  Find total by counting groups");
				title=temptext;
				dispstring(.9,.9,.9,150,200,title);
				sprintf(temptext,"of %d and then counting the remainder.",quant);
				title=temptext;
				dispstring(.9,.9,.9,150,220,title);
				dispfooter(1);
			}
			else if(run<=FRAMES6)
			{
				if(guessyet==0)
				{
					dispstring(.9,.9,.9,guessbox.x-130,guessbox.y+16,"Remainder ");	
					dispbox(&guessbox,guess);
					dispfooter(1);
				}
				else if(guessyet==1)
				{
					if(guess==point.size()) run++;
					else if(run>3)run-=3;
					guessyet=-1;
				}
				else
				{
						groupnum=(2.0*run/20.0)+3;
						remain=(rand()%(quant+2))-1;
						guessyet=guess=0;
						points(groupnum*quant+remain);
						disppoints(quant*interval*((float)groupnum*.9),1);
				}
			}
			else
			{
				start=0;
				run=0;
				subn++;
			}
			break;
		case 6:
			glColor3f(.8,.8,.8);
			glRecti(nextstep.x,nextstep.y,nextstep.xend,nextstep.yend);
			dispstring(0,0,0,nextstep.x+5,nextstep.yend-8,"Increase Difficulty");
			
			glColor3f(.8,.8,.8);
			glRecti(nextstep.x,nextstep.y+50,nextstep.xend,nextstep.yend+50);
			dispstring(0,0,0,nextstep.x+5,nextstep.yend+50-8,"Increase Quantity");
			
			glColor3f(1,.7,.7);
			glRecti(nextstep.x,nextstep.y+100,nextstep.xend,nextstep.yend+100);
			dispstring(0,0,0,nextstep.x+60,nextstep.yend+100-8,"Quit");
			break;
	}
	glFlush();
}               

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,winw,winh,0);
	glMatrixMode(GL_MODELVIEW);
	
	if(intro==0)
		introscreen();
	else
		subscreen();
		
	glutSwapBuffers();
}
	
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(winw,winh);;
	glutInitWindowPosition(250,150);
	glutCreateWindow("Qualitative Counting v1.0");
	init();
	
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(controls);
	glutMouseFunc(mouseaction);
	glutMainLoop();

	return 0;
}

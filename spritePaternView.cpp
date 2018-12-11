#include "spritePaternView.hh"
#include <QPushButton>
#include <QString>
#include <QPainter>
#include <QFont>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


spritePaternView::spritePaternView(QWidget *parent) : QWidget(parent),image(25*5,25,QImage::Format_Indexed8)
{
  setFixedSize(25*5,25);
  image.setColorCount(20);
  image.setColor(16,qRgb(0,0,0));
  image.setColor(17,qRgb(128,128,128));
  image.setColor(18,qRgb(255,255,255));
  image.setColor(19,qRgb(255,0,0));
  setZoom(2.0);
}

void spritePaternView::mousePressEvent ( QMouseEvent * e )
{
  int togle= int((e->x() /zoomFactor)/25) ;
  if (togle>3) return ;
  useSprite[togle]=!useSprite[togle];
  drawFrames();
  update();
}

void spritePaternView::setDataSource(spriteGrid* datasource)
{
    m_datasource=datasource; 
}
void spritePaternView::setGE5DataSource(GE5Widget* datasource)
{
    m_ge5datasource=datasource; 
}


//void spritePaternView::quickndirtyconvert(int x,int y,int spritenr)
//{
//	//this routine is more at its place in the spritegrid class
//	
//	//quick and dirty test routine to see if updateSprite functions
//	m_datasource->spritesinfo[spritenr]=4;
//	for (int a=0 ; a<32 ; ++a){
//		for (int b=0 ; b<4 ; ++b ){
//			m_datasource->spritescol[b][a&15][spritenr]=(char)(1<<b);
//			m_datasource->spritespat[b][a][spritenr]=char(0);
//			for (int w=0;w<8;++w){
//				int c=m_datasource->image.pixelIndex(x+((a&16)>>1)+w,y+(a&15));
//				//printf("color at (%i,%i) is %i ",x+((a&16)>>1)+w,y+(a&15),c);
//				m_datasource->spritespat[b][a][spritenr]|=(char)((1<<b)&c?(1<<(7-w)):0);
//				//printf("(and %i-> value to or %i)\n",(1<<b),((1<<b)&c?(1<<(7-w)):0));
//			}
//
//		};
//	};
//}

void spritePaternView::drawFrames()
{
    //draw the frames in correct color
    for (int v=0 ; v< 4; ++v){
	    int c=(useSprite[v]?(v < m_datasource->getSpritesinfo(currentspritenr)?18:17):19);
	    for (int w=0 ; w< 18; ++w){
		image.setPixel(v*25+3+w  ,3    ,c);
		image.setPixel(v*25+3+w  ,3+17 ,c);
		image.setPixel(v*25+3    ,3+w  ,c);
		image.setPixel(v*25+3+17 ,3+w  ,c);
	    }
    };
    //draw combined image
    for (int x=0 ; x< 16; ++x){
	    for (int y=0 ; y< 16; ++y){
		    char col=0;
		    for (int i=0 ; i< m_datasource->getSpritesinfo(currentspritenr) ; ++i){
			    if (useSprite[i]) col |= image.pixelIndex(i*25+3+1+x ,3+1+y );
		    }
		    image.setPixel(4*25+3+1+x ,3+1+y  ,col);
	    }
    };
}

void spritePaternView::updateSprite(int x,int y,int spritenr)
{
    currentspritenr=spritenr;
    //display the sprite info on our image
    for (int i=0 ; i<8 ; ++i){
	    useSprite[i]=true;
    }
    //first clean the image
    for (int v=0 ; v< 25*5; ++v){
	    for (int w=0 ; w< 25; ++w){
		image.setPixel(v,w,16);
	    }
    };
    // then draw the sprite patterns as needed
	//printf("\n draw nr grids=%i\n",m_datasource->getSpritesinfo(spritenr));
	//fflush(stdout);
    for (int v=0 ; v< m_datasource->getSpritesinfo(spritenr) ; ++v){
	    for (int w=0 ; w<32 ; ++w){
		    char pat=m_datasource->getSpritespat(v,w,spritenr);
		    int f=(w & 16)>>1;
		    int g=w & 15;
		    char col=m_datasource->getSpritescol(v,g,spritenr);
		    for (int z=0 ; z<8 ; ++z){
			    image.setPixel(v*25+3+1+f+z ,3+1+g  ,
				((1<<(7-z)) & pat? col : 0 ) );
		    }
	    }
    }
  drawFrames();
  update();
}

void spritePaternView::updateImage()
{
	for (int i=0;i<16;++i){
		int r,g,b;
		r=(m_ge5datasource->msxpallet[i*2] & 0xf0)>>4;
		r=(r>>1)|(r<<2)|(r<<5);
		b=(m_ge5datasource->msxpallet[i*2] & 0x0f);
		b=(b>>1)|(b<<2)|(b<<5);
		g=(m_ge5datasource->msxpallet[i*2+1]  & 0x0f);
		g=(g>>1)|(g<<2)|(g<<5);
		image.setColor(i,qRgb(r,g,b));
	};
	updateSprite(1,1,0); 
	update();
}

void spritePaternView::setZoom(float zoom) 
{
  if (zoom<1.0) zoom=1.0;
  setFixedSize(int(25*5*zoom), int(25*zoom));
  zoomFactor=zoom;
  update();
}

void spritePaternView::paintEvent( QPaintEvent *event )
{
	QRect srcRect(0,0,25*5,25);
	QPainter qp(this);
	qp.drawImage(rect(),image,srcRect);
	//qp.drawPixmap(rect(),piximage,srcRect);
	qp.end();
}
		

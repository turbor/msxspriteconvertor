#include "Screen4Widget.hh"
#include <QPushButton>
#include <QString>
#include <QPainter>
#include <QFont>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


Screen4Widget::Screen4Widget(QWidget *parent) : QWidget(parent),image(256,212,QImage::Format_Indexed8)
{
  image.setColorCount(16);
  showGrid=false;
  setZoom(2.0);
  gridoffsetx=0;
  gridoffsety=0;
  gridwidth=8;
  gridheight=8;
}

void Screen4Widget::setZoom(float zoom) 
{
  if (zoom<1.0) zoom=1.0;
  setFixedSize(int(256*zoom), int(212*zoom));
  zoomFactor=zoom;
}

void Screen4Widget::setDataSource(GE5Widget* datasource)
{
    m_datasource=datasource; 
}

void Screen4Widget::updateImage()
{
	for (int i=0;i<16;++i){
		int r,g,b;
		r=(m_datasource->msxpallet[i*2] & 0xf0)>>4;
		r=(r>>1)|(r<<2)|(r<<5);
		b=(m_datasource->msxpallet[i*2] & 0x0f);
		b=(b>>1)|(b<<2)|(b<<5);
		g=(m_datasource->msxpallet[i*2+1]  & 0x0f);
		g=(g>>1)|(g<<2)|(g<<5);
		image.setColor(i,qRgb(r,g,b));
	};

	// First of all try the real conversion here
	for (int table=0 ; table<3 ; ++table){
		for (int row=0 ; row<8 ; ++row){
			for (int column=0 ; column < 32 ; ++column ){
				for (int y=0 ; y<8 ;++y){
					char pattern=m_datasource->sc4charspat[(table*256+row*32+column)*8+y];
					char color=m_datasource->sc4charscol[(table*256+row*32+column)*8+y];
					int color1=int(color&0x0f);
					int color2=int((color&0xf0)>>4);
					for (int x=0 ; x < 8 ; ++x){
						image.setPixel(column*8+x,table*64+row*8+y,pattern&(1<<(7-x))?color2:color1);	
					}
				}
			}
		}
	}
	update();
}


void Screen4Widget::setShowGrid(bool value)
{
	showGrid=value;
	update();
}


void Screen4Widget::paintEvent( QPaintEvent *event )
{
	QRect srcRect(0,0,255,211);
	QPainter qp(this);
	qp.drawImage(rect(),image,srcRect);
	//qp.drawPixmap(rect(),piximage,srcRect);
	if (showGrid){
		qp.scale(zoomFactor,zoomFactor);
		qp.setPen(QColor(0,0,0,128));
		for (int i=gridoffsetx; i<255 ; i+=gridwidth){
			qp.drawLine(i,0,i,211);
		}
		for (int i=gridoffsety; i<211 ; i+=gridheight){
			qp.drawLine(0,i,255,i);
		}
	}
	qp.end();
}
		

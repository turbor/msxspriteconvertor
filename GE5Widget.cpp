#include "GE5Widget.hh"
#include <QPushButton>
#include <QString>
#include <QPainter>
#include <QFont>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


GE5Widget::GE5Widget(QWidget *parent) : QWidget(parent),image(256,212,QImage::Format_Indexed8)
{
  image.setColorCount(16);
  numberOfWrongs=0;
  saveAsGen=true;
  forceConvert=true;
  showGrid=false;
  showError=true;
  maxtables=3;
  setZoom(2.0);
  gridoffsetx=0;
  gridoffsety=0;
  gridwidth=16;
  gridheight=16;
}

GE5Widget::~GE5Widget()
{

}

void GE5Widget::setNrTables(int value) 
{
 if (value<1) value=1;
 if (value>3) value=3;
 maxtables=value;
}

void GE5Widget::setZoom(float zoom) 
{
  if (zoom<1.0) zoom=1.0;
  setFixedSize(int(256*zoom), int(212*zoom));
  zoomFactor=zoom;
}

//void GE5Widget::imageChanged()
//{
//	// nothing so far since this is a signal ?
//}
//
//void GE5Widget::imageClicked(int xcoormsx,int ycoormsx)
//{
//	// nothing so far since this is a signal ?
//	printf("mouse click x=%i y=%i",xcoormsx,ycoormsx);
//}

void GE5Widget::mousePressEvent( QMouseEvent * e )
{
	emit imageClicked( int(e->x()/zoomFactor) , int(e->y()/zoomFactor) ); 
}

	
void GE5Widget::load(QString ge5filename) 
{
	int GE5file;
		
    GE5file=open(ge5filename.toLatin1().constData(),O_RDONLY);
	if (GE5file != -1) {
		filename=ge5filename;
		lseek (GE5file,7+30336,SEEK_SET);
		for (int i=0;i<16;++i){
			char ng,rb;
			int r,g,b;
			read(GE5file,&rb,1);
			read(GE5file,&ng,1);
			msxpallet[i*2]=rb;
			msxpallet[i*2+1]=ng;
			r=(rb & 0xf0)>>4;
			r=(r>>1)|(r<<2)|(r<<5);
			b=(rb & 0x0f);
			b=(b>>1)|(b<<2)|(b<<5);
			g=(ng & 0x0f);
			g=(g>>1)|(g<<2)|(g<<5);
			image.setColor(i,qRgb(r,g,b));
		};
		lseek (GE5file,7,SEEK_SET);
		for (int y=0;y<212;++y){
			for (int x=0;x<128;++x){
				char val;
				read(GE5file,&val,1);
				image.setPixel(x*2,y,(val>>4)&15);
				image.setPixel(1+x*2,y,15&val);
			}
		}
	}
	piximage=piximage.fromImage(image);
	clearInternals();
	emit imageChanged();
	update();
}

void GE5Widget::setShowGrid(bool value)
{
	showGrid=value;
	update();
}

void GE5Widget::setShowErrors(bool value)
{
	showError=value;
	if (showError){
		convert();
	} else {
		piximage=piximage.fromImage(image);
	}
	emit imageConverted();
	update();
}

void GE5Widget::setForced(bool value)
{
	forceConvert=value;
}

void GE5Widget::clearInternals()
{
	piximage=piximage.fromImage(image);
	for (int i=0 ; i<256*3*8 ; ++i){
		sc4charspat[i]=0;
		sc4charscol[i]=0;
	}
}

void GE5Widget::convert()
{
	//printf("to screen 4 convertion started\n");
	numberOfWrongs=0;
	// First of all try the real conversion here
	for (int table=0 ; table<maxtables ; ++table){
		for (int row=0 ; row<8 ; ++row){
			for (int column=0 ; column < 32 ; ++column ){
				bool wrong=false;
				for (int y=0 ; y<8 ;++y){
					int color1=image.pixelIndex(column*8,table*64+row*8+y);
					int color2=-1;
					char pattern=0;
					for (int x=0 ; x < 8 ; ++x){
						int colornow=image.pixelIndex(column*8+x,table*64+row*8+y);
						if (colornow != color1){
							pattern=pattern|(1<<(7-x));	
							if (color2 == -1) color2=colornow;
							if ((colornow != color1) &&(colornow != color2)) {
								wrong=true;
								QPainter qp(&piximage);
								qp.setPen(QColor(255,0,0,128));
								qp.drawPoint(column*8+x,table*64+row*8+y);
								qp.end();
							}

						}
					}
					if ( color2== -1 ) color2=0 ;
					sc4charspat[(table*256+row*32+column)*8+y]=pattern;
					sc4charscol[(table*256+row*32+column)*8+y]=16*color2+color1;
				}
				if (wrong){
					//printf("wrong at x=%i,y=%i",column,row);
					if (showError){
						QPainter qp(&piximage);
						qp.setPen(QColor(255,0,0, 8));
						qp.drawRect(column*8,table*64+row*8,7,7);
						qp.end();
						update();
					};
					++numberOfWrongs;
				}

			}
		}
	}

}

int GE5Widget::toScreen4()
{
//	if (filename.isEmpty()) return (int)0;
	clearInternals();
	convert();
	emit imageConverted();

	// Now save to a file
	if (forceConvert || (numberOfWrongs==0)){
		int datfile;
		QString datfilename=filename+".dat";
        datfile=open(datfilename.toLatin1().constData(),O_WRONLY|O_CREAT|O_TRUNC);
		if (datfile != -1) {
			for (int i=0 ; i<32 ; ++i ){
				write(datfile, &msxpallet[i],1);
			};
			for (int i=0 ; i<256*8*maxtables ; ++i ){
				write(datfile, &sc4charspat[i],1);
			};
			for (int i=0 ; i<256*8*maxtables ; ++i ){
				write(datfile, &sc4charscol[i],1);
			};
		}
	}
	// Now save to a file
	if (saveAsGen && (forceConvert || (numberOfWrongs==0))){
		FILE* GE5file;
		QString genfilename=filename+".gen";
        GE5file=fopen(genfilename.toLatin1().constData(),"w");
        if (GE5file != nullptr) {
            fprintf(GE5file,"#File for %s\n",genfilename.toLatin1().constData() );
			fprintf(GE5file,"#Color definitions\n" );
			for (int i=0 ; i<32 ; i+=2 ){
				fprintf(GE5file," db #%02x,#%02x\n",
				msxpallet[i],
				msxpallet[i+1]);
			};
			fprintf(GE5file,"#Patern definitions" );
			for (int table=0 ; table<maxtables ; ++table){
				for (int x=0 ; x<256 ; ++x ){
					fprintf(GE5file,"\n db #%02x",sc4charspat[256*table+x*8] );
					for (int y=1 ; y<8 ; ++y){
						fprintf(GE5file,",#%02x",sc4charspat[256*table+x*8] );
					}
				}
			}	
			fprintf(GE5file,"\n#\n#Color definitions\n#" );
			for (int table=0 ; table<maxtables ; ++table){
				for (int x=0 ; x<256 ; ++x ){
					fprintf(GE5file,"\n db #%02x",sc4charscol[256*table+x*8] );
					for (int y=1 ; y<8 ; ++y){
						fprintf(GE5file,",#%02x",sc4charscol[256*table+x*8] );
					}
				}
			}	
		}
		fprintf(GE5file,"\n\n\n");
		fclose(GE5file);
		
	}
	return numberOfWrongs;
}

void GE5Widget::paintEvent( QPaintEvent *event )
{
	QRect srcRect(0,0,255,211);
	QPainter qp(this);
	//qp.drawImage(rect(),image,srcRect);
	qp.drawPixmap(rect(),piximage,srcRect);
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
		

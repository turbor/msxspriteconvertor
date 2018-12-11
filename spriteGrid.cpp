#include "spriteGrid.hh"
#include <QPushButton>
#include <QString>
#include <QPainter>
#include <QFont>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


spriteGrid::spriteGrid(QWidget *parent) : QWidget(parent),image(1+17*NRSPRITESHOR,1+17*NRSPRITESVER,QImage::Format_Indexed8)
{
  setFixedSize(17*NRSPRITESHOR,17*NRSPRITESVER);
  image.setColorCount(19);
  image.setColor(16,qRgb(0,0,0));
  image.setColor(17,qRgb(128,128,128));
  image.setColor(18,qRgb(255,255,255));
  activeGridItem=0;
  updateActiveGrid();
}

char spriteGrid::getSpritesinfo(int a)
{
	return spritesinfo[a];
}
char spriteGrid::getSpritescol(int a,int b,int c)
{
	return spritescol[a][b][c];
}
char spriteGrid::getSpritespat(int a,int b,int c)
{
	return spritespat[a][b][c];
}

void spriteGrid::setDataSource(GE5Widget* datasource)
{
    m_datasource=datasource; 
}

void spriteGrid::avoidBitOverlap(int* a,int* b)
{
 	int c=(*a)&(*b);
	if ( (*a^c) != 0){ // we do not want colors that are 0 :-)
		*a=(*a)^c;
	} else {
		*b=(*b)^c;
	}
}

void spriteGrid::convert(int x,int y,int spritenr)
{
	bool colorused[16] ;
	int basecolor[4] ;
	int nrbasecolors=0;
	spritesinfo[spritenr]=(char)0;
	for (int row=0 ; row<16 ; ++row){
		//first get the used colors on this row
		nrbasecolors=0;
		for (int i=0 ; i<16 ; ++i) colorused[i]=false;
		for (int column=0 ; column<16 ; ++column){
			int i=image.pixelIndex(x+column,y+row);
			colorused[i]=true;
		};
		//then determine base colors
		for (int i=1 ; i<16 ; ++i){
			if (colorused[i]){
				switch (nrbasecolors){
					case 0:
						basecolor[nrbasecolors++]=i;
						break;
					case 1:
						basecolor[nrbasecolors++]=i;
						break;
					case 2:
						avoidBitOverlap(&basecolor[0],&basecolor[1]);
						if (!(i==(basecolor[0]|basecolor[1]))){
							basecolor[nrbasecolors++]=i;
							//now remove bits that overlap with previous colors
							avoidBitOverlap(&basecolor[2],&basecolor[0]);
							avoidBitOverlap(&basecolor[2],&basecolor[1]);
						};
						break;
					case 3:
						if (!(
							( i== (basecolor[0]|basecolor[1]) )
							|( i== (basecolor[0]|basecolor[2]) )
							|( i== (basecolor[1]|basecolor[2]) )
							|( i== (basecolor[0]|basecolor[1]|basecolor[2]) )
						     )){
							basecolor[nrbasecolors++]=i;
							//now remove bits that overlap with previous colors
							avoidBitOverlap(&basecolor[3],&basecolor[0]);
							avoidBitOverlap(&basecolor[3],&basecolor[1]);
							avoidBitOverlap(&basecolor[3],&basecolor[2]);
							// more then 4 base colors should be impossible
							// we should end up with 1,2,4 and 8 as
							// basecolors in this case
						};
				}

			}
		}
		// now that we have the basecolors for this row the rest is simple :-)
		for (int i=0 ; i<4 ; ++i ){
			spritescol[i][row][spritenr]=(char)(i<nrbasecolors?basecolor[i]:0);
			//printf("\nbasecolors[%i]=%i\n",i,(i<nrbasecolors?basecolor[i]:0));
			spritespat[i][row][spritenr]=(char)0;
			spritespat[i][row+16][spritenr]=(char)0;
		}
		fflush(stdout);
		for (int i=0 ; i<nrbasecolors ; ++i ){
			for (int j=0 ; j<8 ; ++j){
				int c=image.pixelIndex(x+j,y+row);
				spritespat[i][row][spritenr]|=(char)((basecolor[i]&c)==basecolor[i]?(1<<(7-j)):0);
				c=image.pixelIndex(x+j+8,y+row);
				spritespat[i][row+16][spritenr]|=(char)((basecolor[i]&c)==basecolor[i]?(1<<(7-j)):0);
			}
		}
		if (spritesinfo[spritenr] < (char)nrbasecolors){
			spritesinfo[spritenr]=(char)nrbasecolors;
			//printf("\n nrbasecolors=%i\n",nrbasecolors);
			//fflush(stdout);
		}
	}

}

void spriteGrid::mousePressEvent(  QMouseEvent * e )
{
	activeGridItem=NRSPRITESHOR*(e->y()/17)+e->x()/17;
	updateActiveGrid();
	int scrx=1+17*(activeGridItem%NRSPRITESHOR);
	int scry=1+17*int(activeGridItem/NRSPRITESHOR);
	
	convert(scrx,scry,activeGridItem);
	emit spriteChanged(scrx,scry,activeGridItem);
	update();
}

void spriteGrid::resetGrid()
{
	//QPainter qp(&image);
	//qp.fillRect(image.rect(),QBrush(image.color(17)));
	for (int x=0; x<(1+17*NRSPRITESHOR) ; ++x){
		for (int y=0; y<(1+17*NRSPRITESVER) ; ++y){
				image.setPixel(x,y,16);
		}
	}
	for (int x=0; x<NRSPRITESVER*NRSPRITESHOR ; ++x){
		spritesinfo[x]=0;
	}
	update();
}

void spriteGrid::updateImage()
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
	//clear the image with color 16
	
	resetGrid();
	updateActiveGrid();
	update();
}


void spriteGrid::updateActiveGrid()
{
	// first redraw all grid handlers
	//QPainter qp(&image); QRasterPaintEngine doesn't support indexed images :-((
	int l=3;
	int c;
	for (int x=0; x<NRSPRITESHOR ; ++x){
		for (int y=0; y<NRSPRITESVER ; ++y){
			for (int i=0; i<8 ; ++i){
				c=(i<l)?17:16;
				image.setPixel(x*17+i,y*17,c);
				image.setPixel((1+x)*17-i,y*17,c);
				image.setPixel(x*17+i,(1+y)*17,c);
				image.setPixel((1+x)*17-i,(1+y)*17,c);
				image.setPixel(x*17,y*17+i,c);
				image.setPixel((1+x)*17,y*17+i,c);
				image.setPixel(x*17,(1+y)*17-i,c);
				image.setPixel((1+x)*17,(1+y)*17-i,c);
			}
		}
	}
	//now draw the active one
	int x=activeGridItem%NRSPRITESHOR;
	int y=int(activeGridItem/NRSPRITESHOR);
	for (int i=0; i<8 ; ++i){
				image.setPixel(x*17+i,y*17,18);
				image.setPixel((1+x)*17-i,y*17,18);
				image.setPixel(x*17+i,(1+y)*17,18);
				image.setPixel((1+x)*17-i,(1+y)*17,18);
				image.setPixel(x*17,y*17+i,18);
				image.setPixel((1+x)*17,y*17+i,18);
				image.setPixel(x*17,(1+y)*17-i,18);
				image.setPixel((1+x)*17,(1+y)*17-i,18);
	}
}

void spriteGrid::spriteClicked(int x,int y)
{
	int scrx=1+17*(activeGridItem%NRSPRITESHOR);
	int scry=1+17*int(activeGridItem/NRSPRITESHOR);
	int sprx=x & 0xf0;
	int spry=y & 0xf0;
	//printf(" \n");
	//printf("x %i \n",x);
	//printf("y %i \n",y);
	//printf("scrx %i \n",scrx);
	//printf("scry %i \n",scry);
	//printf("sprx %i \n",sprx);
	//printf("spry %i \n",spry);
	//printf(" \n");
	for (int x=0 ; x<16 ; ++x){
		for (int y=0; y<16 ; ++y){
		image.setPixel(scrx+x,scry+y,m_datasource->image.pixelIndex(sprx+x,spry+y));
		}
	};
	convert(scrx,scry,activeGridItem);
	emit spriteChanged(scrx,scry,activeGridItem);
	if (activeGridItem < (NRSPRITESHOR*NRSPRITESVER-1)) ++activeGridItem;
	
	updateActiveGrid();
	update();
}

void spriteGrid::loadFromGE5( QString *filename )
{

	m_datasource->load(*filename);
	//this will also update the GE5 widget and have the colors update
	//on all other widgets;
	int GE5file;
		
	GE5file=open(filename->toLatin1().constData(),O_RDONLY);
	if (GE5file != -1) {
		resetGrid();
/*		lseek (GE5file,7+30336,SEEK_SET);
		for (int i=0;i<16;++i){
			char ng,rb;
			int r,g,b;
			read(GE5file,&rb,1);
			read(GE5file,&ng,1);
			m_datasource->msxpallet[i*2]=rb;
			m_datasource->msxpallet[i*2+1]=ng;
			r=(rb & 0xf0)>>4;
			r=(r>>1)|(r<<2)|(r<<5);
			b=(rb & 0x0f);
			b=(b>>1)|(b<<2)|(b<<5);
			g=(ng & 0x0f);
			g=(g>>1)|(g<<2)|(g<<5);
			image.setColor(i,qRgb(r,g,b));
		};
*/
		lseek (GE5file,7,SEEK_SET);

		for (int y=0 ; y <13*16 ; ++y){
			int offy=(y & 0x0f);
			for (int x=0;x<256;x+=2){
				char val;
				int sprite=(y & 0xf0)+((x & 0xf0)>>4);
				if (sprite < (NRSPRITESHOR*NRSPRITESVER)){
					int srcx=1+17*(sprite%NRSPRITESHOR);
					int srcy=1+17*int(sprite/NRSPRITESHOR);
					int offx=(x & 0x0f);
					read(GE5file,&val,1);
					image.setPixel(srcx+offx,srcy+offy,(val>>4)&15);
					image.setPixel(srcx+1+offx,srcy+offy,15&val);
				}
			}
		}
		for (int i=0 ; i < (NRSPRITESHOR*NRSPRITESVER) ; ++i){
			int scrx=1+17*(i%NRSPRITESHOR);
			int scry=1+17*int(i/NRSPRITESHOR);
			convert(scrx,scry,i);
		}
	//close(GE5file);
	}
	//emit imageChanged();
	updateActiveGrid();
	update();
}

void spriteGrid::save2GE5( QString *filename )
{
	int GE5file;
		
	GE5file=open(filename->toLatin1().constData(),O_WRONLY|O_TRUNC|O_CREAT);
	if (GE5file != -1) {
		//first the header
		char header[]="\376\0\0\237\166\0\0";
		write(GE5file,header,7);

		for (int y=0 ; y <13*16 ; ++y){
			int offy=(y & 0x0f);
			for (int x=0;x<256;x+=2){
				char val;
				int sprite=(y & 0xf0)+((x & 0xf0)>>4);
				if (sprite < (NRSPRITESHOR*NRSPRITESVER)){
					int srcx=1+17*(sprite%NRSPRITESHOR);
					int srcy=1+17*int(sprite/NRSPRITESHOR);
					int offx=(x & 0x0f);
					read(GE5file,&val,1);
					char c1=image.pixelIndex(srcx+offx,srcy+offy);
					char c2=(c1<<4)|image.pixelIndex(srcx+1+offx,srcy+offy);
					write(GE5file,&c2,1);
				}
			}
		}
		lseek (GE5file,7+30336,SEEK_SET);
		for (int i=0;i<16;++i){
			write(GE5file,&(m_datasource->msxpallet[i*2]),1);
			write(GE5file,&(m_datasource->msxpallet[i*2+1]),1);
		};
	//close(GE5file);
	}
	
}

void spriteGrid::save2file( QString *filename )
{
  FILE* genfile;
  genfile=fopen(filename->toLatin1().constData(),"w");
  if (genfile != NULL) {
    fprintf(genfile,";File for %s\n",filename->toLatin1().constData() );
    fprintf(genfile,";Color definitions\n" );
    for (int i=0 ; i<32 ; i+=2 ){
      fprintf(genfile," db #%02x,#%02x\n",
          m_datasource->msxpallet[i],
          m_datasource->msxpallet[i+1]);
    };
    fprintf(genfile,";\n;Sprite pattern definitions\n;");
    int physsprite=0;
    for (int i=0 ; i<NRSPRITESVER*NRSPRITESHOR ; ++i ){
      if (spritesinfo[i]){
        fprintf(genfile,"\n;Sprite %i ( %i 16x16 hardware sprites needed)",i,spritesinfo[i]);
        for (int k=0 ; k<spritesinfo[i] ; ++k ){
	fprintf(genfile,"\n;  definition hardware sprite %i till %i",physsprite,physsprite+3);
	physsprite+=4;
            for (int l=0 ; l<4 ; ++l ){
              fprintf(genfile,"\n db #%02x",(unsigned char)spritespat[k][l*8][i]);
              for (int m=1 ; m<8 ; ++m ){
                fprintf(genfile,",#%02x",(unsigned char)spritespat[k][l*8+m][i]);
              }
            }
          }
        }
      }
    fprintf(genfile,"\n;\n;Sprite color definitions\n;");
    physsprite=0;
    for (int i=0 ; i<NRSPRITESVER*NRSPRITESHOR ; ++i ){
      if (spritesinfo[i]){
        fprintf(genfile,"\n;Sprite %i ( %i 16x16 hardware sprites needed)",i,spritesinfo[i]);
        for (int k=0 ; k<spritesinfo[i] ; ++k ){
	fprintf(genfile,"\n;  definition hardware sprite %i till %i",physsprite,physsprite+3);
	physsprite+=4;
            for (int l=0 ; l<2 ; ++l ){
              fprintf(genfile,"\n db #%02x",(unsigned char)spritespat[k][l*8][i]);
              for (int m=1 ; m<8 ; ++m ){
                fprintf(genfile,",#%02x",(unsigned char)spritespat[k][l*8+m][i]);
              }
            }
          }
        }
      }
    }
  fprintf(genfile,"\n\n");
  fclose(genfile);
}

void spriteGrid::paintEvent( QPaintEvent *event )
{
	QRect srcRect(0,0,1+17*NRSPRITESHOR,1+17*NRSPRITESVER);
	QPainter qp(this);
	qp.drawImage(rect(),image,srcRect);
	qp.end();
}
		

#ifndef SPRITEGRID
#define SPRITEGRID

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include "GE5Widget.hh"
#include <QMouseEvent>

#define NRSPRITESHOR 8
#define NRSPRITESVER 16

class spriteGrid : public QWidget
{
	Q_OBJECT

public:
    spriteGrid(QWidget *parent = 0);
    void setDataSource(GE5Widget* datasource);
    void mousePressEvent ( QMouseEvent * e );
   
    char getSpritesinfo(int a);
    char getSpritescol(int a,int b,int c);
    char getSpritespat(int a,int b,int c);
	/* spritesinfo[x]=how many sprites needed
	 */
    QImage image;
    
public slots:
    void spriteClicked(int x,int y);
    void resetGrid();
    void updateImage();
    void save2file( QString *filename );
    void loadFromGE5( QString *filename );
    void save2GE5( QString *filename );

signals:
    void spriteChanged(int x,int y,int spritenr);

protected:
    void paintEvent( QPaintEvent* );
private:

    void updateActiveGrid();
    void avoidBitOverlap(int *a,int *b);

    void convert(int x,int y,int spritenr);
    
    int activeGridItem;

    GE5Widget* m_datasource;

    char spritespat[4][32][NRSPRITESVER*NRSPRITESHOR];
    char spritescol[4][16][NRSPRITESVER*NRSPRITESHOR];
    char spritesinfo[NRSPRITESVER*NRSPRITESHOR];

};

#endif // SPRITEGRID


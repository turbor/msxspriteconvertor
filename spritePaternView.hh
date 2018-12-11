#ifndef SPRITEPATERNVIEW
#define SPRITEPATERNVIEW

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include "spriteGrid.hh"

class spritePaternView : public QWidget
{
	Q_OBJECT

public:
    spritePaternView(QWidget *parent = 0);
    void setZoom(float zoom);
    void setDataSource(spriteGrid* datasource);
    void setGE5DataSource(GE5Widget* datasource);
    void mousePressEvent ( QMouseEvent * e );

public slots:
    void updateSprite(int x,int y,int spritenr);
    void updateImage();
	
protected:
    void paintEvent( QPaintEvent* );
private:
    QImage image;
    QLabel label;
    int currentspritenr;
    float zoomFactor;
    bool useSprite[8];
    void drawFrames();
    //void quickndirtyconvert(int x,int y,int spritenr);
    GE5Widget* m_ge5datasource;
    spriteGrid* m_datasource;

};

#endif // SPRITEPATERNVIEW


#ifndef SCREEN4WIDGET
#define SCREEN4WIDGET

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include "GE5Widget.hh"

class Screen4Widget : public QWidget
{
	Q_OBJECT

public:
    Screen4Widget(QWidget *parent = 0);
    void setShowGrid(bool value);
    void setZoom(float zoom);
    void setDataSource(GE5Widget* datasource);

public slots:
    void updateImage();
	
protected:
    void paintEvent( QPaintEvent* );
private:
    QImage image;
    float zoomFactor;
    
    char sc4charspat[256*3*8];
    char sc4charscol[256*3*8];
 
    bool showGrid;
    int gridoffsetx;
    int gridoffsety;
    int gridwidth;
    int gridheight;

    GE5Widget* m_datasource;

};

#endif // SCREEN4WIDGET


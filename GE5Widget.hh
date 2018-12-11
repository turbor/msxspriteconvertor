#ifndef GE5WIDGET
#define GE5WIDGET

#include <QString>
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QMouseEvent>
class GE5Widget : public QWidget
{
	Q_OBJECT

public:
    GE5Widget(QWidget *parent = nullptr);
    ~GE5Widget();
    void load(QString filename);
    void setZoom(float zoom);
    void setForced(bool value);
    void setShowErrors(bool value);
    void setShowGrid(bool value);
    void setNrTables(int value);

    char msxpallet[32];
    char sc4charspat[256*3*8];
    char sc4charscol[256*3*8];

    void mousePressEvent ( QMouseEvent * e );
    
    QImage image;

public slots:
    int toScreen4();

signals:
    void imageChanged();
    void imageConverted();
    void imageClicked(int xcoormsx,int ycoormsx);
	
protected:
    void paintEvent( QPaintEvent* );
    void convert();
    void clearInternals();
private:
    QPixmap piximage;
    QString filename;
    bool saveAsGen;
    bool forceConvert ;
    bool showError ;
    bool showGrid ;
    int numberOfWrongs;
    int maxtables;
    float zoomFactor;

    int gridoffsetx;
    int gridoffsety;
    int gridwidth;
    int gridheight;

};

#endif // GE5WIDGET


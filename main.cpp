#include <QCoreApplication>
#include <QImage>
#include <QPainter>
#include <QSurfaceFormat>
#include <QPainterPath>

#include <spline.h>

void generateSegments(const tk::spline& sp, QPainter& p, double step,const QColor& color, bool exchangeXY=false) {
  QPainterPath path;
  double x0 = sp.interpolateX(0);
  if(exchangeXY) {
    path.moveTo(sp(x0), x0);
  }
  else {
    path.moveTo(x0, sp(x0));
  }
  for(double t = 0; t <=1.0 ; t+=step) {
    double x = sp.interpolateX(t);
    if(exchangeXY){
      path.lineTo(sp(x), x);
      continue;
    }
    path.lineTo(x, sp(x));
  }
  p.setPen(color);
  p.drawPath(path);
}

void interpolateBetween( double u, double v, const tk::spline& sp0, const tk::spline& sp1) {

}

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);


  QSurfaceFormat format;
  QSurfaceFormat::setDefaultFormat(format);

  QImage m(1024, 768, QImage::Format_ARGB32);

  QPainter p(&m);

  tk::spline sp;

  double x0 = 100; double x1 = 300;
  double y0 = 100; double y1 = 500;

  QPointF p00(100,100);
  QPointF p10(200,100);
  QPointF p20(300,100);
  QPointF p01(100,200);
  QPointF p21(300,200);
  QPointF p02(100,300);
  QPointF p12(200,300);
  QPointF p22(300,300);

  tk::spline sy0, sy2, sx0, sx2;

  sy0.set_points({ p00.x(), p10.x(), p20.x() }, {p00.y(), p10.y(), p20.y()});
  sy2.set_points({ p02.x(), p12.x(), p22.x()}, {p02.y(), p12.y(), p22.y()});

  sx0.set_points({ p00.y(), p01.y(), p02.y() }, {p00.x(), p01.x(), p02.x()});
  sx2.set_points({ p20.y(), p21.y(), p22.y()}, {p20.x(), p21.x(), p22.x()});

  p.setRenderHint(QPainter::Antialiasing);
  generateSegments(sy0, p, 1.0/200, QColor(255,0,0,255));
  generateSegments(sy2, p, 1.0/200, QColor(0, 255,0,255));
  generateSegments(sx0, p, 1.0/200, QColor(0, 0, 255,255), true);
  generateSegments(sx2, p, 1.0/200, QColor(0, 0, 255,255), true);


  m.save("/Users/pabloelias/Documents/Development/TensorProduct/result.png");

  return 0;
}

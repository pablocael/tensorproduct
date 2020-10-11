#include <spline.h>

#include <QCoreApplication>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QSurfaceFormat>

QPointF interpolateBetween(double u, double v, const tk::spline& sp0,
                           const tk::spline& sp1) {
    QPointF p0(sp0.interpolateX(u), sp0(sp0.interpolateX(u)));
    QPointF p1(sp1.interpolateX(u), sp1(sp1.interpolateX(u)));
    return (1 - v) * p0 + v * p1;
}
QPointF interpolateBetweenU(double u, double v, const tk::spline& sp0,
                           const tk::spline& sp1) {
    QPointF p0( sp0(sp0.interpolateX(u)), sp0.interpolateX(u));
    QPointF p1( sp1(sp1.interpolateX(u)), sp1.interpolateX(u));
    return (1 - v) * p0 + v * p1;
}
QPointF interpolateBetweenV(double u, double v, const tk::spline& sp0,
                           const tk::spline& sp1) {
    QPointF p0(sp0.interpolateX(v), sp0(sp0.interpolateX(v)));
    QPointF p1(sp1.interpolateX(v), sp1(sp1.interpolateX(v)));
    return (1 - u) * p0 + u * p1;
}

void generateSegments(const tk::spline& sp, QPainter& p, double step,
                      const QColor& color, bool exchangeXY = false) {
    QPainterPath path;
    double x0 = sp.interpolateX(0);
    if (exchangeXY) {
        path.moveTo(sp(x0), x0);
    } else {
        path.moveTo(x0, sp(x0));
    }
    for (double t = 0; t <= 1.0; t += step) {
        double x = sp.interpolateX(t);
        if (exchangeXY) {
            path.lineTo(sp(x), x);
            continue;
        }
        path.lineTo(x, sp(x));
    }
    p.setPen(color);
    p.drawPath(path);
}

void generateInterSplinesSegments(const tk::spline& sp0, const tk::spline& sp1,
                                  QPainter& p, double step,
                                  const QColor& color0, const QColor& color1,
                                  bool exchangeXY = false) {
    QPainterPath path;
    QPointF p0 = interpolateBetween(0, 0, sp0, sp1);
    p.setOpacity(0.5);
    if (exchangeXY) {
        path.moveTo(p0.y(), p0.x());
    } else {
        path.moveTo(p0.x(), p0.y());
    }
    for (double u = 0; u <= 1.0; u += step) {
        for (double v = 0; v <= 1.0; v += step) {
            QPointF p = interpolateBetween(u, v, sp0, sp1);
            if (exchangeXY) {
                path.lineTo(p.y(), p.x());
                continue;
            }
            path.lineTo(p.x(), p.y());
        }
    }
    p.setPen(color0);
    p.drawPath(path);
}

QPointF generateSplinePatch(double u, double v, const tk::spline& spU0, const tk::spline& spU1, const tk::spline& spV0, const tk::spline& spV1)
{
    QPointF result;
    // generate a bilinear interpolation of 4 corner points at (u,v) = (0, 0) and (u,v) = (1, 1)
    QPointF corner00(spU0(spU0.interpolateX(0)), spU0.interpolateX(0));
    QPointF corner01(spU0(spU0.interpolateX(1)), spU0.interpolateX(1));
    QPointF corner10(spU1(spU1.interpolateX(0)), spU1.interpolateX(0));
    QPointF corner11(spU1(spU1.interpolateX(1)), spU1.interpolateX(1));

    QPointF B = corner00*(1-u)*(1-v) + corner01*u*(1-v) + corner10*(1-u)*v + corner11*u*v;

    QPointF Lc = interpolateBetweenU(u, v, spU0, spU1);
    result = Lc;
    result += interpolateBetweenV(u, v, spV0, spV1);
    result -= B;
    return result;
}

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    QSurfaceFormat format;
    QSurfaceFormat::setDefaultFormat(format);

    QImage m(1024, 768, QImage::Format_ARGB32);

    QPainter p(&m);

    tk::spline sp;

    double x0 = 100;
    double x1 = 300;
    double y0 = 100;
    double y1 = 500;

    QPointF p00(100, 50);
    QPointF p10(200, 100);
    QPointF p20(300, 100);
    QPointF p01(250, 200);
    QPointF p21(350, 250);
    QPointF p02(100, 300);
    QPointF p12(200, 300);
    QPointF p22(300, 350);

    tk::spline sy0, sy2, sx0, sx2;

    sy0.set_points({p00.x(), p10.x(), p20.x()}, {p00.y(), p10.y(), p20.y()});
    sy2.set_points({p02.x(), p12.x(), p22.x()}, {p02.y(), p12.y(), p22.y()});

    sx0.set_points({p00.y(), p01.y(), p02.y()}, {p00.x(), p01.x(), p02.x()});
    sx2.set_points({p20.y(), p21.y(), p22.y()}, {p20.x(), p21.x(), p22.x()});

    p.setRenderHint(QPainter::Antialiasing);
    generateSegments(sy0, p, 1.0 / 200, QColor(255, 0, 0, 255));
    generateSegments(sy2, p, 1.0 / 200, QColor(0, 255, 0, 255));
    generateSegments(sx0, p, 1.0 / 200, QColor(0, 0, 255, 255), true);
    generateSegments(sx2, p, 1.0 / 200, QColor(0, 0, 255, 255), true);
    /* generateInterSplinesSegments(sy0, sy2, p, 0.1, QColor(0,255,0), QColor(255,255,0)); */
    /* generateInterSplinesSegments(sx0, sx2, p, 0.1, QColor(255,255,0), QColor(255,255,0), true); */
    p.setPen(QPen(QColor(0,0,0,255), 4));
    for (double u = 0; u <= 1.025; u += 0.025) {
        for (double v = 0; v <= 1.025; v += 0.025) {
            QPointF point = generateSplinePatch(u, v, sx0, sx2, sy0, sy2 );
            p.drawPoint(point);
        }
    }
    m.save("result.png");


    return 0;
}

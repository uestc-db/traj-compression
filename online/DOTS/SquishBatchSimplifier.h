/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#ifndef SQUISHBATCHSIMPLIFIER_H
#define SQUISHBATCHSIMPLIFIER_H

#include <QObject>
#include<QMap>
#include<QPair>

class SpatioPoint {
public:
    SpatioPoint(double _x, double _y, double _t, int _SED)
    {
        x = _x;
        y = _y;
        t = _t;
        SED = _SED;
    }

    SpatioPoint()
    {
        x = y = t = 0.0;
        SED = 0;
    }

    double x;
    double y;
    double t;
    int SED;
};

class SquishBatchSimplifier : public QObject
{
    Q_OBJECT
public:
    explicit SquishBatchSimplifier(QObject *parent = 0);

    static void simplify(const QVector<double> x, const QVector<double> y, const QVector<double> t,
                         QVector<double> &ox, QVector<double> &oy, QVector<double> &ot,
                         int bufferSize);

    static void simplifyByIndex(const QVector<double> x, const QVector<double> y, const QVector<double> t,
                                QVector<int> &outIndex,
                                int bufferSize);

protected:
    static int estimateLossingSED(const QMap<int, SpatioPoint> &indexMap, int i);

    static void removeFirst(QMap< QPair<int, int>, bool > &buffer, QMap<int, SpatioPoint> &indexMap);

    static const double SED_ACCURACY;
signals:

public slots:
};

#endif // SQUISHBATCHSIMPLIFIER_H

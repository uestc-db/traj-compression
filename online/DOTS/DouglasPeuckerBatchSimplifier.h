/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#ifndef DOUGLASPEUCKERBATCHSIMPLIFIER_H
#define DOUGLASPEUCKERBATCHSIMPLIFIER_H

#include <QObject>
#include<QVector>

class DouglasPeuckerBatchSimplifier : public QObject
{
    Q_OBJECT
public:
    explicit DouglasPeuckerBatchSimplifier(QObject *parent = 0);

    static void simplify(const QVector<double> x, const QVector<double> y,
                         QVector<double> &ox, QVector<double> &oy,
                         double minimumDistance);

    static void simplifyByIndex(const QVector<double> x, const QVector<double> y,
                                QVector<int> &simplifiedIndex,
                                double minimumDistance);

signals:

public slots:
};

#endif // DOUGLASPEUCKERBATCHSIMPLIFIER_H

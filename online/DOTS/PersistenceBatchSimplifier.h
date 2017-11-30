/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#ifndef PERSISTENCEBATCHSIMPLIFIER_H
#define PERSISTENCEBATCHSIMPLIFIER_H

#include <QObject>
#include<QVector>

/**
 * @brief The PersistenceBatchSimplifier class is a trival implementation of the PERSISTENCE algorithm. It's just meant
 * to do performance comparison (with our DOTS). Actually this is quite a ugly implementation. Do more improvements
 * before you decide to use this algorithm in products.
 */
class PersistenceBatchSimplifier : public QObject
{
    Q_OBJECT
protected:
    /**
     * @brief PersistenceBatchSimplifier is the default constructor. We define it as protected to prevent it from
     * being instanced. We would only use this class in static manner.
     * @param parent is the QT parent of this object.
     */
    explicit PersistenceBatchSimplifier(QObject *parent = 0);

public:
    static void simplify(const QVector<double> x, const QVector<double> y,
                         QVector<double> &ox, QVector<double> &oy,
                         double minimumDistance);

    static void simplifyCascade(const QVector<double> x, const QVector<double> y,
                         QVector<double> &ox, QVector<double> &oy,
                         double minimumDistance);

    static void simplifyByIndexCascade(const QVector<double> x, const QVector<double> y,
                                QVector<int> &outIndex, double minimumDistance);

protected:
    static void simplifyByIndex(const QVector<double> x, const QVector<double> y, const QVector<int> inIndex,
                                QVector<int> &outIndex,
                                double minimumDistance);

    /**
     * @brief DEFAULT_BETA is the crucial parameter BETA of PERSISTENCE algorithm. The suggested value of BETA is
     * 10 degrees. We would use this value by default.
     */
    static const double DEFAULT_BETA;

    /**
     * @brief DEFAULT_DISTANCE_START represents the start point for the multi-resolution version of PERSISTENCE
     * algorithm. The default value is 2(m). We will decide start point from MIN(DEFAULT_DISTANCE_START, target/8.0);
     */
    static const double DEFAULT_DISTANCE_START;

signals:

public slots:
};

#endif // PERSISTENCEBATCHSIMPLIFIER_H

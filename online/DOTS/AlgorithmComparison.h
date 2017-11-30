/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#ifndef ALGORITHMCOMPARISON_H
#define ALGORITHMCOMPARISON_H

#include <QObject>
#include<QString>
#include<QVector>
#include<QMap>
#include"DotsException.h"

class EvaluationPoint {
public:
    EvaluationPoint(double _r, double _e)
    {
        this->compressionRate = _r;
        this->error = _e;
    }
    EvaluationPoint()
    {
        this->compressionRate = 0.0;
        this->error = 0.0;
    }

    double compressionRate;
    double error;
};

class AlgorithmComparison : public QObject
{
    Q_OBJECT
public:
    explicit AlgorithmComparison(QObject *parent = 0);

    static void evaluateAlgorithm(const QVector<double> &x, const QVector<double> &y, const QVector<double> &t,
                           int algorithm,
                           double compressMin, double compressMax, int numSteps,
                           QMap< int, QVector<EvaluationPoint> > &errorToEval, int errorTypes);

    static void compareAlgorithms(QString fileName, int algorithmsToCompare, int errorTypesToCompare);

protected:
    static double tryToSimplify(const QVector<double> &x, const QVector<double> &y, const QVector<double> &t,
                              int algorithm, double compressRate,
                              QVector<int> &simplifiedIndex);

    static double generalSimplify(const QVector<double> &x, const QVector<double> &y, const QVector<double> &t,
                                int algorithm, double param,
                                QVector<int> &simplifiedIndex);

    static void calculateStatisticsForSSED(const QVector<double> &x, const QVector<double> &y, const QVector<double> &t,
                                           QVector<double> &xSum, QVector<double> &ySum, QVector<double> &tSum,
                                           QVector<double> &x2Sum, QVector<double> &y2Sum, QVector<double> &t2Sum,
                                           QVector<double> &xtSum, QVector<double> &ytSum);

    static double evaluateResult(const QVector<double> &x, const QVector<double> &y, const QVector<double> &t,
                           const QVector<int> simplifiedIndex, int errorType,
                           const QVector<double> &xSum, const QVector<double> &ySum, const QVector<double> &tSum,
                           const QVector<double> &x2Sum, const QVector<double> &y2Sum, const QVector<double> &t2Sum,
                           const QVector<double> &xtSum, const QVector<double> &ytSum);

    static inline double getLSSD(
            int fst, int lst,
            const QVector<double> &ptx, const QVector<double> &pty, const QVector<double> &ptt,
            const QVector<double> &xSum, const QVector<double> &ySum, const QVector<double> &tSum,
            const QVector<double> &x2Sum, const QVector<double> &y2Sum, const QVector<double> &t2Sum,
            const QVector<double> &xtSum, const QVector<double> &ytSum)
    {
        if (fst+1>=lst)
            return 0;
        if (fst<0 || lst>=xSum.count())
            DotsException(QString("Index out of bound error.")).raise();

        int plst = lst-1;
        double c1x = ptx[fst]*ptt[lst]-ptx[lst]*ptt[fst];
        double c2x = c1x*c1x;
        double c3x = ptt[lst]-ptt[fst];
        double c4x = c3x*c3x;
        double c5x = ptx[lst]-ptx[fst];
        double c6x = c5x*c5x;

        double c1y = pty[fst]*ptt[lst]-pty[lst]*ptt[fst];
        double c2y = c1y*c1y;
        double c3y = c3x;
        double c4y = c3y*c3y;
        double c5y = pty[lst]-pty[fst];
        double c6y = c5y*c5y;

        double distance = (plst-fst)*c2x/c4x
                + c6x/c4x*(t2Sum[plst]-t2Sum[fst])
                + (x2Sum[plst]-x2Sum[fst])
                + 2*c1x*c5x/c4x*(tSum[plst]-tSum[fst])
                - 2*c1x/c3x*(xSum[plst]-xSum[fst])
                - 2*c5x/c3x*(xtSum[plst]-xtSum[fst])
                + (plst-fst)*c2y/c4y
                + c6y/c4y*(t2Sum[plst]-t2Sum[fst])
                + (y2Sum[plst]-y2Sum[fst])
                + 2*c1y*c5y/c4y*(tSum[plst]-tSum[fst])
                - 2*c1y/c3y*(ySum[plst]-ySum[fst])
                - 2*c5y/c3y*(ytSum[plst]-ytSum[fst]);
        return distance;
    }

public:
//    static const int ERR_AVERAGE_SED = 0x01;
//    static const int ERR_MAX_SED = 0x02;
//    static const int ERR_MEAN_SED = 0x04;
//    static const int ERR_AVERAGE_SSED_PER_POINT = 0x08;
//    static const int ERR_MAX_LSSED = 0x10;
//    static const int ERR_TIME_COST = 0x20;

//    static const int ALG_DOTS = 0x01;
//    static const int ALG_DOTS_CASCADE = 0x02;
//    static const int ALG_DP = 0x04;
//    static const int ALG_PERSISTENCE = 0x08;
//    static const int ALG_SQUISH = 0x10;
//    static const int ALG_MRPA = 0x20;
//    static const int ALG_TS = 0x40;
    static const int ERR_AVERAGE_SED;
    static const int ERR_MAX_SED;
    static const int ERR_MEAN_SED;
    static const int ERR_AVERAGE_SSED_PER_POINT;
    static const int ERR_MAX_LSSED;
    static const int ERR_TIME_COST;

    static const int ALG_DOTS;
    static const int ALG_DOTS_CASCADE;
    static const int ALG_DP;
    static const int ALG_PERSISTENCE;
    static const int ALG_SQUISH;
    static const int ALG_MRPA;
    static const int ALG_TS;

signals:

public slots:
};

#endif // ALGORITHMCOMPARISON_H

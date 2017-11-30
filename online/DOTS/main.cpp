/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#include<QApplication>
#include<QDebug>
#include<QException>
#include<QVector>
#include<QElapsedTimer>
#include"DotsSimplifier.h"
#include"Helper.h"
#include"DotsException.h"
#include"PersistenceBatchSimplifier.h"
#include"DouglasPeuckerBatchSimplifier.h"
#include"SquishBatchSimplifier.h"
#include"mainwindow.h"
#include"AlgorithmComparison.h"
#include<QtMath>

/**
 * @brief main is the entry point of the whole application.
 * @param argc is number of arguments transfered to the application by invoker.
 * @param argv represents the arguments trasnfered to the application by invoker.
 * @return 0 if the application exits normally, non-zero otherwise.
 */
int main(int argc, char *argv[])
{
    // The application.
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QApplication::setGraphicsSystem("raster");
#endif
    QApplication a(argc, argv);

    try
    {
        int algorithms = 0x1f;
        int errTypes = 0x3f;
        AlgorithmComparison::compareAlgorithms(
                    "../test_files/r6.txt",
                    algorithms, errTypes);
        return a.exec();

        // Parse the MOPSI/GeoLife data file as trajectory x/y/t.
        QVector<double> x,y,t;
        QElapsedTimer timer;
        timer.start();
        QString dataFileName = "../test_files/r6.txt";
        dataFileName = "E:\\Geolife Trajectories 1.3\\Data\\032\\Trajectory\\20081202231826.plt";
        if (dataFileName.endsWith(".txt")) // MOPSI dataset.
            Helper::parseMOPSI(dataFileName, x, y, t);
        else // GeoLife dataset.
            Helper::parseGeoLife(dataFileName, x, y, t);
        qDebug("Parsing file OK, time: %d ms.", timer.elapsed());

        // Construct singular data for testing.
//        x = Helper::range<double>(0, 1000, 1);
//        y = Helper::range<double>(0, 1000, 1);
//        t = Helper::range<double>(0, 1000, 1);

        // Simplify the parsed trajectory.
        QVector<double> dotsX, dotsY, dotsT, dotsCascadeX, dotsCascadeY, dotsCascadeT,
                persistenceX, persistenceY, persistenceT, DPX, DPY, DPT,
                squishX, squishY, squishT;
        timer.start();
        DotsSimplifier::batchDots(x, y, t, dotsX, dotsY, dotsT, 4000);
        qDebug("%15s - Original curve size: %d, output curve size: %d, time: %d ms",
               "DOTS", x.count(), dotsX.count(), timer.elapsed());
        timer.start();
        DotsSimplifier::batchDotsCascade(x, y, t, dotsCascadeX, dotsCascadeY, dotsCascadeT, 6000000);
        qDebug("%15s - Original curve size: %d, output curve size: %d, time: %d ms",
               "DOTS CASCADE", x.count(), dotsCascadeX.count(), timer.elapsed());
        timer.start();
        PersistenceBatchSimplifier::simplifyCascade(x, y, persistenceX, persistenceY, 600);
        qDebug("%15s - Original curve size: %d, output curve size: %d, time: %d ms",
               "PERSISTENCE", x.count(), persistenceX.count(), timer.elapsed());
        timer.start();
        DouglasPeuckerBatchSimplifier::simplify(x, y, DPX, DPY, 3.0);
        qDebug("%15s - Original curve size: %d, output curve size: %d, time: %d ms",
               "DP", x.count(), DPX.count(), timer.elapsed());
        timer.start();
        SquishBatchSimplifier::simplify(x, y, t, squishX, squishY, squishT, 40);
        qDebug("%15s - Original curve size: %d, output curve size: %d, time: %d ms",
               "SQUISH", x.count(), squishX.count(), timer.elapsed());

        MainWindow figure;
        figure.plot(x, y, Qt::blue);
        figure.plot(dotsX, dotsY, Qt::red);
        figure.plot(dotsCascadeX, dotsCascadeY, Qt::green);
        figure.plot(persistenceX, persistenceY, Qt::cyan);
        figure.plot(DPX, DPY, Qt::black);
        figure.plot(squishX, squishY, Qt::magenta);
        figure.show();
        qDebug("\nPress any key to continue...");
        return a.exec();
    }
    catch (DotsException &e)
    {
        qDebug()<<"ERROR: "<<e.getMessage();
    }
    catch (QException &)
    {
        qDebug()<<"ERROR: Unknown exception.";
    }

    return a.exec();
}


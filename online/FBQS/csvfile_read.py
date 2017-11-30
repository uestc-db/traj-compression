
import csv
import numpy as np


def readcsvfile(path, delimiter=','):
    data = []
    with open(path, 'rb') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=delimiter, quotechar='|')
        for row in spamreader:
            data.append([float(x) for x in row])

    return np.array(data)


def writecsvfile(data, path, delimiter=','):
    print data

    with open(path, 'wb') as csvfile:
        spamwriter = csv.writer(csvfile, delimiter=delimiter,
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        for row in data:
            #print row
            spamwriter.writerow(row)

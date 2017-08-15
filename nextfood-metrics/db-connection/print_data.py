#!/usr/bin/python

from pymongo import MongoClient
from datetime import datetime,timedelta,tzinfo
import dateutil.parser
import sys
import json
import pytz
from tzlocal import get_localzone


class MongoDBDump:
    _growsystem = None
    _module = None
    _min_back = 12 * 60.0
    _doc = {}
    _dbHostname = 'localhost'

    def printAll(self):
        client = MongoClient(host=self._dbHostname)
        db = client[self._growsystem]
        collection = db[self._module]
        cursor = collection.find({'date': {'$gte': datetime.utcnow() - timedelta( minutes=self._min_back ) } })
        for d in cursor:
             if 'value' in d:
                 print("{} ({}/{}) [{}] {}".format(d['date'].replace(tzinfo=pytz.utc).astimezone(get_localzone()).isoformat(),self._growsystem,self._module,d['action'],d['value']))
             else:
                 print("{} ({}/{}) [{}]".format(d['date'].replace(tzinfo=pytz.utc).astimezone(get_localzone()).isoformat(),self._growsystem,self._module,d['action']))


def main():
    if len(sys.argv) < 3:
        print "Usage: print_data.py <minutes back> <grow system> [grow box]"
        sys.exit(1)

    

    m = MongoDBDump()

    m._min_back = float(sys.argv[1])

    m._growsystem = "gs{}".format(sys.argv[2])
    m._module = "system"

    if len(sys.argv) >= 4:
        m._module = "gb{}".format(sys.argv[3])

    m.printAll()


if __name__ == "__main__":
    main()

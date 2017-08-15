#!/usr/bin/python

from pymongo import MongoClient
import datetime
import sys
import json


class MongoDBLogger:
    _growsystem = None
    _module = None
    _doc = {}
    _dbHostname = 'localhost'

    def insertToDB(self):
        client = MongoClient(host=self._dbHostname)
        db = client[self._growsystem]

        self._doc['date'] = datetime.datetime.utcnow()

        result = db[self._module].insert_one(self._doc)


def main():
    if len(sys.argv) < 3:
        print "Usage: log_to_mongodb.py <grow system> <module> [json document]"
        sys.exit(1)

    

    m = MongoDBLogger()

    m._growsystem = sys.argv[1]
    m._module = sys.argv[2]
    if len(sys.argv) >= 4:
        jsonStr = sys.argv[3].strip('\"\'')
        m._doc = json.loads(jsonStr)
    m.insertToDB()


if __name__ == "__main__":
    main()

#! /usr/bin/env python

from pycrtools import crdatabase
import sys

db = crdatabase.CRDatabase("cr.db", host="coma00.science.ru.nl", user="crdb", password="crdb", dbname="crdb")

for dbid in sys.argv[1:]:
    db.deleteEvent(int(dbid))


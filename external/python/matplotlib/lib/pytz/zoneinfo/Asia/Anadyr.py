'''tzinfo timezone information for Asia/Anadyr.'''
from pytz.tzinfo import DstTzInfo
from pytz.tzinfo import memorized_datetime as d
from pytz.tzinfo import memorized_ttinfo as i

class Anadyr(DstTzInfo):
    '''Asia/Anadyr timezone definition. See datetime.tzinfo for details'''

    _zone = 'Asia/Anadyr'

    _utc_transition_times = [
d(1,1,1,0,0,0),
d(1924,5,1,12,10,4),
d(1930,6,20,12,0,0),
d(1981,3,31,11,0,0),
d(1981,9,30,10,0,0),
d(1982,3,31,11,0,0),
d(1982,9,30,11,0,0),
d(1983,3,31,12,0,0),
d(1983,9,30,11,0,0),
d(1984,3,31,12,0,0),
d(1984,9,29,14,0,0),
d(1985,3,30,14,0,0),
d(1985,9,28,14,0,0),
d(1986,3,29,14,0,0),
d(1986,9,27,14,0,0),
d(1987,3,28,14,0,0),
d(1987,9,26,14,0,0),
d(1988,3,26,14,0,0),
d(1988,9,24,14,0,0),
d(1989,3,25,14,0,0),
d(1989,9,23,14,0,0),
d(1990,3,24,14,0,0),
d(1990,9,29,14,0,0),
d(1991,3,30,14,0,0),
d(1991,9,28,15,0,0),
d(1992,1,18,15,0,0),
d(1992,3,28,11,0,0),
d(1992,9,26,10,0,0),
d(1993,3,27,14,0,0),
d(1993,9,25,14,0,0),
d(1994,3,26,14,0,0),
d(1994,9,24,14,0,0),
d(1995,3,25,14,0,0),
d(1995,9,23,14,0,0),
d(1996,3,30,14,0,0),
d(1996,10,26,14,0,0),
d(1997,3,29,14,0,0),
d(1997,10,25,14,0,0),
d(1998,3,28,14,0,0),
d(1998,10,24,14,0,0),
d(1999,3,27,14,0,0),
d(1999,10,30,14,0,0),
d(2000,3,25,14,0,0),
d(2000,10,28,14,0,0),
d(2001,3,24,14,0,0),
d(2001,10,27,14,0,0),
d(2002,3,30,14,0,0),
d(2002,10,26,14,0,0),
d(2003,3,29,14,0,0),
d(2003,10,25,14,0,0),
d(2004,3,27,14,0,0),
d(2004,10,30,14,0,0),
d(2005,3,26,14,0,0),
d(2005,10,29,14,0,0),
d(2006,3,25,14,0,0),
d(2006,10,28,14,0,0),
d(2007,3,24,14,0,0),
d(2007,10,27,14,0,0),
d(2008,3,29,14,0,0),
d(2008,10,25,14,0,0),
d(2009,3,28,14,0,0),
d(2009,10,24,14,0,0),
d(2010,3,27,14,0,0),
d(2010,10,30,14,0,0),
d(2011,3,26,14,0,0),
d(2011,10,29,14,0,0),
d(2012,3,24,14,0,0),
d(2012,10,27,14,0,0),
d(2013,3,30,14,0,0),
d(2013,10,26,14,0,0),
d(2014,3,29,14,0,0),
d(2014,10,25,14,0,0),
d(2015,3,28,14,0,0),
d(2015,10,24,14,0,0),
d(2016,3,26,14,0,0),
d(2016,10,29,14,0,0),
d(2017,3,25,14,0,0),
d(2017,10,28,14,0,0),
d(2018,3,24,14,0,0),
d(2018,10,27,14,0,0),
d(2019,3,30,14,0,0),
d(2019,10,26,14,0,0),
d(2020,3,28,14,0,0),
d(2020,10,24,14,0,0),
d(2021,3,27,14,0,0),
d(2021,10,30,14,0,0),
d(2022,3,26,14,0,0),
d(2022,10,29,14,0,0),
d(2023,3,25,14,0,0),
d(2023,10,28,14,0,0),
d(2024,3,30,14,0,0),
d(2024,10,26,14,0,0),
d(2025,3,29,14,0,0),
d(2025,10,25,14,0,0),
d(2026,3,28,14,0,0),
d(2026,10,24,14,0,0),
d(2027,3,27,14,0,0),
d(2027,10,30,14,0,0),
d(2028,3,25,14,0,0),
d(2028,10,28,14,0,0),
d(2029,3,24,14,0,0),
d(2029,10,27,14,0,0),
d(2030,3,30,14,0,0),
d(2030,10,26,14,0,0),
d(2031,3,29,14,0,0),
d(2031,10,25,14,0,0),
d(2032,3,27,14,0,0),
d(2032,10,30,14,0,0),
d(2033,3,26,14,0,0),
d(2033,10,29,14,0,0),
d(2034,3,25,14,0,0),
d(2034,10,28,14,0,0),
d(2035,3,24,14,0,0),
d(2035,10,27,14,0,0),
d(2036,3,29,14,0,0),
d(2036,10,25,14,0,0),
d(2037,3,28,14,0,0),
d(2037,10,24,14,0,0),
        ]

    _transition_info = [
i(42600,0,'LMT'),
i(43200,0,'ANAT'),
i(46800,0,'ANAT'),
i(50400,3600,'ANAST'),
i(46800,0,'ANAT'),
i(46800,0,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(43200,0,'ANAST'),
i(39600,0,'ANAT'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
i(46800,3600,'ANAST'),
i(43200,0,'ANAT'),
        ]

Anadyr = Anadyr()


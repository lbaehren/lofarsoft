class Parset(dict):

    def __init__(self, fileName=None):
        if fileName: self.readFromFile(fileName)

    def readFromFile(self, fileName):
        lastline = ''
        for line in open(fileName, 'r').readlines():
            lastline = lastline + line.split('#')[0]
            lastline = lastline.rstrip()
            if len(lastline) > 0 and lastline[-1] == '\\':
                lastline = lastline[:-1]
            elif '=' in lastline:
                key, value = lastline.split('=')
                self[key.strip()] = value.strip()
                lastline = ''

    def writeToFile(self, fileName):
        outf = open(fileName, 'w')
        for key in sorted(self.keys()):
            outf.write(key + ' = ' + str(self[key]) + '\n')
        outf.close()

    def getString(self, key):
        return self[key]

    def getInt(self, key):
        return int(self[key])

    def getFloat(self, key):
        return float(self[key])

    def getStringVector(self, key):
        if type(self[key]) is str:
            vec = self[key].strip('[]').split(',')
        else:
            vec = self[key]
        return [lp.strip() for lp in vec]

    def getIntVector(self, key):
        if type(self[key]) is str:
            vec = self[key].strip('[]').split(',')
        else:
            vec = self[key]
        return [int(lp) for lp in vec]

    def getFloatVector(self, key):
        if type(self[key]) is str:
            vec = self[key].strip('[]').split(',')
        else:
            vec = self[key]
        return [float(lp) for lp in vec]


### Self tests ###
    
if __name__ == '__main__':
    import sys
    import os
    # First create a parset in memory.
    p = Parset()
    p['anInt'] = str(42)
    p['aFloat'] = str(3.141592653589793)
    p['aString'] = str('hello world')
    p['anIntVec'] = str([1, 2, 3, 4, 5])
    p['aFloatVec'] = str([2.5, 4.25,
                          8.125, 16.0625])
    p['aStringVec'] = str(['aap', 'noot', 'mies', 'wim', 'zus', 'jet',
                           'teun', 'vuur', 'gijs', 'lam', 'kees', 'bok',
                           'weide', 'does', 'hok', 'duif', 'schapen'])
    # Write the parset out to file
    p.writeToFile('p.parset');

    # Create another parset by reading the written parset file
    q = Parset()
    q.readFromFile('p.parset')

    # Clean-up temporary parset file
    os.remove('p.parset')

    # Parsets p and q must be equal
    sys.stdout.write('Comparing parameter sets ...   ')
    if p == q:
        print 'ok'
    else:
        print 'FAIL: Expected equal parameter sets!'


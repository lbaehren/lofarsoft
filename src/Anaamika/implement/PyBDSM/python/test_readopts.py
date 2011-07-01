

keyline = 'OPTS PARA FOR GUI'
f=open('/Users/mohan/lofarsoft/src/pybdsm/implement/bdsm_test/opts.py')

paralist = []
line=''
while keyline not in line: 
  line=f.readline()
line=''
while keyline not in line: 
    line = f.readline().strip()
    if keyline in line: break
    if len(line.strip()) > 0:
      while line[-1] == '\\': 
        line = line[:-2] + f.readline().strip()
        line = line.strip()
      llist = line.split('=')
      parameter, descr = llist[0].strip(), llist[1].strip()
      para_doc = ''.join(llist[2:])
      para_doc = para_doc.strip()[:-1]
      if descr[-3:] == 'doc':
        descr = descr[:-3]
      else:
        print 'Problem with keyword ', parameter
      ind = descr.find('(')
      para_type = descr[:ind]
      para_descr = descr[ind+1:]
      paralist.append([parameter, para_type, para_descr, para_doc])



f.close()


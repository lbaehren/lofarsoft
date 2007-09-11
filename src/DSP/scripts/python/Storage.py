"""Storage module using a hdf5 file. This is for simple array storage with some metadata and comments attached to it"""
import tables
import string
import numpy

class Storage:
   def __init__(self, hdffile, mode="a"):
      "mode: a - append;  r - read only;   w - write (if a h5 file already exists, it will be deleted)"
      self.mode = mode
      self.froot = tables.openFile(hdffile, mode=mode)
      self.currentdir = '/'
      
   def canWrite(self):
      "returns true if you can write to the storage file"
      return not (self.mode == "r")
      
   def __resolvepath(self, oper, src=None):
      "Determines the new path"
      if src == None: src = self.currentdir
      seq = oper.split('/')
      for c in seq:
         if c == '': src = '/'
         elif c == '..' and '/' in src and len(src) > 1:
            src = src[:src.rfind('/')]
         else:
            if src[-1] != '/':
               src += '/' + c
            else:
               src += c
      return src
   
   def listdir(self, relpath=''):
      "List subdirs at the given path"
      return [a._v_name for a in self.froot.iterNodes(self.__resolvepath(relpath)) if isinstance(a, tables.Group)]
   
   def cd(self, currd=''):
      "Change 'directory' location, .. is supported"
      self.currentdir = self.__resolvepath(currd)
   
   def listdata(self, relpath=''):
      "List datasets at the given path"
      return [a._v_name for a in self.froot.iterNodes(self.__resolvepath(relpath)) if not isinstance(a, tables.Group)]
   
   def listall(self, relpath=''):
      "List datasets and nodes at the given path"
      return [a._v_name for a in self.froot.iterNodes(self.__resolvepath(relpath))]
      
   def datainfo(self, path=''):
      "Describe the data at the given path"
      return [a._v_title for a in self.froot.iterNodes(self.__resolvepath(path)) if not isinstance(a, tables.Group)]
   
   def get(self, path):
      "Get the data at the given path"
      return self.froot.getNode(self.__resolvepath(path)).read()
   
   def save(self, path, data, metadata='', compress=False):
      "Save data and metadata info at the given path"
      ppath = self.__resolvepath('..', path)
      name = path.split('/')[-1]
      try: # remove pre-existing data if it exists
         self.delete(path)
      except:
         pass
      if compress:
         if not isinstance(data, numpy.ndarray): data = numpy.array(data)
         af = tables.Filters(complevel=5, complib='zlib')
         atm = tables.Atom.from_dtype(data.dtype)
         thec = self.froot.createCArray(self.__resolvepath(ppath), name, atm, data.shape, metadata, filters=af, createparents=True)
         thec[:] = data
      else:
         self.froot.createArray(self.__resolvepath(ppath), name, data, metadata, createparents=True)
      self.froot.flush()
      
   def createGroup(self, path):
      "Create a group"
      ppath = self.__resolvepath(self.__resolvepath('..', path))
      name = path.split('/')[-1]
      self.froot.createGroup(ppath, name)
      
   def move(self, source, parent):
      "move a node at source to a new parent"
      self.froot.moveNode(self.__resolvepath(source), self.__resolvepath(parent))
      
   def rename(self, source, newname):
      "rename a node at source to newname"
      self.froot.renameNode(self.__resolvepath(source), newname)
      
   def delete(self, path):
      "remove node or group at path"
      self.froot.removeNode(self.__resolvepath(path))
   
   def deleteAll(self, path):
      "remove node or group at path, kills all children as well"
      self.froot.removeNode(self.__resolvepath(path), recursive=True)
      
   def groupTree(self, root=None, i=0):
      "Get a string representation of the groups, in a tree"
      s = ''
      root = root or self.currentdir
      for e in self.listdir(root):
         s += string.join([' ' for cnt in range(i)], '') + e + '\n'
         s += self.groupTree(root=self.__resolvepath(e, root), i=i+3)
      return s
   
   def nodeTree(self, root=None, i=0):
      "Get a string representation of the groups and nodes, in a tree"
      s = ''
      root = root and self.__resolvepath(root) or self.currentdir
      for e in self.listall(root):
         s += string.join([' ' for cnt in range(i)], '')
         if isinstance(self.froot.getNode(self.__resolvepath(e, root)), tables.Group):
            s += '(G) %s\n' % e
            s += self.nodeTree(root=self.__resolvepath(e, root), i=i+3)
         else:
            s += e + '\n'
      return s
   
   def __getitem__(self, key):
      if isinstance(key, tuple):
         path = string.join(key, '/')
      else: path = key
      return self.get(path)

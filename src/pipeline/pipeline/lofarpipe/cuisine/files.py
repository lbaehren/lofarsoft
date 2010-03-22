# The feeling is this needs to be part of the ingredient, not the recipe
# not used currently, maybe integrate it in some way with the data returned by MSinfo?
def zap(filepath):
    import os #, exception
##    if filepath == '/' or filepath == '~/':
##      raise Exception
##    else:
##      for root, dirs, files in os.walk(filepath, topdown=False):
##        for name in files:
##            os.remove(join(root, name))
##        for name in dirs:
##            os.rmdir(join(root, name))
    if os.path.isdir(filepath):
        self.execute('rm', ' -rf ' + filepath)
    elif os.path.isfile(filepath):
        self.execute('rm', ' -f ' + filepath)
    else:
        self.messages.append(DebugLevel, filepath + " doesn't seem to exist")


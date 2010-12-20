import sys
from lofarpipe.support.baserecipe import BaseRecipe

class thumbnail_combine(BaseRecipe):
    def go(self):
        self.logger.info("Starting thumbnail_combine run")
        super(thumbnail_combine, self).go()
        self.logger.info("This recipe does nothing")


if __name__ == '__main__':
    sys.exit(thumbnail_combine().main())

#!/bin/tcsh
#!/bin/tcsh
set scripts = `find . -name "*.pm" | sort`

foreach script ($scripts)
  perl -c $script
end


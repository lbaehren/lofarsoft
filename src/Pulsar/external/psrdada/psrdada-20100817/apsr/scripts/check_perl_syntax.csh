#!/bin/tcsh
#!/bin/tcsh
set scripts = `find . -name "*.pl" -o -name "*.pm" | sort`

foreach script ($scripts)
  perl -c $script
end


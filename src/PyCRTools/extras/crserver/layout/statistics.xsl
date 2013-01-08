<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <head>
    <link rel="stylesheet" type="text/css" href="layout/database.css" />
    <title>LOFAR Cosmic Ray Statistics</title>
    <script src="layout/sorttable.js"></script>
  </head>
  <body>
  <h1>LOFAR Cosmic Ray Statistics</h1>
  <xsl:value-of select="test"/>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>


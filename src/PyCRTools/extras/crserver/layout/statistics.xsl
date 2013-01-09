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
  <xsl:for-each select="/elements/info">
    <div class="info">
        <xsl:if test="graph">
            <img><xsl:attribute name="src"><xsl:value-of select="graph/path"/></xsl:attribute><xsl:attribute name="alt"><xsl:value-of select="graph/path"/></xsl:attribute></img>
        </xsl:if>
    </div>
  </xsl:for-each>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>


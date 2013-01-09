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
        <h2><xsl:value-of select="caption"/></h2>
        <xsl:if test="data">
            <table class="parameters">
            <xsl:for-each select="data/record">
            <tr>
                <td><xsl:value-of select="key"/></td>
                <td><xsl:value-of select="value"/></td>
            </tr>
            </xsl:for-each>
            <xsl:if test="data/total">
            <tr>
                <td class="total">Total</td>
                <td><xsl:value-of select="data/total"/></td>
            </tr>
            </xsl:if>
            </table>
        </xsl:if>
        <xsl:if test="graph">
            <figure>
            <img><xsl:attribute name="src"><xsl:value-of select="graph/path"/></xsl:attribute><xsl:attribute name="alt"><xsl:value-of select="graph/path"/></xsl:attribute></img>
            </figure>
        </xsl:if>
    </div>
  </xsl:for-each>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>


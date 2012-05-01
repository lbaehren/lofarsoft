<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <head>
    <link rel="stylesheet" type="text/css" href="layout/database.css" />
    <title>LOFAR Cosmic Ray Events</title>
  </head>
  <body>
  <h1>LOFAR Cosmic Ray Events</h1>
  <table>
    <tr>
      <th>id</th>
      <th>timestamp</th>
      <th>status</th>
    </tr>
    <xsl:for-each select="/elements/event">
      <xsl:sort select="key"/>
      <tr>
        <td>
          <a><xsl:attribute name="href">
          events/<xsl:value-of select="id"/></xsl:attribute> 
          <xsl:value-of select="id"/> 
          </a>
        </td>
        <td><xsl:value-of select="timestamp"/></td>
        <td><xsl:value-of select="status"/></td>
      </tr>
    </xsl:for-each>
  </table>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>


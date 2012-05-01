<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <head>
    <link rel="stylesheet" type="text/css" href="/layout/database.css" />
    <title>LOFAR Cosmic Ray Event <xsl:value-of select="/elements/header/id"/> station <xsl:value-of select="/elements/header/station"/></title>
  </head>
  <body>
  <h1>LOFAR Cosmic Ray Event <xsl:value-of select="/elements/header/id"/> station <xsl:value-of select="/elements/header/station"/></h1>
  <nav>
      <ul class="cf">
          <li><a href="/events">All events</a></li>
          <li><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/></xsl:attribute>Current Event</a></li>
          <li><a class="dropdown" href="#">Datafiles</a>
              <ul>
                <xsl:for-each select="/elements/header/datafiles/datafile">
                <xsl:sort select="name"/>
                  <li><a href="#"><xsl:value-of select="name"/></a></li>
                </xsl:for-each>
              </ul>
          </li>
          <li><a class="dropdown" href="#">Stations</a>
              <ul>
                <xsl:for-each select="/elements/header/stations/station">
                <xsl:sort select="name"/>
                  <li><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/>/<xsl:value-of select="name"/></xsl:attribute><xsl:value-of select="name"/></a></li>
                </xsl:for-each>
              </ul>
          </li>
          <li><a class="dropdown" href="#">Polarizations</a>
              <ul>
                <xsl:for-each select="/elements/header/polarizations/polarization">
                <xsl:sort select="name"/>
                  <li><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/>/<xsl:value-of select="/elements/header/station"/>/<xsl:value-of select="name"/></xsl:attribute><xsl:value-of select="name"/></a></li>
                </xsl:for-each>
              </ul>
          </li>
      </ul>
  </nav>
  <table id="header">
  <caption>Header</caption>
    <tr>
      <th>id</th>
      <th>timestamp</th>
      <th>status</th>
    </tr>
    <tr>
      <td>
        <td><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/></xsl:attribute><xsl:value-of select="/elements/header/id"/></a></td>
      </td>
      <td>
        <td><xsl:value-of select="/elements/header/timestamp"/></td>
      </td>
      <td>
        <td><xsl:value-of select="/elements/header/status"/></td>
      </td>
    </tr>
  </table>
  <table>
    <caption>Parameters</caption>
    <tr>
      <th>key</th>
      <th>value</th>
    </tr>
    <xsl:for-each select="/elements/parameters/parameter">
      <xsl:sort select="key"/>
      <tr>
        <td><xsl:value-of select="key"/></td>
        <td><xsl:value-of select="value"/></td>
      </tr>
    </xsl:for-each>
  </table>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>


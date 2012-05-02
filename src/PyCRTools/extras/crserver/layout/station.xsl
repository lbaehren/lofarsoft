<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <head>
    <link rel="stylesheet" type="text/css" href="/layout/database.css" />
    <title>LOFAR Cosmic Ray Event <xsl:value-of select="/elements/header/id"/> station <xsl:value-of select="/elements/header/station/name"/></title>
  </head>
  <body>
  <h1>LOFAR Cosmic Ray Event <xsl:value-of select="/elements/header/id"/> station <xsl:value-of select="/elements/header/station/name"/></h1>
  <nav>
      <ul class="cf">
          <li><a class="dropdown" href="#">Section</a>
              <ul>
                  <li><a href="#header">Header</a></li>
                  <li><a href="#figures">Figures</a></li>
                  <li><a href="#parameters">Parameters</a></li>
              </ul>
          </li>
          <li><a href="/events">All events</a></li>
          <li><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/></xsl:attribute>Current Event</a></li>
          <!--
          <li><a class="dropdown" href="#">Datafiles</a>
              <ul>
                <xsl:for-each select="/elements/header/datafiles/datafile">
                <xsl:sort select="name"/>
                  <li><a href="#"><xsl:value-of select="name"/></a></li>
                </xsl:for-each>
              </ul>
          </li>
          -->
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
                  <li><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/>/<xsl:value-of select="/elements/header/station/name"/>/<xsl:value-of select="name"/></xsl:attribute><xsl:value-of select="name"/></a></li>
                </xsl:for-each>
              </ul>
          </li>
      </ul>
  </nav>
  <table>
  <caption id="header">Header</caption>
    <tr>
      <th>Id</th>
      <th>Timestamp</th>
      <th>Event status</th>
      <th>Station status</th>
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
      <td>
        <td><xsl:value-of select="/elements/header/station/status"/></td>
      </td>
    </tr>
  </table>
  <table>
    <caption id="polarizations">Polarizations</caption>
    <tr>
      <th>Polarization</th>
      <th>Polarization Status</th>
    </tr>
    <xsl:for-each select="/elements/header/polarizations/polarization">
    <xsl:sort select="name"/>
      <tr>
        <td><a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/>/<xsl:value-of select="/elements/header/station/name"/>/<xsl:value-of select="name"/></xsl:attribute><xsl:value-of select="name"/></a></td>
        <td><xsl:value-of select="status"/></td>
      </tr>
    </xsl:for-each>
  </table>
  <table>
    <caption id="figures">Figures</caption>
    <xsl:for-each select="/elements/figures/figure">
      <tr>
        <td><img><xsl:attribute name="src"><xsl:value-of select="path"/></xsl:attribute><xsl:attribute name="alt"><xsl:value-of select="path"/></xsl:attribute></img></td>
      </tr>
    </xsl:for-each>
  </table>
  <table class="parameters">
    <caption id="parameters">Parameters</caption>
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


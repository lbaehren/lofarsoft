<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <head>
    <link rel="stylesheet" type="text/css" href="/layout/database.css" />
    <title>LOFAR Cosmic Ray Event <xsl:value-of select="/elements/header/id"/> station <xsl:value-of select="/elements/header/station/name"/> polarization <xsl:value-of select="/elements/header/polarization/name"/></title>
  </head>
  <body>
  <h1>LOFAR Cosmic Ray Event <xsl:value-of select="/elements/header/id"/> station <xsl:value-of select="/elements/header/station/name"/> polarization <xsl:value-of select="/elements/header/polarization/name"/></h1>
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
      <td colspan="2">
        <a><xsl:attribute name="href">/events/<xsl:value-of select="/elements/header/id"/></xsl:attribute><xsl:value-of select="/elements/header/id"/></a>
      </td>
    </tr>
    <tr>
      <th>Timestamp</th>
      <td colspan="2">
        <xsl:value-of select="/elements/header/timestamp"/>
      </td>
    </tr>
    <tr>
      <th>Antennaset</th>
      <td colspan="2">
        <xsl:value-of select="/elements/header/antennaset"/>
      </td>
    </tr>
    <tr>
      <th>Event status</th>
      <td>
        <xsl:value-of select="/elements/header/status"/>
      </td>
      <td>
        <xsl:value-of select="/elements/header/statusmessage"/>
      </td>
    </tr>
    <tr>
      <th>Event alt_status</th>
      <td>
        <xsl:value-of select="/elements/header/alt_status"/>
      </td>
      <td>
        <xsl:value-of select="/elements/header/alt_statusmessage"/>
      </td>
    </tr>
    <tr>
      <th>Station status</th>
      <td>
        <xsl:value-of select="/elements/header/station/status"/>
      </td>
      <td>
        <xsl:value-of select="/elements/header/station/statusmessage"/>
      </td>
    </tr>
    <tr>
      <th>Station alt_status</th>
      <td>
        <xsl:value-of select="/elements/header/station/alt_status"/>
      </td>
      <td>
        <xsl:value-of select="/elements/header/station/alt_statusmessage"/>
      </td>
    </tr>
    <tr>
      <th>Polarization status</th>
      <td>
        <xsl:value-of select="/elements/header/polarization/status"/>
      </td>
      <td>
        <xsl:value-of select="/elements/header/polarization/statusmessage"/>
      </td>
    </tr>
    <tr>
      <th>Polarization alt_status</th>
      <td>
        <xsl:value-of select="/elements/header/polarization/alt_status"/>
      </td>
      <td>
        <xsl:value-of select="/elements/header/polarization/alt_statusmessage"/>
      </td>
    </tr>
  </table>
  <table>
    <caption id="figures">Figures</caption>
    <tr>
      <td>
        <xsl:for-each select="/elements/figures/figure">
          <div class="img">
            <a><xsl:attribute name="href"><xsl:value-of select="path"/></xsl:attribute>
              <img><xsl:attribute name="src"><xsl:value-of select="path"/></xsl:attribute><xsl:attribute name="alt"><xsl:value-of select="path"/></xsl:attribute></img>
            </a>
          </div>
        </xsl:for-each>
      </td>
    </tr>
  </table>
  <table class="parameters">
    <caption id="parameters">Parameters</caption>
    <tr>
      <th>key</th>
      <th>value</th>
    </tr>
    <xsl:for-each select="/elements/parameters/parameter">
      <xsl:sort select="key"/>
      <xsl:if test="value != ''">
      <tr>
        <td><xsl:value-of select="key"/></td>
        <td><xsl:value-of select="value"/></td>
      </tr>
      </xsl:if>
    </xsl:for-each>
  </table>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>

